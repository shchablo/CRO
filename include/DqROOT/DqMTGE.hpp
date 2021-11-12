//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DqMTGE_h
#define DqMTGE_h

#include <iostream>
#include <stdio.h>
#include <vector>
#include <fstream>

#include <iomanip> 

#include <TAxis.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include "DqParser.hpp"
#include "DqTGE.hpp"

class DqMTGE : public TMultiGraph
{
public:

  std::string getTitleX();
  std::string getTitleY();
  /* Constructor, destructor. */ 
  DqMTGE(const char *name, const char *title) {
    
    this->SetName(name);
    this->SetTitle(title);
    
    _isConfig = false; 
    _titleXstr = "&titleX";
    _titleYstr = "&titleY";
    _colorAddDqTGE = 1;
  }
  virtual ~DqMTGE();
  bool config(std::string name, std::map<std::string, std::string>* params);
  bool addDqTGE(DqTGE *gr);
  //----------------------------------------------------------------------------
protected:
    bool _isConfig;
    std::string _titleXstr;
    std::string _titleYstr;
    int _colorAddDqTGE;
};
#endif
//------------------------------------------------------------------------------
