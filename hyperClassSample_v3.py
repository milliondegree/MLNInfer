from operator import index
import os
import argparse
import random
import numpy as np
import shutil


parser = argparse.ArgumentParser()
parser.add_argument("-dir", type=str, help="target directory for one time sample")
parser.add_argument("-n", type=int, help="number of documents")
args = parser.parse_args()

if not os.path.exists(args.dir):
  os.makedirs(args.dir)
  os.makedirs(os.path.join(args.dir, "prov"))
  os.makedirs(os.path.join(args.dir, "images"))
  os.makedirs(os.path.join(args.dir, "toplist"))
  os.makedirs(os.path.join(args.dir, "counterfactual"))
else:
  print(args.dir+" already exists, want delete?")
  s = input()
  if s=="yes":
    shutil.rmtree(args.dir)
    os.makedirs(args.dir)
    os.makedirs(os.path.join(args.dir, "prov"))
    os.makedirs(os.path.join(args.dir, "images"))
    os.makedirs(os.path.join(args.dir, "toplist"))
    os.makedirs(os.path.join(args.dir, "counterfactual"))
  else:
    exit(0)

name = args.dir.split("/")[-1]

of = open(os.path.join(args.dir, "sample.obs"), 'w')
df = open(os.path.join(args.dir, "sample.db"), 'w')

mapfile = os.path.join(args.dir, name+".map")
mapf = open(mapfile, 'w')


of.write("hasword 0\n")
of.write("topic -2.04659\n")
of.write("links 0\n")
of.write("rl -0.11167\n")

obsFile = open("./data/hypertext-class/hypertext-class-test.db", "r")

# parse and sample observed file
obsList = []
for line in obsFile.readlines():
  line = line.strip()
  obsList.append(line)


dic = []
links = []
hasWords = {}
hasLinks = {}
beHasLinks = {}
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
    if a2 not in hasWords:
      hasWords[a2] = []
    hasWords[a2].append(a1)
    if a1 not in dic:
      dic.append(a1)
    if a2 not in links:
      links.append(a2)
  elif predicate=="links":
    if a1 not in beHasLinks:
      beHasLinks[a1] = []
    beHasLinks[a1].append(a2)
    if a2 not in hasLinks:
      hasLinks[a2] = []
    hasLinks[a2].append(a1)
    if a1 not in links:
      links.append(a1)
    if a2 not in links:
      links.append(a2)

print(len(hasWords))
print(len(dic))

m = {}
index_list = list(range(0, len(links)))
random.shuffle(index_list)
print(index_list)
for i in range(args.n):
  ind = index_list[i]
  raw_link = links[ind]
  if raw_link not in m:
    m[raw_link] = len(m)+1
  for word in hasWords[raw_link]:
    df.write("hasword "+word+" "+str(m[raw_link])+"\n")
    of.write("hasword_"+word+"_"+str(m[raw_link])+" 1\n")
  if raw_link not in hasLinks:
    continue
  for pointed in hasLinks[raw_link]:
    if pointed not in m:
      m[pointed] = len(m)+1
    df.write("links "+str(m[pointed])+" "+str(m[raw_link])+"\n")
    of.write("links_"+str(m[pointed])+"_"+str(m[raw_link])+" 1\n")
  if raw_link not in beHasLinks:
    continue
  for fromfile in beHasLinks[raw_link]:
    if fromfile not in m:
      m[fromfile] = len(m)+1
    df.write("links "+str(m[raw_link])+" "+str(m[fromfile])+"\n")
    of.write("links_"+str(m[raw_link])+"_"+str(m[fromfile])+" 1\n")
of.close()
df.close()


ml = sorted([(m[key], key) for key in m], key=lambda ele : ele[0])
for num, link in ml:
  mapf.write(str(num)+" "+link+"\n")
mapf.close()