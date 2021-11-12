#include "DqTH1D.hpp"

DqTH1D::~DqTH1D()
{
//  if(_fitFunction != 0)
//    delete _fitFunction;

}

bool DqTH1D::config(std::string name, std::map<std::string, std::string>* params)
{
  if(!_isConfig) {
    DqParser parser;
    double param = 0;
    std::string strParam = "";
    parser.strToDouble(&_chamber, parser.chamberName(name));
    bool isMod = parser.findObjectParam((name + _modStr), &strParam, params);
    if(isMod)
      _mod = strParam;
    bool isSize = parser.findObjectParam((name + _sizeStr), &strParam, params);
    if(isSize) {
      parser.strToDouble(&param, strParam);
      _size = (int)param;
      _isSize = false;
    }
    bool isBeg = parser.findObjectParam((name + _begStr), &strParam, params);
    if(isBeg) {
      parser.strToDouble(&param, strParam);
      _beg = param;
      _isBeg = false;
    }
    bool isEnd = parser.findObjectParam((name + _endStr), &strParam, params);
    if(isEnd) {
      parser.strToDouble(&param, strParam);
      _end = param;
      _isEnd = false;
    }
    std::string titleX  = "X";
    bool isTitleX = parser.findObjectParam((name + _titleXstr), &strParam, params);
    if(isTitleX) titleX = strParam;
    this->GetXaxis()->SetTitle(titleX.c_str());
    
    std::string titleY  = "Y";
    bool isTitleY = parser.findObjectParam((name + _titleYstr), &strParam, params);
    if(isTitleY) titleY = strParam;
    this->GetYaxis()->SetTitle(titleY.c_str());
    _isConfig = true;
    return true;
  }
  return false;
}

bool DqTH1D::setting(double value)
{
 
  // Size of bins for strips --------------------------------------------------- 
  if(_mod == "default") {
    if(value <= _beg && _isBeg)
      _beg = value;
    if(value > _end && _isEnd)
      _end = value;
  }
  //----------------------------------------------------------------------------
  // Size of bins for strips --------------------------------------------------- 
  if(_mod == "strips") {
//    if(value <= _beg && _isBeg)
//      _beg = value;
//    if(value > _end && _isEnd)
//      _end = value;
  }
  //----------------------------------------------------------------------------
  
  // Rice Rule FILLING ----------------------------------------------------- 
  if(_mod == "rice") {
    if(value <= _beg && _isBeg)
      _beg = value;
    if(value > _end && _isEnd)
      _end = value;
    if(_isSize)
      _size += 1;
  }
  //----------------------------------------------------------------------------
  else
    return false;
return true;
}

bool DqTH1D::setting()
{
 
  // Rice Rule CALCULATION ------------------------------------------------- 
  if(_mod == "default") {
    if(_beg == std::numeric_limits<double>::max() ||
       _end == std::numeric_limits<double>::lowest()) {
      _beg = -1;
      _end = 1;
      _size = 1000;
    }
    else if((1000 > (int)std::abs(_end - _beg)) && _isSize) {
      _size = std::abs(_end - _beg);
    }
    else if(_isSize)
      _size = 1000;
    _beg = _beg - std::abs(_end-_beg)/_size/2;
    _end = _end + std::abs(_end-_beg)/_size/2;
  }

  if(_mod == "rice") { 
    if(_isSize) {
      _size = (int)(6*2*pow(_size, 0.333));
      _beg = 1; _end = -1;
    }
  }

  if(_mod == "strips") {
      _size = std::abs(_end - _beg) + 1;
      _beg = _beg - 0.5;
      _end = _end + 0.5;
  }
  //----------------------------------------------------------------------------
 
  this->SetBins(_size, _beg, _end);
  return true;
}

std::map<std::string, double> DqTH1D::fit(std::string function, std::map<std::string, double> *args) 
{
  std::map<std::string, double> results; 
  if(function == "gaus") {
    double nRMS = 3; 
    auto it = args->find("nRMS");
    if(it != args->end())
      nRMS = it->second;
    double mean  = this->GetMean();
    double RMS  = this->GetRMS();
    TF1 *fitFunction = new TF1(Form("fit_%s", this->GetName()), "gaus",  mean - nRMS*RMS, mean + nRMS*RMS);
    this->Fit(Form("fit_%s", this->GetName()), "QR");
	_fitFunction = this->GetFunction(fitFunction->GetName());
  }
  else
    return results;
  return results;
}
TF1 *DqTH1D::getFit()
{	
	return _fitFunction;
}
