#include "ChannelMap.h"
#include "LogMsg.h"
#include "tde_cmap_utils.h"

#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>

#include <iomanip>
#include <utility>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace np02daq;

//
template<typename T>
void unpackCMapOpts( const string stropt, std::vector<T> &opts ){
  if( stropt.empty() ) return;
  
  string str1 = stropt;
  string str2 = "";
  auto ipos = stropt.find(":");
  if( ipos != string::npos ){
    str1 = stropt.substr(0, ipos);
    str2 = stropt.substr(ipos + 1);
  }
   
  std::stringstream ss(str1);
  T tmp;
  if( (ss >> tmp).fail() ){
    cout<<"could not parse the option\n";
  }
  else {
    opts.push_back( tmp );
  }
  if( !str2.empty() ) 
    unpackCMapOpts<T>( str2, opts );
  
  return;
}

//
void ChannelMap::init( std::string mapname )//, unsigned ncrates,
//		       unsigned ncards, unsigned nviews )
{
  // already defined?
  if( mapname_.compare( mapname ) == 0 )
    {
      //cout<<"nothing to do"<<endl;
      return;
    }
  
  clear();
  mapname_ = mapname;
  if(mapname.compare("pddp2crp") == 0) {
    pddp2crpMap();
  }
  else if(mapname.compare("vdcblcrp") == 0) {
    vdcb1crpMap();
  }
  else {
      unsigned ncrates = 1;
      unsigned ncards  = 10;
      unsigned nviews  = 1;
      int counts = std::count(mapname_.begin(), mapname_.end(), ':');
      if( counts <= 0 || counts > 2 ) {
        msg_warn << "Could not interpret the string '" << mapname_ << "'" << endl;
      }
      else {
        vector<unsigned> iopts;
        unpackCMapOpts<unsigned>( mapname_, iopts );
        for( unsigned i = 0; i < iopts.size(); ++i) {
	        if( i == 0 ) ncrates = iopts[i];
	        else if( i == 1 ) ncards = iopts[i];
	        else if( i == 2 ) nviews = iopts[i];
	      }
      }
      msg_info << "Simple channel map with number of crates / cards per crate / views : "
	            << ncrates << " / " << ncards << " / " << nviews << endl;
      
      simpleMap( ncrates, ncards, nviews );
    }
}


// simple channel map
void ChannelMap::simpleMap( unsigned ncrates, unsigned ncards, unsigned nviews )
{
  unsigned nctot  = ncards * ncrates; // total number of cards
  unsigned ncview = nctot / nviews;   // allocate the same for each view
  unsigned nch    = nch_;             // number of ch per card (fixed)
  
  unsigned seqn  = 0;
  unsigned crate = 0;
  unsigned crp   = 0;
  unsigned view  = 0;
  unsigned vch   = 0;
  for( unsigned card = 0; card < nctot; card++ )
    {
      if( card > 0 ) 
	{
	  if( card % ncards == 0 ) crate++;
	  if( card % ncview == 0 ) {view++; vch=0;}
	}
      for( unsigned ch = 0; ch < nch; ch++ )
	{
	  add( seqn++, crate, card % ncards, ch, crp, view, vch++);
	  //cout<<seqn<<" "<<crate<<" "<<card%ncards
	}
    }
  //
}


//
void ChannelMap::pddp2crpMap()
{
  // all idices run from 0
  vector<unsigned> cards_per_crate_real{5, 5, 5, 10, 5, 5, 10, 10, 10, 5};
  //unsigned ncrates = cards_per_crate.size();
  unsigned ncrates = 12;
  vector<unsigned> cards_per_crate(ncrates, 10);
  unsigned nch  = 64;
  unsigned nchc = nch/2;  // logical data channels per KEL/VHDCI connector 
  unsigned ncrp = 4;
  vector<unsigned> crpv(2*ncrp, 0);

  // all connector mappings should include ADC channel inversion on AMC 
  // the inversion is in group of 8ch: AMC ch 0 -> 7 should be remapped to 7 -> 0
  // these connector mappings should be (re)generated with the python script card2crp.py
  
  // kel connector orientation in a given view, chans 0 -> 31
  vector<unsigned> kel_nor = { 7,  6,  5,  4,  3,  2,  1,  0, 15, 14, 13, 12, 11, 10,  9,  8, 23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24 };
  // kel connector orientation in a given view, chans 31 -> 0
  vector<unsigned> kel_inv = {24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23,  8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7 };
  
  //
  unsigned seqn = 0;
  unsigned crp, view;
  for(unsigned crate = 0;crate<ncrates;crate++)
    {
      // number of cards in this crate
      unsigned ncards = cards_per_crate[ crate ];
      for( unsigned card = 0;card<ncards;card++ )
	{
	  // which view ?
	  if( crate < 6 ) view = 0;
	  else view = 1;
	  
	  // which CRP ?
	  if( crate < 3 ) 
	    {
	      crp = 1;
	      if( card >= 5 ) crp = 2;
	    }
	  else if( crate >= 3 && crate < 6 )
	    {
	      crp = 0;
	      if( card >= 5 ) crp = 3;
	    }
	  else if( crate >=6 && crate < 9 )
	    {
	      crp  = 0;
	      if( card >= 5 ) crp = 1;
	    }
	  else
	    {
	      crp = 3;
	      if( card >= 5 ) crp = 2;
	    }
	  
	  
	  // get iterator to view channel numbering
	  auto kel = kel_inv; // inverted order
	  bool topAmcFirst = true;
	  if( view == 0 )
	    {
	      // order 31 -> 0 for these KEL connectors
	      kel = kel_nor; //normal order
	      topAmcFirst = false;
	    }

	  auto vchIt      = crpv.begin() + view * ncrp + crp;

	  if( topAmcFirst ) // bot connector goes second
	    *vchIt = *vchIt + nchc;
	  
	  unsigned cstart = 0; // bottom AMC connector
	  for( unsigned ch = cstart; ch<cstart+nchc; ch++ )
	    {
	      int idx = ch - cstart;
	      if( idx < 0 || idx >= (int)kel.size() )
		{
		  // should not happen
		  cerr<<"I screwed this up\n";
		  continue;
		}
	      // view channel
	      unsigned vch = *vchIt + kel[ idx ];
	      if( view == 1 )
		{
		  int ival = -vch + 959;
		  if( ival < 0 )
		    {
		      cerr<<"Bad view channel number\n";
		      continue;
		    }
		  vch = (unsigned)ival;
		}
	      
	      // check if the channel actually exits
	      // since l1evb writes in the same format
	      unsigned state = 0;
	      if( crate >= cards_per_crate_real.size() )
		{
		  state = 1;
		}
	      else
		{
		  if( card >= cards_per_crate_real[ crate ] )
		    state = 1;
		}

	      add( seqn++, crate, card, ch, crp, view, vch, state );
	    }

	  if( topAmcFirst )  // first half: top  connector is first
	    *vchIt = *vchIt - nchc;
	  else
	    *vchIt = *vchIt + nchc;
	  
	  // move to the second connector
	  cstart = nchc; // top amc connector
	  for( unsigned ch = cstart; ch<cstart+nchc; ch++ )
	    {
	      int idx = ch - cstart;
	      if( idx < 0 || idx >= (int)kel.size() )
		{
		  // should not happen
		  cerr<<"I screwed this up\n";
		  continue;
		}
	      // view channel
	      unsigned vch = *vchIt + kel[ idx ];
	      if( view == 1 )
		{
		  int ival = -vch + 959;
		  if( ival < 0 )
		    {
		      cerr<<"Bad view channel number\n";
		      continue;
		    }
		  vch = (unsigned)ival;
		}
	      
	      // check if the channel actually exits
	      // since l1evb writes in the same format
	      unsigned state = 0;
	      if( crate >= cards_per_crate_real.size() )
		{
		  state = 1;
		}
	      else
		{
		  if( card >= cards_per_crate_real[ crate ] )
		    state = 1;
		}

	      add( seqn++, crate, card, ch, crp, view, vch, state );
	    }
	  
	  // all done
	  if( topAmcFirst )
	    *vchIt = *vchIt + nch;
	  else
	    *vchIt = *vchIt + nchc;
	  
	  //
	}
    }
  //for( auto i: crpv ){ cout<<i<<" "; }
  //cout<<'\n';
}


// 
//
boost::optional<ChannelId> ChannelMap::find_by_seqn( unsigned seqn ) const
{
  auto it = chanTable.get<IndexRawSeqnHash>().find( seqn );
  if( it != chanTable.get<IndexRawSeqnHash>().end() )
    return *it;
  
  return boost::optional<ChannelId>();
}

//
// the most low level info
std::vector<ChannelId> ChannelMap::find_by_seqn( unsigned from, unsigned to ) const
{
  if( to < from ) std::swap( from, to );
  
  std::vector<ChannelId> res;
  
  if( from == to )
    {
      if( boost::optional<ChannelId> id = find_by_seqn( from ) ) 
	res.push_back( *id );
      //auto it = chanTable.find(from);
      //if( it != chanTable.end() )
      //res.push_back( *it );
    }
  else
    {
      auto first = chanTable.get<IndexRawSeqn>().lower_bound( from );
      auto last  = chanTable.get<IndexRawSeqn>().upper_bound( to );
      res.insert( res.begin(), first, last );
    }
  
  return res;
}

//
//
std::vector<ChannelId> ChannelMap::find_by_crate( unsigned crate, bool ordered ) const
{
  if( not ordered ) // get from hashed index
    {
      const auto r = chanTable.get<IndexCrate>().equal_range( crate );
      std::vector<ChannelId> res(r.first, r.second);
      return filter( res );
    }

  const auto r = chanTable.get<IndexCrateCardChan>().equal_range( boost::make_tuple(crate) );
  std::vector<ChannelId> res(r.first, r.second);

  return filter( res );
}

//
//
std::vector<ChannelId> ChannelMap::find_by_crate_card( unsigned crate, unsigned card, bool ordered ) const
{
  if( not ordered ) // get from hashed index
    {
      const auto r = chanTable.get<IndexCrateCard>().equal_range( boost::make_tuple(crate, card) );
      std::vector<ChannelId> res(r.first, r.second);
      return filter( res );
    }
  
  // ordered accodring to channel number
  const auto r = chanTable.get<IndexCrateCardChan>().equal_range( boost::make_tuple( crate, card) );
  std::vector<ChannelId> res(r.first, r.second);
  
  return filter( res );
}

//
//
boost::optional<ChannelId> ChannelMap::find_by_crate_card_chan( unsigned crate,
								unsigned card, unsigned chan ) const
{
  auto it = chanTable.get<IndexCrateCardChanHash>().find( boost::make_tuple(crate, card, chan) );
  if( it != chanTable.get<IndexCrateCardChanHash>().end() )
    return *it;
  
  return boost::optional<ChannelId>();
}

//
//
std::vector<ChannelId> ChannelMap::find_by_crp( unsigned crp, bool ordered ) const
{
  if( not ordered ) // get from hashed index
    {
      const auto r = chanTable.get<IndexCrp>().equal_range( crp );
      std::vector<ChannelId> res(r.first, r.second);
      return filter( res );
    }

  const auto r = chanTable.get<IndexCrpViewChan>().equal_range( crp );
  std::vector<ChannelId> res(r.first, r.second);
  return filter( res );
}

//
//
std::vector<ChannelId> ChannelMap::find_by_crp_view( unsigned crp, unsigned view, bool ordered ) const
{
  if( not ordered ) // get from hashed index
    {
      const auto r = chanTable.get<IndexCrpView>().equal_range( boost::make_tuple(crp, view) );
      std::vector<ChannelId> res(r.first, r.second);
      return filter( res );
    }
  
  // ordered accodring to channel number
  const auto r = chanTable.get<IndexCrpViewChan>().equal_range( boost::make_tuple(crp, view) );
  std::vector<ChannelId> res(r.first, r.second);
  return filter( res );
}

//
//
boost::optional<ChannelId> ChannelMap::find_by_crp_view_chan( unsigned crp,
							      unsigned view, unsigned chan ) const
{
  auto it = chanTable.get<IndexCrpViewChanHash>().find( boost::make_tuple(crp, view, chan) );
  if( it != chanTable.get<IndexCrpViewChanHash>().end() )
    return *it;
  
  return boost::optional<ChannelId>();
}
  
//
// add
void ChannelMap::add( unsigned seq, unsigned crate, unsigned card, unsigned cch,
		      unsigned crp, unsigned view, unsigned vch, unsigned short state )
{
  
  // for debug ...
  bool ok = true;
  if( boost::optional<ChannelId> id = find_by_seqn( seq ) )
    {
      cerr<<"arealdy have added "<<seq<<" sequence number\n"; ok = false; 
    }
  if( boost::optional<ChannelId> id = find_by_crate_card_chan( crate, card, cch ) ) 
    {
      cerr<<"arealdy have added this crate/card/ch "<<crate<<"/"<<card<<"/"<<cch<<"\n"; ok = false;
    }
  if( boost::optional<ChannelId> id = find_by_crp_view_chan( crp, view, vch ) ) 
    {
      cerr<<"arealdy have added this crp/view/ch "<<crp<<"/"<<view<<"/"<<vch<<"\n"; ok = false;
    }
  if( !ok ) return;
  //*/

  chanTable.insert( ChannelId(seq, crate, card, cch, crp, view, vch, state) );
  //
  ntot_    = chanTable.size();

  if( state == 0 )
    {
      crateidx_.insert( crate );
      ncrates_ = crateidx_.size();
      
      crpidx_.insert( crp );
      ncrps_ = crpidx_.size();
    }
  
  //
  //ncrates_ = cdistinct( chanTable.get<IndexCrateCardChan>(), 
  //[](const ChannelId& c){ return c.crate();} );
  
  //
  //ncrps_   = cdistinct( chanTable.get<IndexCrpViewChan>(), 
  //[](const ChannelId& c){ return c.crp();} );
}
  
//
// clear
void ChannelMap::clear()
{
  ChannelTable().swap( chanTable );
  ncrates_ = 0;
  ncrps_   = 0;
  ntot_    = 0;
  mapname_ = "";
  crateidx_.clear();
  crpidx_.clear();

}

//
// MapToCRP
int ChannelMap::MapToCRP(int seqch, int &crp, int &view, int &chv) const
{
  crp = view = chv = -1;
  if( boost::optional<ChannelId> id = find_by_seqn( (unsigned)seqch ) ) 
    {
      crp  = id->crp();
      view = id->view();
      chv  = id->viewch();
      return 1;
    }
  return -1;
}

//
// number of cards assigned to a given crate
unsigned ChannelMap::ncards( unsigned crate ) const
{
  // assumes it is sorted according to card number
  unsigned count  = 0;
  auto r = chanTable.get<IndexCrateCardChan>().equal_range( crate );
  ssize_t last    = -1;
  for( ChannelId const &ch : boost::make_iterator_range( r ) )
    {
      if( !ch.exists() ) continue;
      unsigned val = ch.card();
      if( val != last ) 
	{
	  count++;
	  last = val;
	}
    }
  
  return count;
}

// number of views assigned to a given CRP
unsigned ChannelMap::nviews( unsigned crp ) const 
{
  // assumes it is sorted according to crp number
  unsigned count  = 0;
  auto r = chanTable.get<IndexCrpViewChan>().equal_range( crp );
  ssize_t last    = -1;
  for( ChannelId const &ch : boost::make_iterator_range( r ) )
    {
      if( !ch.exists() ) continue;
      unsigned val = ch.view();
      if( val != last ) 
	{
	  count++;
	  last = val;
	}
    }
  
  return count;
}

//
void ChannelMap::print( std::vector<ChannelId> &vec )
{
  for( auto it = vec.begin();it!=vec.end();++it )
    {
      unsigned seqn   = it->seqn();
      unsigned crate  = it->crate();
      unsigned card   = it->card();
      unsigned cch    = it->cardch();
      unsigned crp    = it->crp();
      unsigned view   = it->view();
      unsigned vch    = it->viewch();
      unsigned state  = it->state();
      bool     exists = it->exists();
      cout<<setw(7)<<seqn
	  <<setw(4)<<crate
	  <<setw(3)<<card
	  <<setw(3)<<cch
	  <<setw(3)<<crp
	  <<setw(2)<<view
	  <<setw(4)<<vch
	  <<setw(2)<<state
	  <<setw(2)<<exists<<endl;
    }
}


// CRP for 1st coldbox test
void ChannelMap::vdcb1crpMap(){

  //int ncrates = 3;
  int nslots  = 10;
  int nview   = 3;

  // utca crate 1
  tde::crate c1( 0, nslots );
  vector<int> c1_kel{34, 36, 35, 38, 37, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49};
  c1.add_crp_connection( 0, 0, c1_kel );

  // utca crate 2
  tde::crate c2( 1, nslots );
  vector<int> c2_kel{14, 16, 15, 18, 17, 19, 20, 22, 21, 25, 23, 24, 26, 27, 28, 32, 29, 30, 31, 33};
  c2.add_crp_connection( 0, 0, c2_kel );

  // utca crate 3
  tde::crate c3( 2, nslots );
  vector<int> c3_kel{0, 2, 1, 5, 3, 4, 6, 7, 8, 12, 9, 10, 11, 13};
  c3.add_crp_connection( 0, 0, c3_kel );
  
  vector<tde::crate> crates{c1, c2, c3};

  // all connector mappings should include ADC channel inversion on AMCs 
  // the inversion is in group of 8ch: AMC ch 0 -> 7 should be remapped to 7 -> 0
  
  // kel connector orientation in a given view, chans 0 -> 31
  vector<unsigned> kel_nor = { 7,  6,  5,  4,  3,  2,  1,  0, 15, 14, 13, 12, 11, 10,  9,
			       8, 23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24 };
  // kel connector orientation in a given view, chans 31 -> 0
  vector<unsigned> kel_inv = {24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22,
			      23,  8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7 };

  
  // kel connectors for each view sorted in the view channel order
  // induction 1
  vector<int> kel_view0{1,5,8,12,15,18,21,25,28,32,35,38};
  std::reverse(kel_view0.begin(), kel_view0.end());
  // induction 2
  vector<int> kel_view1{40,41,42,43,44,45,46,47,48,49, 13,11,10,9,7,6,4,3,2,0};
  // collection
  vector<int> kel_view2{14,16,17,19,20,22,23,24,26,27,29,30,31,33,34,36,37,39};
  std::reverse(kel_view2.begin(), kel_view2.end());

  
  tde::crp_connectors crp_conn( 0, nview );
  int ch_start = 0;
  for( auto const k : kel_view0 ){
    crp_conn.add_connector( k, 0, false, ch_start );
    ch_start += tde::ch_per_kel;
  }
  ch_start = 0;
  for( auto const k : kel_view1 ){
    //bool reverse = (k <= 13);
    bool reverse = (k > 13);
    crp_conn.add_connector( k, 1, reverse, ch_start );
    ch_start += tde::ch_per_kel;
  }

  ch_start = 0;
  for( auto const k : kel_view2 ){
    crp_conn.add_connector( k, 2, false, ch_start );
    ch_start += tde::ch_per_kel;
  }

  // only one crp
  unsigned crp_id  = 0;
  unsigned seqn    = 0;

  // map the existing DAQ channels to 4th view
  // not connected view id
  unsigned view_na    = (unsigned)nview;
  unsigned view_na_ch = 0;

  for( auto const &utca : crates ) {
    unsigned utca_id = (unsigned)utca._id;
    //if( utca_id <= 1 ) continue;
    auto utca_conn   = utca._crp_conn;
    auto utca_nconn  = utca_conn.size();
    auto utca_slots  = (unsigned)utca._cards;
    for( unsigned amc = 0; amc < utca_slots; ++amc ){
      // unconnected AMCs
      if( amc >= utca_nconn ){
	for( unsigned cardch = 0; cardch < tde::ch_per_amc; ++cardch ){
	  add( seqn++, utca_id, amc, cardch, crp_id, view_na, view_na_ch++, 0);
	}
	continue;
      }
      
      // connected to CRU
      auto conn = utca_conn[ amc ];
      unsigned islot = (unsigned)std::get<0>(conn);
      //unsigned icrp  = (unsigned)std::get<1>(conn);
      auto kel1_id  = std::get<2>(conn);
      auto kel2_id  = std::get<3>(conn);
      if( islot != amc ){
	cerr<<"Mismatch in slot and AMC index. Should not happen\n";
	amc = islot;
      }

      // 1st connector
      if( kel1_id < (int)crp_conn._kels.size() ){
	auto kel_ = std::next(crp_conn._kels.begin(), kel1_id);
	if( kel_->_id != kel1_id ) {
	  cerr <<"Mismatch in KEL numbering\n"; continue;
	}
	
	vector<unsigned> order_ = (kel_->_reverse)? kel_inv : kel_nor;
	unsigned iview     = (unsigned)kel_->_view;
	unsigned vch_start = (unsigned)kel_->_first_view_ch;
	//cout<<" amc "<<amc<<"    KEL connectors -> "<<kel1_id<<" "<<kel2_id<<" "<<iview<<" "<<vch_start<<endl;    
	for( unsigned cardch = 0; cardch < tde::ch_per_kel; ++cardch ){
	  unsigned viewch = vch_start + order_[cardch];
	  add( seqn++, utca_id, amc, cardch, crp_id, iview, viewch, 0);
	}
      } // 1st connector
      
      // 2nd connector
      if( kel2_id < (int)crp_conn._kels.size() ){
	auto kel_ = std::next(crp_conn._kels.begin(), kel2_id);
	if( kel_->_id != kel2_id ) {
	  cerr <<"Mismatch in KEL numbering\n"; continue;
	}
	
	vector<unsigned> order_ = (kel_->_reverse)? kel_inv : kel_nor;
	unsigned iview     = (unsigned)kel_->_view;
	unsigned vch_start = (unsigned)kel_->_first_view_ch;
	//cout<<" amc "<<amc<<"    KEL connectors "<<kel1_id<<" -> "<<kel2_id<<" "<<iview<<" "<<vch_start<<endl; 
	for( unsigned cardch = 0; cardch < tde::ch_per_kel; ++cardch ){
	  unsigned viewch = vch_start + order_[cardch];
	  add( seqn++, utca_id, amc, cardch + tde::ch_per_kel, crp_id, iview, viewch, 0);
	}
      }// 2nd connector
    } // loop over AMCs
  } // loop over crates

  //cout<<" seqn = "<<seqn<<endl;
  // that is it...
}
