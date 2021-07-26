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


def main(run, root, path, threshold, pad):

thr = threshold
pick = + path + "/ju_" + run + "_thr-" + str(thr) + "_pad-" + str(pad) + ".pik"
print("ROOT file: {}".format(root))
print("PICK file: {}".format(pick))

f = ROOT.TFile(root)
myTree = f.Get("data")

#loking signals
ro_signals = []
numEv = myTree.GetEntries()
for entry in myTree:
    events = entry.events
    numCh = events.getNumPayloads()
    ev = events.getHeader().getEvtNum()
    print("Ev {}:{} is processing... ".format(ev, numEv), end='\r')
    for n in range(numCh):
        ch = events.getPayloads().AddrAt(n).getCh()
        cro = events.getPayloads().AddrAt(n).getCRO()
        numSa = events.getPayloads().AddrAt(n).getNsaCRO()
        data = lycro.alignment(cro, 0)
        ev_signals_pos = lycro.findSignals(ev, ch, data, thr, pad, 1)
        ev_signals_neg = lycro.findSignals(ev, ch, data, thr, pad, -1)
        ro_signals.extend(ev_signals_pos)
        ro_signals.extend(ev_signals_neg)

with open(pick, "wb") as f:
    pickle.dump(len(ro_signals), f)
    for value in ro_signals:
        pickle.dump(value, f)

parser = argparse.ArgumentParser();
parser = argparse.ArgumentParser(description='This is the script that tries to find picks of signals by the threshold in a tree (roCRO event) and save the peak file.')
parser.add_argument("-run", "--run", type=str, default="", required=False, help='An run number..')
parser.add_argument("-in", "--root", type=str, default="", required=False, help='An input file with tree of roCro classes.')
parser.add_argument("-out", "--path", type=str, default="", required=False, help='An path to folder of an output file.')
parser.add_argument("-thr", "--threshold", type=float, default=70, required=False, help='The threshold from the pedestal (pedestal alignment to 0) (ADCu).')
parser.add_argument("-pad", "--pad", type=float, default=30, required=False, help='window = [peak - pad; peak + pad].')

args = parser.parse_args(sys.argv[1:])

main(**vars(args)) 
