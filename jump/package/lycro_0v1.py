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
    
    run = 0 ; ev = 0 
    card = 0; ch = 0
    
    # Signal
    polarity = 0
    threshold = 0 
    
    peak_index = 0
    peak_value = 0
    
    signal = []
    pedestal = []
    
    height = 0 
    integral = 0
    
    level = 0
    result = []
    resample = []
    
    # Analysis
    unequal = 0

def findPedestal(data, window = 30, thr = 10, offset = 5000):
    
    isFind = False; index = offset; limit = len(data) - window; ped = []
    while(not isFind and index < limit):
        ped = data[index: index + window]
        if(np.max(ped) - np.min(ped) >= thr):
            index += 1
        else:
            isFind = True
    return ped 
# ---

def findRelativePedestal(data, window = 30, thr = 10, offset = 5000):
   
    beg = 0; end = 0
    isFind = False; index = offset; ped = [];
    while(not isFind and index > window):
        ped = data[index-window: index]
        if(np.max(ped) - np.min(ped) >= thr):
            index -= 1
        else:
            isFind = True
    beg = index - window; end = index
    return beg, end

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
    
    sa0[0] = 0 # level 
    
    result = pyfftw.interfaces.numpy_fft.ifft(sa0).real
    return np.around(result)

def grouper(iterable, pad):
    prev = None
    group = []
    for item in iterable:
        if not prev or item - prev <= pad:
            group.append(item)
        else:
            yield group
            group = [item]
        prev = item
    if group:
        yield group
    return group

def findSignals(cro, level, thr = 70, window = 30, pol = 1, run = 0, ev = 0, card = 0, ch = 0):
    
    output = []
    pad = int(window/2)
    
    data = pol*(cro - level)
    
    peaks = []
    raw_peaks, _ = find_peaks(data, height = thr)
    groups = dict(enumerate(grouper(raw_peaks, 2*pad), 1))
    for gr, group in groups.items():
        ind = np.argmax(data[group])
        peaks.append(group[ind])
    for p in range(len(peaks)):
        sig = Signal()
        
        sig.run = run 
        sig.ev = ev
        sig.card = card
        sig.ch = ch
        
        sig.threshold = thr
        sig.polarity = pol

        sig.signal = cro[peaks[p] - pad: peaks[p] + pad]
        sig.level = level
        sig.peak_index = peaks[p]
        sig.peak_value = cro[peaks[p]]
        
        beg, end = findRelativePedestal(data, window, int(thr/3), peaks[p])
        sig.pedestal = cro[beg: end];
        if(len(sig.pedestal) != len(sig.signal)): 
            sig.unequal = 1
            continue
        
        #conv = signal.convolve(ped, sig.data, mode='same')
        #index = np.argmax(conv); ped = np.roll(ped, pad-index)
        
        #sig.result = subSpectrum(sig.data, ped)
        #sig.resample = signal.resample(sig.result, window*10) 
        
        #sig.integral = np.sum(sig.result)
        #sig.height  = np.max(sig.resample)

        output.append(sig)
    
    return output
#---

def getProfs(ju_signals, window, cards, chs, pol, offset = 0, aling = 1):
    
    size = window
    th2d_sig = ROOT.TH2D("profile_" + str(pol), "profile_" + str(pol), size, 0, size, 4096, 0, 4096)
    th2d_ped = ROOT.TH2D("pedestal_" + str(pol), "pedestal_" + str(pol), size, 0, size, 4096, 0, 4096)
    
    isRef = 1; ped_ref = []; sig_ref = []
    for sig in ju_signals:
        
        if(not(sig.card in cards)): continue
        if(not(sig.ch in chs)): continue
        if(sig.unequal == 1): continue
        if(sig.polarity != pol): continue
        if(sig.peak_index < offset): continue
        
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
