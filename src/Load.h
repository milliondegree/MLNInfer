#ifndef _LOAD_H
#define _LOAD_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>

using namespace std;



class Load {
public:
  Load();
  Load(string pfile, string ofile);

  vector<string> getProv();
  void setProv(string pfile);
  map<string, double> getProb();
  map<string, string> getSames();
  void setProb(string ofile); 

  ~Load();

private:
  vector<string> prov;
  map<string, double> prob;
  map<string, string> sames;
};


#endif