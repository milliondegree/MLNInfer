import re

def compareInputs(counterfactual, origin):
  c = open(counterfactual, "r")
  o = open(origin, "r")
  c_dict = {}
  o_dict = {}
  for line in c.readlines():
    line = line.strip()
    splits = re.split(":| ", line)
    c_dict[splits[1]] = float(splits[-1])
  for line in o.readlines():
    line = line.strip()
    splits = re.split(":| ", line)
    o_dict[splits[2]] = float(splits[-1])
  diff = {}
  for name in c_dict:
    if name in o_dict:
      diff[name] = c_dict[name]-o_dict[name]
  l = [(name, diff[name]) for name in diff]
  l = sorted(l, key=lambda x: abs(x[1]), reverse=True)
  for name, value in l:
    print(name+": ",value)



if __name__ == "__main__":
  compareInputs("./data/vqa/church/counterfactual/ans_church.txt", "./data/vqa/church/counterfactual/origin.txt")