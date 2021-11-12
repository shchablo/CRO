//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DQPARSER_h
#define DQPARSER_h

#include <iostream>
#include <vector>
#include <map>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <dirent.h>
#include<stdio.h>
class DqParser
{
public:

  /* Constructor, destructor. */ 
  DqParser();
  virtual ~DqParser();
  //----------------------------------------------------------------------------

  /* Set functions. */ 
  bool setArrayDelimiter(std::vector<std::string> delimiter); 
  //----------------------------------------------------------------------------

  /* Get functions. */ 
  std::vector<std::string> getArrayDelimiter(); 
  //----------------------------------------------------------------------------
  
  /* Function. */
	std::string histName(std::string str); // поменять на paramName 
  std::string chamberName(std::string str);
  std::string first(std::string str);
  std::string last(std::string str);
  // ---	
	bool paramFromStr(int index, std::string str, std::string* param);
	bool findParamFromStr(std::string param, std::string str);
//  std::string takeLast

  std::string fileName(std::string str);
	std::string filePath(std::string str);
  std::string deleteFormat(std::string *name);
  std::vector<std::string> parsArray(std::string param); 
  
  std::vector<int> strToIntArray(std::vector<std::string> array); 
  std::vector<double> strToDoubleArray(std::vector<std::string> array); 
  
  bool strToInt(int* result, std::string str); 
  bool strToDouble(double* result, std::string str); 
  
  std::vector<std::string> checkPath(std::string input);
  
  bool findObjectParam(std::string name, std::string* param,
                       std::map<std::string, std::string>* params);
  // ---
  unsigned long combine(unsigned long a, unsigned long b); 
  unsigned long convertToASCII(std::string letter);
  //----------------------------------------------------------------------------
protected:

  std::vector<std::string> _arrayDelimiter;

};
#endif
//------------------------------------------------------------------------------
