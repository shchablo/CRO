#include "DqTH2D.hpp"

DqTH2D::~DqTH2D()
{
}

bool DqTH2D::config(std::string name, std::map<std::string, std::string>* params)
{
  if(!_isConfig) {
    DqParser parser;
    double param = 0;
    std::string strParam = "";
    parser.strToDouble(&_chamber, parser.chamberName(name));
    
    bool isModX = parser.findObjectParam((name + _modXStr), &strParam, params);
    if(isModX)
      _modX = strParam;
    bool isSizeX = parser.findObjectParam((name + _sizeXStr), &strParam, params);
    if(isSizeX) {
      parser.strToDouble(&param, strParam);
      _sizeX = (int)param;
      _isSizeX = false;
    }
    bool isBegX = parser.findObjectParam((name + _begXStr), &strParam, params);
    if(isBegX) {
      parser.strToDouble(&param, strParam);
      _begX = param;
      _isBegX = false;
    }
    bool isEndX = parser.findObjectParam((name + _endXStr), &strParam, params);
    if(isEndX) {
      parser.strToDouble(&param, strParam);
      _endX = param;
      _isEndX = false;
    }

    bool isModY = parser.findObjectParam((name + _modYStr), &strParam, params);
    if(isModY)
      _modY = strParam;
    bool isSizeY = parser.findObjectParam((name + _sizeYStr), &strParam, params);
    if(isSizeY) {
      parser.strToDouble(&param, strParam);
      _sizeY = (int)param;
      _isSizeY = false;
    }
    bool isBegY = parser.findObjectParam((name + _begYStr), &strParam, params);
    if(isBegY) {
      parser.strToDouble(&param, strParam);
      _begY = param;
      _isBegY = false;
    }
    bool isEndY = parser.findObjectParam((name + _endYStr), &strParam, params);
    if(isEndY) {
      parser.strToDouble(&param, strParam);
      _endY = param;
      _isEndY = false;
    }

    std::string titleX  = "X";
    bool isTitleX = parser.findObjectParam((name + _titleXstr), &strParam, params);
    if(isTitleX) titleX = strParam;
    this->GetXaxis()->SetTitle(titleX.c_str());
    
    std::string titleY  = "Y";
    bool isTitleY = parser.findObjectParam((name + _titleYstr), &strParam, params);
    if(isTitleY) titleY = strParam;
    this->GetYaxis()->SetTitle(titleY.c_str());
    
    std::string titleZ  = "Z";
    bool isTitleZ = parser.findObjectParam((name + _titleZstr), &strParam, params);
    if(isTitleZ) titleZ = strParam;
    this->GetZaxis()->SetTitle(titleZ.c_str());
    _isConfig = true;
    return true;
  }
  return false;
}

bool DqTH2D::setting(double valueX, double valueY)
{
 
  // Size of bins for strips --------------------------------------------------- 
  if(_modX == "default") {
    if(valueX <= _begX && _isBegX)
      _begX = valueX;
    if(valueX > _endX && _isEndX)
      _endX = valueX;
  }
  if(_modY == "default") {
    if(valueY <= _begY && _isBegY)
      _begY = valueY;
    if(valueY > _endY && _isEndY)
      _endY = valueY;
  }
  //----------------------------------------------------------------------------
  // Size of bins for strips --------------------------------------------------- 
  if(_modX == "strips") {
//    if(valueX <= _begX && _isBegX)
//      _begX = valueX;
//    if(valueX > _endX && _isEndX)
//      _endX = valueX;
  }
  if(_modY == "strips") {
//    if(valueY <= _begY && _isBegY)
//      _begY = valueY;
//    if(valueY > _endY && _isEndY)
//      _endY = valueY;
  }
  //----------------------------------------------------------------------------
  
  // Rice Rule FILLING ----------------------------------------------------- 
  if(_modX == "rice") {
    if(valueX <= _begX && _isBegX)
      _begX = valueX;
    if(valueX > _endX && _isEndX)
      _endX = valueX;
    if(_isSizeX)
      _sizeX += 1;
  }
  if(_modY == "rice") {
    if(valueY <= _begY && _isBegY)
      _begY = valueY;
    if(valueY > _endY && _isEndY)
      _endY = valueY;
    if(_isSizeY)
      _sizeY += 1;
  }
  //----------------------------------------------------------------------------
  else
    return false;
return true;
}

bool DqTH2D::setting()
{
 
  // CALCULATION --------------------------------------------------------------- 
  //X
  if(_modX == "default") {
    if((2000 > (int)std::abs(_begX - _endX)) && _isSizeX)
      _sizeX = std::abs(_begX - _endX);
    else if(_isSizeX)
      _sizeX = 2000;
    _begX = _begX - std::abs(_endX-_begX)/_sizeX/2;
    _endX = _endX + std::abs(_endX-_begX)/_sizeX/2;
  }
  if(_modX == "rice") { 
    if(_isSizeX)
      _sizeX = (int)(6*2*pow(_sizeX, 0.333));
    _begX = 1;
    _endX = -1;
  }
  if(_modX == "strips") {
      _sizeX = std::abs(_endX - _begX) + 1;
      _begX = _begX - 0.5;
      _endX = _endX + 0.5;
  }
  //Y
  if(_modY == "default") {
    if((2000 > (int)std::abs(_begY - _endY)) && _isSizeY)
      _sizeY = std::abs(_begY - _endY);
    else if(_isSizeY)
      _sizeY = 2000;
    _begY = _begY - std::abs(_endY-_begY)/_sizeY/2;
    _endY = _endY + std::abs(_endY-_begY)/_sizeY/2;
  }
  if(_modY == "rice") { 
    if(_isSizeY) {
      _sizeY = (int)(6*2*pow(_sizeY, 0.333));
      _begY = 1;
      _endY = -1;
    }
  }
  if(_modY == "strips") {
      _sizeY = std::abs(_endY - _begY) + 1;
      _begY = _begY - 0.5;
      _endY = _endY + 0.5;
  }
  //----------------------------------------------------------------------------
 
  this->SetBins(_sizeX, _begX, _endX, _sizeY, _begY, _endY);
  return true;
}
