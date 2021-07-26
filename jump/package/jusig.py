import sys
from scipy.signal import find_peaks, peak_widths
import pyfftw
from array import array
import numpy as np
from scipy import signal

import ROOT
from ROOT import gROOT
from ROOT import gStyle

gROOT.LoadMacro("../style/CMS/tdrstyle.C")
gROOT.ProcessLine("setTDRStyle();")
gROOT.LoadMacro("../style/CMS/CMS_lumi.C")

class Signal(object):
    
    ch = 0;
    card = 0;
    event = 0;
  
    pol = 0;
    window = 0;
    thr = 0;
    peak = 0;
    unequal = 0;
  
    signal = []; 
    pedestal = []; 

def subSpectrum(signal, pedestal):
    s = pyfftw.interfaces.numpy_fft.fft(signal)
    ss = np.abs(s)
    angle = np.angle(s)
    b = np.exp(1.0j* angle)

    ns= pyfftw.interfaces.numpy_fft.fft(pedestal)
    nss = np.abs(ns)
    mns = np.mean(nss)

    sa= ss - nss
    sa0= sa * b 
    
    result = pyfftw.interfaces.numpy_fft.ifft(sa0).real
    return np.around(result)

def getProfs(ju_signals, window, cards, chs, pol, offset = 0, aling = 1):
    
    size = window
    th2d_sig = ROOT.TH2D("profile_" + str(pol), "profile_" + str(pol), size, 0, size, 4096, 0, 4096)
    th2d_ped = ROOT.TH2D("pedestal_" + str(pol), "pedestal_" + str(pol), size, 0, size, 4096, 0, 4096)
    
    isRef = 1; ped_ref = []; sig_ref = []
    for sig in ju_signals:
        
        if(not(sig.card in cards)): continue
        if(not(sig.ch in chs)): continue
        if(sig.unequal == 1): continue
        if(sig.pol != pol): continue
        if(sig.peak < offset): continue
        
        if(isRef == 1): 
            ped_ref = sig.pedestal
            sig_ref = sig.signal
            size = len(sig.signal)
            isRef = 0
        
        sig_tmp = sig.signal; ped_tmp = sig.pedestal; 
        if(aling == 1): 
            conv = signal.convolve(sig_ref, sig.result, mode='same')
            index = np.argmax(conv); sig_tmp = np.roll(sig.result, int(size/2)-index)
            conv = signal.convolve(ped_ref, sig.pedestal, mode='same')
            index = np.argmax(conv); ped_tmp = np.roll(sig.pedestal, int(size/2)-index)
        
        for s in range(len(sig_tmp)):
            th2d_sig.Fill(s, sig_tmp[s])
        for s in range(len(ped_tmp)):
            th2d_ped.Fill(s, ped_tmp[s])
        
    return th2d_sig, th2d_ped
