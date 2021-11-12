//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DqTH1D_h
#define DqTH1D_h

#include <TH1D.h>
#include <TF1.h>
#include <TH1.h>
#include "DqParser.hpp"

class DqTH1D : public TH1D
{
public:

  /* Constructor, destructor. */ 
  DqTH1D(const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup):TH1D(name, title, nbinsx, xlow, xup) {
		_isConfig = false;
    _mod = "default";
    _modStr = "&mod";
    _begStr = "&beg";
    _endStr = "&end";
    _sizeStr = "&size";
    _titleXstr = "&titleX";
    _titleYstr = "&titleY";
    _chamber = 0;
    _size = 0; _isSize = true;
    _beg = std::numeric_limits<double>::max(); _isBeg = true;
    _end = std::numeric_limits<double>::lowest(); _isEnd = true;
  	_fitFunction = 0;
  }
  virtual ~DqTH1D();
  bool config(std::string name, std::map<std::string, std::string>* params);
  bool setTitle(std::string title);
  bool setting(double value);
  bool setting();
  std::map<std::string, double> fit(std::string function, std::map<std::string, double> *args); 
	TF1 *getFit();
  //----------------------------------------------------------------------------
protected:

  
  bool _isConfig; 
  
  TF1 *_fitFunction;
  
  std::string _mod; 
  
  std::string _modStr; 
  std::string _begStr;
  std::string _endStr;
  std::string _sizeStr;
  std::string _titleXstr;
  std::string _titleYstr;
 
  double _chamber;
  int _size;  bool _isSize;
  double _beg; bool _isBeg;
  double _end;  bool _isEnd;
};
#endif
//------------------------------------------------------------------------------
