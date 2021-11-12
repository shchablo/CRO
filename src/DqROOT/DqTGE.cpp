#include "DqTGE.hpp"
#include <algorithm>

DqTGE::~DqTGE()
{
}

std::string DqTGE::getTitleX()
{
  return _titleXstr;
}
std::string DqTGE::getTitleY()
{
  return _titleYstr;
}
bool DqTGE::config(std::string name, std::map<std::string, std::string>* params)
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

bool DqTGE::setting(double x, double ex, double y, double ey)
{
  _x.push_back(x);
  _ex.push_back(ex);
  _y.push_back(y);
  _ey.push_back(ey);
  return true;
}

bool DqTGE::sortByX()
{
  std::vector<int> sizes;
  sizes.push_back(_x.size()); 
  sizes.push_back(_ex.size()); 
  sizes.push_back(_y.size()); 
  sizes.push_back(_ey.size()); 
  if(!(count(sizes.begin(), sizes.end(), sizes[0]) == (int)sizes.size()) ||  sizes[0] == 0)
    return false;
	double tmp = 0;
  for(unsigned int i = 0; i < _x.size() - 1; ++i)	{            
  	for(unsigned int j = 0; j < _x.size() - 1; ++j) {     
    	if(_x[j + 1] < _x[j]) {
      	tmp = _x[j + 1]; 
      	_x[j + 1] = _x[j]; 
      	_x[j] = tmp;
      	
				tmp = _ex[j + 1]; 
      	_ex[j + 1] = _ex[j]; 
      	_ex[j] = tmp;
				
				tmp = _y[j + 1]; 
      	_y[j + 1] = _y[j]; 
      	_y[j] = tmp;
				
				tmp = _ey[j + 1]; 
      	_ey[j + 1] = _ey[j]; 
      	_ey[j] = tmp;
			}
		}
	}	
	return true;
}
bool DqTGE::plot()
{
  std::vector<int> sizes;
  sizes.push_back(_x.size()); 
  sizes.push_back(_ex.size()); 
  sizes.push_back(_y.size()); 
  sizes.push_back(_ey.size()); 
  if(!(count(sizes.begin(), sizes.end(), sizes[0]) == (int)sizes.size()) || sizes[0] == 0)
    return false;
  for(unsigned int i = 0; i < _x.size(); i++) {		
    this->SetPoint(i,_x.at(i), _y.at(i));
    this->SetPointError(i, _ex.at(i), _ey.at(i));
  }
  return true;
}

bool DqTGE::print(std::string mod)
{
  if(mod == "sigmoid") {
	  for(unsigned int i = 0; i < _x.size(); i++)		
  		std::cout << std::setprecision(9) << _x.at(i) << std::endl;
  		//std::cout << std::setprecision(9) << _x.at(i) << "\t" << _y.at(i) << "\t" << _ey.at(i) << std::endl;
  }
  if(mod == "offset") {
	  for(unsigned int i = 0; i < _x.size(); i++)		
  		std::cout << std::setprecision(9) << _x.at(i) << "|" << std::endl;
	  for(unsigned int i = 0; i < _y.size(); i++)		
  		std::cout << std::setprecision(9) << _y.at(i) << "|" << std::endl;
  }
  return true;
}
bool DqTGE::print(std::vector<std::string> *lines)
{
	  for(unsigned int i = 0; i < _y.size(); i++) {
      std::string line = std::to_string(_y.at(i)); 
      //std::string line = std::to_string(_x.at(i)); 
      //std::string line = std::to_string(_x.at(i)) + " " + std::to_string(_ex.at(i)) + " " + std::to_string(_y.at(i)) + " " + std::to_string(_ey.at(i)); 
      lines->push_back(line);		
  		//std::cout << std::setprecision(9) << _x.at(i) << "\t" << _y.at(i) << "\t" << _ey.at(i) << std::endl;
    }
  return true;
}

