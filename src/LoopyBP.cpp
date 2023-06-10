#include "MLN.h"


/**************************** loopy belief propagation *************************/

void enumerateTruth(Clique& c, 
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
      enumerateTruth(c, toSearch, truth, res, pos+1);
    }
  }
}


vector<double> loopyBPRun(MLN* mln, string query) {
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
    enumerateTruth(c, toSearch, truth, potential, 0);
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
      if (abs(it.second[1]-newDists[it.first][1])>1e-3) {
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


void enumerateLoopyBeliefPropagation(MLN* mln,
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
    vector<double> dist = loopyBPRun(mln, query);
    prob += tmp*dist[1];
  }
  else {
    for (int i=0; i<=1; i++) {
      prob_obs_truth[prob_obs[pos]] = i;
      enumerateLoopyBeliefPropagation(mln, query, prob_obs, prob_obs_truth, prev_probs, prob, pos+1);
    }
  }
}


void MLN::loopyBeliefPropagation(string query) {
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
  enumerateLoopyBeliefPropagation(this, query, prob_obs, prob_obs_truth, prev_probs, res, 0);
  this->prob[query] = res;
  for (string literal : this->obs) {
    this->setObsProb(literal, prev_probs[literal]);
  }
  return;
}






/************************* advance loopy belief propagation ***********************/

vector<double> pLoopyBPRun(MLN* mln, string query, map<int, vector<double>>& potentials) {
  // total initialization
  map<string, map<int, vector<double>>> nodeMsgs;
  map<int, map<string, vector<double>>> cliqueMsgs;
  map<string, vector<double>> dists;
  for (string literal : mln->queries) {
    for (int c : mln->c_map[literal]) {
      nodeMsgs[literal][c] = vector<double> (2, 1);
      cliqueMsgs[c][literal] = vector<double> (2, 0);
    }
    dists[literal] = vector<double> (2, 0.5);
  }
  // the loopy bp begins
  bool converge = false;
  int iteration = 0;
  while (!converge&&iteration<1000) {
    // cout << "iteration " << iteration << endl;
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
      // for (string literal : toQuery) {
      //   cout << newcliqueMsgs[c][literal][0] << ' ' << newcliqueMsgs[c][literal][1] << endl;
      // }
    }
    cliqueMsgs = newcliqueMsgs;
    // initialize dists
    map<string, vector<double>> newDists;
    for (string literal : mln->queries) {
      newDists[literal] = vector<double> (2, 0.5);
    }
    // pass clique messages to nodes
    // cout << "clique to node" << endl;
    for (string literal : mln->queries) {
      for (int c : mln->c_map[literal]) {
        newDists[literal][0] *= cliqueMsgs[c][literal][0];
        newDists[literal][1] *= cliqueMsgs[c][literal][1];
      }
      double z = newDists[literal][0]+newDists[literal][1];
      newDists[literal][0] /= z;
      newDists[literal][1] /= z;
      for (int c : mln->c_map[literal]) {
        nodeMsgs[literal][c][0] = dists[literal][0]/cliqueMsgs[c][literal][0];
        nodeMsgs[literal][c][1] = dists[literal][1]/cliqueMsgs[c][literal][1];
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

void enumerateExpectedTruth(Clique& c, 
                    vector<string>& toSearch,
                    set<string>& prob_obs,
                    set<string>& nonprob_obs,
                    map<string, int>& truth, 
                    map<string, double>& prev_probs,
                    vector<double>& potential,
                    double prob,
                    int pos) {
  if (pos==truth.size()) {
    double pot = prob * exp(c.getPotential(truth));
    // compute index
    int index = 0;
    for (int i=toSearch.size()-1; i>=0; i--) {
      int value = truth[toSearch[i]];
      index += (int)pow(2, toSearch.size()-i-1)*value;
    }
    // cout << index << endl;
    potential[index] += pot;
  }
  else {
    string literal = c.getLiterals()[pos];
    if (prob_obs.find(literal)!=prob_obs.end()) {
      truth[literal] = 0;
      enumerateExpectedTruth(c, toSearch, prob_obs, nonprob_obs, truth, prev_probs, potential, prob*(1-prev_probs[literal]), pos+1);
      truth[literal] = 1;
      enumerateExpectedTruth(c, toSearch, prob_obs, nonprob_obs, truth, prev_probs, potential, prob*prev_probs[literal], pos+1);
    }
    else if (nonprob_obs.find(literal)!=nonprob_obs.end()) {
      truth[literal] = (int)prev_probs[literal];
      enumerateExpectedTruth(c, toSearch, prob_obs, nonprob_obs, truth, prev_probs, potential, prob, pos+1);
    }
    else {
      // unobserved tuples
      for (int i=0; i<=1; i++) {
        truth[literal] = i;
        enumerateExpectedTruth(c, toSearch, prob_obs, nonprob_obs, truth, prev_probs, potential, prob, pos+1);
      }
    }
  }
}


void MLN::pLoopyBeliefPropagation(string query) {
  assert(this->queries.find(query)!=this->queries.end());
  // save the original probs first
  map<string, double> prev_probs = this->prob;
  // determine the probabilistic observed tuples
  set<string> prob_obs;
  set<string> nonprob_obs;
  for (string literal : this->obs) {
    if (this->prob[literal]>0&&
        this->prob[literal]<1&&
        !Parser::isRuleName(literal)) {
      prob_obs.insert(literal);
    }
    else {
      nonprob_obs.insert(literal);
    }
  }
  map<int, vector<double>> potentials;
  for (int c=0; c<this->cliques.size(); c++) {
    map<string, int> truth;
    vector<string> toSearch;
    for (string literal : this->cliques[c].getLiterals()) {
      truth[literal] = 0;
      if (this->obs.find(literal)==this->obs.end()) {
        toSearch.push_back(literal);
      }
    }
    potentials[c] = vector<double> ((int)pow(2, toSearch.size()), 0);
    enumerateExpectedTruth(this->cliques[c], toSearch, prob_obs, nonprob_obs, truth, prev_probs, potentials[c], 1, 0);
  }
  // cout << "potential success" << endl;
  // for (int c=0; c<potentials.size(); c++) {
  //   cout << this->cliques[c].toString() << endl;
  //   for (auto it : potentials[c]) {
  //     cout << it << ' ';
  //   }
  //   cout << endl;
  // }
  vector<double> dist = pLoopyBPRun(this, query, potentials);
  this->prob[query] = dist[1];
  return;
}






/************************* naive LBP influence ***********************/

void enumerateNaiveLBPInfluence(MLN* mln, 
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
    double prob_value = loopyBPRun(mln, query)[1];
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
      enumerateNaiveLBPInfluence(mln, query, infl, prob_obs, truth, prev_probs, infl_value, pos+1);
    }
  }
}


void MLN::naiveLBPInfluence(string query, string infl) {
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
    enumerateNaiveLBPInfluence(this, query, infl, prob_obs, truth, prev_prob, infl_value, 0);
  }
  this->pd[query][infl] = infl_value;
}












/********************** Loopy BP with Monte Corlo Simulation ******************/


vector<double> loopyBPRunMCS(MLN* mln, string query, map<int, vector<double>>& potentials,
                            map<string, map<int, vector<double>>>& nodeMsgs,
                            map<int, map<string, vector<double>>>& cliqueMsgs,
                            map<string, vector<double>>& dists,
                            map<string, vector<double>>& newDists) {
  // the loopy bp begins
  bool converge = false;
  int iteration = 0;
  while (!converge&&iteration<10) {
    // pass node messages to cliques:
    for (int c=0; c<mln->cliques.size(); c++) {
      vector<string> toQuery;
      for (string literal : mln->cliques[c].getLiterals()) {
        if (mln->queries.find(literal)!=mln->queries.end()) {
          toQuery.push_back(literal);
          cliqueMsgs[c][literal][0] = 0;
          cliqueMsgs[c][literal][1] = 0;
        }
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
          cliqueMsgs[c][toQuery[s]][truth_value] += value/nodeMsgs[toQuery[s]][c][truth_value];
          tmp /= 2;
        }
      }
      for (string literal : toQuery) {
        double z = cliqueMsgs[c][literal][0]+cliqueMsgs[c][literal][1];
        cliqueMsgs[c][literal][0] /= z;
        cliqueMsgs[c][literal][1] /= z;
      }
    }
    // pass clique messages to nodes
    for (string literal : mln->queries) {
      newDists[literal][0] = 1;
      newDists[literal][1] = 1;
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
    iteration += 1;

    for (auto it : dists) {
      dists[it.first][0] = newDists[it.first][0];
      dists[it.first][1] = newDists[it.first][1];
    }
  }
  return dists[query];
}


void enumerateTotalPotentials(Clique& c,
                              map<int, vector<double>>& clique_potentials,
                              map<string, int>& truth,
                              set<string>& prob_obs,
                              unordered_set<string>& queries,
                              vector<int>& obs_indices,
                              int pos
) {
  if (pos==c.getLiterals().size()) {
    int obs_index = 0;
    int obs_power = 0;
    if (obs_indices.size()==0) {
      obs_index = -1;
    }
    else {
      for (int i=c.getLiterals().size()-1; i>=0; i--) {
        string literal = c.getLiterals()[i];
        if (prob_obs.find(literal)!=prob_obs.end()) {
          obs_index += (int)pow(2, obs_power++)*truth[literal];
        }
      }
    }
    if (clique_potentials.find(obs_index)==clique_potentials.end()) {
      clique_potentials[obs_index] = vector<double> ();
    }
    clique_potentials[obs_index].push_back(exp(c.getPotential(truth)));
  }
  else {
    string literal = c.getLiterals()[pos];
    if (prob_obs.find(literal)!=prob_obs.end()) {
      for (int i=0; i<=1; i++) {
        truth[literal] = i;
        obs_indices.push_back(i);
        enumerateTotalPotentials(c, clique_potentials, truth, prob_obs, queries, obs_indices, pos+1);
        obs_indices.pop_back();
      }
    }
    else if (queries.find(literal)!=queries.end()) {
      for (int i=0; i<=1; i++) {
        truth[literal] = i;
        enumerateTotalPotentials(c, clique_potentials, truth, prob_obs, queries, obs_indices, pos+1);
      }
    }
    else {
      enumerateTotalPotentials(c, clique_potentials, truth, prob_obs, queries, obs_indices, pos+1);
    }
  }
}


void MLN::loopyBeliefPropagationMCS(string query, int rounds) {
  assert(this->queries.find(query)!=this->queries.end());
  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0,1.0);

  // determine the probabilistic observed tuples
  set<string> prob_obs;
  for (string literal : this->obs) {
    if (this->prob[literal]>0&&
        this->prob[literal]<1&&
        !Parser::isRuleName(literal)) {
      prob_obs.insert(literal);
    }
  }

  // build total potentials
  map<int, map<int, vector<double>>> total_potentials;
  for (int i=0; i<this->cliques.size(); i++) {
    Clique c = this->cliques[i];
    map<int, vector<double>> tmp = c.getAllPotentials();
    if (tmp.size()!=0) {
      total_potentials[i] = tmp;
    }
    else {
      map<string, int> truth;
      for (string literal : c.getLiterals()) {
        if (this->obs.find(literal)!=this->obs.end()&&prob_obs.find(literal)==prob_obs.end()) {
          truth[literal] = (int)this->prob[literal];
        }
      }
      vector<int> obs_indices;
      enumerateTotalPotentials(c, total_potentials[i], truth, prob_obs, this->queries, obs_indices, 0);
    }
  }

  // initialization
  map<string, map<int, vector<double>>> nodeMsgs;
  map<int, map<string, vector<double>>> cliqueMsgs;
  map<string, vector<double>> dists;
  map<string, vector<double>> newDists;
  for (string literal : this->queries) {
    for (int c : this->c_map[literal]) {
      nodeMsgs[literal][c] = vector<double> (2, 1);
      cliqueMsgs[c][literal] = vector<double> (2, 0);
    }
    dists[literal] = vector<double> (2, 0.5);
    newDists[literal] = vector<double> (2, 0.5);
  }
  
  // start monte carlo simulation + loopy bp
  map<string, double> records;
  map<string, int> count;
  map<string, int> sample;
  map<int, vector<double>> potentials;
  double sum = 0;
  for (int r=0; r<rounds; r++) {
    // cout << "round "  << r << endl;
    string key = "";
    for (string literal : prob_obs) {
      double r = distribution(generator);
      if (r<this->prob[literal]) {
        sample[literal] = 1;
      }
      else {
        sample[literal] = 0;
      }
      key += to_string(sample[literal]);
      count[literal] += sample[literal];
    }
    if (records.find(key)!=records.end()) {
      sum += records[key];
      continue;
    }
    for (int ci=0; ci<this->cliques.size(); ci++) {
      Clique c = this->cliques[ci];
      int index = 0;
      int index_power = -1;
      for (int i=c.getLiterals().size()-1; i>=0; i--) {
        string literal = c.getLiterals()[i];
        if (prob_obs.find(literal)!=prob_obs.end()) {
          index += (int)pow(2, ++index_power)*sample[literal];
        }
      }
      index = index_power==-1 ? -1 : index;
      potentials[ci] = total_potentials[ci][index];
    }
    
    double p = loopyBPRunMCS(this, query, potentials, nodeMsgs, cliqueMsgs, dists, newDists)[1];
    records[key] = p;
    sum += p;
  }
  this->prob[query] = sum/rounds;
}