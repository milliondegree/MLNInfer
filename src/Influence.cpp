#include "Influence.h"

Influence::Influence() {
}

Influence::Influence(MLN& mln) {
  this->mln = mln;
}


vector<double> Influence::computeObsInfluence(string& query, string& target, vector<double>& probs) {
  set<string> obs = this->mln.getObsLiterals();
  assert(obs.find(target)!=obs.end());
  int n = probs.size();
  vector<double> res (n, 0);
  this->mln.gibbsSampling_v3(100000);
  for (int i=0; i<n; i++) {
    this->mln.setObsProb(target, probs[i]);
    this->mln.gibbsSampling_v3(100000);
    res[i] = this->mln.queryProb(query);
  }
  return res;
}

Influence::~Influence() {
}