import os


if __name__=="__main__":

  sims = open("data/vqa/hotel/sim_obs.txt", "r")
  hasImg = open("data/vqa/hotel/hasimg_obs.txt", "r")
  hasQ = open("data/vqa/hotel/hasq_obs.txt", "r")
  word = open("data/vqa/hotel/word_obs.txt", "r")
  obs = open("data/vqa/observe/hotel.db", "w")

  tuples = ["word", "hasImg", "hasQ", "sim", "simRep1", "simRep2", "candidate",
            "hasImgAns", "ans", "ra", "rb", "r1", "r2", "r3", "r4", "r5", "r6"]

  weights = [0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 2, 2, 2, 2, 2, 10]

  for line in sims.readlines():
    l = line.strip().split("\t")
    ll = [ele for ele in l if len(ele)!=0]
    t = "sim_"
    for i in range(len(ll)):
      if i<len(ll)-2:
        t += ll[i]+"_"
      elif i==len(ll)-2:
        t += ll[i]+" "
      else:
        t += ll[i]+"\n"
    obs.write(t)
    t = t.replace("sim", "simRep1")
    obs.write(t)
    t = t.replace("simRep1", "simRep2")
    obs.write(t)

  for line in hasImg.readlines():
    l = line.strip().split("\t")
    ll = [ele for ele in l if len(ele)!=0]
    t = "hasImg_"
    for i in range(len(ll)):
      if i<len(ll)-2:
        t += ll[i]+"_"
      elif i==len(ll)-2:
        t += ll[i]+" "
      else:
        t += ll[i]+"\n"
    obs.write(t)

  for line in hasQ.readlines():
    l = line.strip().split("\t")
    ll = [ele for ele in l if len(ele)!=0]
    t = "hasQ_"
    for i in range(len(ll)):
      if i<len(ll)-2:
        t += ll[i]+"_"
      elif i==len(ll)-2:
        t += ll[i]+" "
      else:
        t += ll[i]+"\n"
    obs.write(t)

  for line in word.readlines():
    l = line.strip().split("\t")
    ll = [ele for ele in l if len(ele)!=0]
    t = "word_"
    for i in range(len(ll)):
      if i<len(ll)-2:
        t += ll[i]+"_"
      elif i==len(ll)-2:
        t += ll[i]+" "
      else:
        t += ll[i]+"\n"
    obs.write(t)

  for t, w in zip(tuples, weights):
    obs.write(t+" "+str(w)+"\n")