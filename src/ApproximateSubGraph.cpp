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



MLN buildMLNFromCliqueIDs(MLN* mln, set<int>& clique_ids) {
  MLN ret = MLN();
  vector<Clique> cliques;
  unordered_set<string> obs;
  unordered_set<string> queries;
  map<string, vector<int>> c_map;
  for (int ci : clique_ids) {
    Clique c = mln->cliques[ci];
    cliques.push_back(c);
    for (string literal : c.getLiterals()) {
      if (mln->obs.find(literal)!=mln->obs.end()) {
        obs.insert(literal);
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
  ret.setProb(mln->prob);
  return ret;
}


MLN MLN::approximateSubGraph(string query, double target, double delta) {
  // initialization
  clock_t cost_t = 0;
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
  while (!pq.empty()) {
    SubgraphDifference sd = pq.top();
    cout << "queue size: " << pq.size() << " visited size: " << visited.size() << " min diff: " << sd.diff << endl;
    pq.pop();
    if (sd.diff<delta) {
      break;
    }
    else {
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
      for (int cid : new_ids) {
        clique_ids.insert(cid);
        MLN tmp = buildMLNFromCliqueIDs(this, clique_ids);
        clock_t t1 = clock();
        tmp.loopyBeliefPropagationMCS(query, DEFAULT_ROUNDS);
        clock_t t2 = clock();
        cost_t += (t2-t1);
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

  cout << "probability inference time: " << cost_t*1.0/CLOCKS_PER_SEC << endl;
  return ret;
  
}