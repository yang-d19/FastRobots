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

interval_sum = 0
for i in range(1, length):
    interval_sum += timestamps_ms[i] - timestamps_ms[i - 1]
interval = interval_sum / (length - 1)

print(interval)

smooth_velocities = savgol_filter(velocities, window_length=16, polyorder=3)

# 转换时间戳为秒（如果需要）
timestamps_s = [t / 1000.0 for t in timestamps_ms]

# 创建图表和轴
fig, ax1 = plt.subplots()

# 绘制 distance 曲线
# color = 'tab:red'
# ax1.set_xlabel('Time (s)')
# ax1.set_ylabel('Distance', color=color)
# ax1.scatter(timestamps_s, distances, color=color, s=5)
# ax1.tick_params(axis='y', labelcolor=color)

color = 'tab:green'
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Velocity', color=color)
ax1.plot(timestamps_s, smooth_velocities, color=color)
ax1.tick_params(axis='y', labelcolor=color)


# 实例化一个新的轴，共享同一个X轴
ax2 = ax1.twinx()  

# 绘制 control 曲线
color = 'tab:blue'
ax2.set_ylabel('Left PWM', color=color)  
ax2.scatter(timestamps_s, left_pwms, color=color, s=5)
ax2.tick_params(axis='y', labelcolor=color)

# 给图表加上标题和显示图例
plt.title('Velocity and PWM vs. Time')

# 显示图表
plt.show()