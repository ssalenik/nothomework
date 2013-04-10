import scipy.io.wavfile as wave
import numpy as np
import random


sample_rate = 8000
time = 2 # seconds
amp = 0.01

# # generate a file with just white noise
# data = np.random.uniform(-amp, amp, sample_rate*time) # 44100 random samples between -1 and 1
# scaled = np.int16(data/np.max(np.abs(data)) * 32767)
# wave.write('noise.wav', sample_rate, scaled)

rate, audio = wave.read("audio_sample_01.wav")
print audio
#audio_scaled = np.int16(audio/np.max(np.abs(audio)) * 32767)
length = audio.size
noise = np.random.uniform(-amp, amp, length)
noise_scaled = np.int16(noise * 32767)

for i in range(length) :
	audio[i] += noise_scaled[i]

wave.write('audio_sample_noise_03.wav', sample_rate, audio)
