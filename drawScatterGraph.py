from averageTime import parseFileFinal
import matplotlib.pyplot as plt 


def drawScatterGraph(x1, y1, x2, y2):
  plt.scatter(x1, y1, marker='o', c='blue')
  plt.scatter(x2, y2, marker='^', c='red')
  plt.show()



if __name__=="__main__":
  file_name1 = "./data/hypertext-class/sample7/records/influ10.log"
  file_name2 = "./data/hypertext-class/sample7/records/influEquation10.log"
  file1 = open(file_name1, 'r')
  file2 = open(file_name2, 'r')
  dic1 = parseFileFinal(file1)
  x1 = dic1['unobserved']
  y1 = dic1['influ']
  print(len(x1))
  print(x1[:10])
  print(len(y1))
  print(y1[:10])
  dic2 = parseFileFinal(file2)
  x2 = dic2['unobserved']
  y2 = dic2['influ']
  print(len(x2))
  print(x2[:10])
  print(len(y2))
  print(y2[:10])
  drawScatterGraph(x1, y1, x2, y2)