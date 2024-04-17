import numpy as np

# 创建一个长度为30的数组，这里使用0到29的整数填充
array = np.arange(30)

# 要删除的索引列表
indices_to_remove = [1, 5, 7, 15]

# 使用np.delete函数删除指定索引的元素
new_array = np.delete(array, indices_to_remove)

# 打印修改后的数组
print(new_array)
