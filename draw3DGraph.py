import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

import argparse
import numpy as np


def draw3DGraph(input):
  file = open(input, 'r')
  lines = []
  for line in file.readlines():
    line = line.strip()
    if len(line)==0:
      continue
    lines.append(line)

  Xs = lines[0].split()
  Xname = Xs[0]
  X = np.array([float(ele) for ele in Xs[1:]]).reshape(9, 9)
  Xinf = lines[1].split()
  Xinf = np.array([float(ele) for ele in Xinf[1:]]).reshape(9, 9)

  Ys = lines[2].split()
  Yname = Ys[0]
  Y = np.array([float(ele) for ele in Ys[1:]]).reshape(9, 9)
  Yinf = lines[3].split()
  Yinf = np.array([float(ele) for ele in Yinf[1:]]).reshape(9, 9)

  fig = plt.figure("influence of "+Xname)
  ax = plt.axes(projection='3d')
  ax.set_xlabel(Xname)
  ax.set_ylabel(Yname)
  ax.plot_surface(X, Y, Xinf, cmap='rainbow')

  fig = plt.figure("influence of "+Yname)
  ax = plt.axes(projection='3d')
  ax.set_xlabel(Xname)
  ax.set_ylabel(Yname)
  ax.plot_surface(X, Y, Yinf, cmap='rainbow')
  plt.show()



  

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument("--input", type=str)
  parser.add_argument("--output", type=str)
  args = parser.parse_args()

  draw3DGraph(args.input)

