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

int main(int argc, char** argv)
{
  
  argparse::ArgumentParser program("main");
  
  program.add_argument("-input").help("The input files.").nargs(1);
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

  std::string foutput  = foutputs.back();
  TFile f(foutput.c_str(),"recreate");
  
  std::string finput = finputs.back();
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
  
  EventDecoder decoder;
  if( !decoder.Open(finput) ) {
      cout << "Cannot open " << finput << endl;
      return 0;
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
      
      roEvent.addPayload()->init(ch.seqn(), event.crodata.size(), event.lrodata.size());  // loop over boards new board %= ch/64
      
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
      for(auto it = begin; it != end; ++it)
        roEvent.getLastPayload()->addTickCRO(*it, tick++);
      roEvent.getLastPayload()->setNsaCRO(tick);
      // LRO
    }
    
    tree.Fill(); roEvent.Clear("C");
  
    if(ev < nev - 1) processBar.print(ev, 0.02);
    else { processBar.print(nev, 0.02);  std::cout << std::endl; } 
  
  }
  
  f.Write(); f.Close();
  
  return 0;
}
