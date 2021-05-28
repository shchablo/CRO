class Signal(object):
    
    # event info    
    ev = 0
    ch = 0
    
    # data of event
    raw_data = []
   
    # Signal
    polarity = 0
    threshold = 0 
    
    peak_index = 0
    peak_value = 0
    
    integral = 0
    width = 0
    
    data = []
    pedestal = []
    isFFT = 0 # pedestal constant correction with FFT 
   
    # Analysis
    unequal = 0
