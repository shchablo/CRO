#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <future>
#include <sstream>

#include "HuffmanDecompressor.h"
#include "LogMsg.h"

using std::cout;
using std::endl;
using np02daq::msg_err;
using np02daq::msg_info;
using np02daq::msg_warn;
using np02daq::adcdata_t;

//
//
void np02daq::HuffDecompressor( const void *in, const unsigned nb, 
				const unsigned nsa, adcdata_t &data, unsigned fragid )
{
  int logLevel = 0;
  const BYTE* buf   = static_cast<const BYTE*>(in);
  
  // get positions for each channel in the buffer
  vector< pair<unsigned, unsigned> > chsztab = HuffDecompressorChanTable( buf, nb );

  if( logLevel >= 1 ){
    msg_info<<"number of bytes to read in fragment "<<fragid<<" "<<nb<<endl;
    msg_info<<"channel table size in fragment      "<<fragid<<" "<<chsztab.size()<<endl;
  }
  
  if( chsztab.empty() ) {
    msg_err<<"Could not decode chan size table "<<endl;
    return;
  }
  
  /*
  std::stringstream ss;
  ss<<"chtable"<<fragid<<".dat";
  std::ofstream out(ss.str().c_str(), std::ofstream::out);
  out<<chsztab.size()<<endl;
  out<<nb<<endl;
  for( auto &chsz: chsztab ){
    out<<chsz.first<<" "<<chsz.second<<endl;
  }
  out.close();
  */

  // run uncompression for each channel
  std::vector< std::future< adcdata_t > > futures;
  for( auto const &chsz:  chsztab ){
    
    // change 16b words to bigendian
    vector<BYTE> chbuf(buf + chsz.first, buf + chsz.first + chsz.second);
    unsigned nswaps = chbuf.size() / 2;
    unsigned count  = 0;
    for( unsigned i = 0; i < chbuf.size(); i+=2 ){
      std::swap( chbuf[i], chbuf[i+1] );
      count++;
      if( count >= nswaps ) break;
    }
    //
    futures.push_back(std::async( HuffDecompressorChanData, chbuf, nsa, logLevel));
  }

  // get results
  for( size_t i = 0; i < futures.size(); ++i )
    {
      auto chdata = futures[i].get();
      if( chdata.size() != nsa ){
	msg_warn<<"could not decompress data for chan "
		<<i<<" setting all values to 0\n";
	chdata.resize( nsa );
	std::fill( chdata.begin(), chdata.end(), 0 );
      }

      data.insert( data.end(), chdata.begin(), chdata.end() );
    }
  
  // done
}

//
//
vector< pair<unsigned, unsigned> >  np02daq::HuffDecompressorChanTable( const BYTE *buf, 
									const unsigned nb )
{
  // first is the offset from buf start, second number of bytes
  vector< pair<unsigned, unsigned> > chsztab;
  unsigned chblk = 64;
  unsigned hdsz  = chblk * sizeof( uint16_t );
  unsigned nbread = 0;
  unsigned blkcount = 0;
  while( nbread < nb ){
    unsigned tot = hdsz;
    for( unsigned i=0;i<chblk; ++i){
      uint16_t val = ConvertToValue<uint16_t>( buf + nbread + i*sizeof( uint16_t ) );
      unsigned offset = nbread + tot;
      if( offset > nb ){
	msg_err<<"error in decoding channel table\n";
	msg_err<<blkcount<<" "<<nbread<<" "<<i<<" "<<val<<" "<<offset<<endl;
	return chsztab;
      }
      chsztab.push_back( std::make_pair( offset, val) );
      tot    += val;
    }
    // move to next card
    nbread += tot;
    blkcount++;
    if( (nbread + hdsz) > nb ){
      //msg_warn<<"there could be a problem with the event table\n";
      break;
    }
  }
  
  //msg_info<<"found "<<chsztab.size()<<" channels\n";
  return chsztab;
}


//
//
adcdata_t np02daq::HuffDecompressorChanData( const vector<BYTE> bytes,  const unsigned seqlen,
					     int logLevel )
{
  unsigned nbytes = bytes.size();
  const BYTE* buf = &bytes[0];
  
  adcdata_t adc;
  if( nbytes == 0 ){
    adc.resize( seqlen, 0);
    return adc;
  }

  adc.reserve(seqlen);
  
  const BYTE* start = buf;
  const BYTE* stop  = buf + nbytes - 1;
  
  int nzeros  = 0;
  int nseqrep = 4;
  uint16_t bitbytep  = 8;  // bits in a byte
  uint16_t bitsread  = 0;
  uint16_t bitsused  = 13; 
  uint16_t bitsdata  = bitsused - 1;
  short lastdelta    = -999;
  bool newseq        = true;

  // decompression loop
  while(buf <= stop)
    {

      if( bitsread != 0 ) // should never happen
	{
	  if( logLevel >= 2 ){
	    msg_err<<"Problem with decompression loop"<<endl;
	  }
	  break;
	}
      

      // check if the compression bit is set
      bool rawadc = !( (*buf) >> (bitbytep - 1) & 1);

      if( !rawadc && newseq )
	{
	  if( logLevel >= 2){
	    msg_err<<"New sequence should begin with uncompressed value"<<endl;
	  }
	  return adc;
	}
      if(newseq) newseq = false;
      
      //
      bitsread = 0;
      bitbytep--;

      if(bitbytep == 0)  // move to next byte
	{
	  buf++; bitbytep = 8;
	  if( buf > stop ) // finished
	    {
	      if( logLevel >= 2 ){
		msg_warn<<"Problem with decoding encountered. Should have more bytes left"<<endl;
	      }
	      break;
	    }
	}
      
      if( rawadc ) // we got uncompressed bit sequence
	{
	  nzeros    = 0;
	  lastdelta = -999;
	  
	  uint16_t val = 0;
	  while(bitsread < bitsdata)
	    {
	      // number of bits left to read 
	      uint16_t bitsleft   = bitsdata - bitsread;
	      // number of bits left to read in this byte
	      uint16_t bitsinbyte = bitbytep;
	      
	      uint16_t mask       = 0xFF >> (8 - bitbytep);
	      uint16_t bitscarry  = 0;
	      
	      if(bitsinbyte > bitsleft)
		{
		  bitscarry = ( bitsinbyte - bitsleft );
		  mask     &= ( 0xff << bitscarry );
		  bitbytep  = bitscarry;
		  bitsread += bitsleft; 
		  
		  // update the number of left bits to read
		  bitsleft  = 0;
		}
	      else
		{
		  bitbytep  = 0; // got all the bits and will move to next byte
		  bitsread += bitsinbyte; 
		  
		  // update the number of left bits to read
		  bitsleft -= bitsinbyte;
		}
	      
	      // copy the bits to the correct place
	      val += ( ((*buf)  & mask) >> bitscarry ) << ( bitsleft );
	      
	      // move to next byte
	      if( bitbytep == 0 ) { buf++; bitbytep = 8; }
	      if( buf >= stop ) break; // finished
	    }
	  
	  // add to output
	  adc.push_back( val );

	  // read out all bits in this compressed packet
	  bitsread = 0;
	  
	  //
	  newseq = ( (adc.size() % seqlen) == 0);
	  
	  //if(newseq) msg_info<<adc.size()<<" "<<endl;

	  // move to the next byte if not at the byte boundary
	  //if( newseq && bitbytep != 8 )
	  //{ buf++; bitbytep = 8; }
	    
	}
      else // otherwise process compressed bits
	{
	  if( adc.empty() ) 
	    {
	      if( logLevel >= 2){
		msg_err<<"Fatal decoding error: no previous ADC value found."<<endl;
	      }
	      return adc;
	    }
	  
	  // read all the bits in this bit packet
	  while(bitsread < bitsdata)
	    {
	      if( ( (*buf) >> (bitbytep-1) ) & 1 )
		{
		  
		  int addnew = 0;
		  
		  // our Huffman encoding scheme
		  switch (nzeros)
		    {
		    case 0:
		      if( lastdelta == -999 ) 
			{
			  if( logLevel >= 2 ){
			    msg_err<<"Fatal decoding error: no previous delta found."<<endl;
			    msg_err<<"Bytes decoded so far "<<buf - start<<endl;
			  }
			  return adc;
			}
		      addnew = nseqrep;
		      break;
		      
		    case 1:
		      lastdelta = 0;
		      addnew    = 1;
		      break;
		      
		    case 2:
		      lastdelta = -1;
		      addnew    = 1;
		      break;
		      
		    case 3:
		      lastdelta = 1;
		      addnew    = 1;
		      break;

		    case 4:
		      lastdelta = -2;
		      addnew    = 1;
		      break;

		    case 5:
		      lastdelta = 2;
		      addnew    = 1;
		      break;

		    case 6:
		      lastdelta = -3;
		      addnew    = 1;
		      break;

		    case 7:
		      lastdelta = 3;
		      addnew    = 1;
		      break;

		    default: 
		      break;		      
		    }
		  
		  for(int i=0;i<addnew;i++ )
		    {
		      uint16_t val = adc.back() + lastdelta;
		      adc.push_back( val );
		    }
		  
		  // decoded the value
		  if( addnew ) nzeros = 0;
		}
	      else  // increment bit code otherwise
		nzeros++;
	      
	      // increment counters
	      bitsread++;
	      bitbytep--;
	      

	      // a given sequence should be zero-padded 
	      // to nearest byte boundary we want to skip these zeros
	      // move to next byte
	      newseq = (adc.size() % seqlen == 0);
	      
	      if( newseq ) break;
	      
	      if(bitbytep == 0 || newseq)
		{ buf++; bitbytep = 8; }
	      if( buf > stop ) break; // finished
	      
	    }// end while
	  
	  
	  // read out all bits in this compressed packet
	  bitsread = 0;
	}
      
      // finished new sequance
      if( newseq )
	{
	  // number of bytes decompressed so far
	  //size_t nb_rd   = (buf - start + 1);
	  //msg_info<<"    Bytes read / declared "<<nb_rd<<" / "<<nbytes<<endl;
	  
	  // break after decoding the channel sequence
	  break;
	}
      
    } // end while loop over buffer

  
  // that is it
  return adc;
}
