//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef MYROOT_h
#define MYROOT_h


#include <iostream>
#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>

#include <TCanvas.h>
#include <TGraphErrors.h>

#include "DqAnalysis.hpp"
#include "DqCSV.hpp"
#include "DqRoot.hpp"

#include "DqProcessBar.hpp"
#include "DqParser.hpp"

class MyRoot : public DqAnalysis
{
public:

  /* Constructor, destructor. */ 
  MyRoot(std::string sysMods, 
               std::map<std::string, std::string> params,
               std::vector<std::string> inputs,
               std::vector<std::string> outputs);
  virtual ~MyRoot();
  //----------------------------------------------------------------------------
  
  /* Get functions. */ 
  virtual std::string getLog();
  //----------------------------------------------------------------------------
  
  /* Loop function */ 
  bool TGraphErrorsTable();
  //----------------------------------------------------------------------------
  
  virtual bool run();

protected:

  std::string _mod;
  std::map<std::string, std::string> _params;
  std::vector<std::string> _inputs;
  std::vector<std::string> _outputs;
};
#endif
//------------------------------------------------------------------------------
