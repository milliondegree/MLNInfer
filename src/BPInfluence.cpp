#include "MLN.h"


void enumerateNaiveBeliefPropagation(MLN* mln,
                                    string& query,
                                    vector<string> prob_obs,
                                    map<string, int> prob_obs_truth,
                                    map<string, double> prev_probs,
                                    double& prob,
                                    int pos
                                    ) {
  /*
  prob_obs: probabilistic observed tuples
  prob_obs_truth: the truth table of prob_obs to enumerate
  prev_probs: the original probabilities of prob_obs
  prob: the probability of query to estimate
  */
  if (pos==prob_obs.size()) {
    double tmp = 1.0;
    for (string literal : prob_obs) {
      mln->setObsProb(literal, prob_obs_truth[literal]);
      tmp *= prob_obs_truth[literal]==0 ? (1-prev_probs[literal]) : prev_probs[literal];
    }
    vector<double> dist = mln->recursiveOnNode(-1, query);
    prob += tmp*dist[1];
  }
  else {
    for (int i=0; i<=1; i++) {
      prob_obs_truth[prob_obs[pos]] = i;
      enumerateNaiveBeliefPropagation(mln, query, prob_obs, prob_obs_truth, prev_probs, prob, pos+1);
    }
  }
}


void enumerateTruth(Clique& c, 
                    map<string, vector<double>>& nodeMsgs, 
                    vector<string>& toSearch,
                    map<string, int>& truth, 
                    vector<double>& res, 
                    string& node,
                    int pos) {
  if (pos==nodeMsgs.size()) {
    double temp = exp(c.getPotential(truth));
    for (auto it : nodeMsgs) {
      temp *= it.second[truth[it.first]];
    }
    res[truth[node]] += temp;
  }
  else {
    for (int i=0; i<=1; i++) {
      truth[toSearch[pos]] = i;
      enumerateTruth(c, nodeMsgs, toSearch, truth, res, node, pos+1);
    }
  }
}



void MLN::nonProbabilisticBeliefPropagation(string query) {
  assert(this->queries.find(query)!=this->queries.end());
  vector<string> prob_obs;
  for (string literal : this->obs) {
    if (this->prob[literal]>0&&
        this->prob[literal]<1&&
        !Parser::isRuleName(literal)) {
      prob_obs.push_back(literal);
    }
  }
  assert(prob_obs.size()==0);
  vector<double> dist = recursiveOnNode(-1, query);
  this->prob[query] = dist[1];
  return;
}


void MLN::naiveBeliefPropagation(string query) {
  assert(this->queries.find(query)!=this->queries.end());
  // save the original probs first
  map<string, double> prev_probs = this->prob;
  // determine the probabilistic observed tuples
  vector<string> prob_obs;
  for (string literal : this->obs) {
    if (this->prob[literal]>0&&
        this->prob[literal]<1&&
        !Parser::isRuleName(literal)) {
      prob_obs.push_back(literal);
    }
  }
  map<string, int> prob_obs_truth;
  double res = 0;
  enumerateNaiveBeliefPropagation(this, query, prob_obs, prob_obs_truth, prev_probs, res, 0);
  this->prob[query] = res;
  for (string literal : this->obs) {
    this->setObsProb(literal, prev_probs[literal]);
  }
  return;
}


vector<double> MLN::recursiveOnNode(int parent, string node) {
  vector<double> marginalDist (2, 1);
  for (int ci : this->c_map[node]) {
    if (ci!=parent) {
      vector<double> marginalTemp = recursiveOnClique(node, ci);
      marginalDist[0] *= marginalTemp[0];
      marginalDist[1] *= marginalTemp[1];
    }
  }
  double z = marginalDist[0]+marginalDist[1];
  marginalDist[0] /= z;
  marginalDist[1] /= z;
  return marginalDist;
}



vector<double> MLN::recursiveOnClique(string node, int c) {
  vector<double> res (2, 0);
  vector<string> literals = this->cliques[c].getLiterals();
  map<string, vector<double>> nodeMsgs;
  vector<string> toSearch;
  for (string literal : literals) {
    if (literal!=node&&this->queries.find(literal)!=this->queries.end()) {
      nodeMsgs[literal] = recursiveOnNode(c, literal);
      toSearch.push_back(literal);
    }
  }
  map<string, int> truth;
  for (string literal : literals) {
    if (this->obs.find(literal)!=this->obs.end()) {
      truth[literal] = this->prob[literal];
    }
  }
  for (int i=0; i<=1; i++) {
    truth[node] = i;
    enumerateTruth(this->cliques[c], nodeMsgs, toSearch, truth, res, node, 0);
  }
  return res;
}






/********************** naive BP Influence **************************/


void enumerateNaiveBPInfluence(MLN* mln, 
                              string& query,
                              string& infl,
                              vector<string>& prob_obs,
                              map<string, int>& truth, 
                              map<string, double>& prev_probs,
                              double& infl_value, 
                              int pos) {
  if (pos==prob_obs.size()) {
    for (auto it : truth) {
      mln->setObsProb(it.first, it.second);
    }
    mln->nonProbabilisticBeliefPropagation(query);
    double prob_value = mln->queryProb(query);
    // determine the sign of influence
    double tmp = truth[infl]==1 ? 1 : -1;
    tmp *= prob_value;
    for (string literal : prob_obs) {
      if (truth[literal]==0) {
        tmp *= (1-prev_probs[literal]);
      }
      else {
        tmp *= prev_probs[literal];
      }
    }
    infl_value += tmp;
  }
  else {
    for (int i=0; i<=1; i++) {
      truth[prob_obs[pos]] = i;
      enumerateNaiveBPInfluence(mln, query, infl, prob_obs, truth, prev_probs, infl_value, pos+1);
    }
  }
}


void MLN::naiveBPInfluence(string query, string infl) {
  assert(this->queries.find(query)!=this->queries.end() &&
          this->obs.find(infl)!=this->obs.end());
  // save the original probabilities of probabilistic observed tuples first
  map<string, double> prev_prob = this->prob;
  // find all probabilistic observed tuples
  vector<string> prob_obs;
  for (string literal : this->obs) {
    if (this->prob[literal]>0&&
        this->prob[literal]<1&&
        !Parser::isRuleName(literal)) {
      prob_obs.push_back(literal);
    }
  }
  map<string, int> truth;
  double infl_value = 0.0;
  for (int i=0; i<=1; i++) {
    truth[infl] = i;
    enumerateNaiveBPInfluence(this, query, infl, prob_obs, truth, prev_prob, infl_value, 0);
  }
  this->pd[query][infl] = infl_value;
}
