//////////////////////////////////////////////////////////////////////////////////////
//
//  Decoder for the DAQ data for ProtoDUNE-DP
// 
// 
//////////////////////////////////////////////////////////////////////////////////////

#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <mutex>
#include <bitset>

#include "EventDecoder.h"
#include "HuffmanDecompressor.h"
#include "LogMsg.h"

using namespace std;
using namespace np02daq;

void unpackCroData( const char *buf, size_t nb, bool cflag, unsigned nsa, adcdata_t &data, unsigned fragid )
{
  data.clear();
  if( !cflag ) 
    {
      const BYTE* start = buf;
      const BYTE* stop  = start + nb;
      while(start!=stop)
	{
	  BYTE v1 = *start++;
	  BYTE v2 = *start++;
	  BYTE v3 = *start++;
	  
	  uint16_t tmp1 = ((v1 << 4) + ((v2 >> 4) & 0xf)) & 0xfff;
	  uint16_t tmp2 = (((v2 & 0xf) << 8 ) + (v3 & 0xff)) & 0xfff;
	  data.push_back( tmp1 );
	  data.push_back( tmp2 );
	}
    }
  else {
    HuffDecompressor( buf, nb, nsa, data, fragid );
  }  

  //return data.size();
}

//
// ctor
//
EventDecoder::EventDecoder()
{
  m_nev = 0;
  m_filesz = 0;
  m_event.nsacro = nsacro;
}

//
// dtor
//
EventDecoder::~EventDecoder()
{
  Close();
}

//
// unpack event info from each l1evb fragment
//
unsigned EventDecoder::unpack_eve_info(const char* buf,  eveinfo_t &ei)
{
  //
  unsigned rval = 0;
  
  //
  memset(&ei, 0, sizeof(ei));
  try
    {
      // check for delimiting words
      BYTE k0 = buf[0];
      BYTE k1 = buf[1];
      //msg_info<<bitset<8>(k0)<<" "<<bitset<8>(k1)<<endl;
      if( !( ((k0 & 0xFF) == evskey) && ((k1 & 0xFF) == evskey) ) )
	{
	  msg_err<<"Event delimiting word could not be detected "<<endl;
	  throw fex;
	}
      rval += 2;

      // decode run number
      ei.runnum   = ConvertToValue<uint32_t>( buf+rval );
      rval += sizeof( ei.runnum );

      // run flags
      ei.runflags = (uint8_t)buf[rval++];

      // this is actually written in host byte order
      ei.ti = ConvertToValue<triginfo_t>(buf+rval);
      rval += sizeof(ei.ti);
      
      // data quality flags
      ei.evflag = (uint8_t)buf[rval++];
      
      // event number 4 bytes
      ei.evnum   = ConvertToValue<uint32_t>( buf+rval );
      rval += sizeof( ei.evnum );
      
      // size of the lro data segment
      ei.evszlro   = ConvertToValue<uint32_t>( buf+rval );
      rval += sizeof( ei.evszlro );
      
      // size of the cro data segment
      ei.evszcro   = ConvertToValue<uint32_t>( buf+rval );
      rval += sizeof( ei.evszcro );
      
      //msg_info<<"ei.evszcro "<<ei.evszcro<<endl;
    }
  catch(exception &e)
    {
      msg_err<<"Decode event header exception '"<<e.what()<<"'"<<endl;
      rval = 0;
    }
  
  return rval;
}

//
// unpack_nev_info
//
unsigned EventDecoder::unpack_evtable()
{
  unsigned rval = 0;
  m_nev = 0; 
  
  std::vector<BYTE> buf;
  size_t msz = 2*sizeof(uint32_t);
  readChunk( buf, msz);
  if( buf.size() != msz )
    {
      msg_err<<"could not get number of events from the file "<<endl;
      return 0;
    }
  
  // first 32 bits are run sequence
  
  // number of events in the file
  m_nev = ConvertToValue<uint32_t>(&buf[4]);
  rval += buf.size();
  
  // get number of events
  //m_nev  = (((uint16_t)buf[0] << 8) & 0xFF00);
  //m_nev +=  ((uint16_t)buf[1] & 0xFF);
  
  // file 
  if( m_nev == 0 )
    {
      msg_err<<"file does not contain any events"<<endl;
      return 0;
    }
  
  size_t evtsz = m_nev * 4 * sizeof(uint32_t);
  if(  evtsz + rval >= m_filesz )
    {
      msg_err<<"cannot find event table"<<endl;
      return 0;
    }

  readChunk( buf, evtsz );
  rval += buf.size();
  
  // unpack sizes of events in sequence
  m_evsz.clear();

  for( unsigned i=0;i<buf.size();i+=16 )
    {
      unsigned o = 0;
      //
      o = 4;
      uint32_t sz = ConvertToValue<uint32_t>(&buf[i+o]);
      m_evsz.push_back( sz );
    }

  // generate table of positions of events in a file
  m_events.clear();
  
  // first event
  m_events.push_back( m_file.tellg() );
  for(size_t i=0; i<m_evsz.size()-1; i++)
    {
      m_file.seekg( m_evsz[i], std::ios::cur );
      if( !m_file )
	{
	  msg_err<<"event table does not match file size"<<endl;
	  m_file.clear();
	  m_evsz.resize( m_events.size() );
	  m_nev = m_events.size();
	  break;
	}
      m_events.push_back( m_file.tellg() );
    }
  
  //for( auto e : m_events ) msg_info << e <<endl;
  //for( auto e : m_evsz ) msg_info << e <<endl;

  // go back to first event
  //m_file.seekg( m_events[0], ios::beg );
  
  // return number of bytes unpacked
  return rval;
}

//
// close input file
//
void EventDecoder::Close()
{
  if(m_file.is_open())
    m_file.close();  
  
  m_filesz = 0;
}



//
//
//
ssize_t EventDecoder::Open(std::string finname)
{
  // attempt to close any previously opened files
  Close();
  
  m_evid = -1;
  
  //
  m_file.open(finname.c_str(), ios::in | ios::binary | ios::ate);
  
  if( !m_file.is_open() )
    {
      msg_err<<"Could not open "<<finname<<" for reading"<<endl;
      return -1;
    }
  
  // get file size
  m_filesz = m_file.tellg();

  // move to beginning
  m_file.seekg(0, ios::beg);
  
  // unpack event table
  if( unpack_evtable() == 0 )
    {
      Close();
      return 0;
    }
  
  return m_nev;
}

//
// read a given number of bytes from file
//
void EventDecoder::readChunk( std::vector<BYTE> &bytes, size_t sz )
{
  bytes.resize( sz );
  m_file.read( &bytes[0], sz );
  if( !m_file )
    {
      ssize_t nb = m_file.gcount();
      //msg_warn<<"could not read "<<sz<" bytes "<<" only "<<nb<<" were available"<<endl;
      bytes.resize( nb );
      // reset stream state from errors
      m_file.clear();
    }
}


//
//
//
bool EventDecoder::unpackEvent( std::vector<BYTE> &buf )
{
  // fragments from each L1 builder
  std::vector<fragment_t> frags;
  
  unsigned idx = 0;
  for(;;)
    {
      fragment_t afrag;
      if( idx >= buf.size() ) break;
      unsigned rval = unpack_eve_info( &buf[idx], afrag.ei );
      if( rval == 0 ) return false;
      idx += rval;
      // set point to the binary data
      afrag.bytes = &buf[idx];
      size_t dsz = afrag.ei.evszcro + afrag.ei.evszlro;
      idx += dsz;
      idx += 1; // bruno byte

      // some basic checks
      if( frags.size() >= 1 )
	{
	  if( frags[0].ei.runnum != afrag.ei.runnum )
	    {
	      msg_err<<"run numbers do not match between different event fragments"<<endl;
	      continue;
	    }
	  if( frags[0].ei.evnum != afrag.ei.evnum )
	    {
	      msg_err<<"event numbers do not match between different event fragments"<<endl;
	      msg_err<<"event number "<< frags[0].ei.evnum << endl;
	      msg_err<<"event number "<< afrag.ei.evnum << endl;
	      continue;
	    }
	  // check timestamps???
	}
      
      frags.push_back( afrag );
    }
  
  //msg_info<<"number of fragments "<<frags.size()<<endl;
  // 
  std::mutex iomutex;
  std::vector<std::thread> threads(frags.size() - 1);
  unsigned nsa = nsacro;
  for (unsigned i = 1; i<frags.size(); ++i) 
    {
      auto afrag = frags.begin() + i;
      threads[i-1] = std::thread([&iomutex, i, nsa, afrag] {
	  {
	    std::lock_guard<std::mutex> iolock(iomutex);
	    //msg_info << "Unpack thread #" << i << " is running\n";
	  }
	  //unpackLROData( f0->bytes, f0->ei.evszlro, ... );
	  unpackCroData( afrag->bytes + afrag->ei.evszlro, afrag->ei.evszcro, 
			 GETDCFLAG(afrag->ei.runflags), nsa, afrag->crodata, i );
	});
    }
  
  // unpack first fragment in the main thread
  auto f0 = frags.begin();
  m_event.good      = EVDQFLAG( f0->ei.evflag );
  m_event.runnum    = f0->ei.runnum;
  m_event.runflags  = f0->ei.runflags;
  //
  m_event.evnum     = f0->ei.evnum;
  m_event.evflags.push_back( f0->ei.evflag );
  //
  m_event.trigtype  = f0->ei.ti.type;
  m_event.trignum   = f0->ei.ti.num;
  m_event.trigstamp = f0->ei.ti.ts;
  
  //unpackLROData( f0->bytes, f0->ei.evszlro, ... );
  unpackCroData( f0->bytes + f0->ei.evszlro, f0->ei.evszcro, GETDCFLAG(f0->ei.runflags),
		 m_event.nsacro, m_event.crodata, 0 );
  
  // wait for other threads to complete
  for (auto& t : threads) t.join();
  
  // merge data
  for (auto it = frags.begin() + 1; it != frags.end(); ++it )
    {
      m_event.good = ( m_event.good && EVDQFLAG( it->ei.evflag ) );
      m_event.evflags.push_back( it->ei.evflag );
      //m_event.crodata.insert( m_event.crodata.end(), it->crodata.begin(), it->crodata.end() );
      //m_event.lrodata.insert( m_event.lrodata.end(), it->lrodata.begin(), it->lrodata.end() );
      
      m_event.crodata.reserve(m_event.crodata.size() + it->crodata.size());
      std::move( std::begin( it->crodata ), std::end( it->crodata ), 
		   std::back_inserter( m_event.crodata ));
      it->crodata.clear();

    }
  
  return true;
}

//
// get a specific event from file
// 
const DaqEvent& EventDecoder::Get(size_t evid)
{
  // return last event
  if( (ssize_t) evid == m_evid )
    return m_event;

  std::vector<BYTE> buf;  
  if( m_file.is_open() )
    {  
      m_evid = (ssize_t)evid;
      if(m_evid >= (ssize_t)m_nev )
	{
	  msg_warn<<"index of event is too high "<<evid<<">="<<m_nev<<endl;
	  m_evid = m_nev - 1;
	}
      
      if( m_events[ m_evid ] != m_file.tellg() )
	m_file.seekg( m_events[ m_evid ], ios::beg );
      
      size_t bsz = m_evsz[ m_evid ];
      
      readChunk( buf, bsz );
      //unpackEvent( buf );
    }
  return Get( buf );
}


//
// unpack 
// 
const DaqEvent& EventDecoder::Get( std::vector<BYTE> &buf )
{
  m_event.clear();
  if( buf.empty() ) return m_event;
  
  //
  bool ok = unpackEvent( buf );
  m_event.valid = ok;
  //return m_event;
  return Event();
}
