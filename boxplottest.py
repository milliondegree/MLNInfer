import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Polygon

spread = np.random.rand(50) * 100
center = np.ones(25) * 50
flier_high = np.random.rand(10) * 100 + 100
flier_low = np.random.rand(10) * -100
data = np.concatenate((spread, center, flier_high, flier_low))


print(data.shape)
help(plt.boxplot)
plt.boxplot(data, 0, '')
plt.show()