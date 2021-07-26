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
    run = 0 
    ev = 0 # run -1 is pedestal 
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
    overlap = 0
    nBin = 0

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
    ped = data[::-1]; p_peaks = [1]
    ped = np.roll(ped, -1*peaks[p])
    step = 0; beg = 0; end = 0;
    while(len(p_peaks) > 0 and step < (len(ped) - 2*pad)):
       beg = step + pad 
       end = step + 3*pad
       p_peaks, _ = find_peaks(ped[beg : end], height = thr)
       step += 1;
    sig.pedestal = ped[beg : end]
    output.append(sig)
  return output

# ---

def findCRO(path, ev, ch):
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
      if(ev == myTree.events.getHeader().getEvtNum()):
          numCh = myTree.events.getNumPayloads()
          ch_count = 0
          ch_index = ch
          while(ch_count < numCh):
              if(ch_index >= numCh):
                  ch_index = 0;
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

def pedestal(ju_signals, pad = 30, align = 0):
    average = []
    th2d = ROOT.TH2D("pedesta", "pedestal", 2*pad, 0, 2*pad, 4096, -2048, 2048);
    if(len(ju_signals) > 0): ref = ju_signals[0].pedestal[::-1]
    for sig in ju_signals:
        ped = sig.pedestal
        conv = signal.convolve(ped, ref, mode='same')
        index = np.argmax(conv)
        if(align): ped = np.roll(ped, pad-index)
        average.append(ped)
        for s in range(len(ped)):
            th2d.Fill(s, ped[s])
    return th2d, average

def profile(ju_signals, pol = 1, pad = 30, align = 0, avgped = []):
    n_pol = 'neg'; 
    if(pol == 1): n_pol = 'pos'
    else: n_pol = 'neg'
    N = 0; th2d = ROOT.TH2D("profile_" + n_pol, "profile_" + n_pol, 2*pad, 0, 2*pad, 4096, -2048, 2048);
    if(len(ju_signals) > 0): ref = ju_signals[0].data[::-1]
    for sig in ju_signals:
        if(pol == sig.polarity):
            data = sig.data
            if(len(data) != 2*pad):
                sig.unequal = 1
                continue;
            conv = signal.convolve(data, ref, mode='same')
            index = 0 
            np.argmax(conv)
            if(align): data = np.roll(data, pad-index)
            if(len(avgped) == 2*pad):
              if(len(sig.pedestal) == 2*pad):
                data = subSpectrum(data, sig.pedestal)
              else:
                data = subSpectrum(data, avgped)
                sig.unequal = 1
            N += 1;
            for s in range(len(data)):
              th2d.Fill(s, data[s])
    return th2d, N

def cutProf(th2d, pol = 1, pad = 30):
    n_pol = 'neg'; 
    if(pol == 1): n_pol = 'pos'
    else: n_pol = 'neg'
    N = 0; profile = ROOT.TH2D("profile_cut_" + n_pol, "profile_cut_" + n_pol, 2*pad, 0, 2*pad, 4096, -2048, 2048);
    for x in range(2*pad):
        arr_y = []
        for y in range(4096):
            arr_y.append(th2d.GetBinContent(x, y));
        index = np.where(arr_y == np.amax(arr_y))
        i = int(index[0][0]); isDel = False
        while i > 0:
            if(th2d.GetBinContent(x, i) == 0): isDel = True
            if(isDel): profile.SetBinContent(x, i, 0)
            else: profile.SetBinContent(x, i, th2d.GetBinContent(x, i))
            i = i - 1;
        i = int(index[0][0]); isDel = False
        while i < 4096:
            if(th2d.GetBinContent(x, i) == 0): isDel = True
            if(isDel): profile.SetBinContent(x, i, 0)
            else: profile.SetBinContent(x, i, th2d.GetBinContent(x, i))
            i = i + 1;
    return profile

def filter(ju_signals, profile, chs, pol = 1, N = 120, step = 10):
    chs_thr = []; min_thr = []; max_thr = []
    for ch in chs:  
        xmin = sys.float_info.max
        xmax = sys.float_info.min
        for sig in ju_signals:
            if(pol == sig.polarity and (sig.ch == ch)):
                value = 0; bad = 0; good = 0
                for s in range(len(sig.data)):
                    if(sig.unequal == 0): 
                        if(profile.GetBinContent(int(s), int(sig.data[s]) + 2048) != 0):
                            value += profile.GetBinContent(int(s), int(sig.data[s]) + 2048);
                            good += abs(sig.data[s])
                        if(profile.GetBinContent(int(s), int(sig.data[s]) + 2048) == 0):
                            bad += abs(sig.data[s])
                        # here
                if(good <= bad):
                    sig.unequal = 1
                    continue;
                if(xmin > value): xmin = value
                if(xmax < value): xmax = value
                sig.overlap = value
        for thr in range(int(xmin), int(xmax), step):
            sig_N = 0;
            for sig in ju_signals:
                if(sig.overlap > thr and (sig.ch == ch)):
                    if(sig.unequal == 0): sig_N += 1
            #print("ch={} n = {}".format(ch, sig_N), end="\r")
            if(sig_N < N):
                chs_thr.append(thr - step)
                min_thr.append(xmin); max_thr.append(xmax)
                break;
    return chs_thr, min_thr, max_thr

def profSumHeight(ju_signals, pol = 1, pad = 30, align = 0,  chs = [], chs_thr = []):
  
    average = []; height = [] 
    for i in range(len(chs)):
      h_sum = ROOT.TH1D("sum_ch" + str(chs[i]), "sum_ch" + str(chs[i]), 30, 1, -1);
      average.append(h_sum);
      h_h = ROOT.TH1D("height_ch" + str(chs[i]), "height_ch" + str(chs[i]), 30, 1, -1);
      height.append(h_h); 

    N = 0; th2d = ROOT.TH2D("profile_allchs_" + str(pol), "profile_allchs_" + str(pol), 2*pad, 0, 2*pad, 4096, -2048, 2048);
    if(len(ju_signals) > 0): ref = ju_signals[0].data[::-1]
    for sig in ju_signals:
        if(pol == sig.polarity and sig.unequal == 0):
            data = sig.data
            conv = signal.convolve(data, ref, mode='same')
            index = np.argmax(conv)
            if(align): data = np.roll(data, pad-index)     
            index = 0; index = chs.index(sig.ch)
            if(sig.overlap >= chs_thr[index] and sig.ch in chs):
              data = subSpectrum(data, sig.pedestal)
              sums = []; shifts = []
              for shift in range(int(-np.max(data)), int(np.max(data))):
                tmp = data + shift;
                for i in range(len(tmp)):
                    if(tmp[i] < 0): tmp[i] = -1*tmp[i]
                sums.append(int(sum(tmp)))
                shifts.append(shift)
              N += 1;
              index_s = np.where(sums == np.amin(sums))
              shift = shifts[index_s[0][0]]
              data = data + shift
              f = signal.resample(data, 2*pad*10)
              height[index].Fill(np.max(f))
              average[index].Fill(sum(data))              
              for s in range(len(data)):
                th2d.Fill(s, data[s])
            else:
              sig.unequal = 1
    d_sum = array('f'); d_sum_std = array('f')
    d_ch = array('f'); err = array('f')
    d_height = array('f'); d_height_std = array('f');

    for i in range(len(chs)):
      d_sum.append(average[i].GetMean())
      d_sum_std.append(average[i].GetRMS())
    
      d_height.append(height[i].GetMean())
      d_height_std.append(height[i].GetRMS());
    
      d_ch.append(chs[i])
      err.append(0)
    
    gr_h = ROOT.TGraphErrors( len(d_ch), d_ch, d_height, err, d_height_std )
    name = ""; gr_h.SetTitle(name); gr_h.SetName(name)
    gr_h.GetXaxis().SetTitle("chid"); gr_h.GetYaxis().SetTitle("sum")

    gr_sum = ROOT.TGraphErrors( len(d_ch), d_ch, d_sum, err, d_sum_std )
    name = ""; gr_sum.SetTitle(name); gr_sum.SetName(name)
    gr_sum.GetXaxis().SetTitle("chid"); gr_sum.GetYaxis().SetTitle("height")
    return N, th2d, gr_h, gr_sum, average, height

def average(data):
  sig_avg = array('f'); sig_std = array('f')
  tick = array('f'); eTick = array('f')   
  
  for i in range(len(data)):
    sig_avg.append(np.average(data[i]))
    sig_std.append(np.std(data[i]))
    tick.append(i)
    eTick.append(0)
  gr = ROOT.TGraphErrors(len(sig_avg), tick, sig_avg, eTick, sig_std)
  return gr, sig_avg 

def plot(obj, name, title, pathToSave, ptype, titleX, titleY, ymin, ymax, xmin, xmax):
    
    plot = obj.Clone()
    plot.GetXaxis().SetTitle(titleX); 
    plot.GetYaxis().SetTitle(titleY)
    plot.SetFillStyle(3001); 
    plot.SetFillColor(4);
    
    #---
    iPeriod = "0"; iPos="0"
    gROOT.ProcessLine("cmsText = \"\";")
    gROOT.ProcessLine("writeExtraText = true;")
    gROOT.ProcessLine("extraText  = \"\";")
    #---
    
    cName = "c_" + name; cTitle = ""; cX = 800; cY = 600
    if(ptype == 'event'):
      cX = 1000
      cY = 420
    c = ROOT.TCanvas(cName, cTitle, cX, cY)
    
    #---
    p = ROOT.TPad("pad", "", 0, 0, 1, 1);
    p.SetGrid();
    p.Draw();
    p.cd();
    #---
    
    tdraw = ""
    if(ptype == 'profile'):
        tdraw = "colz"
        gROOT.ProcessLine("lumi_sqrtS = \"" + title + "            " + "\";")
        
        plot.GetXaxis().SetRangeUser(xmin, xmax);
        plot.GetYaxis().SetRangeUser(ymin, ymax);
        plot.SaveAs(pathToSave + name + ".root");
        p.SetBottomMargin(0.2)
        p.SetRightMargin(0.15)    
        plot.GetXaxis().SetTitle(""); 
        plot.GetYaxis().SetTitle("");
        plot.GetXaxis().SetLabelSize(0);
        plot.GetXaxis().SetTickLength(0);
        plot.GetYaxis().SetLabelSize(0);
        plot.GetYaxis().SetTickLength(0);
        
        plot.GetZaxis().SetTitle("entries");
        plot.GetZaxis().SetLabelSize(0.03);
        plot.GetZaxis().SetLabelOffset(0.01);
        plot.GetZaxis().SetTitleSize(0.03);
        plot.GetZaxis().SetTitleOffset(1.5);
        
        plot.Draw(tdraw);
        
        yaxis1 = ROOT.TGaxis(xmin, ymin - 0.5, xmin, ymax - 0.5, ymin, ymax, 510,"L");
        yaxis1.SetName("yaxis1");
        yaxis1.SetTitle(titleY)
        yaxis1.SetLabelSize(0.03);
        yaxis1.SetLabelOffset(0.06);
        yaxis1.SetTitleSize(0.03);
        yaxis1.SetTitleOffset(1.5)
        yaxis1.Draw();

        xaxis1 = ROOT.TGaxis(xmin - 0.5, ymin, xmax - 0.5, ymin, xmin, xmax, 510, "L");
        xaxis1.SetName("xaxis1");
        xaxis1.SetTitle(titleX)
        xaxis1.SetTitleOffset(1)
        xaxis1.SetLabelSize(0.03);
        xaxis1.SetLabelOffset(0.01);
        xaxis1.SetTitleSize(0.03);
        xaxis1.Draw(); 
    
    if(ptype == 'average'):
        tdraw = "AP"
        gROOT.ProcessLine("lumi_sqrtS = \"" + title  + "\";")
        plot.GetXaxis().SetRangeUser(xmin, xmax);
        plot.GetYaxis().SetRangeUser(ymin, ymax);
        
        plot.SaveAs(pathToSave + name + ".root");
        plot.GetXaxis().SetTitle(""); 
        plot.GetYaxis().SetTitle("");
        plot.GetXaxis().SetLabelSize(0);
        plot.GetXaxis().SetTickLength(0);
        plot.GetYaxis().SetLabelSize(0);
        plot.GetYaxis().SetTickLength(0);
        
        plot.Draw(tdraw);
        
        yaxis1 = ROOT.TGaxis(xmin, ymin - 0.5, xmin, ymax - 0.5, ymin, ymax, 510,"L");
        yaxis1.SetName("yaxis1");
        yaxis1.SetTitle(titleY)
        yaxis1.SetLabelSize(0.03);
        yaxis1.SetLabelOffset(0.06);
        yaxis1.SetTitleSize(0.03);
        yaxis1.SetTitleOffset(1.5)
        yaxis1.Draw();

        xaxis1 = ROOT.TGaxis(xmin - 0.5, ymin, xmax - 0.5, ymin, xmin, xmax, 510, "L");
        xaxis1.SetName("xaxis1");
        xaxis1.SetTitle(titleX)
        xaxis1.SetTitleOffset(1)
        xaxis1.SetLabelSize(0.03);
        xaxis1.SetLabelOffset(0.01);
        xaxis1.SetTitleSize(0.03);
        xaxis1.Draw(); 
    if(ptype == 'event'):
        p.SetRightMargin(0.05)    
        plot.SetMarkerSize(0.2)
        gROOT.ProcessLine("lumi_sqrtS = \"" + title  + "\";")
        plot.GetXaxis().SetRangeUser(xmin, xmax);
        #plot.GetYaxis().SetRangeUser(ymin, ymax);
        
        plot.SaveAs(pathToSave + name + ".root");
        plot.GetXaxis().SetTitle(titleX); 
        #plot.GetXaxis().SetLabelSize(0);
        #plot.GetXaxis().SetTickLength(0);
        #plot.GetYaxis().SetLabelSize(0);
        #plot.GetYaxis().SetTickLength(0);
        
        plot.Draw("APL");
        
        #yaxis1 = ROOT.TGaxis(xmin, ymin - 0.5, xmin, ymax - 0.5, ymin, ymax, 510,"L");
        #yaxis1.SetName("yaxis1");
        #yaxis1.SetTitle(titleY)
        #yaxis1.SetLabelSize(0.03);
        #yaxis1.SetLabelOffset(0.06);
        #yaxis1.SetTitleSize(0.03);
        #yaxis1.SetTitleOffset(1.5)
        #yaxis1.Draw();

        #xaxis1 = ROOT.TGaxis(xmin - 0.5, ymin, xmax - 0.5, ymin, xmin, xmax, 510, "L");
        #xaxis1.SetName("xaxis1");
        #xaxis1.SetTitle(titleX)
        #xaxis1.SetTitleOffset(1)
        #xaxis1.SetLabelSize(0.03);
        #xaxis1.SetLabelOffset(0.01);
        #xaxis1.SetTitleSize(0.03);
        #xaxis1.Draw(); 
    if(ptype == 'hsum'):
        tdraw = "AP"
        plot.SetMarkerSize(1)
        gROOT.ProcessLine("lumi_sqrtS = \"" + title  + "\";")
        plot.GetXaxis().SetRangeUser(xmin, xmax);
        #plot.GetYaxis().SetRangeUser(ymin, ymax);
        
        plot.SaveAs(pathToSave + name + ".root");
        plot.GetXaxis().SetTitle(titleX); 
        plot.GetYaxis().SetTitle(titleY);
        #plot.GetXaxis().SetLabelSize(0);
        #plot.GetXaxis().SetTickLength(0);
        #plot.GetYaxis().SetLabelSize(0);
        #plot.GetYaxis().SetTickLength(0);
        
        plot.Draw(tdraw);
        
        #yaxis1 = ROOT.TGaxis(xmin, ymin - 0.5, xmin, ymax - 0.5, ymin, ymax, 510,"L");
        #yaxis1.SetName("yaxis1");
        #yaxis1.SetTitle(titleY)
        #yaxis1.SetLabelSize(0.03);
        #yaxis1.SetLabelOffset(0.06);
        #yaxis1.SetTitleSize(0.03);
        #yaxis1.SetTitleOffset(1.5)
        #yaxis1.Draw();

        #xaxis1 = ROOT.TGaxis(xmin - 0.5, ymin, xmax - 0.5, ymin, xmin, xmax, 510, "L");
        #xaxis1.SetName("xaxis1");
        #xaxis1.SetTitle(titleX)
        #xaxis1.SetTitleOffset(1)
        #xaxis1.SetLabelSize(0.03);
        #xaxis1.SetLabelOffset(0.01);
        #xaxis1.SetTitleSize(0.03);
        #xaxis1.Draw(); 
    if(ptype == ''):
        gROOT.ProcessLine("lumi_sqrtS = \"" + title  + "\";")
        plot.SaveAs(pathToSave + name + ".root");
        p.SetRightMargin(0.05)    
        plot.GetXaxis().SetTitle(titleX); 
        plot.GetYaxis().SetTitle(titleY);
        plot.Draw(tdraw);
	
    cms_lumi_str = "CMS_lumi("+cName+", "+iPeriod+", "+iPos+");"; gROOT.ProcessLine(cms_lumi_str)
    c.Draw()
    c.SaveAs(pathToSave + name + ".svg");
    c.SaveAs(pathToSave + name + ".pdf");
