import ROOT
from ROOT import gROOT
from ROOT import gStyle
import sys

ROOT.gInterpreter.AddIncludePath('../include/')
ROOT.gInterpreter.Declare('#include "RoEvent.h"')
ROOT.gSystem.Load('../libs/libRoEvent.so')

f = ROOT.TFile(sys.argv[1])
myTree = f.Get("data")

#myTree.Print()
myTree.Show(1)
