#ifndef _MLN_H
#define _MLN_H

#include <iostream>
#include <string>
#include <map>
#include <assert.h>
#include "Clique.h"
using namespace std;


class MLN {
public:
  MLN();
  MLN(string prov, map<string, double> prob);

  void removeRedundant(string& prov);
  void build(string prov);

  ~MLN();
private:
  string prov;
  vector<Clique> cliques;
  set<string> obs;
  map<string, double> prob;
  map<string, vector<int>> c_map;
};


#endif