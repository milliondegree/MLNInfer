from averageTime import parseFileFinal
import matplotlib.pyplot as plt 


def drawSample7InfluGraph():
  x1 = []
  y1 = []
  x2 = []
  y2 = []
  for i in range(1, 11):
    file_name1 = "./data/hypertext-class/sample7/records/influ"+str(i)+".log"
    file_name2 = "./data/hypertext-class/sample7/records/influEquation"+str(i)+".log"
    file1 = open(file_name1, 'r')
    file2 = open(file_name2, 'r')
    dic1 = parseFileFinal(file1)
    x1 += dic1['unobserved']
    y1 += dic1['influ']
    dic2 = parseFileFinal(file2)
    x2 += dic2['unobserved']
    y2 += dic2['influ']

  fig, ax = plt.subplots()
  ax.scatter(x1, y1, marker='o', c='blue', label="approx", alpha=0.5, edgecolors='none')
  ax.scatter(x2, y2, marker='^', c='red', label="exact", alpha=0.5, edgecolors='none')
  ax.legend()
  ax.set_xlabel("number of unobserved tuples")
  ax.set_ylabel("influence inference time")
  ax.set_yscale('log')
  ax.set_ylim(1e-5, 1e-1)
  plt.show()


def drawSample7ProbGraph():
  x1 = []
  y1 = []
  x2 = []
  y2 = []
  for i in range(1, 11):
    file_name1 = "./data/hypertext-class/sample7/records/Gibbssample"+str(i)+".log"
    file_name2 = "./data/hypertext-class/sample7/records/sample"+str(i)+".log"
    file1 = open(file_name1, 'r')
    file2 = open(file_name2, 'r')
    dic1 = parseFileFinal(file1)
    x1 += dic1['unobserved']
    y1 += dic1['gibbs']
    dic2 = parseFileFinal(file2)
    x2 += dic2['unobserved']
    y2 += dic2['pgibbs']

  fig, ax = plt.subplots()
  ax.scatter(x2, y2, marker='o', c='blue', label="iterative", alpha=0.5, edgecolors='none')
  ax.scatter(x1, y1, marker='^', c='red', label="sampling", alpha=0.5, edgecolors='none')
  ax.set_xlabel("number of unobserved tuples")
  ax.set_ylabel("probability inference time")
  ax.set_yscale("log")
  ax.set_ylim(1e-5, 10)
  ax.legend()
  plt.show()



if __name__=="__main__":

  drawSample7InfluGraph()
  # drawSample7ProbGraph()