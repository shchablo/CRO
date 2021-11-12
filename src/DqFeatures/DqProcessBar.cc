#include "DqProcessBar.h"

/* Constructor, destructor. */ 
//------------------------------------------------------------------------------
DqProcessBar::DqProcessBar()
{
  _endIndex = 0;
  _currentStep = 0;
}

DqProcessBar::~DqProcessBar()
{
}
//------------------------------------------------------------------------------

/* Set functions. */ 
bool DqProcessBar::setEndIndex(unsigned int index) 
{
  _endIndex = index;
  return true;

}

bool DqProcessBar::setCurrentStep(unsigned int step) 
{
  _currentStep = step;
  return true;
}
//------------------------------------------------------------------------------

/* Get functions. */ 
unsigned int DqProcessBar::getEndIndex() 
{
  return _endIndex;
}
//------------------------------------------------------------------------------

/* Functions. */ 
bool DqProcessBar::print(unsigned int index , double step) 
{

  std::string PBSTR = "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";
  int PBWIDTH = 60;
  double percentage = (double)index/(double)_endIndex;
  if(step*_currentStep <= percentage) {
    _currentStep = _currentStep + 1;
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR.c_str(), rpad, "");
    fflush (stdout);
    return true;
  }
  
  return false;
}

bool DqProcessBar::print(double percentage) 
{
  std::string PBSTR = "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";
  int PBWIDTH = 60;
  
  int val = (int) (percentage * 100);
  int lpad = (int) (percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;
  printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR.c_str(), rpad, "");
  fflush (stdout);
  
  return true;
}
//------------------------------------------------------------------------------

