import numpy as np



def parseFileFinal(f):
  dic = {}
  dic["names"] = []
  dic["influ"] = []
  dic["cliques"] = []
  dic["unobserved"] = []
  dic["pgibbs"] = []
  dic["gibbs"] = []
  for line in f.readlines():
    line = line.strip()
    splits = line.split(" ")
    if splits[0]=="mode":
      dic["influ"].append(float(splits[6]))
    elif splits[0]=="influence":
      dic["names"].append(splits[4])
    elif splits[0]=="clique":
      dic["cliques"].append(float(splits[2]))
    elif splits[0]=="unobserved":
      dic["unobserved"].append(float(splits[3]))
    elif splits[0]=="pgibbs":
      dic["pgibbs"].append(float(splits[3]))
    elif splits[0]=="gibbs":
      dic["gibbs"].append(float(splits[3]))
    elif splits[0]=="probability":
      dic["names"].append(splits[2])
  return dic



# def selectDict(dic, l):
#   ndic = {}
#   for key in dic:
#     if key in l:
#       ndic[key] = dic[key]
#   return ndic


if __name__=="__main__":
  dire = "sample11"
  name = "influEquation"
  queryList = ["cliques", "unobserved", "pgibbs", "gibbs", "influ"]
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
  dics = {}
  dics["influ"] = []
  dics["cliques"] = []
  dics["unobserved"] = []
  dics["pgibbs"] = []
  dics["gibbs"] = []
  dics["prbobabilities"] = []
  avg_times = []
  for file_name in file_names:
    file = open(file_name, "r")
    dic = parseFileFinal(file)
    for name in queryList:
      if len(dic[name])!=0:
        dics[name].append(sum(dic[name])/len(dic[name]))
  for name in queryList:
    print(name, dics[name])
  print(avg_times)
