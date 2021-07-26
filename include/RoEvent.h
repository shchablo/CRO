#ifndef ROEVENT_H
#define ROEVENT_H

/* ROOT includes */
#include <TROOT.h>
#include <TArrayI.h>
#include "TObject.h"
#include "TClonesArray.h"

//------------------------------------------------------------------------------

class RoHeader : public TObject {

private:
  
  UInt_t    fEvtNum;
  Bool_t    fValid;
  Bool_t    fGood;
  UShort_t  fFlags[8];
  
  UInt_t    fRunNum;
  UShort_t  fRunFlags;
  
  UShort_t  fTrigType;
  UInt_t    fTrigNum;
  Long64_t  fTvNsec;
  
public:
  
  RoHeader() {;};
  virtual ~RoHeader() {;};
  
  UInt_t    getEvtNum() { return fEvtNum; }
  Bool_t    getValid() { return fValid; }
  Bool_t    getGood() { return fGood; }
  UShort_t  *getFlags() { return fFlags; }
  UInt_t    getRunNum() { return fRunNum; }  
  UShort_t  getRunFlags() { return fRunFlags; }  
  UShort_t  getTrigType() { return fTrigType; }  
  UInt_t    getTrigNum() { return fTrigNum; }  
  Long64_t  getTvNsec() { return fTvNsec; }
  
  void  setEvtNum(UInt_t n) { fEvtNum = n; }
  void  setValid(Bool_t valid) { fValid = valid; }
  void  setGood(Bool_t good) { fGood = good; }
  void  setFlags(UShort_t *flags) { for(UInt_t i = 0; i < 8; i++) fFlags[i] = flags[i]; }
  void  setRunNum(UInt_t runNum) { fRunNum = runNum; }
  void  setRunFlags(UShort_t runFlags) { fRunFlags = runFlags; }
  void  setTrigType(UShort_t trigType) { fTrigType = trigType; }
  void  setTrigNum(UInt_t trigNum) { fTrigNum = trigNum; }
  void  setTvNsec(Long64_t tvNsec) { fTvNsec = tvNsec; }
  
  void Clear(Option_t * option ) { TObject::Clear(option); };
  
  ClassDef(RoHeader, 1)
};

class RoMap : public TObject {

private:

  UInt_t    fSeqn;
  UShort_t  fCrate;
  UShort_t  fCard;
  UShort_t  fCardCh;
  UShort_t  fCrp;
  UShort_t  fView;
  UShort_t  fViewCh;
  UShort_t  fState;
  Bool_t    fExists;

public:

  RoMap() {;};
  virtual ~RoMap() {;};

  UInt_t getSeqn() { return fSeqn; }
  UShort_t getCrate() { return fCrate; }
  UShort_t getCard() { return fCard; } 
  UShort_t getCardCh() { return fCardCh; }
  UShort_t getCrp() { return fCrp; }
  UShort_t getView() { return fView; }
  UShort_t getViewCh() { return fViewCh; } 
  UShort_t getState() { return fState; } 
  Bool_t geExists() { return fExists; } 

  void  setSeqn(UInt_t seqn) { fSeqn = seqn; }
  void  setCrate(UShort_t crate) { fCrate = crate; }
  void  setCard(UShort_t card) { fCard = card; }
  void  setCardCh(UShort_t cardCh) { fCardCh = cardCh; }
  void  setCrp(UShort_t crp) { fCrp = crp; }
  void  setView(UShort_t view) { fView = view; }
  void  setViewCh(UShort_t viewCh) { fViewCh = viewCh; }
  void  setState(UShort_t state) { fState = state; }
  void  setExists(Bool_t exists) { fExists = exists; }
  
  void Clear(Option_t * option ) { TObject::Clear(option); };
  
  ClassDef(RoMap, 1)
};

class RoPayload : public TObject {

private:
  
  UInt_t    fCh;
  RoMap     fMap;

  UInt_t    fNsaCRO;
  TArrayI   fCRO; 
  //Int_t fCRO[10000]; 
  
  UInt_t    fNsaLRO;
  TArrayI   fLRO; 
  //Int_t fLRO[10000]; 

public:
  
  RoPayload() {;};
  virtual ~RoPayload() {;};
  
  void init(UInt_t ch, UInt_t nsaCRO, UInt_t nsaLRO) {
    fCh = ch; fNsaCRO = nsaCRO; fNsaLRO = nsaLRO;  
    fCRO.Set(fNsaCRO); for(UInt_t tick = 0; tick < fNsaCRO; tick++) fCRO.AddAt(-1, tick); 
    fLRO.Set(fNsaLRO); for(UInt_t tick = 0; tick < fNsaLRO; tick++) fLRO.AddAt(-1, tick); 
  }
  void addTickCRO(Int_t digi, Int_t tick) { fCRO.AddAt(digi, tick); };
  void addTickLRO(Int_t digi, Int_t tick) { fLRO.AddAt(digi, tick); };
  //void addTickCRO(Int_t digi, Int_t tick) { fCRO[tick] = digi; };
  //void addTickLRO(Int_t digi, Int_t tick) { fLRO[tick] = digi; };
  
  UInt_t getCh() { return fCh; }
  RoMap *getMap() { return &fMap; }
  
  UInt_t getNsaCRO() { return fNsaCRO; }
  void setNsaCRO(UInt_t nsaCRO) { fNsaCRO = nsaCRO;  }
  TArrayI getCRO() { return fCRO; }
  //Int_t *getCRO() { return fCRO; }
  
  UInt_t getNsaLRO() { return fNsaLRO; }
  void setNsaLRO(UInt_t nsaLRO) { fNsaLRO = nsaLRO;  }
  TArrayI getLRO() { return fLRO; }
  //Int_t *getLRO() { return fLRO; }
  
  void Clear(Option_t * option ) { fCh = 0; fNsaCRO = 0; fCRO.Reset(); fNsaLRO = 0; fLRO.Reset(); fMap.Clear(option); TObject::Clear(option); };
  //void Clear(Option_t *option ) { fCh = 0; fNsaCRO = 0; fNsaLRO = 0;  fMap.Clear(option); TObject::Clear(option); };
   
   ClassDef(RoPayload, 1) 
};

class RoEvent : public TObject {

private:
   
  RoHeader fEvtHdr;
   
  Int_t fNumPayloads; TClonesArray *fPayloads;
  static TClonesArray *fgPls;

public :
  
  RoEvent(Int_t numPayloads = 64) {
    if(!fgPls) fgPls = new TClonesArray("RoPayload", numPayloads); 
    fPayloads = fgPls; fNumPayloads = 0;
  };

  virtual ~RoEvent() {;};
   
  RoHeader *getHeader() { return &fEvtHdr; }
  TClonesArray *getPayloads() const { return fPayloads; }
  Int_t getNumPayloads() { return fNumPayloads; }
  
  RoPayload *addPayload() { 
    fPayloads->Expand(fNumPayloads + 1); 
    RoPayload *it = (RoPayload*)fPayloads->ConstructedAt(fNumPayloads++); 
    return it; 
  };
  RoPayload *getLastPayload() { 
    RoPayload *it = (RoPayload*)fPayloads->AddrAt(fNumPayloads - 1); 
    return it; 
  };
  
  void Clear(Option_t * option) {  fNumPayloads = 0; fEvtHdr.Clear(option); fPayloads->Clear(option); };
  
  ClassDef(RoEvent, 1)
};


class RoSigHeader: public TObject {

public:
  
  int numev;

  RoSigHeader() {;};
  virtual ~RoSigHeader() {;};
   
  void Clear() {;}; 
  ClassDef(RoSigHeader, 1)
};
class RoSig: public TObject {

public:
  
  int ch;
  int card;
  int event;
  //int isEvent;
  
  int pol;
  int window;
  int thr;
  int peak;
  int unequal;
  
  std::vector<int> signal; 
  std::vector<int> pedestal;
  std::vector<int> peaks;

  int level; 
  int sum;
  int height;

  RoSig() {;};
  virtual ~RoSig() {;};
   
  void Clear() {
    ch = 0;
    card = 0;
    event = 0;
    
    pol = 0;
    window = 0;
    thr = 0;
    peak = 0;
    unequal = 0;
    level = 0; 
    sum = 0;
    height = 0;
    signal.clear(); 
    pedestal.clear(); 
    peaks.clear(); };
  
  ClassDef(RoSig, 1)
};

#endif // ROEVENT_H
