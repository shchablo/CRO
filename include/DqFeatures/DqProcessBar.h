//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DQPROCESSBAR_h
#define DQPROCESSBAR_h


#include <iostream>
#include <stdio.h>
#include <string.h>

class DqProcessBar
{
public:

  /* Constructor, destructor. */ 
  DqProcessBar();
  virtual ~DqProcessBar();
  //----------------------------------------------------------------------------

  /* Set Function. */ 
  bool setEndIndex(unsigned int index); 
  bool setCurrentStep(unsigned int step); 
  //----------------------------------------------------------------------------
  
  /* Get Function. */ 
  unsigned int getEndIndex(); 
  //----------------------------------------------------------------------------

  /* Function. */ 
  bool print(unsigned int index, double step);
  bool print(double percentage);
  //----------------------------------------------------------------------------
protected:

  unsigned int _endIndex; 
  unsigned int _currentStep; 
};
#endif
//------------------------------------------------------------------------------

