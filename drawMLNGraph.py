import matplotlib.pyplot as plt
import pygraphviz as pgv
import argparse


def parseFile(file_name):
  f = open(file_name, 'r');
  cliques = []
  obs = []
  que = []
  c_map = {}
  for line in f.readlines():
    line = line.strip()
    l = line.split(' ')
    if l[0]=='observed:':
      for ele in l[1:]:
        obs.append(ele)
    elif l[0]=='queried:':
      for ele in l[1:]:
        que.append(ele)
    elif l[0]=='clique:':
      ll = []
      for ele in l[1:]:
        ll.append(ele)
      cliques.append(ll)
    elif l[0]=='c_map:':
      ll = []
      for ele in l[2:]:
        ll.append(ele)
      c_map[l[1]] = ll
  return obs, que, cliques, c_map



if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument("--input", type=str)
  args = parser.parse_args()

  obs, que, cliques, c_map = parseFile(args.input)
  print(obs)
  print(que)
  print(cliques)
  print(c_map)