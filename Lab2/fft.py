import numpy as np
import matplotlib.pyplot as plt


start_time = 39.335
end_time = 39.859
total_length = 100

# Generate time array
# t = np.arange(0, 1, sampling_time)

t = np.linspace(start_time, end_time, total_length)

sampling_space = t[1] - t[0]

print(sampling_space)

# Generate a sample signal composed of two sine waves

# f1 = 5  # Frequency of the first sine wave (Hz)
# f2 = 50  # Frequency of the second sine wave (Hz)
# amplitude1 = 1
# amplitude2 = 0.5
# signal = amplitude1 * np.sin(2 * np.pi * f1 * t) + amplitude2 * np.sin(2 * np.pi * f2 * t)

pitch_signal = []

# Open the file
with open('extract_pitch_data.txt', 'r') as file:
    # Iterate through each line in the file
    for line in file:
        # Convert the line to a number (assuming it's an integer)
        number = float(line.strip())
        # Append the number to the list
        pitch_signal.append(number)

roll_signal = []

# Open the file
with open('extract_roll_data.txt', 'r') as file:
    # Iterate through each line in the file
    for line in file:
        # Convert the line to a number (assuming it's an integer)
        number = float(line.strip())
        # Append the number to the list
        roll_signal.append(number)

# Perform FFT
pitch_fft_result = np.fft.fft(pitch_signal)
pitch_freq = np.fft.fftfreq(len(pitch_signal), d=sampling_space)

# print(len(freq), len(fft_result))

roll_fft_result = np.fft.fft(roll_signal)
roll_freq = np.fft.fftfreq(len(roll_signal), d=sampling_space)

# Plot the signal
plt.figure(figsize=(12, 6))
plt.subplot(2, 2, 1)
plt.plot(t, pitch_signal)
plt.title('Pitch')
plt.xlabel('Time')
plt.ylabel('Amplitude')

# # Plot the FFT result
plt.subplot(2, 2, 3)
plt.plot(pitch_freq, np.abs(pitch_fft_result))
plt.title('Pitch FFT Result')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude')
plt.xlim(0, 90)
plt.ylim(0, 80)


plt.subplot(2, 2, 2)
plt.plot(t, roll_signal, color='y')
plt.title('Roll')
plt.xlabel('Time')
plt.ylabel('Amplitude')

# # Plot the FFT result
plt.subplot(2, 2, 4)
plt.plot(roll_freq, np.abs(roll_fft_result), color='y')
plt.title('Roll FFT Result')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude')
plt.xlim(0, 90)
plt.ylim(0, 80)


plt.tight_layout()
plt.show()
