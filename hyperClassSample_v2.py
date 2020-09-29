import os
import argparse
import random
import numpy as np


parser = argparse.ArgumentParser()
parser.add_argument("-dir", type=str, help="target directory for one time sample")
parser.add_argument("-n", type=int, help="number of samples")
# parser.add_argument("-nr", type=int, help="number of rules to sample")
# parser.add_argument("-nh", type=int, help="number of hasword tuples to sample")
# parser.add_argument("-nl", type=int, help="number of link tuples to sample")
args = parser.parse_args()

if not os.path.exists(args.dir):
  os.makedirs(args.dir)
  os.makedirs(os.path.join(args.dir, "prov"))
  os.makedirs(os.path.join(args.dir, "images"))
else:
  print(args.dir+" already exists")

name = args.dir.split("/")[-1]

mfile = os.path.join(args.dir, "sample"+".olg")
ofileList = []
dfileList = []
for i in range(1, args.n+1):
  ofileList.append(os.path.join(args.dir, "sample"+str(i)+".obs"))
  dfileList.append(os.path.join(args.dir, "sample"+str(i)+".db"))

mf = open(mfile, "w")
ofList = []
dfList = []
print(ofileList)
for ele in ofileList:
  ofList.append(open(ele, 'w'))
for ele in dfileList:
  dfList.append(open(ele, 'w'))

mf.write("materialize(hasword, infinity, infinity, keys(1, 2:str, 3:int32)).\n")
mf.write("materialize(topic, infinity, infinity, keys(1, 2:str, 3:int32)).\n")
mf.write("materialize(links, infinity, infinity, keys(1, 2:int32, 3:int32)).\n")
mf.write("materialize(shaResult, infinity, infinity, keys(1, 2:cid, 3:str)).\n")
mf.write("\n\n")
mf.write("rl topic(@Local, T, P1) :- links(@Local, P1, P2), topic(@Local, T, P2), P1!=P2.\n")
mf.write("\n")

for of in ofList:
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
# mlnList = np.array(mlnList)[indices[:int(len(indices)*args.rm)]].tolist()
mlnList = np.array(mlnList)[indices[:min(len(mlnList), 5000)]].tolist()
countr = len(mlnList)

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
    for of in ofList:
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



hl = []
ll = []
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
    hl.append([predicate, a1, a2])
  elif predicate=="links":
    ll.append([predicate, a1, a2])

print(len(hl), len(ll))

# for i in range(args.n):
#   of = ofList[i]
#   df = dfList[i]
#   m = {}
#   random.shuffle(hl)
#   hll = hl[:min(len(hl), (i+1)*70)]
#   for predicate, a1, a2 in hll:
#     if not a2 in m:
#       m[a2] = len(m)+1
#     of.write(predicate+"_"+a1+"_"+str(m[a2])+" 1\n")
#     df.write(predicate+" "+a1+" "+str(m[a2])+"\n")

#   # random.shuffle(ll)
#   # lll = ll[:min(len(hl), (i+1)*50)]
#   # for predicate, a1, a2 in lll:
#   #   if not a1 in m:
#   #     m[a1] = len(m)+1
#   #   if not a2 in m:
#   #     m[a2] = len(m)+1
#   #   of.write(predicate+"_"+str(m[a1])+"_"+str(m[a2])+" 1\n")
#   #   df.write(predicate+" "+str(m[a1])+" "+str(m[a2])+"\n")

#   random.shuffle(ll)
#   lcount = 0
#   for j in range(len(ll)):
#     predicate, a1, a2 = ll[j]
#     if a1 in m and a2 in m:
#       of.write(predicate+"_"+str(m[a1])+"_"+str(m[a2])+" 1\n")
#       df.write(predicate+" "+str(m[a1])+" "+str(m[a2])+"\n")
#       lcount += 1
#     if lcount >= (i+1)*30:
#       break
#   print(i, (i+1)*40, lcount)


m = {}
olist = []
dlist = []
ih = 0
il = 0
random.shuffle(hl)
random.shuffle(ll)
for i in range(args.n):
  of = ofList[i]
  df = dfList[i]
  hcount = 0
  lcount = 0
  while hcount < 70:
    predicate, a1, a2 = hl[ih]
    if a2 not in m:
      m[a2] = len(m)+1
    olist.append(predicate+"_"+a1+"_"+str(m[a2])+" 1\n")
    dlist.append(predicate+" "+a1+" "+str(m[a2])+"\n")
    hcount += 1
    ih += 1
    if ih == len(hl):
      ih = 0
  while lcount < 30:
    predicate, a1, a2 = ll[il]
    if a1 in m and a2 in m:
      olist.append(predicate+"_"+str(m[a1])+"_"+str(m[a2])+" 1\n")
      dlist.append(predicate+" "+str(m[a1])+" "+str(m[a2])+"\n")
      lcount += 1
    il += 1
    if il == len(ll):
      il = 0
  for j in range(len(olist)):
    of.write(olist[j])
    df.write(dlist[j])

  


mf.close()
for of in ofList:
  of.close()
for df in dfList:
  df.close()