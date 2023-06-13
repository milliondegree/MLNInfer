from turtle import title
import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42

def modifyWords(word):
  if word=="robert":
    return "robot"
  if word=="hardwar":
    return "hardware"
  if word=="univers":
    return "university"
  if word=="depart":
    return "department"
  if word=="cours":
    return "course"
  if word=="technologi":
    return "technology"
  if word=="colleg":
    return "college"
  if word=="advanc":
    return "advance"
  return word

def buildRuleNameLabel(rule_name):
  if rule_name=="r1274":
    return "r1274(\"text\")"
  elif rule_name=="r2182":
    return "r2182(\"graphic\")"
  elif rule_name=="r371":
    return "r371(\"algorithm\")"
  elif rule_name=="r3936":
    return "r3936(\"time\")"
  elif rule_name=="r1328":
    return "r1328(\"hardware\")"
  elif rule_name=="r388":
    return "r388(\"software\")"
  elif rule_name=="r2400":
    return "r2400(\"game\")"
  elif rule_name=="r4992":
    return "r4992(\"manage\")"

def intToRank(n):
  if n%10==1 and n!=11:
    return str(n)+"st"
  elif n%10==2 and n!=12:
    return str(n)+"nd"
  elif n%10==3 and n!=13:
    return str(n)+"rd"
  return str(n)+"th"

def drawRanks(file_list, top_k, query, xticklabels, to_save):
  data = {}
  for clique_number, file_name in enumerate(file_list):
    f = open(file_name, 'r')
    rank = 1
    for line in f.readlines():
      splits = line.strip().split(' ')
      tuple_name = splits[2]
      influ = splits[-1]
      if tuple_name not in data:
        data[tuple_name] = {'x': [],
                            'rank': []}
      data[tuple_name]['x'].append(clique_number)
      data[tuple_name]['rank'].append(rank)
      rank += 1
  data_list = sorted([(key, data[key]['x'], data[key]['rank']) for key in data], key=lambda ele: ele[2][-1])


  fs1 = 34
  fs2 = 26
  fig, ax = plt.subplots(figsize=(13,9))
  for i in range(top_k):
    x = []
    y = []
    splits = data_list[i][0].split('_')
    tuple_name = "sim" if splits[0].startswith("sim") else splits[0]
    tuple_name = "hasWord" if splits[0].startswith("hasword") else tuple_name
    label = ""
    if len(splits)==1:
      label = buildRuleNameLabel(tuple_name)
    else:
      label = tuple_name+"("
      for j in range(1, len(splits)):
        label += "\""+modifyWords(splits[j])+"\"" if not splits[j].isdigit() else splits[j]
        if j!=len(splits)-1:
          label += ","
      label += ")"
    for j in range(len(data_list[i][1])):
      if data_list[i][2][j]<=top_k:
        x.append(data_list[i][1][j])
        y.append(data_list[i][2][j])
    ax.plot(x, y, label=label, marker='o', markersize=15, linewidth=3)
  box = ax.get_position()
  ax.set_position([box.x0, box.y0, box.width, box.height*0.77])
  ax.set_xlabel("Approximate subgraph size (# of cliques)", fontsize=fs1)
  ax.set_ylabel("Rank of "+query, fontsize=fs1)
  ax.set_xticks(list(range(len(file_list))))
  ax.set_xticklabels(xticklabels, fontsize=fs2)
  ax.set_yticks(list(range(1,top_k+1)))
  ax.set_yticklabels([intToRank(ele) for ele in range(1,top_k+1)], fontsize=fs2)
  ax.grid()
  ax.legend(fontsize=27, ncol=2, loc='upper center', bbox_to_anchor=(0.475,1.5),fancybox=True)
  # plt.title(query, fontsize=fs1)
  # plt.show()
  plt.savefig(to_save)



if __name__ == '__main__':
  # drawRanks([
  #         './data/vqa/church/toplist/after_3.txt',
  #         './data/vqa/church/toplist/after_4.txt',
  #         './data/vqa/church/toplist/after_5.txt',
  #         './data/vqa/church/toplist/after_6.txt',
  #         './data/vqa/church/toplist/after_7.txt',
  #         './data/vqa/church/toplist/after_8.txt',
  #         './data/vqa/church/toplist/after_9.txt',
  #         './data/vqa/church/toplist/before.txt'
  #         ],
  #         8,
  #         "ans(\"barn\")",
  #         ['3','4','5','6','7','8', '9', '28'],
  #         "./data/vqa/image/rank_ans_barn.pdf")  
  # drawRanks([
  #         './data/vqa/church/toplist2/after_3.txt',
  #         './data/vqa/church/toplist2/after_4.txt',
  #         './data/vqa/church/toplist2/after_5.txt',
  #         './data/vqa/church/toplist2/after_6.txt',
  #         './data/vqa/church/toplist2/after_7.txt',
  #         './data/vqa/church/toplist2/after_8.txt',
  #         './data/vqa/church/toplist2/after_9.txt',
  #         './data/vqa/church/toplist2/before2.txt'
  #         ],
  #         8,
  #         "ans(\"church\")",
  #         ['3','4','5','6','7','8', '9', '28'],
  #         "./data/vqa/image/rank_ans_church.pdf")    
  drawRanks([
          './data/hypertext-class/sample7/toplist/after_5.txt',
          './data/hypertext-class/sample7/toplist/after_10.txt',
          './data/hypertext-class/sample7/toplist/after_15.txt',
          './data/hypertext-class/sample7/toplist/after_20.txt',
          './data/hypertext-class/sample7/toplist/after_25.txt',
          './data/hypertext-class/sample7/toplist/after_30.txt',
          './data/hypertext-class/sample7/toplist/before.txt',
          ],
          8,
          "logical tuples",
          ["5", "10", "15", "20", "25", "30", "103"],
          "./data/hypertext-class/sample7/images/researchproject_43.pdf")   
  # drawRanks([
  #         './data/hypertext-class/sample7/toplist_r/after_5.txt',
  #         './data/hypertext-class/sample7/toplist_r/after_10.txt',
  #         './data/hypertext-class/sample7/toplist_r/after_15.txt',
  #         './data/hypertext-class/sample7/toplist_r/after_20.txt',
  #         './data/hypertext-class/sample7/toplist_r/after_25.txt',
  #         './data/hypertext-class/sample7/toplist_r/after_30.txt',
  #         './data/hypertext-class/sample7/toplist_r/before.txt',
  #         ],
  #         8,
  #         "rule weight tuples",
  #         ["5", "10", "15", "20", "25", "30", "103"],
  #         "./data/hypertext-class/sample7/images/r_researchproject_43.pdf") 
  # drawRanks([
  #         './data/hypertext-class/docbase2/toplist/after_5.txt',
  #         './data/hypertext-class/docbase2/toplist/after_10.txt',
  #         './data/hypertext-class/docbase2/toplist/after_15.txt',
  #         './data/hypertext-class/docbase2/toplist/after_20.txt',
  #         './data/hypertext-class/docbase2/toplist/after_25.txt',
  #         './data/hypertext-class/docbase2/toplist/after_30.txt',
  #         './data/hypertext-class/docbase2/toplist/before.txt',
  #         ],
  #         8,
  #         "logical tuples",
  #         ["5", "10", "15", "20", "25", "30", "117"],
  #         "./data/hypertext-class/docbase2/images/department_29.pdf") 
  # drawRanks([
  #         './data/vqa/terminal/toplist/after_3.txt',
  #         './data/vqa/terminal/toplist/after_4.txt',
  #         './data/vqa/terminal/toplist/after_5.txt',
  #         './data/vqa/terminal/toplist/after_6.txt',
  #         './data/vqa/terminal/toplist/after_7_heu.txt',
  #         './data/vqa/terminal/toplist/after_8_heu.txt',
  #         './data/vqa/terminal/toplist/after_9_heu.txt',
  #         './data/vqa/terminal/toplist/origin.txt'
  #         ],
  #         8,
  #         "ans(\"terminal\")",
  #         ['3','4','5','6','7','8','9', '31'],
  #         "./data/vqa/image/rank_ans_terminal.pdf")  