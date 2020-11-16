from averageTime import parseFileFinal
import matplotlib.pyplot as plt 


def drawScatterGraph(x1, y1, x2, y2):
  fig, ax = plt.subplots()
  ax.scatter(x1, y1, marker='o', c='blue', label="approx", alpha=0.5, edgecolors='none')
  ax.scatter(x2, y2, marker='^', c='red', label="exact", alpha=0.5, edgecolors='none')
  ax.legend()
  ax.set_xlabel("number of unobserved tuples")
  ax.set_ylabel("influence inference time")
  plt.show()



if __name__=="__main__":
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
  drawScatterGraph(x1, y1, x2, y2)