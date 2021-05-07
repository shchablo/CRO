#ifndef __DAQCOMMON_H__
#define __DAQCOMMON_H__

#include <exception>
#include <vector>
#include <string>
#include <ctime>
#include <stdint.h>

// bit check
#define SETBYTEBIT(var, pos) ( var |= 1 << pos )
#define CLEARBYTEBIT(var, pos) ( var &= ~(1 << pos) )
#define CHECKBYTEBIT(var, pos) ( (var) & (1<<pos) )

// flag bit for compression
#define DCBITFLAG 0x0 // 0x0 LSB -> 0x7 MSB
#define GETDCFLAG(info) (CHECKBYTEBIT(info, DCBITFLAG)>0)
#define SETDCFLAG(info) (SETBYTEBIT(info, DCBITFLAG))

// event data quality flag
#define EVCARD0 0x19   // non instrumented cards
#define EVDQFLAG(info) ( (info & 0x3F ) == EVCARD0 )

namespace np02daq
{
  typedef char BYTE;
  
  static const size_t nsacro = 10000; // total number of samples per CRO ch
  static const unsigned evskey = 0xFF;
  static const unsigned endkey = 0xF0;

  
  // get byte content for a given data type
  // NOTE: cast assumes host byte order
  template<typename T> T ConvertToValue(const void *in)
    {
      const T *ptr = static_cast<const T*>(in);
      return *ptr;
    }
};

#endif
