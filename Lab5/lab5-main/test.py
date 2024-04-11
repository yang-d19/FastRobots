import matplotlib.pyplot as plt

# 假设这里是你的数据
timestamps_ms = [1, 2, 3, 4, 5]  # 示例时间戳（单位：毫秒）
distances = [5, 6, 13, 8, 6]       # 示例距离数据
controls = [4, 2, 3, 9, 1]        # 示例控制数据

# 转换时间戳为秒（如果需要）
timestamps_s = [t / 1000.0 for t in timestamps_ms]

# 创建图表和轴
fig, ax1 = plt.subplots()

# 绘制 distance 曲线
color = 'tab:red'
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Distance', color=color)
ax1.plot(timestamps_s, distances, color=color)
ax1.tick_params(axis='y', labelcolor=color)

# 实例化一个新的轴，共享同一个X轴
ax2 = ax1.twinx()  

# 绘制 control 曲线
color = 'tab:blue'
ax2.set_ylabel('Control', color=color)  
ax2.plot(timestamps_s, controls, color=color)
ax2.tick_params(axis='y', labelcolor=color)

# 给图表加上标题和显示图例
plt.title('Distance and Control vs. Time')

# 显示图表
plt.show()

# from enum import Enum, auto

# class AutoNumber(Enum):
#     """
#     枚举的基类，修改生成枚举值的规则使其从 0 开始自动递增。
#     """
#     def _generate_next_value_(name, start, count, last_values):
#         return count

# class Color(AutoNumber):
#     RED = auto()
#     GREEN = auto()
#     BLUE = auto()

# print(Color.RED.value)
# print(Color.GREEN.value)
# print(Color.BLUE.value)

