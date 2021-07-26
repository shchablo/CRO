#include <string>

#include "LogMsg.h"
#include "EventDecoder.h"
#include "ChannelMap.h"

#include "RoEvent.h"

#include "TTree.h"
#include "TFile.h"

using namespace np02daq;

#include <iostream>
using namespace std;

#include <argparse.h>
#include "DqProcessBar.h"

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
  
  RoEvent roEvent(chans.size()); // numPayloads = max numChannels
  TTree tree("data", "data");
  tree.Branch("events", "events", &roEvent);
  tree.SetAutoSave(0);
  tree.SetAutoFlush(0);
  
  //std::string finput = finputs.back();

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

    for(unsigned ev = 0; ev < nev; ev++) {
      
      const DaqEvent event = decoder.Get(ev);
      
      roEvent.getHeader()->setEvtNum(event.evnum);
      roEvent.getHeader()->setValid(event.valid);
      roEvent.getHeader()->setGood(event.good);
      roEvent.getHeader()->setRunNum(event.runnum);
      roEvent.getHeader()->setTrigNum(event.trignum);
      roEvent.getHeader()->setTvNsec(event.trigstamp.tv_nsec);
      
      if(!event.valid) continue;
      
      unsigned nsa = event.nsacro;
      for(auto &ch: chans) {
        
        roEvent.addPayload()->init(ch.seqn(), nsa, 0);  // loop over boards new board %= ch/64 
        
        roEvent.getLastPayload()->getMap()->setSeqn(ch.seqn()); 
        roEvent.getLastPayload()->getMap()->setCrate(ch.crate()); 
        roEvent.getLastPayload()->getMap()->setCard(ch.card()); 
        roEvent.getLastPayload()->getMap()->setCardCh(ch.cardch()); 
        roEvent.getLastPayload()->getMap()->setCrp(ch.crp()); 
        roEvent.getLastPayload()->getMap()->setView(ch.view()); 
        roEvent.getLastPayload()->getMap()->setViewCh(ch.viewch()); 
        roEvent.getLastPayload()->getMap()->setState(ch.state()); 
        roEvent.getLastPayload()->getMap()->setExists(ch.exists()); 

        auto begin = event.crodata.begin() + ch.seqn() * nsa;
	      auto end   = begin + nsa;
        
        unsigned tick = 0;  
        //std::cout << nsa << std::endl;
        for(auto it = begin; it != end; ++it)
          roEvent.getLastPayload()->addTickCRO(*it, tick++);
        roEvent.getLastPayload()->setNsaCRO(tick);
        // LRO
      }
		  
      tree.Fill(); roEvent.Clear("C");
    
      if(ev < nev - 1) processBar.print(ev, 0.02);
      else { processBar.print(nev, 0.02);  std::cout << std::endl; } 
    
    }
  } 
  f.Write(); f.Close();
  
  return 0;
}
