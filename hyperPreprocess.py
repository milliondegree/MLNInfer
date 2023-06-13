
def to_alchemy_db(file_list, to_write_dict):
  for file_path in file_list:
    file_name = file_path.split("/")[-1]
    with open(file_path, 'r') as f:
      with open(to_write_dict+"/"+file_name, 'w') as w_f:
        for line in f.readlines():
          splits = line.strip().split(" ")
          if splits[0]=="hasword":
            w_f.write("HasWord(\""+splits[1]+"\",\""+splits[2]+"\")\n")
          elif splits[0]=="links":
            w_f.write("Links(\""+splits[1]+"\",\""+splits[2]+"\")\n")



if __name__=="__main__":
  to_alchemy_db(["./data/hypertext-class/sample7/sample71.db",
                "./data/hypertext-class/sample7/sample72.db",
                "./data/hypertext-class/sample7/sample73.db",
                "./data/hypertext-class/sample7/sample74.db",
                "./data/hypertext-class/sample7/sample75.db",
                "./data/hypertext-class/sample7/sample76.db",
                "./data/hypertext-class/sample7/sample77.db",
                "./data/hypertext-class/sample7/sample78.db",
                "./data/hypertext-class/sample7/sample79.db",
                "./data/hypertext-class/sample7/sample710.db"],
                "./data/hypertext-class/sample7/alchemy")