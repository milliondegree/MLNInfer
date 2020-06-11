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
  Influence(MLN& mln);

  double getPartialDeriv(string& query, string& infl);
  vector<string> getQueries();
  vector<vector<double>> getPartialDeriv();

  void computePartialDerivatives(MLN& mln, string& infl);
 
  ~Influence();

private:
  vector<string> queries;
  vector<vector<double>> partialDerivs;
  unordered_map<string, int> l_index;

  double getAccuPotential(string& numerator, string& denominator, map<string, double>& probs, vector<Clique>& cliques, map<string, vector<int>> c_map);
};


#endif