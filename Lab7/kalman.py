import numpy as np

d = 0.4
m = 187.8

A = np.array([[0, 1], [0, -d/m]])
B = np.array([[0], [1/m]])
C = np.array([[-1, 0]])

sig1 = 3
sig2 = 3
sig3 = 20

Sig_u = np.array([[sig1**2, 0], [0, sig2**2]])
Sig_z = np.array([[sig3**2]])

delta_t = 8.1

Ad = np.eye(2) + delta_t * A
Bd = delta_t * B


def kf(mu, sigma, u, y):
    
    mu_p = Ad.dot(mu) + Bd.dot(u) 
    sigma_p = Ad.dot(sigma.dot(Ad.transpose())) + Sig_u
    
    sigma_m = C.dot(sigma_p.dot(C.transpose())) + Sig_z
    kkf_gain = sigma_p.dot(C.transpose().dot(np.linalg.inv(sigma_m)))

    y_m = y - C.dot(mu_p)
    mu = mu_p + kkf_gain.dot(y_m)    
    sigma = (np.eye(2) - kkf_gain.dot(C)).dot(sigma_p)

    return mu, sigma


import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import savgol_filter

# Load the CSV file
df = pd.read_csv('output-2-interpolated.csv')
# df = pd.read_csv('history_records/output-2.csv')

cut_idx = len(df) // 2

# Extract columns as arrays
timestamps_ms = df['timestamp_ms'].values[:cut_idx]
distances = df['distance'].values[:cut_idx]
left_pwms = df['left_pwm'].values[:cut_idx]
right_pwms = df['right_pwm'].values[:cut_idx]

# velocities = np.diff(distances)
# velocities = np.insert(velocities, 0, 0)

velocities = [0]

length = len(timestamps_ms)

for i in range(1, length):
    velocities.append((distances[i] - distances[i - 1]) / (timestamps_ms[i] - timestamps_ms[i - 1]))

# init position state
x = np.array([[-distances[0]],[0]])
# init guess of uncertainty
sig = np.array([[5**2,0],[0,5**2]])

kf_state = []
for u, d in zip(left_pwms, distances):
    x, sig = kf(x, sig, [[u / 210]], [[-d]])
    kf_state.append(x[0])


timestamps_s = [t / 1000.0 for t in timestamps_ms]

import matplotlib.pyplot as plt

# 创建图形和轴
fig, ax = plt.subplots()

# 绘制第一组数据
ax.plot(timestamps_s, distances, label='Measurement', color='blue')

# 绘制第二组数据
ax.plot(timestamps_s, kf_state, label='Kf estimate', color='orange')

# 添加图例
ax.legend()

# 添加标题和轴标签
ax.set_title('Kalman Filter vs Raw Data')
ax.set_xlabel('Time (s)')
ax.set_ylabel('Distance (mm)')

# 显示图表
plt.show()