#include "DqTG2D.hpp"
#include <algorithm>

DqTG2D::~DqTG2D()
{
}

std::string DqTG2D::getTitleX()
{
  return _titleXstr;
}
std::string DqTG2D::getTitleY()
{
  return _titleYstr;
}
std::string DqTG2D::getTitleZ()
{
  return _titleZstr;
}
bool DqTG2D::config(std::string name, std::map<std::string, std::string>* params)
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
    
    std::string titleZ  = "Z";
    bool isTitleZ = parser.findObjectParam((name + _titleZstr), &strParam, params);
    if(isTitleZ) titleZ = strParam;
    _titleZstr = titleZ.c_str();
    this->GetZaxis()->SetTitle(titleZ.c_str());
    _isConfig = true;
    return true;
  }
  return false;

}

bool DqTG2D::setting(double x, double y, double z)
{
  _x.push_back(x);
  _y.push_back(y);
  _z.push_back(z);
  return true;
}

bool DqTG2D::sortByX()
{
  std::vector<int> sizes;
  sizes.push_back(_x.size()); 
  sizes.push_back(_y.size()); 
  sizes.push_back(_z.size()); 
  if(!(count(sizes.begin(), sizes.end(), sizes[0]) == (int)sizes.size()) ||  sizes[0] == 0)
    return false;
	double tmp = 0;
  for(unsigned int i = 0; i < _x.size() - 1; ++i)	{            
  	for(unsigned int j = 0; j < _x.size() - 1; ++j) {     
    	if(_x[j + 1] < _x[j]) {
      	tmp = _x[j + 1]; 
      	_x[j + 1] = _x[j]; 
      	_x[j] = tmp;
      	
				tmp = _y[j + 1]; 
      	_y[j + 1] = _y[j]; 
      	_y[j] = tmp;
				
        tmp = _z[j + 1]; 
      	_z[j + 1] = _z[j]; 
      	_z[j] = tmp;
				
			}
		}
	}	
	return true;
}
bool DqTG2D::plot()
{
  std::vector<int> sizes;
  sizes.push_back(_x.size()); 
  sizes.push_back(_y.size()); 
  sizes.push_back(_z.size()); 
  if(!(count(sizes.begin(), sizes.end(), sizes[0]) == (int)sizes.size()) || sizes[0] == 0)
    return false;
  for(unsigned int i = 0; i < _x.size(); i++) {		
    this->SetPoint(i,_x.at(i), _y.at(i), _z.at(i));
  }
  return true;
}
