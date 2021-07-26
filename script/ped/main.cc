#include <string>

#include "LogMsg.h"
#include "EventDecoder.h"
#include "ChannelMap.h"

#include "TTree.h"
#include "TFile.h"

using namespace np02daq;

#include <iostream>
#include <math.h>
using namespace std;

#include <argparse.h>
#include "DqProcessBar.h"

#include "RoEvent.h"
#include "DqPeaks.h"
#include <fftw3.h>
int main(int argc, char** argv)
{
  
  argparse::ArgumentParser program("main");
  
  program.add_argument("-input").help("The input files.").nargs(1);
  program.add_argument("-N").help("Number ofinput files.").default_value("0");
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
  
  auto finputs = program.get<std::vector<std::string>>("-input");  
  auto foutputs = program.get<std::vector<std::string>>("-output");  
  auto map = program.get<std::string>("-m");
  int N = std::stoi(program.get<std::string>("-N"));

  std::string foutput  = foutputs.back();
  TFile f(foutput.c_str(),"recreate");
  
  for(int n = 0; n < N; n++) {
    std::string str = finputs.back().substr(0, finputs.back().find("_s", 0)) + "_s" + std::to_string(n+1) + ".dat";
    finputs.push_back(str);
  }

  string cmapname = map;
  vector<string> chopts;
  ChannelMap *cmap = &ChannelMap::Instance();
  cmap->init(cmapname);
  vector<ChannelId> chans;
  
  chans = cmap->find_by_seqn(0, cmap->ntot()); // only all chs.
  
  int window = 100; int pad = int(window/2); int threshold = 80; int offset = 10000; int fluctuations = int(threshold/3); int pol = 1; int ref = 2048; 
  
  DqPeaks peak;
  
  RoSig roSig;
  
  RoSig roSigPed;
  TTree tree("ped", "ped");
  tree.Branch("sigs", "sigs", &roSigPed);
  tree.SetAutoSave(0);
  tree.SetAutoFlush(0);
  
  //std::string finput = finputs.back();

  std::cout << "N: " << finputs.size() << " file(s) have to be processed!" << std::endl;
  for(int in = 0; in < finputs.size(); in++) {
    std::cout << "Processing " << in << ":" << finputs.size() - 1 <<  " file!" << std::endl;
    EventDecoder decoder;
    if( !decoder.Open(finputs.at(in)) ) {
        std::cout << "Cannot open " << finputs.at(in) << endl;
        continue;
    }
    
    unsigned nev = decoder.Nev();
    
    // event loop
    
    DqProcessBar processBar;
    processBar.setEndIndex(nev);
    processBar.setCurrentStep(0);

    for(unsigned ev = 0; ev < nev; ev++) {
      
      const DaqEvent event = decoder.Get(ev);
      
      roSig.event = event.evnum;
      
      if(!event.valid) continue;
      
      unsigned nsa = event.nsacro;
      for(auto &ch: chans) {
        
        roSig.ch = ch.seqn();
        roSig.card = ch.card();

        auto begin = event.crodata.begin() + ch.seqn() * nsa;
	      auto end   = begin + nsa;
        
        unsigned tick = 0;  
        std::vector<int> cro; cro.insert(cro.end(), begin, end);
          
		    std::vector<int> empty;
		    std::vector<int> ped = peak.findPedestal(&cro, empty, window, fluctuations, offset);
        if(ped.size() == 0)  { 
          roSig.unequal = 1;
          roSig.pol = 0;
          roSigPed = roSig;
          tree.Fill(); roSigPed.Clear(); roSig.Clear();
          continue;
        }
        roSig.event = event.evnum;
        roSig.ch = ch.seqn();
        roSig.pedestal = ped; 
        tree.Fill(); roSigPed.Clear(); roSig.Clear();
        cro.clear();
      }
    
      if(ev < nev - 1) processBar.print(ev, 0.02);
      else { processBar.print(nev, 0.02);  std::cout << std::endl; } 
    
    }
  } 
  f.Write(); f.Close();
  
  return 0;
}
