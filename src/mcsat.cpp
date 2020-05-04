#include "MLN.h"


void MLN::mcsat(int round, string query) {
  // random generator
  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0, 1.0);
  // find valid unknown tuples and valid cliques
  unordered_set<int> c_idx;
  unordered_set<string> valid_unknown;
  vector<bool> visited (this->cliques.size());
  dfsSearch(valid_unknown, visited, query);
  for (auto& valid : valid_unknown) {
    for (int c_id : this->c_map[valid]) {
      c_idx.insert(c_id);
    }
  }
  unordered_set<string> valid_known;
  for (int c_id : c_idx) {
    for (auto& valid : this->cliques[c_id].getLiterals()) {
      if (this->obs.find(valid)!=this->obs.end()) {
        valid_known.insert(valid);
      }
    }
  }
  // initialize state map
  unordered_map<string, int> state;
  for (auto& query_name : valid_unknown) {
    double prand = distribution(generator);
    if (prand>0.5) {
      state[query_name] = 1;
    }
    else {
      state[query_name] = 0;
    }
  }
  for (auto& literal : valid_known) {
    state[literal] = 1;
  }
  // initialize numTrue map
  unordered_map<string, int> numTrue;
  for (auto& query_name : valid_unknown) {
    numTrue[query_name] = 0;
  }
  // make the current state satisify hard clauses
  for (int id : c_idx) {
    if (this->cliques[id].isHard()) {
      // to be implemented
    }
  }
  // start sampling
  for (int i=0; i<round; i++) {
    // initialize state value of observed tuples with probability
    /*
    for (auto& literal : valid_known) {
      double prand = distribution(generator);
      // double prand = (double)rand()/RAND_MAX;
      if (prand<this->prob[literal]) {
        state[literal] = 1;
      }
      else {
        state[literal] = 0;
      }
    }
    */
    unordered_set<int> m;
    // sample clauses and insert clique id into m
    for (int id : c_idx) {
      if (this->cliques[id].satisifiablity(state)) {
        double prand = distribution(generator);
        // double prand = (double)rand()/RAND_MAX;
        double p = 1-exp(-abs(this->cliques[id].getRuleWeight()));
        if (prand<p) {
          m.insert(id);
        }
      }
    }
    // cout << "size of subset: " << m.size() << endl;
    // for (int id : m) {
    //   this->cliques[id].printClique();
    // }
    sampleSAT(state, m, MAXFLIPS, MAXTRIES, TARGET, NOISE);
    // maxWalkSAT(state, m, MAXFLIPS, MAXTRIES, TARGET, NOISE);
    for (auto& query_name : valid_unknown) {
      if (state[query_name]==1) {
        numTrue[query_name]++;
      }
    }
  }
  // calculate probabilities of queried tuples
  for (auto& query_name : valid_unknown) {
    this->prob[query_name] = numTrue[query_name]*1.0 / round;
  }
}


void MLN::sampleSAT(unordered_map<string, int>& state, unordered_set<int>& c_idx, int maxFlips, int maxTries, double target, double p) {
  // initialize solution
  unordered_map<string, int> solu;
  string mode = "ss";
  for (string observe : this->obs) {
    solu[observe] = 1;
  }
  for (int t=0; t<maxTries; t++) {
    // initialize solution randomly
    // cout << "sample round " << t << endl;
    for (auto& query : this->queries) {
      double prand = (double)rand()/RAND_MAX;
      if (prand<0.5) {
        solu[query] = 1;
      }
      else {
        solu[query] = 0;
      }
    }
    double cost = getCost(solu, c_idx, mode);
    // flip literals
    for (int f=0; f<maxFlips; f++) {
      // terminate when cost < target
      if (cost<target) {
        for (auto& query : this->queries) {
          state[query] = solu[query];
        }
        return;
      }
      vector<int> false_idx;
      for (int c_id : c_idx) {
        if (!this->cliques[c_id].satisifiablity(solu)) {
          false_idx.push_back(c_id);
        }
      }
      int c_id = false_idx[rand()%false_idx.size()];
      double prand = (double)rand()/RAND_MAX;
      string toFlip;
      if (prand<p) {
        toFlip = randomPick(this->cliques[c_id]);
      }
      else {
        // toFlip = optimalPick(state, c_idx, mode);
        // toFlip = lowestPick(this->cliques[c_id], state, c_idx, mode);
        toFlip = ssPick(state, c_idx, mode);
      }
      if (toFlip!="_STAY") {
        solu[toFlip] = 1-solu[toFlip];
      }
      cost = getCost(solu, c_idx, mode);
    }
  }
  for (auto& query : this->queries) {
    state[query] = solu[query];
  }
}


void MLN::maxWalkSAT(unordered_map<string, int>& state, unordered_set<int>& c_idx, int maxFlips, int maxTries, double target, double p) {
  // initialize solution
  unordered_map<string, int> solu;
  string mode = "mws";
  for (string observe : this->obs) {
    solu[observe] = 1;
  }
  for (int t=0; t<maxTries; t++) {
    // initialize solution randomly
    // cout << "sample round " << t << endl;
    for (auto& query : this->queries) {
      double prand = (double)rand()/RAND_MAX;
      if (prand<0.5) {
        solu[query] = 1;
      }
      else {
        solu[query] = 0;
      }
    }
    double cost = getCost(solu, c_idx, mode);
    // flip literals
    for (int f=0; f<maxFlips; f++) {
      // cout << "flip round " << f << endl;
      if (cost<target) {
        for (auto& query : this->queries) {
          state[query] = solu[query];
        }
        return;
      }
      vector<int> false_idx;
      for (int c_id : c_idx) {
        if (!this->cliques[c_id].satisifiablity(solu)) {
          false_idx.push_back(c_id);
        }
      }
      int c_id = false_idx[rand()%false_idx.size()];
      double prand = (double)rand()/RAND_MAX;
      string toFlip;
      if (prand<p) {
        toFlip = randomPick(this->cliques[c_id]);
      }
      else {
        toFlip = lowestPick(this->cliques[c_id], state, c_idx, mode);
      }
      solu[toFlip] = 1-solu[toFlip];
      cost = getCost(solu, c_idx, mode);
    }
  }
  for (auto& query : this->queries) {
    state[query] = solu[query];
  }
}


/*
double MLN::getCost(unordered_map<string, int>& state, unordered_set<int>& c_idx) {
  double cost = 0.0;
  for (int id : c_idx) {
    if (!this->cliques[id].satisifiablity(state)) {
      cost += cliques[id].getCost("mws");
    }
  }
  return cost;
}
*/


double MLN::getCost(unordered_map<string, int>& state, unordered_set<int>& c_idx, string& mode) {
  double cost = 0.0;
  for (int id : c_idx) {
    if (!this->cliques[id].satisifiablity(state)) {
      cost += cliques[id].getCost(mode);
    }
  }
  return cost;
}


/*
double MLN::getSampleSATCost(unordered_map<string, int>& state, unordered_set<int>& c_idx) {
  double cost = 0.0;
  for (int id : c_idx) {
    if (!this->cliques[id].satisifiablity(state)) {
      cost += cliques[id].getCost("ss");
    }
  }
  return cost;
}
*/


string MLN::randomPick(Clique& c) {
  vector<string> valid;
  vector<string> literals = c.getLiterals();
  for (auto& literal : literals) {
    if (this->queries.find(literal)!=this->queries.end()) {
      valid.push_back(literal);
    }
  }
  int id = rand()%valid.size();
  return valid[id];
} 


string MLN::lowestPick(Clique& c, unordered_map<string, int>& state, unordered_set<int>& c_idx, string& mode) {
  vector<string> valid;
  vector<string> literals = c.getLiterals();
  for (auto& literal : literals) {
    if (this->queries.find(literal)!=this->queries.end()) {
      valid.push_back(literal);
    }
  }
  string res;
  double costChange = DBL_MAX;
  for (auto& literal : valid) {
    double prevCost = 0.0;
    for (int c_id : this->c_map[literal]) {
      if (c_idx.find(c_id)!=c_idx.end() && !this->cliques[c_id].satisifiablity(state)) {
      // if (!this->cliques[c_id].satisifiablity(state)) {
        prevCost += this->cliques[c_id].getCost(mode);
      }
    }
    state[literal] = 1-state[literal];
    double afterCost = 0.0;
    for (int c_id : c_idx) {
      if (c_idx.find(c_id)!=c_idx.end() && !this->cliques[c_id].satisifiablity(state)) {
      // if (!this->cliques[c_id].satisifiablity(state)) {
        prevCost += this->cliques[c_id].getCost(mode);
      }
    }
    state[literal] = 1-state[literal];
    if (afterCost-prevCost < costChange) {
      costChange = afterCost-prevCost;
      res = literal;
    }
  }
  return res;
}



string MLN::optimalPick(unordered_map<string, int>& state, unordered_set<int>& c_idx, string& mode) {
  unordered_set<string> valid;
  for (int c_id : c_idx) {
    vector<string> literals = this->cliques[c_id].getLiterals();
    for (auto& literal : literals) {
      if (this->queries.find(literal)!=this->queries.end()) {
        valid.insert(literal);
      }
    }
  }
  string res;
  double costChange = DBL_MAX;
  for (auto& literal : valid) {
    double prevCost = 0.0;
    for (int c_id : this->c_map[literal]) {
      if (!this->cliques[c_id].satisifiablity(state)) {
        prevCost += this->cliques[c_id].getCost(mode);
      }
    }
    state[literal] = 1-state[literal];
    double afterCost = 0.0;
    for (int c_id : this->c_map[literal]) {
      if (!this->cliques[c_id].satisifiablity(state)) {
        afterCost += this->cliques[c_id].getCost(mode);
      }
    }
    state[literal] = 1-state[literal];
    if (afterCost-prevCost < costChange) {
      costChange = afterCost-prevCost;
      res = literal;
    }
  }
  return res;
}


string MLN::ssPick(unordered_map<string, int>& state, unordered_set<int>& c_idx, string& mode) {
  unordered_set<string> valid_set;
  for (int c_id : c_idx) {
    vector<string> literals = this->cliques[c_id].getLiterals();
    for (auto& literal : literals) {
      if (this->queries.find(literal)!=this->queries.end()) {
        valid_set.insert(literal);
      }
    }
  }
  vector<string> valid;
  for (auto& literal : valid_set) {
    valid.push_back(literal);
  }
  string res = valid[rand()%valid.size()];
  double prev = 0.0;
  for (int c_id : this->c_map[res]) {
    if (c_idx.find(c_id)!=c_idx.end() && !this->cliques[c_id].satisifiablity(state)) {
      prev += this->cliques[c_id].getCost(mode);
    }
  }
  state[res] = 1-state[res];
  double after = 0.0;
  for (int c_id : this->c_map[res]) {
    if (c_idx.find(c_id)!=c_idx.end() && !this->cliques[c_id].satisifiablity(state)) {
      after += this->cliques[c_id].getCost(mode);
    }
  }
  state[res] = 1-state[res];
  double delta = after-prev;
  if (delta<0) {
    // if cost is less, directly return the random result
    return res;
  }
  double p = exp(-(delta)/SATEMPERATURE);
  double prand = (double)rand() / RAND_MAX;
  if (prand<p) {
    return res;
  }
  return "_STAY";
}







// probabilistic mcsat algorithm
void MLN::pmcsat(int round, string query) {
  // cout << "running pmcsat" << endl;
  // random generator
  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0, 1.0);
  // find valid unknown tuples and valid cliques
  unordered_set<int> c_idx;
  unordered_set<string> valid_unknown;
  vector<bool> visited (this->cliques.size());
  dfsSearch(valid_unknown, visited, query);
  for (auto& valid : valid_unknown) {
    for (int c_id : this->c_map[valid]) {
      c_idx.insert(c_id);
    }
  }
  unordered_set<string> valid_known;
  for (int c_id : c_idx) {
    for (auto& valid : this->cliques[c_id].getLiterals()) {
      if (this->obs.find(valid)!=this->obs.end()) {
        valid_known.insert(valid);
      }
    }
  }
  // initialize state map
  unordered_map<string, double> state;
  for (auto& query_name : valid_unknown) {
    double prand = (double)rand()/RAND_MAX;
    if (prand>0.5) {
      state[query_name] = 1.0;
    }
    else {
      state[query_name] = 0.0;
    }
  }
  for (auto& literal : valid_known) {
    state[literal] = this->prob[literal];
  }
  // initialize numTrue map
  unordered_map<string, int> numTrue;
  for (auto& query_name : valid_unknown) {
    numTrue[query_name] = 0;
  }
  // make the current state satisify hard clauses
  for (int id : c_idx) {
    if (this->cliques[id].isHard()) {
      // to be implemented
    }
  }
  // start sampling
  for (int i=0; i<round; i++) {
    unordered_set<int> m;
    // sample clauses and insert clique id into m
    for (int id : c_idx) {
      double pSat = this->cliques[id].satisifiablity(state);
      /*
      cout << this->cliques[id].toString() << endl;
      for (auto& s : this->cliques[id].getLiterals()) {
        cout << s << ' ' << state[s] << ' ';
      }
      cout << endl;
      cout << pSat << endl;
      */
      double p = 1-exp(-pSat*abs(this->cliques[id].getRuleWeight()));
      double prand = (double)rand()/RAND_MAX;
      if (prand<p) {
        m.insert(id);
      }
    }
    pSampleSAT(state, m, MAXFLIPS, MAXTRIES, PTARGET, NOISE);
    for (auto& query_name : valid_unknown) {
      if (abs(state[query_name]-1.0)<1e-10) {
        numTrue[query_name]++;
      }
    }
  }
  // calculate probabilities of queried tuples
  for (auto& query_name : valid_unknown) {
    this->prob[query_name] = numTrue[query_name]*1.0 / round;
  }
}


void MLN::pSampleSAT(unordered_map<string, double>& state, unordered_set<int>& c_idx, int maxFlips, int maxTries, double target, double p) {
  // initialize solution
  unordered_map<string, double> solu;
  string mode = "ss";
  for (string observe : this->obs) {
    solu[observe] = this->prob[observe];
  }
  for (int t=0; t<maxTries; t++) {
    // initialize solution randomly
    for (auto& query : this->queries) {
      double prand = (double)rand()/RAND_MAX;
      if (prand<0.5) {
        solu[query] = 1.0;
      }
      else {
        solu[query] = 0.0;
      }
    }
    double cost = pGetCost(solu, c_idx, mode);
    // flip literals
    for (int f=0; f<maxFlips; f++) {
      // terminate when cost < target
      if (cost<target) {
        for (auto& query : this->queries) {
          state[query] = solu[query];
        }
        return;
      }
      vector<int> false_idx;
      for (int c_id : c_idx) {
        if (this->cliques[c_id].satisifiablity(solu)<0.5) {
          false_idx.push_back(c_id);
        }
      }
      if (false_idx.size()==0) {
        for (auto& query : this->queries) {
          state[query] = solu[query];
        }
        return;
      }
      int c_id = false_idx[rand()%false_idx.size()];
      // cout << c_id << endl;
      double prand = (double)rand()/RAND_MAX;
      string toFlip;
      if (prand<p) {
        toFlip = randomPick(this->cliques[c_id]);
      }
      else {
        // toFlip = optimalPick(state, c_idx, mode);
        // toFlip = lowestPick(this->cliques[c_id], state, c_idx, mode);
        toFlip = pSsPick(state, c_idx, mode);
      }
      if (toFlip!="_STAY") {
        solu[toFlip] = 1-solu[toFlip];
      }
      cost = pGetCost(solu, c_idx, mode);
      // cout << cost << endl;
    }
  }
  for (auto& query : this->queries) {
    state[query] = solu[query];
  }
}


string MLN::pSsPick(unordered_map<string, double>& state, unordered_set<int>& c_idx, string& mode) {
  unordered_set<string> valid_set;
  for (int c_id : c_idx) {
    vector<string> literals = this->cliques[c_id].getLiterals();
    for (auto& literal : literals) {
      if (this->queries.find(literal)!=this->queries.end()) {
        valid_set.insert(literal);
      }
    }
  }
  vector<string> valid;
  for (auto& literal : valid_set) {
    valid.push_back(literal);
  }
  string res = valid[rand()%valid.size()];
  double prev = 0.0;
  for (int c_id : this->c_map[res]) {
    if (c_idx.find(c_id)!=c_idx.end()) {
      prev += (1-this->cliques[c_id].satisifiablity(state));
    }
  }
  state[res] = 1-state[res];
  double after = 0.0;
  for (int c_id : this->c_map[res]) {
    if (c_idx.find(c_id)!=c_idx.end()) {
      prev += (1-this->cliques[c_id].satisifiablity(state));
    }
  }
  state[res] = 1-state[res];
  double delta = after-prev;
  if (delta<0) {
    // if cost is less, directly return the random result
    return res;
  }
  double p = exp(-(delta)/SATEMPERATURE);
  double prand = (double)rand()/RAND_MAX;
  if (prand<p) {
    return res;
  }
  return "_STAY";
}


double MLN::pGetCost(unordered_map<string, double>& state, unordered_set<int>& c_idx, string& mode) {
  double cost = 0.0;
  for (int id : c_idx) {
    cost += 1-this->cliques[id].satisifiablity(state);
  }
  return cost;
}