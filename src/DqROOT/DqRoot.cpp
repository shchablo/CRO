#include "DqRoot.hpp"
//#include "tdrstyle.cpp"
//#include "CMS_lumi.hpp"
/* Constructor, destructor. */ 
//------------------------------------------------------------------------------
DqRoot::DqRoot()
{
  gROOT->ProcessLine( "gErrorIgnoreLevel = 1001;");
  gROOT->ProcessLine( "gErrorIgnoreLevel = 2001;");
  gROOT->ProcessLine( "gErrorIgnoreLevel = 2001;");
isOutFile_ = false;
}

DqRoot::~DqRoot()
{
  if(isOutFile_) {
  	 outTree_->Delete();
		 outFile_->Close();
		 outFile_->Delete();
  }
}
//------------------------------------------------------------------------------

/* Set functions. */ 
//------------------------------------------------------------------------------
bool DqRoot::setOutputFile(std::string outputFileName, std::string outputTreeName)
{
  outFile_ = new TFile(outputFileName.c_str(), "UPDATE");
  if(!outFile_)
    return false;
  outTree_ = new TTree(outputTreeName.c_str(), outputTreeName.c_str());
  if(!outTree_)
    return false;
  isOutFile_ = true;
  return true;
}

bool DqRoot::setBranch(double *data, std::string branchName)
{
  if(!isOutFile_)
    return false;
  //outTree_->Branch(branchName.c_str(), &data, branchName.c_str());
  return true;
}
//------------------------------------------------------------------------------

/* Functions */ 
//------------------------------------------------------------------------------
bool DqRoot::writeObject(std::string dirName, TObject *object)
{
  if(!outFile_->GetDirectory(dirName.c_str())) {
    outFile_->mkdir(dirName.c_str());
    outFile_->cd(dirName.c_str());
  }
  else
    outFile_->cd(dirName.c_str());

 // if(outFile_->FindObject(object->GetName()))
 //   return false;

  object->Write();
  return true;
}
bool DqRoot::writeCanvas(std::string dirName, TCanvas* canvas)
{
  if(!outFile_->GetDirectory(dirName.c_str())) {
    outFile_->mkdir(dirName.c_str());
    outFile_->cd(dirName.c_str());
  }
  else
    outFile_->cd(dirName.c_str());

 // if(outFile_->FindObject(object->GetName()))
 //   return false;

  canvas->Write();
  return true;
}

bool DqRoot::writeDqToCanvasCMS(std::string path, DqTGE* object, bool isCanvas)
{
	TCanvas *c1 = new TCanvas(Form("c_%s", object->GetName()), Form("%s", object->GetName()));
	object->GetXaxis()->SetTitle(object->getTitleX().c_str());
	object->GetYaxis()->SetTitle(object->getTitleY().c_str());
  object->SetMarkerColor(1);
  object->SetMarkerSize(0.5);
  object->SetMarkerStyle(20);
  object->Draw();
  TString tsPath(path.c_str());
  const char* cDirName = tsPath.Data();
  if(isCanvas) {
	  c1->SetCanvasSize(700, 600);
	  c1->SetWindowSize(700, 600);
	  c1->SetGrid();
    TString c1Path((path + "/CMS/").c_str());
    const char* c1DirName = c1Path.Data();
	  this->writeObject(c1DirName, c1);
  }
	this->writeObject(cDirName, object);
  delete c1;
	return true;
}
bool DqRoot::writeDqToCanvasCMS(std::string path, DqTGAE* object, bool isCanvas)
{
	TCanvas *c1 = new TCanvas(Form("c_%s", object->GetName()), Form("%s", object->GetName()));
	object->GetXaxis()->SetTitle(object->getTitleX().c_str());
	object->GetYaxis()->SetTitle(object->getTitleY().c_str());
  object->SetMarkerColor(1);
  object->SetMarkerSize(0.5);
  object->SetMarkerStyle(20);
  object->Draw();
  TString tsPath(path.c_str());
  const char* cDirName = tsPath.Data();
  if(isCanvas) {
	  c1->SetCanvasSize(700, 600);
	  c1->SetWindowSize(700, 600);
	  c1->SetGrid();
    TString c1Path((path + "/CMS/").c_str());
    const char* c1DirName = c1Path.Data();
	  this->writeObject(c1DirName, c1);
  }
	this->writeObject(cDirName, object);
  delete c1;
	return true;
}
bool DqRoot::writeDqToCanvasCMS(std::string path, DqTG2D* object, bool isCanvas)
{
	TCanvas *c1 = new TCanvas(Form("c_%s", object->GetName()), Form("%s", object->GetName()));
	//object->GetXaxis()->SetTitle(object->getTitleX().c_str());
	//object->GetYaxis()->SetTitle(object->getTitleY().c_str());
	//object->GetZaxis()->SetTitle(object->getTitleZ().c_str());
	object->GetXaxis()->SetTitleOffset(2);
	object->GetYaxis()->SetTitleOffset(2);
	object->GetZaxis()->SetTitleOffset(2);
  object->Draw("surf1");
  TString tsPath(path.c_str());
  const char* cDirName = tsPath.Data();
  if(isCanvas) {
	  c1->SetCanvasSize(700, 600);
	  c1->SetWindowSize(700, 600);
	  c1->SetGrid();
	  c1->SetGrid();
    TString c1Path((path + "/CMS/").c_str());
    const char* c1DirName = c1Path.Data();
	  this->writeObject(c1DirName, c1);
  }
	this->writeObject(cDirName, object);
  delete c1;
	return true;
}
bool DqRoot::writeDqToCanvasCMS(std::string path, DqMTGE* object, bool isCanvas)
{
	TCanvas *c1 = new TCanvas(Form("c_%s", object->GetName()), Form("%s", object->GetName()));
	//object->GetXaxis()->SetTitle(object->getTitleX().c_str());
	//object->GetYaxis()->SetTitle(object->getTitleY().c_str());
  object->Draw("AP");
  TString tsPath(path.c_str());
  const char* cDirName = tsPath.Data();
  if(isCanvas) {
	  c1->SetCanvasSize(700, 600);
	  c1->SetWindowSize(700, 600);
	  c1->SetGrid();
    TString c1Path((path + "/CMS/").c_str());
    const char* c1DirName = c1Path.Data();
	  this->writeObject(c1DirName, c1);
  }
	this->writeObject(cDirName, object);
  delete c1;
	return true;
}
bool DqRoot::writeDqToCanvasCMS(std::string path, TH1* object, TF1 *fit,  bool isCanvas)
{
  TCanvas *c1 = new TCanvas(Form("c_%s", object->GetName()), Form("%s", object->GetName()));
  object->SetFillColor(38);
  object->SetLineColor(1);
  TString tsPath(path.c_str());
  const char* cDirName = tsPath.Data();
	this->writeObject(cDirName, object);
  if(isCanvas) {
		
		if(fit != 0) { 
			object->Fit(fit, "Q");
			fit->SetTitle("");
 		}
		object->Draw();
  	object->SetFillColor(4);
		object->SetTitle("");
	  c1->SetCanvasSize(800, 600);
		c1->SetFillColor(0);
    c1->SetBorderMode(0);
    c1->SetBorderSize(2);
    c1->SetLeftMargin(0.17);
    c1->SetRightMargin(0.048);
    c1->SetTopMargin(0.07);
    c1->SetBottomMargin(0.14);
    c1->SetFrameFillStyle(0);
    c1->SetFrameBorderMode(0);
    c1->SetFrameFillStyle(0);
    c1->SetFrameBorderMode(0);
    
		c1->Update();
    TString c1Path((path + "/CMS/").c_str());
    const char* c1DirName = c1Path.Data();
	  this->writeCanvas(c1DirName, c1);
  }
	delete c1;
	return true;
}
bool DqRoot::writeDqToCanvasCMS(std::string path, TH2* object, bool isCanvas)
{
	TCanvas *c1 = new TCanvas("c_", "");
	object->Draw();
  TString tsPath(path.c_str());
  const char* cDirName = tsPath.Data();
  if(isCanvas) {
	  c1->SetCanvasSize(700, 600);
	  c1->SetWindowSize(700, 600);
	  c1->SetGrid();
    TString c1Path((path + "/CMS/").c_str());
    const char* c1DirName = c1Path.Data();
	  this->writeObject(c1DirName, c1);
  }
	this->writeObject(cDirName, object);
  delete c1;
	return true;
}
bool DqRoot::writeDqToCanvasCMS(std::string path, std::vector<TH1>* objects, bool isCanvas)
{
  
	TCanvas *c1 = new TCanvas("c_", "");
  TMultiGraph *mg = new TMultiGraph();
  for(unsigned int i = 0; i < objects->size(); i++) {
	  objects->at(i).Draw(); 
  } 
  mg->Draw();
  TString tsPath(path.c_str());
  const char* cDirName = tsPath.Data();
  if(isCanvas) {
	  c1->SetCanvasSize(700, 600);
	  c1->SetWindowSize(700, 600);
	  c1->SetGrid();
    TString c1Path((path + "/CMS/").c_str());
    const char* c1DirName = c1Path.Data();
	  this->writeObject(c1DirName, c1);
  }
	this->writeObject(cDirName, mg);
  delete mg;
	delete c1;
	return true;
}

std::map<std::string, DqTH2D>::iterator DqRoot::addDqTH2D(std::string name, std::string title, std::map<std::string, DqTH2D>* dqTH2Ds)
{
    auto it = dqTH2Ds->find(name.c_str());
    if(it == dqTH2Ds->end()) {
      int sizeX = 1000; double begX = 1; double endX = -1;
      int sizeY = 1000; double begY = 1; double endY = -1;
      dqTH2Ds->insert(std::make_pair(name, 
              DqTH2D{Form("h_%s", name.c_str()), Form("%s", title.c_str()), sizeX, begX, endX, sizeY, begY, endY}));
      it = dqTH2Ds->find(name.c_str());
    }
    return it;
}
std::map<std::string, DqTH1D>::iterator DqRoot::addDqTH1D(std::string name, std::string title, std::map<std::string, DqTH1D>* dqTH1Ds)
{
    auto it = dqTH1Ds->find(name.c_str());
    if(it == dqTH1Ds->end()) {
      int size = 1000; double beg = 1; double end = -1;
      dqTH1Ds->insert(std::make_pair(name, 
              DqTH1D{Form("h_%s", name.c_str()), Form("%s", title.c_str()), size, beg, end}));
      it = dqTH1Ds->find(name.c_str());
    }
    return it;
}
std::map<std::string, DqTGE*>::iterator DqRoot::addDqTGE(std::string name, std::string title, std::map<std::string, DqTGE*>* dqTGEs)
{
    auto it = dqTGEs->find(name.c_str());
    if(it == dqTGEs->end()) {
      dqTGEs->insert(std::make_pair(name, new DqTGE{Form("gr_%s", name.c_str()), Form("%s", title.c_str())}));
      it = dqTGEs->find(name.c_str());
    }
    return it;
}
std::map<std::string, DqTGAE*>::iterator DqRoot::addDqTGAE(std::string name, std::string title, std::map<std::string, DqTGAE*>* dqTGAEs)
{
    auto it = dqTGAEs->find(name.c_str());
    if(it == dqTGAEs->end()) {
      dqTGAEs->insert(std::make_pair(name, new DqTGAE{Form("gr_%s", name.c_str()), Form("%s", title.c_str())}));
      it = dqTGAEs->find(name.c_str());
    }
    return it;
}
std::map<std::string, DqTG2D*>::iterator DqRoot::addDqTG2D(std::string name, std::string title, std::map<std::string, DqTG2D*>* dqTG2Ds)
{
    auto it = dqTG2Ds->find(name.c_str());
    if(it == dqTG2Ds->end()) {
      dqTG2Ds->insert(std::make_pair(name, new DqTG2D{Form("gr_%s", name.c_str()), Form("%s", title.c_str())}));
      it = dqTG2Ds->find(name.c_str());
    }
    return it;
}
std::map<std::string, DqMTGE*>::iterator DqRoot::addDqMTGE(std::string name, std::string title, std::map<std::string, DqMTGE*>* dqMTGEs)
{
    auto it = dqMTGEs->find(name.c_str());
    if(it == dqMTGEs->end()) {
      dqMTGEs->insert(std::make_pair(name, new DqMTGE{Form("gr_%s", name.c_str()), Form("%s", title.c_str())}));
      it = dqMTGEs->find(name.c_str());
    }
    return it;
}

//-----
//------------------------------------------------------------------------------
