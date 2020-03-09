#include "MLN.h"
#include "Clique.h"


Grader::Grader() {
  this->target = "";
}


void Grader::computeGradients(MLN& mln, string query) {
  if (mln.pd.find(query)==mln.pd.end()) {
    mln.pd[query] = unordered_map<string, double> ();
  }
  else {
    cout << "influence of " << query << "has already been computed" << endl;
    return;
  }
  this->target = query;
  vector<bool> visited (mln.cliques.size(), false);
  this->dfsBuild(mln, visited, query, 1.0);
}



void Grader::dfsBuild(MLN& mln, vector<bool>& visited, string query, double grad) {
  vector<int> c_list = mln.c_map[query];
  for (int id : c_list) {
    if (!visited[id]) {
      visited[id] = true;
      Clique c = mln.cliques[id];
      vector<string> literals = c.getLiterals();
      unordered_map<string, double> truth;
      for (string s : literals) {
        truth[s] = mln.prob[s];
      }
      for (string s : literals) {
        if (mln.pd[this->target].find(s)==mln.pd[this->target].end()) {
          double pd_value = c.getPartialDerivative(truth, query, s);
          double n_grad = pd_value * grad
          mln.pd[this->target][s] = n_grad;
          dfsBuild(mln, visited, s, n_grad);
        } 
      }
    }
  }
}