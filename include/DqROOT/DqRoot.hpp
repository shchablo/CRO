//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DQROOT_h
#define DQROOT_h

/* ROOT includes */
# include <TApplication.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TDirectory.h>
#include <TObject.h>
#include <TCanvas.h>
#include <TImage.h>
#include <TFrame.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TH1F.h>
#include <TString.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TGraph.h>
#include <TMath.h>
#include <TLatex.h>
#include <TPaveStats.h>
//------------------------------------------------------------------------------

/* DQ includes */

#include "DqParser.hpp"
#include "DqTH2D.hpp"
#include "DqTH1D.hpp"
#include "DqTGE.hpp"
#include "DqTGAE.hpp"
#include "DqTG2D.hpp"
#include "DqMTGE.hpp"
//------------------------------------------------------------------------------

/* C++ includes */
#include <string>
#include <iostream>
#include <vector>
#include <math.h>
//------------------------------------------------------------------------------

class DqRoot
{
public:

  /* Constructor, destructor. */ 
  DqRoot();
  virtual ~DqRoot();
  //----------------------------------------------------------------------------

  /* Set functions. */ 
  bool setOutputFile(std::string outputFileName, std::string outputTreeName);
  bool setBranch(double *data, std::string branchName);
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  bool histParams(std::string name, double value, 
                        std::map<std::string, double>* params, int mod);
  bool fillHist(std::string name, double value, 
                             std::map<std::string, TH1D>* hists,
                             std::map<std::string, double>* histsParams,
                             std::map<std::string, std::string>* params);
  
  bool writeObject(std::string dirName, TObject* object);
  bool writeCanvas(std::string dirName, TCanvas* canvas);
  bool writeDqToCanvasCMS(std::string path, TH1* object, TF1 *fit, bool isCanvas);
	bool writeDqToCanvasCMS(std::string path, DqTGE* object, bool isCanvas);
	bool writeDqToCanvasCMS(std::string path, DqTGAE* object, bool isCanvas);
	bool writeDqToCanvasCMS(std::string path, DqTG2D* object, bool isCanvas);
	bool writeDqToCanvasCMS(std::string path, DqMTGE* object, bool isCanvas);
	bool writeDqToCanvasCMS(std::string path, TH2* object, bool isCanvas);
  bool writeDqToCanvasCMS(std::string path, std::vector<TH1>* objects, bool isCanvas);
  //----------------------------------------------------------------------------

  std::map<std::string, DqTH2D>::iterator addDqTH2D(std::string name, std::string title, std::map<std::string, DqTH2D>* dqTH2Ds);
  std::map<std::string, DqTH1D>::iterator addDqTH1D(std::string name, std::string title, std::map<std::string, DqTH1D>* dqTH1Ds);
  std::map<std::string, DqTGE*>::iterator addDqTGE(std::string name, std::string title, std::map<std::string, DqTGE*>* dqTGEs);
  std::map<std::string, DqTGAE*>::iterator addDqTGAE(std::string name, std::string title, std::map<std::string, DqTGAE*>* dqTGAEs);
  std::map<std::string, DqTG2D*>::iterator addDqTG2D(std::string name, std::string title, std::map<std::string, DqTG2D*>* dqTG2Ds);
  std::map<std::string, DqMTGE*>::iterator addDqMTGE(std::string name, std::string title, std::map<std::string, DqMTGE*>* dqMTGEs);
  //----------------------------------------------------------------------------
protected:

  bool isOutFile_;
  TFile *outFile_;
  TTree *outTree_;

//char  *dirName_;
//char  *plotName_;
};
#endif
//------------------------------------------------------------------------------
