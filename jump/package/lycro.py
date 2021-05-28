from scipy.signal import find_peaks, peak_widths
import pyfftw
from array import array
import numpy as np

class Signal(object):
    
    ev = 0 
    ch = 0
    
    # Signal
    polarity = 0
    threshold = 0 
    
    peak_index = 0
    peak_value = 0
    
    integral = 0
    width = 0
    
    data = []
    pedestal = []
    
    peaks = []
   
    # Analysis
    unequal = 0

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
    return result

def alignment(data, level = 0):
    fft = pyfftw.interfaces.numpy_fft.fft(data)
    fft[0] = level 
    ifft = pyfftw.interfaces.numpy_fft.ifft(fft).real
    return ifft

# ---
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

def findSignals(ev, ch, data, thr = 70, pad = 30, pol = 1):
	output = []
	data = pol*data
	raw_peaks, _ = find_peaks(data, height = thr)
	groups = dict(enumerate(grouper(raw_peaks, 2*pad), 1))
	peaks = []
	for gr, group in groups.items():
	    ind = np.argmax(data[group])
	    peaks.append(group[ind])
	for p in range(len(peaks)):
	    sig = Signal()
	    sig.ev = ev 
	    sig.ch = ch
	    sig.peaks = peaks # all peaks inside 2*pad
	    sig.peak_index = peaks[p]
	    sig.peak_value = data[peaks[p]]
	    sig.threshold = thr
	    sig.polarity = pol
	    sig.integral = sum(data[peaks[p] - pad: peaks[p] + pad])
	    sig.data = data[peaks[p] - pad: peaks[p] + pad]
	    p_peaks, _ = find_peaks(data[peaks[p] - pad - 2*pad : peaks[p] - pad], height = thr)
	    if(len(p_peaks) == 2*pad):
	        sig.pedestal = data[peaks[p] - pad - 2*pad : peaks[p] - pad]
	    output.append(sig)
	return output

# ---

def findCRO(path, ev, ch):

	#get data from RoEvent class (to generate it use dq script)
	f = ROOT.TFile(path)
	myTree = f.Get("data")
	
	cro = []
	
	numEv = myTree.GetEntries()
	ev_count = 0
	ev_index = ev - 1
	isBreak = False
	while(ev_count < numEv and not(isBreak)):
	    if(ev_index >= numEv):
	        ev_index = 0;
	    myTree.GetEntry(ev - 1)
	    ev = myTree.events.getHeader().getEvtNum()
	    #print(myTree.events.getHeader().getEvtNum())
	    if(ev == myTree.events.getHeader().getEvtNum()):
	        numCh = myTree.events.getNumPayloads()
	        ch_count = 0
	        ch_index = ch
	        while(ch_count < numCh):
	            if(ch_index >= numCh):
	                ch_index = 0;
	            #print(myTree.events.getPayloads().AddrAt(ch_index).getCh())
	            if(ch == myTree.events.getPayloads().AddrAt(ch_index).getCh()):
	                cro = myTree.events.getPayloads().AddrAt(ch_index).getCRO()
	                numSa = myTree.events.getPayloads().AddrAt(ch_index).getNsaCRO()
	                isBreak = True 
	                break;
	            else:
	                ch_count += 1
	                ch_index += 1
	    else:
	        ev_count += 1
	        ev_index += 1
	return cro 
