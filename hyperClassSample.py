import argparse
import random
import numpy as np


parser = argparse.ArgumentParser()
parser.add_argument("--mfile", type=str)
parser.add_argument("--ofile", type=str)
parser.add_argument("--mapfile", type=str)
parser.add_argument("--dfile", type=str)
parser.add_argument("-rm", type=float)
parser.add_argument("-ro", type=float)
args = parser.parse_args()

mf = open(args.mfile, "w")
of = open(args.ofile, "w")
mapf = open(args.mapfile, "w")
df = open(args.dfile, "w")

mf.write("materialize(hasword, infinity, infinity, keys(1, 2:str, 3:int32)).\n")
mf.write("materialize(topic, infinity, infinity, keys(1, 2:str, 3:int32)).\n")
mf.write("materialize(links, infinity, infinity, keys(1, 2:int32, 3:int32)).\n")
mf.write("materialize(shaResult, infinity, infinity, keys(1, 2:cid, 3:str)).\n")
mf.write("\n\n")
mf.write("rl topic(@Local, T, P1) :- links(@Local, P1, P2), topic(@Local, T, P2), P1!=P2.\n")
mf.write("\n")

of.write("hasword 0\n")
of.write("topic -2.04659\n")
of.write("links 0\n")
of.write("rl -0.11167\n")


mlnFile = open("./data/hypertext-class/hypertext-class-out.mln", "r")
obsFile = open("./data/hypertext-class/hypertext-class-test.db", "r")

# first parse and sample mlnFile
mlnList = []
for line in mlnFile.readlines():
  line = line.strip()
  l = line.split(' ')
  if len(l)>5:
    mlnList.append(l)

indices = [i for i in range(len(mlnList))]
random.shuffle(indices)
mlnList = np.array(mlnList)[indices[:int(len(indices)*args.rm)]].tolist()

for i, l in enumerate(mlnList):
  try:
    rule_name = "r"+str(i)
    rule_weight = float(l[1])
    p1 = 0
    while l[3][p1]!="\"":
      p1 += 1
    p2 = p1+1
    while l[3][p2]!="\"":
      p2 += 1
    W = l[3][p1+1:p2]
    p1 = 0
    while l[5][p1]!="(":
      p1 += 1
    p2 = p1+1
    while l[5][p2]!=",":
      p2 += 1
    T = l[5][p1+1:p2]
    mf.write(rule_name+" topic(@Local, T, P) :- hasword(@Local, W, P), T:=\""+T+"\", W==\""+W+"\".\n")
    of.write(rule_name+" "+str(rule_weight)+"\n")
  except:
    print(l)

mf.write("\n")
mf.write("rs1 shaResult(@Local, VID, Content) :- topic(@Local, T, P), VID:=f_sha1(\"topic\"+Local+T+P), Name:=\"topic\", Content:=Name+\"_\"+T+\"_\"+P.\n")
mf.write("rs2 shaResult(@Local, VID, Content) :- hasword(@Local, W, P), VID:=f_sha1(\"hasword\"+Local+W+P), Name:=\"hasword\", Content:=Name+\"_\"+W+\"_\"+P.\n")
mf.write("rs3 shaResult(@Local, VID, Content) :- links(@Local, P1, P2), VID:=f_sha1(\"links\"+Local+P1+P2), Name:=\"links\", Content:=Name+\"_\"+P1+\"_\"+P2.\n")


# parse and sample observed file
obsList = []
for line in obsFile.readlines():
  line = line.strip()
  obsList.append(line)

m = {}
counth = 0
countl = 0
for i, line in enumerate(obsList):
  p = 0
  while line[p]!='(':
    p += 1
  predicate = line[:p].lower()
  p += 2
  p2 =  p
  while line[p2]!="\"":
    p2 += 1
  a1 = line[p:p2]
  p2 += 3
  p3 = p2
  while line[p3]!="\"":
    p3 += 1
  a2 = line[p2:p3]
  if predicate=="hasword":
    if random.random()<args.ro:
      counth += 1
      if not a2 in m:
        m[a2] = len(m)+1
        mapf.write(str(m[a2])+" "+a2+"\n")
      of.write(predicate+"_"+a1+"_"+str(m[a2])+" 1\n")
      df.write(predicate+" "+a1+" "+str(m[a2])+"\n")
  elif predicate=="links":
    if random.random()<args.ro:
      countl += 1
      if not a1 in m:
        m[a1] = len(m)+1
        mapf.write(str(m[a1])+" "+a1+"\n")
      if not a2 in m:
        m[a2] = len(m)+1
        mapf.write(str(m[a2])+" "+a2+"\n")
      of.write(predicate+"_"+str(m[a1])+"_"+str(m[a2])+" 1\n")
      df.write(predicate+" "+str(m[a1])+" "+str(m[a2])+"\n")

print(counth, countl)


mf.close()
of.close()
mapf.close()
df.close()