
def transDBFile():
  dbin = open("sample33.db", 'r')
  dbout = open("test2.db", "w")

  for line in dbin.readlines():
    words = line.strip().split(' ')
    if words[0]=="hasword":
      dbout.write("HasWord(\""+words[1]+"\",\""+words[2]+"\")\n")
    elif words[0]=="links":
      dbout.write("Links(\""+words[1]+"\",\""+words[2]+"\")\n")

  dbin.close()
  dbout.close()



def transMLNFile():
  obs = open("sample33.obs", 'r')
  mlnin = open("sample3.olg", 'r')
  mlnout = open("sample3.mln", "w")

  probs = {}
  for line in obs.readlines():
    words = line.strip().split(' ')
    if len(words)!=0:
      probs[words[0]] = float(words[1])

  mlnout.write("Topic(class,page)\n")
  mlnout.write("HasWord(word,page)\n")
  mlnout.write("Links(page,page)\n")
  mlnout.write("0 HasWord(a1,a2)\n")
  mlnout.write("0 Topic(a1,a2)\n")
  mlnout.write("0 Links(a1,a2)\n")
  mlnout.write("1 !Links(a3,a2) v !Topic(a1,a3) v Topic(a1,a2)\n")

  for line in mlnin.readlines():
    words = line.strip().split(' ')
    if len(words)>1 and words[0]!="rl" and not words[0].startswith("mat") and not words[0].startswith("rs"):
      mlnout.write(str(probs[words[0]])+
                    " !HasWord(\""+
                    words[-1].split("\"")[-2]+
                    "\",a1) v Topic("+
                    words[-2].split("\"")[-2]+
                    ",a1)\n"
                  )

  obs.close()
  mlnin.close()
  mlnout.close()


if __name__=="__main__":
  transMLNFile()