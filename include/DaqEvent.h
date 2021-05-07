#ifndef __DAQEVENT_H__
#define __DAQEVENT_H__

#include <ctime>
#include <vector>
#include <stdint.h>

namespace np02daq
{
  typedef struct timespec trigstamp_t; // time_t tv_sec, long tv_nsec
  typedef std::vector<uint16_t> adcdata_t;
  typedef std::vector<uint8_t>  eflags_t;
  
  struct DaqEvent
  {
    // event has been correctly loaded
    bool valid;

    // global event quality flag
    bool good;
    
    // run info
    uint32_t runnum;       // run number
    uint8_t  runflags;
    
    // event info
    uint32_t evnum;
    eflags_t evflags;
    //uint32_t evszcro;
    //uint32_t evszlro;
    
    // trigger info
    uint8_t     trigtype;
    uint32_t    trignum;
    trigstamp_t trigstamp;
    
    // number of ADC samples per CRO ch
    unsigned nsacro;
    
    // number of ADC samples per LRO ch
    unsigned nsalro;
    
    // unpacked CRO ADC data
    adcdata_t crodata;

    // unpacked CRO ADC data
    adcdata_t lrodata;
      
    // number of decoded channels
    unsigned chcro() const { return nsacro!=0?crodata.size()/nsacro:0; }
    
    //
    void clear()
    {
      valid    = false;
      good     = false;
      runnum   = 0;
      runflags = 0; 
      evnum    = 0;
      //evszcro  = 0;
      //evszlro  = 0;
      evflags.clear();
      trigtype = 0;
      trignum  = 0;
      crodata.clear();
    }
  };
};


#endif
