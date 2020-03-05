#ifndef _INFLUENCE_H
#define _INFLUENCE_H


#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <string>
#include <assert.h>
#include <set>
#include "MLN.h"

using namespace std;

class Influence {
public:
  Influence();
  Influence(MLN mln);

  void computeObsInfluence(string& target, vector<double>& probs);
  void generateInfluenceMap();
  vector<double> getProbs(string& query);
 
  ~Influence();

private:
  MLN mln;
  unordered_map<string, vector<double>> pm;
  unordered_map<string, unordered_set<string>> influMap;

};


#endif