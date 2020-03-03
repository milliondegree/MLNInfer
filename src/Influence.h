#ifndef _INFLUENCE_H
#define _INFLUENCE_H


#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include "MLN.h"

using namespace std;

class Influence {
public:
  Influence();
  Influence(MLN& mln);

  vector<double> computeObsInfluence(string& query, string& target, vector<double>& probs);
 
  ~Influence();

private:
  MLN mln;

};


#endif