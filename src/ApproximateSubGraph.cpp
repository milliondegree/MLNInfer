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
  map<string, vector<int> > c_map;
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
    string rule_name = c.getRuleName();
    // if (!Parser::isRuleName(rule_name)) continue;
    // cout << rule_name << endl;
    assert(mln->obs.find(rule_name)!=mln->obs.end());
    obs.insert(rule_name);
    prob[rule_name] = mln->prob[rule_name];
  }
  ret.setObs(obs);
  ret.setQueries(queries);
  ret.setCliques(cliques);
  ret.setCMap(c_map);
  ret.setProb(prob);
  return ret;
}


MLN MLN::approximateSubGraph(string query, double target, int rounds, double delta, int max_clique_number) {
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
    clock_t t1 = clock();
    tmp.loopyBeliefPropagationMCS(query, rounds);
    prob_cost += clock()-t1;
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
      ret = buildMLNFromCliqueIDs(this, sd.clique_ids);
      break;
    }
    else {
      if (sd.clique_ids.size()==max_clique_number) continue;
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
        tmp.loopyBeliefPropagationMCS(query, rounds);
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

  // cout << "build mln time: " << build_cost*1.0/CLOCKS_PER_SEC << endl;
  // cout << "search time: " << search_cost*1.0/CLOCKS_PER_SEC << endl;
  // cout << "probability inference time: " << prob_cost*1.0/CLOCKS_PER_SEC << endl;
  return ret;
  
}





int draw(set<int>& pool, double prand) {
  assert(prand>=0&&prand<=1);
  vector<int> tmp;
  for (int i : pool) tmp.push_back(i);
  for (int i=0; i<tmp.size(); i++) {
    if (prand>=i*(1.0/tmp.size()) && prand<(i+1)*(1.0/tmp.size())) {
      return *(tmp.begin()+i);
    }
  }
  return 0;
}

MLN MLN::approximateSubGraph_v2(string query, double target, int rounds, double delta, int max_clique_number) {
  /*
  simulated annealing
  */ 

  // initialization
  assert(this->queries.find(query)!=this->queries.end());
  max_clique_number = this->cliques.size()<max_clique_number ? this->cliques.size() : max_clique_number;

  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0,1.0);

  MLN ret;
  unordered_map<set<int>, double, SetHash> visited;

  // init state
  set<int> prev;
  set<int> current;
  double prev_diff = 1;
  double current_diff = 1;

  set<int> global_best = current;
  double global_best_diff = current_diff;

  // heuristic initialization
  cout << "start heuristic initialization" << endl;
  for (int i=0; i<max_clique_number; i++) {
    set<string> query_list;
    set<int> pool;
    query_list.insert(query);
    for (int ci : current) {
      for (string literal : this->cliques[ci].getLiterals()) {
        if (this->queries.find(literal)!=this->queries.end()) {
          query_list.insert(literal);
        }
      }
    }
    for (string to_query : query_list) {
      for (int ci : this->c_map[to_query]) {
        if (current.find(ci)==current.end()) {
          pool.insert(ci);
        }
      }
    }
    set<int> min_mln;
    double min_diff = 1;
    if (pool.size()==1) {
      for (int ci : pool) {
        cout << this->cliques[ci].toString() << endl;
      }
    }
    for (int ci : pool) {
      current.insert(ci);
      MLN tmp = buildMLNFromCliqueIDs(this, current);
      tmp.loopyBeliefPropagationMCS(query, rounds);
      double diff = fabs(tmp.queryProb(query)-target);
      if (diff<min_diff) {
        min_mln = current;
        min_diff = diff;
      }
      current.erase(ci);
    }
    current = min_mln;
    current_diff = min_diff;
    if (current_diff<global_best_diff) {
      global_best = current;
      global_best_diff = current_diff;
    }
    cout << "clique number: " << current.size() << ", min diff: " << current_diff << endl;
    if (global_best_diff<delta) break;
  }
  
  // start search
  int round = 0;
  int stay = 0;
  double temperature = SATEMPERATURE;

  while (stay<10 && global_best_diff>delta && round<MAX_VISIT) {
    // remove a clique
    set<int> pool;
    set<int> tmp = current;
    for (int ci : tmp) {
      current.erase(ci);
      MLN current_mln = buildMLNFromCliqueIDs(this, current);
      bool conn = current_mln.connectivity(query);
      if (conn) pool.insert(ci);
      current.insert(ci);
    }
    current.erase(draw(pool, distribution(generator)));
    
    // add a clique
    pool.clear();
    set<string> query_list;
    query_list.insert(query);
    for (int ci : current) {
      for (string literal : this->cliques[ci].getLiterals()) {
        if (this->queries.find(literal)!=this->queries.end()) {
          query_list.insert(literal);
        }
      }
    }
    for (string to_query : query_list) {
      for (int ci : this->c_map[to_query]) {
        if (current.find(ci)==current.end()) {
          current.insert(ci);
          if (current!=prev) {
            pool.insert(ci);
          }
          current.erase(ci);
        }
      }
    }
    current.insert(draw(pool, distribution(generator)));

    if (visited.find(current)!=visited.end()) {
      current_diff = visited[current];
    }
    else {
      MLN tmp = buildMLNFromCliqueIDs(this, current);
      tmp.loopyBeliefPropagationMCS(query, rounds);
      current_diff = fabs(target-tmp.queryProb(query));
      visited[current] = current_diff;
    }

    // cout << "temperature: " << temperature << ", delta:" << current_diff-prev_diff << ", prob to accept: " << exp(-(current_diff-prev_diff)/temperature) << endl;
    if (current_diff<prev_diff) {
      prev = current;
      prev_diff = current_diff;
      stay = 0;
    }
    else {
      if (distribution(generator)<exp(-(current_diff-prev_diff)/temperature)) {
        prev = current;
        prev_diff = current_diff;
        stay = 0;
      }
      else {
        current = prev;
        current_diff = prev_diff;
        stay++;
      }
    }

    if (global_best_diff>current_diff) {
      global_best = current;
      global_best_diff = current_diff;
    }

    round++;
    if (round%1==0) {
      temperature *= DROP_RATE;
    }
    
    // cout << "round: " << round << endl;
    // cout << "current diff: " << current_diff << endl;
    // cout << endl;

  }
  
  ret = buildMLNFromCliqueIDs(this, global_best);
  return ret;
  
}







bool checkExist(vector<set<int> >& s, set<int> t) {
  for (auto it : s) {
    if (it==t) {
      return true;
    }
  }
  return false;
}


MLN MLN::approximateSubGraph_v3(string query, double target, int rounds, double delta, int max_clique_number) {
  /*
  Local search
  */ 

  // initialization
  assert(this->queries.find(query)!=this->queries.end());
  max_clique_number = this->cliques.size()<max_clique_number ? this->cliques.size() : max_clique_number;

  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0,1.0);

  MLN ret;
  unordered_map<set<int>, double, SetHash> visited;
  unordered_set<set<int>, SetHash> tabu_list;

  // init state
  set<int> current;
  set<int> global_best;
  double global_best_diff = 1;

  for (int i=0; i<max_clique_number; i++) {
    set<string> query_list;
    set<int> pool;
    query_list.insert(query);
    for (int ci : current) {
      for (string literal : this->cliques[ci].getLiterals()) {
        if (this->queries.find(literal)!=this->queries.end()) {
          query_list.insert(literal);
        }
      }
    }
    for (string to_query : query_list) {
      for (int ci : this->c_map[to_query]) {
        if (current.find(ci)==current.end()) {
          pool.insert(ci);
        }
      }
    }
    current.insert(draw(pool, distribution(generator)));
  }
  
  // start search
  int r = 0;
  while (global_best_diff>delta && r<1000) {
    vector<set<int> > neighbors;
    set<int> tmp_set = current;
    for (int ci : current) {
      tmp_set.erase(ci);
      MLN tmp = buildMLNFromCliqueIDs(this, tmp_set);
      if (tmp.connectivity(query)) {
        set<string> query_list;
        for (int cii : tmp_set) {
          for (string literal : this->cliques[cii].getLiterals()) {
            if (this->queries.find(literal)!=this->queries.end()) {
              query_list.insert(literal);
            }
          }
          for (string to_query : query_list) {
            for (int ciii : this->c_map[to_query]) {
              if (tmp_set.find(ciii)==tmp_set.end() && ciii!=ci) {
                tmp_set.insert(ciii);
                if (!checkExist(neighbors, tmp_set) && tabu_list.find(tmp_set)==tabu_list.end()) {
                  neighbors.push_back(tmp_set);
                }
                tmp_set.erase(ciii);
              }
            }
          }
        }
      }
      tmp_set.insert(ci);
    }

    set<int> sub_best=current;
    double sub_best_diff=1;
    for (auto n : neighbors) {
      double diff;
      if (visited.find(n)!=visited.end()) {
        diff = visited[n];
      }
      else {
        MLN tmp = buildMLNFromCliqueIDs(this, n);
        tmp.loopyBeliefPropagationMCS(query, rounds);
        double p = tmp.queryProb(query);
        diff = fabs(p-target);
        visited[n] = diff;
      }
      if (diff<sub_best_diff) {
        sub_best_diff = diff;
        sub_best = n;
      }
    }

    cout << "round: " << r++ << endl;
    cout << "neighbor size: " << neighbors.size() << endl;
    cout << "best diff of this round: " << sub_best_diff << endl;

    tabu_list.insert(sub_best);
    current = sub_best;

    if (sub_best_diff<global_best_diff) {
      global_best = sub_best;
      global_best_diff = sub_best_diff;
    }

  }
  
  ret = buildMLNFromCliqueIDs(this, global_best);
  return ret;
  
}