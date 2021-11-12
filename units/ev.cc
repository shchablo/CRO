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
  
  int window = 45; int pad1 = 10; int pad2 = 35; int threshold = 200; int offset = 10000; int fluctuations = int(threshold/3); int pol = 1; int ref = 2048; 
  
  DqPeaks peak;
  
  RoSigHeader roSigHeader;
  TTree treeHeader(Form("header_thr%d_wind%d", threshold, window) , Form("header_thr%d_wind%d", threshold, window));
  treeHeader.Branch("main", "main", &roSigHeader);
  
  RoSig roSig;
  
  RoSig roSig;
  TTree treePos(Form("thr%d_wind%d", threshold, window) , Form("thr%d_wind%d", threshold, window));
  treePos.Branch("sigs", "sigs", &roSigPos);
  treePos.SetAutoSave(0);
  treePos.SetAutoFlush(0);

  //std::string finput = finputs.back();
  
  int maxNev = 0;
  std::cout << "N: " << finputs.size() << " file(s) have to be processed!" << std::endl;
  for(int in = 0; in < finputs.size(); in++) {
    std::cout << "Processing " << in << ":" << finputs.size() - 1 <<  " file!" << std::endl;
    EventDecoder decoder;
    if( !decoder.Open(finputs.at(in)) ) {
        std::cout << "Cannot open " << finputs.at(in) << endl;
        continue;
    }
    
    unsigned nev = decoder.Nev();
    maxNev += nev;
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
          roSigWrong = roSig;
          treeWrong.Fill(); roSigWrong.Clear(); roSig.Clear();
          continue;
        }
        int level = peak.findLevel(&ped);
        pol = 1; std::vector<int> peaks_pos = peak.findPeaks(&cro, level, pol, threshold);
        pol = -1; std::vector<int> peaks_neg = peak.findPeaks(&cro, level, pol, threshold);
        int max = 0;
        for(int i = 0; i < peaks_pos.size(); i++) {
          if(max < peaks_pos.at(i)) max = peaks_pos.at(i);
        }
        for(int i = 0; i < peaks_neg.size(); i++) {
          if(max < peaks_neg.at(i)) max = peaks_neg.at(i);
        }
        std::vector<int> peaks; 
        peaks.insert(peaks.end(), peaks_pos.begin(), peaks_pos.end());
        peaks.insert(peaks.end(), peaks_neg.begin(), peaks_neg.end());
        roSig.ch = ch.seqn();
        roSig.card = ch.card();
        roSig.level = level;
        if(roSig.pol == 1) roSigPos = roSig;
        treePos.Fill(); roSigPos.Clear(); roSig.Clear();
        cro.clear();
      }
    
      if(ev < nev - 1) processBar.print(ev, 0.02);
      else { processBar.print(nev, 0.02);  std::cout << std::endl; } 
    
    }
  }
  roSigHeader.numev = maxNev;
  treeHeader.Fill();
  f.Write(); f.Close();
  
  return 0;
}
