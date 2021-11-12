//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DqTG2D_h
#define DqTG2D_h

#include <iostream>
#include <stdio.h>
#include <vector>
#include <fstream>

#include <iomanip> 

#include <TAxis.h>
#include <TGraph2D.h>
#include "DqParser.hpp"

class DqTG2D : public TGraph2D
{
public:

  std::string getTitleX();
  std::string getTitleY();
  std::string getTitleZ();
  /* Constructor, destructor. */ 
  DqTG2D(const char *name, const char *title) {
    
    this->SetName(name);
    this->SetTitle(title);
    
    _isConfig = false; 
    _titleXstr = "&titleX";
    _titleYstr = "&titleY";
    _titleZstr = "&titleZ";
  }
  virtual ~DqTG2D();
  bool config(std::string name, std::map<std::string, std::string>* params);
  bool setting(double x, double y, double z);
	bool sortByX();
	bool plot();
  //----------------------------------------------------------------------------
protected:
    bool _isConfig;
    std::string _titleXstr;
    std::string _titleYstr;
    std::string _titleZstr;
    std::vector<double> _x;
    std::vector<double> _y;
    std::vector<double> _z;
};
#endif
//------------------------------------------------------------------------------
