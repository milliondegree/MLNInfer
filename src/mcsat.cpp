#include "MLN.h"


#define MAXTRIES 100
#define MAXFLIPS 10
#define TARGET 0.01
#define NOISE 0.5


void MLN::mcsat(int round) {
  // random generator
  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0, 1.0);
  // initialize numTrue map
  unordered_map<string, int> numTrue;
  for (auto& query : this->queries) {
    numTrue[query] = 0;
  }
  // initialize state map
  unordered_map<string, int> state;
  for (string pred : obs) {
    state[pred] = 1;
  }
  for (auto& query : this->queries) {
    double rand = distribution(generator);
    if (rand>0.5) {
      state[query] = 1;
    }
    else {
      state[query] = 0;
    }
  }
  // find useful cliques
  unordered_set<int> c_idx;
  for (auto& query : this->queries) {
    for (int idx : this->c_map[query]) {
      c_idx.insert(idx);
    }
  }
  cout << "number of related cliques: " << c_idx.size() << endl;
  // for (int id : c_idx) {
  //   this->cliques[id].printClique();
  // }
  // cout << endl;
  // make the current state satisify hard clauses
  for (int id : c_idx) {
    if (this->cliques[id].isHard()) {
      // to be implemented
    }
  }
  // start sampling
  for (int i=0; i<round; i++) {
    vector<int> m;
    // sample clauses and insert clique id into m
    for (int id : c_idx) {
      if (this->cliques[id].satisifiablity(state) && this->cliques[id].getRuleWeight()>0) {
        double prand = distribution(generator);
        double p = 1-exp(-this->cliques[id].getRuleWeight());
        if (prand<p) {
          m.push_back(id);
        }
      }
      else if (!this->cliques[id].satisifiablity(state) && this->cliques[id].getRuleWeight()<0) {
        double prand = distribution(generator);
        double p = 1-exp(this->cliques[id].getRuleWeight());
        if (prand<p) {
          m.push_back(id);
        }
      }
    }
    // cout << "size of subset: " << m.size() << endl;
    // for (int id : m) {
    //   this->cliques[id].printClique();
    // }
    // sampleSAT(state, m, MAXTRIES);
    maxWalkSAT(state, m, MAXFLIPS, MAXTRIES, TARGET, NOISE);
    for (auto& query : this->queries) {
      if (state[query]==1) {
        numTrue[query]++;
      }
    }
  }
  // calculate probabilities of queried tuples
  for (auto& query : this->queries) {
    this->prob[query] = numTrue[query]*1.0 / round;
  }
}


void MLN::sampleSAT(unordered_map<string, int>& state, vector<int>& c_idx, int maxTries) {

}


void MLN::maxWalkSAT(unordered_map<string, int>& state, vector<int>& c_idx, int maxFlips, int maxTries, double target, double p) {
  // initialize random tools
  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0, 1.0);
  // initialize solution
  unordered_map<string, int> solu;
  for (string observe : this->obs) {
    solu[observe] = 1;
  }
  for (int t=0; t<maxTries; t++) {
    // initialize solution randomly
    // cout << "sample round " << t << endl;
    for (auto& query : this->queries) {
      double prand = distribution(generator);
      if (prand<0.5) {
        solu[query] = 1;
      }
      else {
        solu[query] = 0;
      }
    }
    double cost = getCost(solu, c_idx);
    // flip literals
    for (int f=0; f<maxFlips; f++) {
      // cout << "flip round " << f << endl;
      if (cost<target) {
        for (auto& query : this->queries) {
          state[query] = solu[query];
        }
        return;
      }
      int c_id = c_idx[rand()%c_idx.size()];
      double prand = distribution(generator);
      string toFlip;
      if (prand<p) {
        toFlip = randomPick(this->cliques[c_id]);
      }
      else {
        toFlip = lowestPick(this->cliques[c_id], state);
      }
      solu[toFlip] = 1-solu[toFlip];
      cost = getCost(solu, c_idx);
    }
  }
  for (auto& query : this->queries) {
    state[query] = solu[query];
  }
}


double MLN::getCost(unordered_map<string, int>& state, vector<int>& c_idx) {
  double cost = 0.0;
  for (int id : c_idx) {
    if (!this->cliques[id].satisifiablity(state)) {
      cost += this->cliques[id].getRuleWeight();
    }
  }
  return cost;
}


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


string MLN::lowestPick(Clique& c, unordered_map<string, int>& state) {
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
    int prev = state[literal];
    double prevCost = 0.0;
    for (int c_id : this->c_map[literal]) {
      if (!this->cliques[c_id].satisifiablity(state)) {
        prevCost += this->cliques[c_id].getRuleWeight();
      }
    }
    state[literal] = 1-state[literal];
    double afterCost = 0.0;
    for (int c_id : this->c_map[literal]) {
      if (!this->cliques[c_id].satisifiablity(state)) {
        afterCost += this->cliques[c_id].getRuleWeight();
      }
    }
    state[literal] = prev;
    if (afterCost-prevCost < costChange) {
      costChange = afterCost-prevCost;
      res = literal;
    }
  }
  return res;
}