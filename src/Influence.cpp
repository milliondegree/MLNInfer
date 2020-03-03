#include "Influence.h"

Influence::Influence() {
}

Influence::Influence(MLN& mln) {
  this->mln = mln;
}


void Influence::computeObsInfluence(string& target, vector<double>& probs) {
  set<string> obs = this->mln.getObsLiterals();
  set<string> queries = this->mln.getQueryLiterals();
  for (string s : queries) {
    this->pm[s] = vector<double> ();
  }
  int n = probs.size();
  vector<double> res (n, 0);
  // this->mln.gibbsSampling_v3(100000);
  for (int i=0; i<n; i++) {
    this->mln.setObsProb(target, probs[i]);
    this->mln.gibbsSampling_v3(100000);
    for (string s : queries) {
      this->pm[s].push_back(mln.queryProb(s));
    }
  }
}


vector<double> Influence::getProbs(string& query) {
  assert(this->pm.find(query)!=pm.end());
  return this->pm[query];
}

Influence::~Influence() {
}