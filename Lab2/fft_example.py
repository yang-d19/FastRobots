import numpy as np
import matplotlib.pyplot as plt

# Generating a sample signal
sampling_freq = 1000  # Sampling frequency (Hz)
sampling_time = 1 / sampling_freq  # Sampling time

# Generate time array
t = np.arange(0, 1, sampling_time)

# Generate a sample signal composed of two sine waves
f1 = 5  # Frequency of the first sine wave (Hz)
f2 = 50  # Frequency of the second sine wave (Hz)
amplitude1 = 1
amplitude2 = 0.5
signal = amplitude1 * np.sin(2 * np.pi * f1 * t) + amplitude2 * np.sin(2 * np.pi * f2 * t) + 20

# Perform FFT
fft_result = np.fft.fft(signal)
fft_result[0] = 0

freq = np.fft.fftfreq(len(signal), d=sampling_time)

# Plot the signal
plt.figure(figsize=(12, 6))
plt.subplot(2, 1, 1)
plt.plot(t, signal)
plt.title('Original Signal')
plt.xlabel('Time')
plt.ylabel('Amplitude')

# Plot the FFT result
plt.subplot(2, 1, 2)
plt.plot(freq, np.abs(fft_result))
plt.title('FFT Result')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude')
plt.xlim(0, 100)  # Limiting the x-axis for better visualization
plt.tight_layout()
plt.show()
