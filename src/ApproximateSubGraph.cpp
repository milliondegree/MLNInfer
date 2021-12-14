#include "MLN.h"



class SubgraphDifference {

public: 
  set<int> clique_ids;

  double diff;

  SubgraphDifference(set<int> c_ids, double d) {
    this->clique_ids = c_ids;
    diff = d;
  }

  bool operator == (const SubgraphDifference& sd2) const {
    return this->clique_ids==sd2.clique_ids;
  }

};


class DiffComparison {

public: 
  DiffComparison() {};
  bool operator () (const SubgraphDifference& sd1, const SubgraphDifference& sd2) {
    return sd1.diff>sd2.diff; 
  }

};


class DiffHash {

public:
  size_t operator () (const SubgraphDifference& sd) const {
    hash<int> hasher;
    size_t seed = sd.clique_ids.size();
    for (auto& i : sd.clique_ids) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }

};


class SetHash {

public:
  size_t operator () (const set<int>& s) const {
    hash<int> hasher;
    size_t seed = s.size();
    for (auto& i : s) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }

};



MLN buildMLNFromCliqueIDs(MLN* mln, set<int>& clique_ids) {
  MLN ret = MLN();
  vector<Clique> cliques;
  unordered_set<string> obs;
  unordered_set<string> queries;
  map<string, vector<int>> c_map;
  map<string, double> prob;
  for (int ci : clique_ids) {
    Clique c = mln->cliques[ci];
    cliques.push_back(c);
    for (string literal : c.getLiterals()) {
      if (mln->obs.find(literal)!=mln->obs.end()) {
        obs.insert(literal);
        prob[literal] = mln->prob[literal];
      }
      else if (mln->queries.find(literal)!=mln->queries.end()) {
        queries.insert(literal);
        if (c_map.find(literal)==c_map.end()) {
          c_map[literal] = vector<int> (0);
        }
        c_map[literal].push_back(cliques.size()-1);
      }
    }
  }
  ret.setObs(obs);
  ret.setQueries(queries);
  ret.setCliques(cliques);
  ret.setCMap(c_map);
  ret.setProb(prob);
  return ret;
}


MLN MLN::approximateSubGraph(string query, double target, double delta) {
  // initialization
  clock_t prob_cost = 0;
  clock_t build_cost = 0;
  clock_t search_cost = 0;
  assert(this->queries.find(query)!=this->queries.end());
  MLN ret;
  priority_queue<SubgraphDifference, vector<SubgraphDifference>, DiffComparison> pq;
  unordered_set<SubgraphDifference, DiffHash> visited;

  for (int i : this->c_map[query]) {
    set<int> clique_ids;
    clique_ids.insert(i);
    MLN tmp = buildMLNFromCliqueIDs(this, clique_ids);
    ret = tmp;
    tmp.loopyBeliefPropagationMCS(query, 1000);
    double diff = fabs(target-tmp.prob[query]);
    SubgraphDifference sd = SubgraphDifference(clique_ids, diff);
    pq.push(sd);
    visited.insert(sd);
  }

  // heuristic search
  double min_diff = 1;
  while (!pq.empty() && visited.size()<MAX_VISIT) {
    SubgraphDifference sd = pq.top();
    cout << "queue size: " << pq.size() << " visited size: " << visited.size() << " min diff: " << sd.diff << endl;
    pq.pop();
    if (sd.diff<delta) {
      break;
    }
    else {
      clock_t tt = clock();
      set<int> clique_ids = sd.clique_ids;
      set<int> new_ids;
      for (int ci : clique_ids) {
        for (string literal : this->cliques[ci].getLiterals()) {
          if (this->queries.find(literal)!=this->queries.end()) {
            for (int cid : this->c_map[literal]) {
              if (clique_ids.find(cid)==clique_ids.end()) {
                new_ids.insert(cid);
              }
            }
          }
        }
      }
      search_cost += clock()-tt;
      for (int cid : new_ids) {
        clique_ids.insert(cid);
        clock_t t0 = clock();
        MLN tmp = buildMLNFromCliqueIDs(this, clique_ids);
        clock_t t1 = clock();
        build_cost += (t1-t0);
        tmp.loopyBeliefPropagationMCS(query, DEFAULT_ROUNDS);
        clock_t t2 = clock();
        prob_cost += (t2-t1);
        double diff = fabs(target-tmp.prob[query]);
        if (diff<min_diff) {
          min_diff = diff;
          ret = tmp;
        }
        SubgraphDifference sd = SubgraphDifference(clique_ids, diff);
        clique_ids.erase(cid);
        if (visited.find(sd)==visited.end()) {
          pq.push(sd);
          visited.insert(sd);
        }
      }
    }
  }

  cout << "build mln time: " << build_cost*1.0/CLOCKS_PER_SEC << endl;
  cout << "search time: " << search_cost*1.0/CLOCKS_PER_SEC << endl;
  cout << "probability inference time: " << prob_cost*1.0/CLOCKS_PER_SEC << endl;
  return ret;
  
}




MLN MLN::approximateSubGraph_v2(string query, double target, double delta, int max_clique_number) {
  /*
  simulated annealing
  */ 

  // initialization
  assert(this->queries.find(query)!=this->queries.end());

  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0,1.0);

  MLN ret;
  // unordered_set<SubgraphDifference, DiffHash> visited;
  unordered_map<set<int>, double, SetHash> visited;

  set<int> current;
  double prev_diff = 1;
  double min_diff = 1;
  for (int r=1; r<=max_clique_number; r++) {
    // build initial state
    if (current.size()==0) {
      double prand = distribution(generator);
      for (int i=0; i<this->c_map[query].size(); i++) {
        if (i*this->c_map[query].size()<=prand && prand<(i+1)*this->c_map[query].size()) {
          current.insert(this->c_map[query][i]);
        }
      }
    }

    // start searching
    // do {

    // }
  }


  return ret;
  
}