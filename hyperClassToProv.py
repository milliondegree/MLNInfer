import argparse
import matplotlib.pyplot as plt
import numpy as np
import sys
np.set_printoptions(threshold=sys.maxsize)


parser = argparse.ArgumentParser()
parser.add_argument("--input", type=str)
parser.add_argument("--output", type=str)
args = parser.parse_args()


infile = open(args.input, 'r')
outfile = open(args.output, 'w')


times = []
count = 0
for line in infile.readlines():
  line = line.strip()
  split = line.split(' ')
  word = split[0]
  if word.isdigit():
    times.append(int(word))
  elif word.startswith('time='):
    count += 1
    toWrite = split[6][14:]
    outfile.write(toWrite+'\n')

intervals = []
for i in range(2, len(times)):
  intervals.append(times[i]-times[i-1])
intervals = sorted(intervals, reverse=True)
time = sum(intervals[:count])

print("maintenance time:", (times[1]-times[0])/1000000, "s")
print("average query time:", (time/count)/1000000, "s")
# intervals = [ele/1000000 for ele in intervals]
# a, b = np.histogram(intervals, bins=np.arange(intervals[-1], intervals[0]+10, 100))
# print(a)

infile.close()
outfile.close()