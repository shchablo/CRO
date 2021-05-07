//////////////////////////////////////////////////////////////////////////////////////
//
//  Decoder for the DAQ data for ProtoDUNE-DP
// 
//
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __EVENTDECODER_H__
#define __EVENTDECODER_H__

// for compressed data
// #include "HuffDataCompressor.h"
#include <iostream>
#include <fstream>

#include "DaqCommon.h"
#include "DaqEvent.h"


namespace np02daq
{
  class formatexception : public std::exception
  {
    virtual const char* what() const throw()
    {
      return "Bad file format";
    }
  };
  static formatexception fex;

  //
  class EventDecoder
  {
  public: 
    EventDecoder();
    ~EventDecoder();

    // open input file
    ssize_t Open(std::string finname);
    void Close();
    
    //
    size_t Nev() const { return m_nev; }
    
    //
    const DaqEvent& Event(){ return m_event; }
    
    const DaqEvent* EventPtr() { return &m_event; }

    // get event from file
    const DaqEvent& Get( size_t evid );

    // get event from buffer
    const DaqEvent& Get( std::vector<BYTE> &buf );
    
    
  protected:

    // our unpacked event structure
    DaqEvent m_event;
    ssize_t  m_evid;
    
    // read a chunk of binary data
    void readChunk( std::vector<BYTE> &bytes, size_t sz );
    bool unpackEvent( std::vector<BYTE> &buf );
    // file locations
    std::vector<std::streampos> m_events;
    std::vector<uint32_t> m_evsz;
    
    // total number of events in the file
    size_t m_nev;
    
    // input file
    size_t m_filesz;
    std::ifstream m_file;

    // header sizes
    enum { evinfoSz = 44 };
    
    // trigger structure from WR trig handler
    typedef struct triginfo_t
    {
      uint8_t type;
      uint32_t num;
      struct timespec ts; //{ time_t ts.tv_sec, long tv_nsec }
    } triginfo_t;
    
    // strucutre to hold decoded event header
    typedef struct eveinfo_t
    {
      uint32_t runnum;
      uint8_t  runflags; 
      triginfo_t ti;       // trigger info
      uint8_t  evflag;     // data quality flag
      uint32_t evnum;      // event number
      uint32_t evszlro;    // size of event in bytes
      uint32_t evszcro;    // size of event in bytes
    } eveinfo_t;
    
    //
    typedef struct fragment_t
    {
      eveinfo_t ei;
      const BYTE* bytes;
      adcdata_t crodata;
      adcdata_t lrodata;
    } fragment_t;

    //
    unsigned unpack_evtable();
    unsigned unpack_eve_info( const char *buf, eveinfo_t &ei );
  };
};

#endif
