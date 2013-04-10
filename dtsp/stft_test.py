import scipy, pylab
import scipy.io.wavfile as wave
import numpy as np

def stft(x, fs, framesz, hop):
    framesamp = framesz
    hopsamp = hop
    w = scipy.hamming(framesamp)
    X = scipy.array([scipy.fft(w*x[i:i+framesamp]) 
                     for i in range(0, len(x)-framesamp, hopsamp)])
    return X

def istft(X, fs, T, hop):
    x = scipy.zeros(T)
    framesamp = X.shape[1]
    hopsamp = hop
    for n,i in enumerate(range(0, len(x)-framesamp, hopsamp)):
        x[i:i+framesamp] += scipy.real(scipy.ifft(X[n]))
    return x

rate, x = wave.read("audio_sample_noise_02.wav")

t = []
for i in range(x.size):
    if i == 0:
        t.append(0.0)
    else:
        t.append(t[i - 1] + 1/8000.0)

X = stft(x, 8000, 256, 128)

print np.amax(X)

# cancel noise
# noise_average = np.zeros(X.shape[1])

# noise_samples = int(2)

# for sample in range(noise_samples):
#     noise_average += np.absolute(X[sample,:])

# noise_average = noise_average/noise_samples

# max_noise = np.amax(noise_average) *1.55

noise_samples = X[:int(X.shape[0]/3)]

max_noise = np.amax(noise_samples)*1.1
# max_noise = np.absolute(max_noise)

noise_average = np.ones(X.shape[1])
noise_average = noise_average * max_noise

pylab.figure()
pylab.plot(noise_average)

# noise_average = np.ones(X.shape[1])
# noise_average = noise_average* 0.9 * 32767

for i in range(X.shape[0]):
    diff = np.absolute(X[i,:]) - np.absolute(noise_average)
    for j in range(diff.size):
        if diff[j] < 0:
            X[i,j] = 0.0
        # elif X[i,j] < 0.0:
        #     X[i,j] += noise_average[j]
        # elif X[i,j] > 0.0:
        #     X[i,j] -= noise_average[j]

# for i in range(1:X.shape[0]):
#     for j in range(X[i]):
#         if X[i,j] < noise_average[]

# Plot the magnitude spectrogram.
pylab.figure()
pylab.imshow(scipy.absolute(X.T), origin='lower', aspect='auto',
             interpolation='nearest')
pylab.xlabel('Time')
pylab.ylabel('Frequency')
#pylab.show()
# Compute the ISTFT.
xhat = istft(X, 8000, x.size, 128)

print x[8000*1:8000*1+20]
print xhat[8000*1:8000*1+20]

out = np.int16(xhat)
wave.write('out_01.wav', 8000, out)

# Plot the input and output signals over 0.1 seconds.
pylab.figure()
pylab.plot(t,xhat) #t, x, t, xhat)
pylab.xlabel('Time (seconds)')

pylab.figure()
pylab.plot(t,x) #t, x, t, xhat)
pylab.xlabel('Time (seconds)')

pylab.show()