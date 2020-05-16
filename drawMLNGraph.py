import matplotlib.pyplot as plt
import pygraphviz as pgv
import argparse
import random


def setRuleNode(G, node_name):
  G.add_node(node_name)
  n = G.get_node(node_name)
  n.attr['shape'] = 'oval'
  n.attr['color'] = 'red'
  n.attr['fontsize'] = 15
  n.attr['height'] = 0.8
  n.attr['width'] = 1.2
  n.attr['fontname'] = 'sans-serif'
  n.attr['margin'] = 0.1


def setObsNode(G, node_name):
  G.add_node(node_name)
  n = G.get_node(node_name)
  n.attr['shape'] = 'oval'
  n.attr['fontsize'] = 15
  n.attr['fontname'] = 'sans-serif'
  n.attr['height'] = 0.6
  n.attr['width'] = 0.8
  n.attr['color'] = 'blue'


def setQueNode(G, node_name):
  G.add_node(node_name)
  n = G.get_node(node_name)
  n.attr['shape'] = 'oval'
  n.attr['fontsize'] = 15
  n.attr['fontname'] = 'sans-serif'
  n.attr['height'] = 0.6
  n.attr['width'] = 0.8
  n.attr['color'] = 'red'


def drawMLNGraph(obs, que, cliques, c_map, save_name):
  G = pgv.AGraph(strict=True, directed=False)
  # G.graph_attr['rankdir'] = 'BT'
  # G.graph_attr['ranksep'] = 1
  # G.graph_attr['margin'] = 0.1
  # G.graph_attr['mclimit'] = 0.1
  # G.graph_attr['nodesep'] = 0.1
  G.graph_attr['splines'] = False
  G.graph_attr['overlap'] = "scale"
  used = []
  for c in cliques:
    if random.random()>1:
      continue
    if len(c)==1:
      # if it is single clique
      continue
    for node in c[1:]:
      if not node in used:
        if node in obs:
          setObsNode(G, node)
        elif node in que:
          setQueNode(G, node)
        used.append(node)
    for i in range(1, len(c)):
      for j in range(i+1, len(c)):
        G.add_edge(c[i], c[j])
  G.layout()
  G.draw(save_name)


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


def drawDirected():
  G = pgv.AGraph(strict=True, directed=True)
  G.graph_attr['rankdir'] = 'BT'
  G.graph_attr['splines'] = False
  G.graph_attr['overlap'] = "scale"

  n1 = "live(\"Steve\",\"DC\")"
  n2 = "live(\"Elena\",\"DC\")"
  n3 = "like(\"Steve\",\"Veggies\")"
  n4 = "like(\"Elena\",\"Veggies\")"
  n5 = "know(\"Steve\",\"Elena\")"

  setObsNode(G, n1)
  setObsNode(G, n2)
  setRuleNode(G, "r1")
  G.add_edge(n1, "r1")
  G.add_edge(n2, "r1")
  setObsNode(G, n3)
  setObsNode(G, n4)
  setRuleNode(G, "r2")
  G.add_edge(n3, "r2")
  G.add_edge(n4, "r2")
  setObsNode(G, n5)
  G.add_edge("r1", n5)
  G.add_edge("r2", n5)
  
  G.layout(prog="dot")
  G.draw("./data/images/acquaintance.eps")


if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument("--input", type=str)
  parser.add_argument("--output", type=str)
  args = parser.parse_args()


  obs, que, cliques, c_map = parseFile(args.input)
  # print(obs)
  # print(que)
  # print(cliques)
  # print(c_map)
  drawMLNGraph(obs, que, cliques, c_map, args.output)

  # drawDirected()