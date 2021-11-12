//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DqTH2D_h
#define DqTH2D_h

#include <TH2D.h>
#include "DqParser.hpp"

class DqTH2D : public TH2D
{
public:

  /* Constructor, destructor. */ 
  DqTH2D(const char *name, const char *title, 
         Int_t nbinsx, Double_t xlow, Double_t xup, 
         Int_t nbinsy, Double_t ylow, Double_t yup):TH2D(name, title, nbinsx, xlow, xup, nbinsy, ylow, yup) {
    _isConfig = false;
    _modX = "default";
    _modXStr = "&modX";
    _begXStr = "&begX";
    _endXStr = "&endX";
    _sizeXStr = "&sizeX";
    _titleXstr = "&titleX";
    
    _modY = "default";
    _modYStr = "&modY";
    _begYStr = "&begY";
    _endYStr = "&endY";
    _sizeYStr = "&sizeY";
    _titleYstr = "&titleY";
    
    _titleZstr = "&titleZ";
    _chamber = 0;
    _sizeX = 0; _isSizeX = true;
    _begX = std::numeric_limits<double>::max(); _isBegX = true;
    _endX = std::numeric_limits<double>::lowest(); _isEndX = true;
    _sizeY = 0; _isSizeY = true;
    _begY = std::numeric_limits<double>::max(); _isBegY = true;
    _endY = std::numeric_limits<double>::lowest(); _isEndY = true;
  }
  virtual ~DqTH2D();
  bool config(std::string name, std::map<std::string, std::string>* params);
  bool setting(double valueX, double valueY);
  bool setting();
  //----------------------------------------------------------------------------
protected:

  bool _isConfig; 
  
  std::string _modX; 
  std::string _modXStr; 
  std::string _begXStr;
  std::string _endXStr;
  std::string _sizeXStr;
  std::string _titleXstr;
  
  std::string _modY; 
  std::string _modYStr; 
  std::string _begYStr;
  std::string _endYStr;
  std::string _sizeYStr;
  std::string _titleYstr;

  std::string _titleZstr;
 
  double _chamber;
  int _sizeX;  bool _isSizeX;
  double _begX; bool _isBegX;
  double _endX;  bool _isEndX;
  
  int _sizeY;  bool _isSizeY;
  double _begY; bool _isBegY;
  double _endY;  bool _isEndY;
};
#endif
//------------------------------------------------------------------------------
