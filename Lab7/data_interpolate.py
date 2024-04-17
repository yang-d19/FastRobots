import pandas as pd
import numpy as np


# Load the CSV file
df = pd.read_csv('history_records/output-2.csv')

# Extract columns as arrays
timestamps_ms = df['timestamp_ms'].values
distances = df['distance'].values
left_pwms = df['left_pwm'].values
right_pwms = df['right_pwm'].values

length = len(timestamps_ms)
prev_dist = distances[0]
curr_dist = None

to_delete_idxs = []

for idx in range(1, length):
    curr_dist = distances[idx]
    if curr_dist == prev_dist:
        to_delete_idxs.append(idx)
    else:
        prev_dist = curr_dist

print(len(to_delete_idxs))
# print(to_delete_idxs)

new_timestamps = np.delete(timestamps_ms, to_delete_idxs)
new_distances = np.delete(distances, to_delete_idxs)

# Perform interpolation
interpolated_distances = np.interp(timestamps_ms, new_timestamps, new_distances)

rounded_interpolated_distances = np.round(interpolated_distances)

# 将结果转换为整数类型
rounded_interpolated_distances = rounded_interpolated_distances.astype(int)

print(len(interpolated_distances))
print(len(timestamps_ms))

    # Create a DataFrame
data = {
    'timestamp_ms': timestamps_ms,
    'distance': rounded_interpolated_distances,
    'left_pwm': left_pwms,
    'right_pwm': right_pwms
}
df = pd.DataFrame(data)

# Save the DataFrame to a CSV file
df.to_csv('output-2-interpolated.csv', index=False)