#include "Influence.h"

Influence::Influence() {
}

Influence::Influence(MLN& mln) {
  this->mln = mln;
}


vector<double> Influence::computeObsInfluence(string& query, string& target, vector<double>& probs) {
  int n = probs.size();
  vector<double> res (n, 0);
  return res;
}

Influence::~Influence() {
}