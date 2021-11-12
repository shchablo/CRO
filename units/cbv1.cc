#include <string>

#include "LogMsg.h"
#include "EventDecoder.h"
#include "ChannelMap.h"

#include "RoEvent.h"
#include "DqEvent.h"

#include "TTree.h"
#include "TFile.h"

using namespace np02daq;

#include <iostream>
using namespace std;

#include <argparse.h>
#include "DqProcessBar.h"

#include <fftw3.h>

#include <filesystem>
namespace fs = std::filesystem;


float *conv(float *A, float *B, int lenA, int lenB, int *lenC)
{
  int nconv;
  int i, j, i1;
  float tmp;
  float *C;

  // allocated convolution array 
  nconv = lenA+lenB-1;
  C = (float*) calloc(nconv, sizeof(float));

  //convolution process
  for (i=0; i<nconv; i++)
  {
    i1 = i;
    tmp = 0.0;
    for (j=0; j<lenB; j++)
    {
      if(i1>=0 && i1<lenA)
        tmp = tmp + (A[i1]*B[j]);

      i1 = i1-1;
      C[i] = tmp;
    }
  }

  //get length of convolution array 
  (*lenC) = nconv;

  //return convolution array
  return(C);
}



int main(int argc, char** argv)
{
  
  argparse::ArgumentParser program("main");
  
  program.add_argument("-path").help("Path to raw data.").default_value("../data");
  program.add_argument("-N").help("Number of max raw files.").default_value("1");
  program.add_argument("-output").help("The output files.").nargs(1);
  program.add_argument("-m").help("The map of channels.").default_value("1:1");
  
  try {
    program.parse_args(argc, argv);
  }
  catch (const std::runtime_error& err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(0);
  }
  
  auto foutputs = program.get<std::vector<std::string>>("-output");  
  auto map = program.get<std::string>("-m");
  auto path = program.get<std::string>("-path");
  int N = std::stoi(program.get<std::string>("-N"));
  
  std::string foutput = foutputs.back();
  TFile f(foutput.c_str(),"recreate");
  
  std::vector<std::string> finputs; 
  for(const auto & entry : fs::directory_iterator(path)) {
    finputs.push_back(entry.path());
    if(finputs.size() == N) break;
  }

  string cmapname = "vdcblcrp";
  vector<string> chopts;
  ChannelMap *cmap = &ChannelMap::Instance();
  cmap->init(cmapname);
  vector<ChannelId> chans;
  
  chans = cmap->find_by_seqn(0, cmap->ntot()); // only all chs.
  std::cout << cmap->ntot() << std::endl;
  std::cout << "N: " << finputs.size() << " file(s) have to be processed!" << std::endl;
  for(int in = 0; in < finputs.size(); in++) {
    std::cout << "Processing " << in << ":" << finputs.size() - 1 <<  " file!" << std::endl;
    EventDecoder decoder;
    if( !decoder.Open(finputs.at(in)) ) {
        cout << "Cannot open " << finputs.at(in) << endl;
        continue;
    }
    
    unsigned nev = decoder.Nev();
    
    // event loop
    
    DqProcessBar processBar;
    processBar.setEndIndex(nev);
    processBar.setCurrentStep(0);

    std::map<int, DqEvent> views;
    for(int v = 0; v < 3; v++ ) {
      DqEvent e; views.insert(std::make_pair(v, e));
    }
    for(unsigned ev = 0; ev < nev; ev++) {
      
      const DaqEvent event = decoder.Get(ev);
      
      if(!event.valid) continue;
      
      unsigned nsa = event.nsacro;
      std::vector<float> avg(10000, 0);
      std::vector<std::vector<float>> tmps;
      for(auto &ch: chans) {
        
        auto begin = event.crodata.begin() + ch.seqn() * nsa;
	      auto end   = begin + nsa;
        
        unsigned tick = 0;  

        std::vector<double> tmp; std::copy(begin, end, back_inserter(tmp));
        views.find(ch.view())->second.fill(ch.seqn(), ch.viewch(), &tmp); 
      }
      
      for(int v = 0; v < 3; v++) {  
        views.find(v)->second.coherent();
      } 
      
      for(int v = 0; v < 3; v++) views.find(v)->second.clear(); 
		  
      if(ev < nev - 1) processBar.print(ev, 0.02);
      else { processBar.print(nev, 0.02);  std::cout << std::endl; } 
    
    }
  } 
  f.Write(); f.Close();
  
  return 0;
}
