#include "DqParser.hpp"

/* Constructor, destructor. */ 
//------------------------------------------------------------------------------
DqParser::DqParser()
{
  _arrayDelimiter.push_back(";"); // delimiter blocks
  _arrayDelimiter.push_back("-"); // list symbol (example: 4-8)
  _arrayDelimiter.push_back("_"); // param seporatop inside name
  _arrayDelimiter.push_back("&"); // param seporatop name and value
  _arrayDelimiter.push_back("*"); // tmp star
}

DqParser::~DqParser()
{
}
//------------------------------------------------------------------------------
/* Set functions. */ 
bool DqParser::setArrayDelimiter(std::vector<std::string> delimiter) 
{
  if(delimiter.size() >= 5) {
    _arrayDelimiter.clear();
    _arrayDelimiter.push_back(delimiter.at(0));
    _arrayDelimiter.push_back(delimiter.at(1));
    _arrayDelimiter.push_back(delimiter.at(2));
    _arrayDelimiter.push_back(delimiter.at(3));
    _arrayDelimiter.push_back(delimiter.at(4));
    return true;
  }
  else
    return false;
  return false;

}
//------------------------------------------------------------------------------

/* Get functions. */ 
std::vector<std::string> DqParser::getArrayDelimiter() 
{
  return _arrayDelimiter;
}
//------------------------------------------------------------------------------
/* Functions. */ 
std::string DqParser::histName(std::string str)
{
	return this->last(str);
}
std::string DqParser::chamberName(std::string str)
{
	return this->first(str);
}
std::string DqParser::first(std::string str)
{
    std::string result; 
		try {
    	str.find_first_of(_arrayDelimiter.at(2));
		}
		catch(std::invalid_argument& e) {
	    return str;
		}
    
    std::size_t found = str.find_first_of(_arrayDelimiter.at(2));
    result = str.substr(0, found);

	return result;
}
std::string DqParser::last(std::string str)
{
    std::string result; 
		try {
    	str.find_last_of(_arrayDelimiter.at(2));
		}
		catch(std::invalid_argument& e) {
	    return str;
		}
    std::size_t found = str.find_last_of(_arrayDelimiter.at(2));
    result = str.substr(found + 1);

	return result;
}
//---

bool DqParser::paramFromStr(int index, std::string str, std::string* param)
{
	bool result = false;
	int count = 0;
	std::string delim = _arrayDelimiter.at(2);
 		auto start = 0U;
    auto end = str.find(delim);
    while (end != std::string::npos) {
			count += 1;
			if(count == index) {
        *param = str.substr(start, end - start);
				result = true;
			}
    	start = end + delim.length();
    	end = str.find(delim, start);
    }

		count += 1;
		if(count == index) {
    	*param = str.substr(start, end);
			result = true;
		}
	return result;

}

bool DqParser::findParamFromStr(std::string param, std::string str)
{
	bool result = false;
	std::string delim = _arrayDelimiter.at(2);
 		auto start = 0U;
    auto end = str.find(delim);
    while (end != std::string::npos) {
			if(param == str.substr(start, end - start))
				result = true;
    	start = end + delim.length();
    	end = str.find(delim, start);
    }
		if(param == str.substr(start, end))
			result = true;
	return result;

}
//---

std::string DqParser::fileName(std::string str)
{
	std::string result = "";
  std::size_t found = str.find_last_of("/\\");
  result = str.substr(found + 1);

	return result;
}

std::string DqParser::filePath(std::string str)
{
	std::string result = "";
  std::size_t found = str.find_last_of("/\\");
  result = str.substr(0, found + 1);

	return result;
}

std::string DqParser::deleteFormat(std::string *name)
{
	std::string result = "";
	std::string tmp = "";
  std::size_t found = name->find_last_of(".");
  if(found != std::string::npos) {
    result = name->substr(found);
    tmp = name->substr(0, found);
    name->clear();
    *name = tmp;
  }

	return result;
}

std::vector<std::string> DqParser::parsArray(std::string param) 
{
  std::vector<std::string> parsParams;
  
  std::size_t prev = 0; 
  std::size_t next = 0; 
  bool flag = true;
  while(flag) {
      next = param.find(_arrayDelimiter.at(0), prev);
      if(next != std::string::npos) {
        parsParams.push_back(param.substr(prev, next-prev));
        prev = next + _arrayDelimiter.at(0).size();
      }
      else {
        parsParams.push_back(param.substr(prev));
        if(prev + 1 <= param.size())
          parsParams.push_back(param.substr(prev + 1));
        flag = false;
        break;
      }
  }

  std::vector<std::string> result;
  for(unsigned int i = 0; i < parsParams.size() - 1; i++) {
    if(parsParams.at(i).size() <= 0)
      continue;

  std::string str0 = "";
  std::string str1 = "";
  std::string str2 = "";
  int first = 0; int last = 0; 
  int step = 1;
  std::size_t pos = 0; 
  std::size_t found = parsParams.at(i).find(_arrayDelimiter.at(1));
      pos = parsParams.at(i).find(_arrayDelimiter.at(2), 0);
    if(found != std::string::npos) {
      pos = parsParams.at(i).find(_arrayDelimiter.at(2), 0);
      if(pos != std::string::npos) {
        str0 = parsParams.at(i).substr(0, pos);  
        step = atoi(str0.c_str());
        pos = pos + _arrayDelimiter.at(2).size();
      }
      else
        pos = 0;
      found = found - pos;
      str1 = parsParams.at(i).substr(pos, found);
      found = found + pos;
      str2 = parsParams.at(i).substr(found + _arrayDelimiter.at(1).size());
      first = atoi(str1.c_str()); last = atoi(str2.c_str());
      if(first < last) {
        for(int j = first; j <= last; j+=step) {
          result.push_back(std::to_string(j));
        }
      }
    }
    else {
      result.push_back(parsParams.at(i));
    }
  }
  return result;
}

std::vector<int> DqParser::strToIntArray(std::vector<std::string> array) 
{
  std::vector<int> result;
  for(unsigned int i = 0; i < array.size(); i++) {
    try {
      int* isTry = new int();
      *isTry = std::atoi(array.at(i).c_str());
      delete isTry;
    }
		catch(std::invalid_argument& e) {
			continue;
		}
    result.push_back(std::atoi(array.at(i).c_str()));
  }
  return result;
}
std::vector<double> DqParser::strToDoubleArray(std::vector<std::string> array) 
{
  std::vector<double> result;
  for(unsigned int i = 0; i < array.size(); i++) {
    try {
      double* isTry = new double();
      *isTry = std::atof(array.at(i).c_str());
      delete isTry;
    }
		catch(std::invalid_argument& e) {
			continue;
		}
    result.push_back(std::atof(array.at(i).c_str()));
  }
  return result;
}

bool DqParser::strToInt(int* result, std::string str) 
{
    try {
      int* isTry = new int();
      *isTry = std::atoi(str.c_str());
      delete isTry;
    }
		catch(std::invalid_argument& e) {
			return false;
		}
    *result = std::atoi(str.c_str());
  return true;
}

bool DqParser::strToDouble(double* result, std::string str) 
{
    try {
      double* isTry = new double();
      *isTry = std::atof(str.c_str());
      delete isTry;
    }
		catch(std::invalid_argument& e) {
			return false;
		}
    *result = std::atof(str.c_str());
  return true;
}

std::vector<std::string> DqParser::checkPath(std::string input) 
{
  std::vector<std::string> result;
	std::string str; 	
	std::string path; 	
	  DIR *dir = opendir(input.c_str());
    if(dir) {
    	struct dirent *ent;
    	while((ent = readdir(dir)) != NULL) {
				str = ent->d_name;	
				if(str != "." && str != "..") {
					path = input + "/" + str;
	  			DIR *dir2 = opendir(path.c_str());
						if(!dir2) {
  				 		result.push_back(path);
//							puts(path.c_str());
						}
				}
    	}
    } else {
				result.clear();
  			return result;
    }
  return result;
}
//---
/* Functions for parse and find params for ROOT Objets  */
bool DqParser::findObjectParam(std::string name, std::string* param,
                                std::map<std::string, std::string>* params)
{

  std::string defaultName = this->chamberName(name) + "_" + this->histName(name);
  auto it = params->find(defaultName.c_str());
  if(!(it == params->end()))
    *param = params->find(defaultName.c_str())->second;
  else {
    defaultName = this->histName(name);
    auto itDefault = params->find(defaultName.c_str());
    if(!(itDefault == params->end()))
      *param = params->find(defaultName.c_str())->second;
    else
      return false;
  }

  return true;
}

// -----
unsigned long DqParser::combine(unsigned long a, unsigned long b) 
{
	unsigned long times = 1;
   while (times <= b)
      times *= 10;
   return a*times + b;
}
unsigned long DqParser::convertToASCII(std::string letter)
{
		unsigned long result = 0;
    for(unsigned int i = 0; i < letter.length(); i++) {
			unsigned long tmp = letter.at(i);
			result = this->combine(result, tmp);
		} 
		
	return result;
}
//------------------------------------------------------------------------------
