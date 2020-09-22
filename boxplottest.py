import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Polygon
import argparse

'''
spread = np.random.rand(50) * 100
center = np.ones(25) * 50
flier_high = np.random.rand(10) * 100 + 100
flier_low = np.random.rand(10) * -100
data = np.concatenate((spread, center, flier_high, flier_low))


print(data.shape)
help(plt.boxplot)
plt.boxplot(data, 0, '')
plt.show()
'''

def drawBoxPlot(input):
  file = open(input, 'r')
  rounds = 0
  values = {}
  for line in file.readlines():
    line = line.strip()
    if len(line)==0:
      continue
    splits = line.split(' ')
    if splits[0]=="rounds:":
      rounds = float(splits[1])
    else:
      query = splits[0][:-1]
      if not query in values:
        values[query] = {}
      values[query][rounds] = [float(ele) for ele in splits[1:]]
  for k in values:
    v = values[k]
    labels = v.keys()
    data = v.values()
    plt.boxplot(data, labels=labels)
    plt.title(k)
    plt.show()
    

def drawVariancePlot(input):
  file = open(input, 'r')
  rounds = []
  stds = []
  for line in file.readlines():
    line = line.strip()
    if len(line)==0:
      continue
    splits = line.split(' ')
    if splits[0]=="rounds:":
      rounds.append(splits[1])
    else:
      probs = [float(ele) for ele in splits[1:]]
      stds.append(np.std(probs))
  plt.bar(rounds, stds, color="blue")
  plt.xlabel("gibbs sampling rounds")
  plt.ylabel("standard derivation of 100 results")
  plt.show()

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument("--input", type=str)
  parser.add_argument("--output", type=str)
  args = parser.parse_args()

  drawBoxPlot(args.input)
  drawVariancePlot(args.input)