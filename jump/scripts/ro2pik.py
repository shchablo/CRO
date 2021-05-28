import sys
import ROOT
import pyfftw
import argparse

ROOT.gInterpreter.AddIncludePath('../../include/')
ROOT.gInterpreter.Declare('#include "RoEvent.h"')
ROOT.gSystem.Load('../../libs/libRoEvent.so')

import pickle
from array import array
sys.path.append("../package/")
import lycro 


def main():

  run = "602"
  thr = 70; pad = 30
  root = "../../tmp/ro_r" + run + ".root"
  pick = "../../tmp/ju_r" + run + "_thr-" + str(thr) + "_pad-" + str(pad) + ".pik"
  print("ROOT file: {}".format(root))
  print("PICK file: {}".format(pick))

  #get data from RoEvent class (to generate it use dq script)
  f = ROOT.TFile(root)
  myTree = f.Get("data")

  #myTree.Print()
  #myTree.Show(560)

  #loop over chs
  events = []

  print(myTree.GetEntries())
  myTree.GetEntry(570)
  print(myTree.events.getHeader().getEvtNum())

  for entry in myTree:
    ro_signals = []
    events = entry.events
    numCh = events.getNumPayloads()
    ev = events.getHeader().getEvtNum()
    print("Ev {} is processing... ".format(ev), end='\r')
    for n in range(numCh):
      ch = events.getPayloads().AddrAt(n).getCh()
      cro = events.getPayloads().AddrAt(n).getCRO()
      numSa = events.getPayloads().AddrAt(n).getNsaCRO()
      fft = pyfftw.interfaces.numpy_fft.fft(cro)
      #data = array('i')
      #for ns in range(numSa): 
      #    data.append(int(cro[ns]));
      #data = lycro.alignment(cro, 0)
      #ev_signals_pos = lycro.findSignals(data, thr=thr, pad=pad, pol = 1)
      #ev_signals_neg = lycro.findSignals(data, thr=thr, pad=pad, pol = -1)
      #print(data)
      #ro_signals.extend(ev_signals_pos)
      #ro_signals.extend(ev_signals_neg)
    
parser = argparse.ArgumentParser();
parser = argparse.ArgumentParser(description='This is the script that tries to find picks of signals by the threshold in a tree (roCRO event) and save the peak file.')
parser.add_argument("-in", "--input", type=str, default="", required=False, help='An input file with tree of roCro classes.')
parser.add_argument("-out", "--output", type=str, default="", required=False, help='An output file.')
parser.add_argument("-thr", "--threshold", type=float, default=70, required=False, help='The threshold from the pedestal (pedestal alignment to 0) (ADCu).')
parser.add_argument("-pad", "--pad", type=float, default=30, required=False, help='window = [peak - pad; peak + pad].')

args = parser.parse_args(sys.argv[1:])

main(**vars(args)) 
