//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DqTGAE_h
#define DqTGAE_h

#include <iostream>
#include <stdio.h>
#include <vector>
#include <fstream>

#include <iomanip> 

#include <TAxis.h>
#include <TGraphAsymmErrors.h>
#include "DqParser.hpp"

class DqTGAE : public TGraphAsymmErrors
{
public:

  std::string getTitleX();
  std::string getTitleY();
  /* Constructor, destructor. */ 
  DqTGAE(const char *name, const char *title) {
    
    this->SetName(name);
    this->SetTitle(title);
    
    _isConfig = false; 
    _titleXstr = "&titleX";
    _titleYstr = "&titleY";
  }
  virtual ~DqTGAE();
  bool config(std::string name, std::map<std::string, std::string>* params);
  bool setting(double x, double exl, double exh, double y, double eyl, double eyh);
	bool sortByX();
	bool plot();
  bool print(std::string mod);
  bool print(std::vector<std::string> *lines);
  //----------------------------------------------------------------------------
protected:
    bool _isConfig;
    std::string _titleXstr;
    std::string _titleYstr;
    std::vector<double> _x;
    std::vector<double> _exl;
    std::vector<double> _exh;
    std::vector<double> _y;
    std::vector<double> _eyl;
    std::vector<double> _eyh;
};
#endif
//------------------------------------------------------------------------------
