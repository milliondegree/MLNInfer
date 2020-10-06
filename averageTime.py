import numpy as np


def parseFile(f):
  dic = {}
  times = []
  tuples = []
  for line in f.readlines():
    line = line.strip()
    splits = line.split(" ")
    if splits[0]=="pgibbs":
      times.append(float(splits[3]))
    elif splits[0]=="probability":
      tuples.append(splits[2])
  for t, time in zip(tuples, times):
    dic[t] = time
  return dic



def selectDict(dic, l):
  ndic = {}
  for key in dic:
    if key in l:
      ndic[key] = dic[key]
  return ndic


if __name__=="__main__":
  dire = "sample7"
  file_names = [
            "./data/hypertext-class/"+dire+"/records/sample1.log",
            "./data/hypertext-class/"+dire+"/records/sample2.log",
            "./data/hypertext-class/"+dire+"/records/sample3.log",
            "./data/hypertext-class/"+dire+"/records/sample4.log",
            "./data/hypertext-class/"+dire+"/records/sample5.log",
            "./data/hypertext-class/"+dire+"/records/sample6.log",
            "./data/hypertext-class/"+dire+"/records/sample7.log",
            "./data/hypertext-class/"+dire+"/records/sample8.log",
            "./data/hypertext-class/"+dire+"/records/sample9.log",
            "./data/hypertext-class/"+dire+"/records/sample10.log"
          ]
  dics = []
  avg_times = []
  for file_name in file_names:
    file = open(file_name, "r")
    dic = parseFile(file)
    dics.append(dic)
    # dics[-1] = selectDict(dics[-1], list(dics[0].keys()))
    avg_times.append(np.mean(np.array(list(dics[-1].values()))))
  print(avg_times)