#ifndef __HUFFMANDECOMPRESSOR_H__
#define __HUFFMANDECOMPRESSOR_H__

#include <vector>
#include <utility>

#include "DaqCommon.h"
#include "DaqEvent.h"

using std::pair;
using std::vector;

namespace np02daq
{
  //
  void HuffDecompressor( const void *in, const unsigned nb, const unsigned nsa, adcdata_t &data, unsigned fragid );
  vector< pair<unsigned, unsigned> > HuffDecompressorChanTable( const BYTE *buf, const unsigned nb );
  //adcdata_t HuffDecompressorChanData( const BYTE *buf, const unsigned nbytes, const unsigned seqlen );
  adcdata_t HuffDecompressorChanData( const vector<BYTE> bytes, const unsigned seqlen, int logLevel ); 
}

#endif
