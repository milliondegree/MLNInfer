#include "MLN.h"

struct Comparison {
  bool operator () (Clique& c1, Clique& c2) {
    return c1.getRuleWeight()<c2.getRuleWeight();
  }
};

typedef priority_queue<Clique, vector<Clique>, Comparison> clique_pq;

Pruner::Pruner() {
}

MLN Pruner::prune(MLN& mln, string query, double delta) {
  if (mln.prob.find(query)==mln.prob.end()) {
    mln.gibbsSampling_v4(100000, query);
  }
  double original = mln.queryProb(query);

  MLN nmln;
  map<string, double> prob = mln.prob;
  vector<Clique> cliques;
  unordered_set<string> obs;
  unordered_set<string> queries;
  map<string, vector<int>> c_map;
  nmln.setProb(prob);

  queue<int> q;
  vector<bool> visited (mln.cliques.size(), false);
  for (int i=0; i<mln.c_map[query].size(); i++) {
    q.push(mln.c_map[query][i]);
    visited[mln.c_map[query][i]] = true;
  }

  int layer = 1;
  while (!q.empty()) {
    queue<int> qq;
    while (!q.empty()) {
      int top = q.front();
      q.pop();
      // insert clique to new mln 
      cliques.push_back(mln.cliques[top]);
      for (string literal : mln.cliques[top].getLiterals()) {
        // if entry of literal does not exist, initialize it
        if (c_map.find(literal)==c_map.end()) {
          c_map[literal] = vector<int> ();
        }
        // insert the current clique into new mln
        c_map[literal].push_back(cliques.size()-1);
        if (mln.obs.find(literal)!=mln.obs.end()) {
          obs.insert(literal);
        }
        else {
          queries.insert(literal);
        }
      }
      // bfs search
      for (string literal : mln.cliques[top].getLiterals()) {
        if (mln.queries.find(literal)!=mln.queries.end()) {
          for (int c : mln.c_map[literal]) {
            if (!visited[c]) {
              if (mln.cliques[c].isSingular()) {
                // check singular cliques
                cliques.push_back(mln.cliques[c]);
                c_map[literal].push_back(cliques.size()-1);
              }
              else {
                qq.push(c);
              }
              visited[c] = true;
            }
          }
        }
      }
    }
    nmln.setCliques(cliques);
    nmln.setObs(obs);
    nmln.setQueries(queries);
    nmln.setCMap(c_map);
    nmln.gibbsSampling_v4(100000, query);
    double newProb = nmln.queryProb(query);
    std::cout << "layer: " << layer << ", delta: " << abs(newProb-original) << endl;
    // for (Clique cl : cliques) {
    //   cout << cl.toString() << endl;
    // }
    cout << endl;
    if (abs(newProb-original)<delta) {
      break;
    }
    // bfs part
    layer++;
    q = qq;
  }
  return nmln;
}


MLN Pruner::prune_v2(MLN& mln, string query, double delta) {
  if (mln.prob.find(query)==mln.prob.end()) {
    mln.gibbsSampling_v4(100000, query);
  }
  double original = mln.queryProb(query);

  MLN nmln;
  map<string, double> prob = mln.prob;
  vector<Clique> cliques;
  unordered_set<string> obs;
  unordered_set<string> queries;
  map<string, vector<int>> c_map;
  nmln.setProb(prob);

  clique_pq pq;
  

  return nmln;
}


Pruner::~Pruner() {
}