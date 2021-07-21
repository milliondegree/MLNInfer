import random

def monteCarlo(preds, cnf, n):
  pos = 0
  for i in range(n):
    sampled = {}
    for name in preds:
      if random.random() < preds[name]:
        sampled[name] = 1
      else:
        sampled[name] = 0
    for dnf in cnf:
      flag = True
      for name in dnf:
        if sampled[name] == 0:
          flag = False
          break
      if flag:
        pos += 1
        break
  return pos*1.0/n

if __name__ == '__main__':
  pa = 0.4
  pb = 0.5
  pc = 0.7
  assumption = 1-((1-pa*pb)*(1-pb*pc)*(1-pc*pa))
  preds = {
    'a': 0.4,
    'b': 0.5,
    'c': 0.7
  }
  cnf = [
    ['a', 'b'],
    ['b', 'c'],
    ['c', 'a']
  ]
  simulated = monteCarlo(preds, cnf, 100000)
  print(assumption)
  print(simulated)