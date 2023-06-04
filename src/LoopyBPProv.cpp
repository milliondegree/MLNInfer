#include "MLN.h"
#include "CProvGraph.h"


void enumerateTruthWithProv(Clique& c, 
                    vector<string>& toSearch,
                    map<string, int>& truth, 
                    vector<double>& res,
                    int pos) {
  if (pos==toSearch.size()) {
    res.push_back(exp(c.getPotential(truth)));
  }
  else {
    for (int i=0; i<=1; i++) {
      truth[toSearch[pos]] = i;
      enumerateTruthWithProv(c, toSearch, truth, res, pos+1);
    }
  }
}


vector<double> loopyBPRunWithProv(MLN* mln, string query) {
  // total initialization
  map<string, map<int, vector<double>>> nodeMsgs;
  map<int, map<string, vector<double>>> cliqueMsgs;
  map<int, vector<double>> potentials;
  map<string, vector<double>> dists;
  for (string literal : mln->queries) {
    for (int c : mln->c_map[literal]) {
      nodeMsgs[literal][c] = vector<double> (2, 1);
      cliqueMsgs[c][literal] = vector<double> (2, 0);
    }
    dists[literal] = vector<double> (2, 0.5);
  }
  for (auto it1 : cliqueMsgs) {
    Clique c = mln->cliques[it1.first];
    vector<string> toSearch;
    for (auto it2 : it1.second) {
      toSearch.push_back(it2.first);
    }
    map<string, int> truth;
    for (string literal : c.getLiterals()) {
      if (mln->obs.find(literal)!=mln->obs.end()) {
        truth[literal] = mln->prob[literal];
      }
    }
    vector<double> potential;
    enumerateTruthWithProv(c, toSearch, truth, potential, 0);
    potentials[it1.first] = potential;
  }

  // the loopy bp begins
  bool converge = false;
  int iteration = 0;
  while (!converge&&iteration<1000) {
    // initialization newcliqueMsgs
    map<int, map<string, vector<double>>> newcliqueMsgs;
    for (auto it1 : nodeMsgs) {
      for (auto it2 : it1.second) {
        newcliqueMsgs[it2.first][it1.first] = vector<double> (2, 0);
      }
    }
    // pass node messages to cliques:
    for (auto it1 : cliqueMsgs) {
      int c = it1.first;
      vector<string> toQuery;
      for (auto it : cliqueMsgs[c]) {
        toQuery.push_back(it.first);
      }
      for (int i=0; i<potentials[c].size(); i++) {
        int tmp = i;
        double value = potentials[c][i];
        for (int s=toQuery.size()-1; s>=0; s--) {
          int truth_value = tmp%2;
          value *= nodeMsgs[toQuery[s]][c][truth_value];
          tmp /= 2;
        }
        tmp = i;
        for (int s=toQuery.size()-1; s>=0; s--) {
          int truth_value = tmp%2;
          newcliqueMsgs[c][toQuery[s]][truth_value] += value/nodeMsgs[toQuery[s]][c][truth_value];
          tmp /= 2;
        }
      }
      for (string literal : toQuery) {
        double z = newcliqueMsgs[c][literal][0]+newcliqueMsgs[c][literal][1];
        newcliqueMsgs[c][literal][0] /= z;
        newcliqueMsgs[c][literal][1] /= z;
      }
    }
    cliqueMsgs = newcliqueMsgs;
    // initialize dists
    map<string, vector<double>> newDists;
    for (string literal : mln->queries) {
      newDists[literal] = vector<double> (2, 0.5);
    }
    // pass clique messages to nodes
    for (string literal : mln->queries) {
      for (int c : mln->c_map[literal]) {
        newDists[literal][0] *= cliqueMsgs[c][literal][0];
        newDists[literal][1] *= cliqueMsgs[c][literal][1];
      }
      double z = newDists[literal][0]+newDists[literal][1];
      newDists[literal][0] /= z;
      newDists[literal][1] /= z;
      for (int c : mln->c_map[literal]) {
        nodeMsgs[literal][c][0] = newDists[literal][0]/cliqueMsgs[c][literal][0];
        nodeMsgs[literal][c][1] = newDists[literal][1]/cliqueMsgs[c][literal][1];
      }
    }
    // check convergence
    converge = true;
    for (auto it : dists) {
      if (abs(it.second[1]-newDists[it.first][1])>1e-7) {
        converge = false;
      }
    }
    dists = newDists;
    // cout << dists[query][0] << ' ' << dists[query][1] << endl;
    iteration += 1;
  }
  // get the final result
  // cout << "final iterations: " << iteration << endl;
  // cout << dists[query][0] << ' ' << dists[query][1] << endl; 
  return dists[query];
}

void enumerateLoopyBeliefPropagationWithProv(MLN* mln,
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
    vector<double> dist = loopyBPRunWithProv(mln, query);
    prob += tmp*dist[1];
  }
  else {
    for (int i=0; i<=1; i++) {
      prob_obs_truth[prob_obs[pos]] = i;
      enumerateLoopyBeliefPropagationWithProv(mln, query, prob_obs, prob_obs_truth, prev_probs, prob, pos+1);
    }
  }
}

void MLN::loopyBeliefPropagationWithProv(string query) {
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
  enumerateLoopyBeliefPropagationWithProv(this, query, prob_obs, prob_obs_truth, prev_probs, res, 0);
  this->prob[query] = res;
  for (string literal : this->obs) {
    this->setObsProb(literal, prev_probs[literal]);
  }
  return;
}
