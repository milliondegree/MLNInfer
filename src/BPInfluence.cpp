#include "MLN.h"



/*************************** naive belief propagation *************************/
vector<double> recursiveOnClique(MLN* mln, string node, int c);
vector<double> recursiveOnNode(MLN* mln, int parent, string node);


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


vector<double> recursiveOnNode(MLN* mln, int parent, string node) {
  vector<double> marginalDist (2, 1);
  for (int ci : mln->c_map[node]) {
    if (ci!=parent) {
      vector<double> marginalTemp = recursiveOnClique(mln, node, ci);
      marginalDist[0] *= marginalTemp[0];
      marginalDist[1] *= marginalTemp[1];
    }
  }
  double z = marginalDist[0]+marginalDist[1];
  marginalDist[0] /= z;
  marginalDist[1] /= z;
  return marginalDist;
}



vector<double> recursiveOnClique(MLN* mln, string node, int c) {
  vector<double> res (2, 0);
  vector<string> literals = mln->cliques[c].getLiterals();
  map<string, vector<double>> nodeMsgs;
  vector<string> toSearch;
  for (string literal : literals) {
    if (literal!=node&&mln->queries.find(literal)!=mln->queries.end()) {
      nodeMsgs[literal] = recursiveOnNode(mln, c, literal);
      toSearch.push_back(literal);
    }
  }
  map<string, int> truth;
  for (string literal : literals) {
    if (mln->obs.find(literal)!=mln->obs.end()) {
      truth[literal] = mln->prob[literal];
    }
  }
  for (int i=0; i<=1; i++) {
    truth[node] = i;
    enumerateTruth(mln->cliques[c], nodeMsgs, toSearch, truth, res, node, 0);
  }
  return res;
}


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
    vector<double> dist = recursiveOnNode(mln, -1, query);
    prob += tmp*dist[1];
  }
  else {
    for (int i=0; i<=1; i++) {
      prob_obs_truth[prob_obs[pos]] = i;
      enumerateNaiveBeliefPropagation(mln, query, prob_obs, prob_obs_truth, prev_probs, prob, pos+1);
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
  vector<double> dist = recursiveOnNode(this, -1, query);
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







/************************ advance belief propagation **********************/

// map<string, vector<double>> recursiveOnCliqueAdvance(MLN* mln, string node, int c, vector<string>& prob_obs);
map<string, vector<double>> recursiveOnCliqueAdvance(
                                MLN* mln, 
                                string node, 
                                int c,
                                map<string, int> prob_obs
                                );

string buildIndex(string s1, string s2) {
  string res = "";
  for (int i=0; i<s1.size(); i++) {
    if (s1[i]=='2'&&s2[i]=='2') {
      res += '2';
    }
    else if (s1[i]!='2'&&s2[i]=='2') {
      res += s1[i];
    }
    else if (s1[i]=='2'&&s2[i]!='2') {
      res += s2[i];
    }
    else {
      cout << "index building error" << endl;
      exit(1);
    }
  }
  return res;
}


double priorProbability(string index, map<string, int>& prob_obs, map<string, double>& prev_probs) {
  double res = 1;
  for (auto it : prob_obs) {
    res *= index[it.second]=='0' ? (1-prev_probs[it.first]) : prev_probs[it.first];
  }
  return res;
}



string buildIndexFromMap(map<string, int>& truth, map<string, int>& prob_obs) {
  string res (prob_obs.size(), '2');
  for (auto it : truth) {
    if (prob_obs.find(it.first)!=prob_obs.end()) {
      res[prob_obs[it.first]] = it.second==0 ? '0' : '1';
    }
  }
  return res;
}


void enumerateCompAdvance(map<string, map<string, vector<double>>>& nodeMsgs,
                          vector<string>& queries,
                          string obs_index,
                          double potential,
                          map<string, int>& truth, 
                          map<string, vector<double>>& res, 
                          string& node,
                          int pos) {
  if (pos==nodeMsgs.size()) {
    if (res.find(obs_index)==res.end()) {
      res[obs_index] = vector<double> (2, 0);
    }
    res[obs_index][truth[node]] += potential;
    // cout << "node: " << node << ' ' << obs_index << ' ' << res[obs_index][truth[node]] << endl;
  }
  else {
    string tnode = queries[pos];
    // cout << tnode << endl;
    for (auto it : nodeMsgs[tnode]) {
      string s = buildIndex(obs_index, it.first);
      // cout << "new index: " << s << endl;
      enumerateCompAdvance(nodeMsgs, queries, s, potential*it.second[truth[tnode]], truth, res, node, pos+1);
    }
  }
}


void enumerateQueryTruthAdvance(Clique& c, 
                    map<string, map<string, vector<double>>>& nodeMsgs, 
                    vector<string>& queries,
                    string& obs_index,
                    map<string, int>& truth, 
                    map<string, vector<double>>& res, 
                    string& node,
                    int pos) {
  if (pos==nodeMsgs.size()) {
    double temp = exp(c.getPotential(truth));
    string index = obs_index;
    // cout << index << ' ' << temp << endl;
    // for (auto it : truth) {
    //   cout << it.first << ": " << it.second << ", ";
    // }
    // cout << endl;
    enumerateCompAdvance(nodeMsgs, queries, obs_index, temp, truth, res, node, 0);
  }
  else {
    for (int i=0; i<=1; i++) {
      truth[queries[pos]] = i;
      enumerateQueryTruthAdvance(c, nodeMsgs, queries, obs_index, truth, res, node, pos+1);
    }
  }
}


void enumerateObsTruthAdvance(Clique& c, 
                    map<string, map<string, vector<double>>>& nodeMsgs, 
                    vector<string>& obs,
                    vector<string>& queries,
                    map<string, int>& truth, 
                    map<string, int>& prob_obs,
                    map<string, vector<double>>& res, 
                    string& node,
                    int pos) {
  if (pos==obs.size()) {
    // cout << "obs size: " << obs.size() << endl; 
    string obs_index = buildIndexFromMap(truth, prob_obs);
    enumerateQueryTruthAdvance(c, nodeMsgs, queries, obs_index, truth, res, node, 0);
  }
  else {
    for (int i=0; i<=1; i++) {
      truth[obs[pos]] = i;
      enumerateObsTruthAdvance(c, nodeMsgs, obs, queries, truth, prob_obs, res, node, pos+1);
    }
  }
}




map<string, vector<double>> recursiveOnNodeAdvance(
                            MLN* mln,
                            int parent, 
                            string node,
                            map<string, int>& prob_obs
                            ) {
  map<string, vector<double>> marginalDist;
  for (int ci : mln->c_map[node]) {
    if (ci!=parent) {
      map<string, vector<double>> marginalTemp = recursiveOnCliqueAdvance(mln, node, ci, prob_obs);
      // cout << "node: " << node << " ci " << ci << endl;
      // cout << mln->cliques[ci].toString() << endl;
      // for (auto it : marginalTemp) {
      //   cout << it.first << ' ' << it.second[0] << ' ' << it.second[1] << endl;
      // }
      if (marginalDist.size()==0) {
        marginalDist = marginalTemp;
      }
      else {
        map<string, vector<double>> temp;
        for (auto it1 : marginalDist) {
          for (auto it2 : marginalTemp) {
            string index = buildIndex(it1.first, it2.first);
            temp[index] = vector<double> {it1.second[0]*it2.second[0], it1.second[1]*it2.second[1]};
          }
        }
        marginalDist = temp;
      }
    }
  }
  for (auto it : marginalDist) {
    double z = it.second[0]+it.second[1];
    marginalDist[it.first][0] /= z;
    marginalDist[it.first][1] /= z;
    // cout << "node: " << node << ' ' << it.first << ' ' << it.second[0] << ' ' << it.second[1] <<endl;
  }
  // cout << endl;
  return marginalDist;
}


map<string, vector<double>> recursiveOnCliqueAdvance(
                                MLN* mln, 
                                string node, 
                                int c,
                                map<string, int> prob_obs
                                ) {
  map<string, vector<double>> res;
  vector<string> literals = mln->cliques[c].getLiterals();
  map<string, map<string, vector<double>>> nodeMsgs;
  vector<string> queries;
  vector<string> obs;
  for (string literal : literals) {
    if (literal!=node&&mln->queries.find(literal)!=mln->queries.end()) {
      nodeMsgs[literal] = recursiveOnNodeAdvance(mln, c, literal, prob_obs);
      queries.push_back(literal);
    }
    else if (prob_obs.find(literal)!=prob_obs.end()) {
      obs.push_back(literal);
    }
  }
  map<string, int> truth;
  for (string literal : literals) {
    if (mln->obs.find(literal)!=mln->obs.end()&&prob_obs.find(literal)==prob_obs.end()) {
      truth[literal] = mln->prob[literal];
    }
  }
  // cout << "node: " << node << "ci: " << mln->cliques[c].toString() << endl;
  // for (auto it : nodeMsgs) {
  //   cout << it.first << ' ';
  // }
  // cout << endl;
  for (int i=0; i<=1; i++) {
    truth[node] = i;
    enumerateObsTruthAdvance(mln->cliques[c], nodeMsgs, obs, queries, truth, prob_obs, res, node, 0);
  }
  return res;
}


void MLN::advanceBeliefPropagation(string query) {
  assert(this->queries.find(query)!=this->queries.end());
  // save the original probs first
  map<string, double> prev_probs = this->prob;
  // find the probabilistic observed tuples
  map<string, int> prob_obs;
  for (string literal : this->obs) {
    if (this->prob[literal]>0&&
        this->prob[literal]<1&&
        !Parser::isRuleName(literal)) {
      prob_obs[literal] = prob_obs.size();
    }
  }
  // for (auto it : prob_obs) {
  //   cout << it.first << ' ' << it.second << endl;
  // }
  map<string, vector<double>> dists = recursiveOnNodeAdvance(this, -1, query, prob_obs);
  double res = 0;
  for (auto it : dists) {
    res += priorProbability(it.first, prob_obs, prev_probs)*it.second[1];
  }
  this->prob[query] = res;
  return;
}


