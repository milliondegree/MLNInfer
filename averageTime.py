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



def parseInfluFile(f):
  dic = {}
  times = []
  tuples = []
  for line in f.readlines():
    line = line.strip()
    splits = line.split(" ")
    if splits[0]=="mode":
      times.append(float(splits[6]))
    elif splits[0]=="influence":
      tuples.append(splits[4])
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
  name = "influ"
  file_names = [
            "./data/hypertext-class/"+dire+"/records/"+name+"1.log",
            "./data/hypertext-class/"+dire+"/records/"+name+"2.log",
            "./data/hypertext-class/"+dire+"/records/"+name+"3.log",
            "./data/hypertext-class/"+dire+"/records/"+name+"4.log",
            "./data/hypertext-class/"+dire+"/records/"+name+"5.log",
            "./data/hypertext-class/"+dire+"/records/"+name+"6.log",
            "./data/hypertext-class/"+dire+"/records/"+name+"7.log",
            "./data/hypertext-class/"+dire+"/records/"+name+"8.log",
            "./data/hypertext-class/"+dire+"/records/"+name+"9.log",
            "./data/hypertext-class/"+dire+"/records/"+name+"10.log"
          ]
  dics = []
  avg_times = []
  for file_name in file_names:
    file = open(file_name, "r")
    dic = parseInfluFile(file)
    dics.append(dic)
    # dics[-1] = selectDict(dics[-1], list(dics[0].keys()))
    avg_times.append(np.mean(np.array(list(dics[-1].values()))))
  print(avg_times)