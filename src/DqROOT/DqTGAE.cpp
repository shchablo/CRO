#include "DqTGAE.hpp"
#include <algorithm>

DqTGAE::~DqTGAE()
{
}

std::string DqTGAE::getTitleX()
{
  return _titleXstr;
}
std::string DqTGAE::getTitleY()
{
  return _titleYstr;
}
bool DqTGAE::config(std::string name, std::map<std::string, std::string>* params)
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

bool DqTGAE::setting(double x, double exl, double exh, double y, double eyl, double eyh)
{
  _x.push_back(x);
  _exl.push_back(exl);
  _exh.push_back(exh);
  _y.push_back(y);
  _eyl.push_back(eyl);
  _eyh.push_back(eyh);
  return true;
}

bool DqTGAE::sortByX()
{
  std::vector<int> sizes;
  sizes.push_back(_x.size()); 
  sizes.push_back(_exl.size()); 
  sizes.push_back(_exh.size()); 
  sizes.push_back(_y.size()); 
  sizes.push_back(_eyl.size()); 
  sizes.push_back(_eyh.size()); 
  if(!(count(sizes.begin(), sizes.end(), sizes[0]) == (int)sizes.size()) ||  sizes[0] == 0)
    return false;
	double tmp = 0;
  for(unsigned int i = 0; i < _x.size() - 1; ++i)	{            
  	for(unsigned int j = 0; j < _x.size() - 1; ++j) {     
    	if(_x[j + 1] < _x[j]) {
      	tmp = _x[j + 1]; 
      	_x[j + 1] = _x[j]; 
      	_x[j] = tmp;
      	
				tmp = _exl[j + 1]; 
      	_exl[j + 1] = _exl[j]; 
      	_exl[j] = tmp;
				tmp = _exh[j + 1]; 
      	_exh[j + 1] = _exh[j]; 
      	_exh[j] = tmp;
				
				tmp = _y[j + 1]; 
      	_y[j + 1] = _y[j]; 
      	_y[j] = tmp;
				
				tmp = _eyl[j + 1]; 
      	_eyl[j + 1] = _eyl[j]; 
      	_eyl[j] = tmp;
				tmp = _eyh[j + 1]; 
      	_eyh[j + 1] = _eyh[j]; 
      	_eyh[j] = tmp;
			}
		}
	}	
	return true;
}
bool DqTGAE::plot()
{
  std::vector<int> sizes;
  sizes.push_back(_x.size()); 
  sizes.push_back(_exl.size()); 
  sizes.push_back(_exh.size()); 
  sizes.push_back(_y.size()); 
  sizes.push_back(_eyl.size()); 
  sizes.push_back(_eyh.size()); 
  if(!(count(sizes.begin(), sizes.end(), sizes[0]) == (int)sizes.size()) || sizes[0] == 0)
    return false;
  for(unsigned int i = 0; i < _x.size(); i++) {		
    this->SetPoint(i,_x.at(i), _y.at(i));
    this->SetPointError(i, _exl.at(i), _exh.at(i), _eyl.at(i), _eyh.at(i));
  }
  return true;
}

bool DqTGAE::print(std::string mod)
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
bool DqTGAE::print(std::vector<std::string> *lines)
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

