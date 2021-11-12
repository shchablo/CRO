#include "DqMTGE.hpp"
#include <algorithm>

DqMTGE::~DqMTGE()
{
}

std::string DqMTGE::getTitleX()
{
  return _titleXstr;
}
std::string DqMTGE::getTitleY()
{
  return _titleYstr;
}
bool DqMTGE::config(std::string name, std::map<std::string, std::string>* params)
{
  if(!_isConfig) {
    DqParser parser;
    std::string strParam = "";
    
    std::string titleX  = "X";
    bool isTitleX = parser.findObjectParam((name + _titleXstr), &strParam, params);
    if(isTitleX) titleX = strParam;
    this->GetXaxis()->SetTitle(titleX.c_str());
    _titleXstr = titleX.c_str();
    
    std::string titleY  = "Y";
    bool isTitleY = parser.findObjectParam((name + _titleYstr), &strParam, params);
    if(isTitleY) titleY = strParam;
    _titleYstr = titleY.c_str();
    this->GetYaxis()->SetTitle(titleY.c_str());
    _isConfig = true;
    return true;
  }
  return false;

}
bool DqMTGE::addDqTGE(DqTGE *gr)
{
   gr->SetMarkerStyle(20);
   gr->SetMarkerColor(_colorAddDqTGE);
   gr->SetLineColor(_colorAddDqTGE);
  this->Add(gr);
  _colorAddDqTGE += 1;
  return false;

}
