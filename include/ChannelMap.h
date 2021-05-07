//////////////////////////////////////////////////////////////////////////////////
//
//  Classes to facility channel order translation between different 
//  representations
// 
//  seqn   - unique counter to give channel data position in the record
//  crate  - utca crate number starting
//  card   - AMC card number
//  cardch - AMC card channel number 
//  crp    - CRP number
//  view   - view number (0/1)
//  viewch - view channel number 
// 
//  Boost multi_index_container provides interface to search and order various 
//  indicies. The container structure is defined in ChannelTable, which provides
//  the following interfaces:
//   - via raw sequence index, tag IndexRawSeqn
//   - via crate number, tag IndexCrate, to get all channels to a given crate
//   - via crate number & card number, tag IndexCrateCard, 
//     to get all channels for a given card in a crate
//   - via crate, card, and channel number, tag IndexCrateCardChan, to access
//     a given channel of a given card in a given crate
//   - via CRP index, tag IndexCrp, to access channels assigned to a given CRP
//   - via CRP & view index, tag IndexCrpView, to access channels assigned to 
//     a given view in a specified CRP
//   - via CRP, view, and channel number, tag IndexCrpViewChan, to access 
//     a given view channel in a given CRP
//
//  TODO add composite key to facilitate selecting only real channels
//       i.e., get rid of the fake data inserted by l1evb
//
//  Created: VG, Fri Mar 15 14:03:49 CET 2019
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __CHAN_MAP_H__
#define __CHAN_MAP_H__

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/optional.hpp>

#include <set>
#include <vector>
#include <string>
#include <sstream>

using namespace boost::multi_index;

namespace np02daq
{

  // container to store various indicies
  class ChannelId
  {
  private:
    unsigned seqn_;
    unsigned short crate_, card_, cardch_;
    unsigned short crp_, view_, viewch_;
    //bool exists_;
    unsigned short state_;

  public:
    ChannelId() {;}
    ChannelId( unsigned seqn, 
	       unsigned short crate, unsigned short card, unsigned short cardch, 
	       unsigned short crp, unsigned short view, unsigned short viewch, 
	       unsigned short state = 0 ) : 
    seqn_(seqn), crate_(crate), card_(card), cardch_(cardch), crp_(crp), view_(view), viewch_(viewch), state_(state) {}
    
    bool operator<(const ChannelId &rhs) const { return seqn_ < rhs.seqn_; }
    
    const unsigned seqn() const { return seqn_; }
    const unsigned short crate() const { return crate_; }
    const unsigned short card() const { return card_; }
    const unsigned short cardch() const { return cardch_; }
    const unsigned short crp() const { return crp_; }
    const unsigned short view() const { return view_; }
    const unsigned short viewch() const { return viewch_; }
    const unsigned short state() const { return state_; }
    const bool exists() const { return (state_ == 0); }
    
    
    //
    std::string info() const
      {
	std::stringstream ss;
	ss<<seqn_<<" "
	  <<crate_<<" "
	  <<card_<<" "
	  <<cardch_<<" "
	  <<crp_<<" "
	  <<view_<<" "
	  <<viewch_;
	return ss.str();	  
      }
    
  };

  // container tags 
  struct IndexRawSeqn{};
  struct IndexRawSeqnHash{};   // hashed todo check which is faster...
  struct IndexCrate{};         // hashed
  struct IndexCrateCard{};     // hashed
  struct IndexCrateCardChan{}; // ordered
  struct IndexCrateCardChanHash{}; 
  struct IndexCrp{};           // hashed
  struct IndexCrpView{};       // hashed 
  struct IndexCrpViewChan{};   // ordered
  struct IndexCrpViewChanHash{};
  
  // boost multi index container
  typedef multi_index_container<
    ChannelId,
    indexed_by<
    ordered_unique< tag<IndexRawSeqn>, const_mem_fun< ChannelId, const unsigned, &ChannelId::seqn > >,
    hashed_unique< tag<IndexRawSeqnHash>, const_mem_fun< ChannelId, const unsigned, &ChannelId::seqn > >,
    hashed_non_unique< tag<IndexCrate>, const_mem_fun< ChannelId, const unsigned short, &ChannelId::crate > >,
    hashed_non_unique< tag<IndexCrateCard>, composite_key< ChannelId, const_mem_fun< ChannelId, const unsigned short, &ChannelId::crate >, const_mem_fun< ChannelId, const unsigned short, &ChannelId::card > > >,
    ordered_unique< tag<IndexCrateCardChan>, composite_key< ChannelId, const_mem_fun< ChannelId, const unsigned short, &ChannelId::crate >, const_mem_fun< ChannelId, const unsigned short, &ChannelId::card >, const_mem_fun< ChannelId, const unsigned short, &ChannelId::cardch > > >,
    hashed_unique< tag<IndexCrateCardChanHash>, composite_key< ChannelId, const_mem_fun< ChannelId, const unsigned short, &ChannelId::crate >, const_mem_fun< ChannelId, const unsigned short, &ChannelId::card >, const_mem_fun< ChannelId, const unsigned short, &ChannelId::cardch > > >,
    hashed_non_unique< tag<IndexCrp>, const_mem_fun< ChannelId, const unsigned short, &ChannelId::crp > >,
    hashed_non_unique< tag<IndexCrpView>, composite_key< ChannelId, const_mem_fun< ChannelId, const unsigned short, &ChannelId::crp >, const_mem_fun< ChannelId, const unsigned short, &ChannelId::view > > >,
    ordered_unique< tag<IndexCrpViewChan>, composite_key< ChannelId, const_mem_fun< ChannelId, const unsigned short, &ChannelId::crp >, const_mem_fun< ChannelId, const unsigned short, &ChannelId::view >, const_mem_fun< ChannelId, const unsigned short, &ChannelId::viewch > > >,
    hashed_unique< tag<IndexCrpViewChanHash>, composite_key< ChannelId, const_mem_fun< ChannelId, const unsigned short, &ChannelId::crp >, const_mem_fun< ChannelId, const unsigned short, &ChannelId::view >, const_mem_fun< ChannelId, const unsigned short, &ChannelId::viewch > > >
    >
    > ChannelTable; //
  
  //
  //
  //
  class ChannelMap
  {
  public:
    static ChannelMap& Instance()
    {
      static ChannelMap inst;
      return inst;
    }
    
    std::string getMapName() const { return mapname_; }

    void init( std::string mapname );
    //, unsigned ncrates = 1, 
    //unsigned ncards = 10, unsigned nviews = 1 );
    
    // legacy function 
    int MapToCRP(int seqch, int &crp, int &view, int &chv) const;
    
    //                          //
    // retrive various map data //
    //                          //
    
    // from seq num in raw data file
    boost::optional<ChannelId> find_by_seqn( unsigned seqn ) const;
    std::vector<ChannelId> find_by_seqn( unsigned from, unsigned to ) const;

    // from crate info
    std::vector<ChannelId> find_by_crate( unsigned crate, bool ordered = true ) const;
    std::vector<ChannelId> find_by_crate_card( unsigned crate, unsigned card, 
					       bool ordered = true ) const;
    boost::optional<ChannelId> find_by_crate_card_chan( unsigned crate, 
							unsigned card, unsigned chan ) const;
    // from CRP info
    std::vector<ChannelId> find_by_crp( unsigned crp, bool ordered = true ) const;
    std::vector<ChannelId> find_by_crp_view( unsigned crp, unsigned view, bool ordered = true ) const;
    boost::optional<ChannelId> find_by_crp_view_chan( unsigned crp,
						      unsigned view, unsigned chan ) const;
    
    unsigned ncrates() const { return ncrates_; }
    unsigned ncrps() const { return ncrps_; }
    unsigned ntot() const { return ntot_; }
    unsigned ncards( unsigned crate ) const;
    unsigned nviews( unsigned crp ) const;
    
    //
    void clear(); 
    void print( std::vector<ChannelId> &vec );

    std::set< unsigned > get_crateidx(){ return crateidx_; }
    std::set< unsigned > get_crpidx(){ return crpidx_; }

  protected:
    //
    void add( unsigned seq, unsigned crate, unsigned card, unsigned cch,
	      unsigned crp, unsigned view, unsigned vch, unsigned short state = 0);
    
    template<typename Index,typename KeyExtractor>
      std::size_t cdistinct(const Index& i, KeyExtractor key)
    {
      std::size_t res = 0;
      for(auto it=i.begin(),it_end=i.end();it!=it_end;)
	{
	  ++res;
	  it = i.upper_bound( key(*it) );
	}
      return res;
    }
    
    ChannelTable chanTable;
    
    std::string mapname_;
    std::set< unsigned > crateidx_;
    std::set< unsigned > crpidx_;
    unsigned ncrates_;
    unsigned ncrps_;
    unsigned ntot_;
    unsigned nch_;

  private:
    ChannelMap()
      {
	mapname_  = ""; 
	ntot_    = 0;
	ncrates_ = 0;
	ncrps_   = 0;
	nch_     = 64;
      }
    
    ChannelMap(const ChannelMap&);
    ChannelMap& operator=(const ChannelMap&);

    // Prevent unwanted destruction
    ~ChannelMap(){;}

    // simple map
    void simpleMap( unsigned ncrates, unsigned ncards, unsigned nviews );

    // ProtoDUNE DP map with 2 CRPs + 2 x 2 anodes
    void pddp2crpMap();

    // ProtoDUNE DP map with 4 CRPs instrumented
    void pddp4crpMap();

    // Just remove by hand channels which do not exists here 
    // since I do not want to be bothered anymore with this ...
    std::vector<ChannelId> filter( std::vector<ChannelId> &sel ) const
      {
	std::vector<ChannelId> res;
	for( auto &c : sel )
	  if( c.exists() ) res.push_back( c );
	return res;
      }
    
  };
  
};

#endif
