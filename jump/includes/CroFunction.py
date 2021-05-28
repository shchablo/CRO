# Remove Pedestal
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
