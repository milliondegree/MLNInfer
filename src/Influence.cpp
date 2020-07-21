#include "Influence.h"

Influence::Influence() {
}


Influence::Influence(MLN& mln) {
  this->queries = vector<string> ();
  this->l_index = unordered_map<string, int> ();
  for (auto query : mln.getQueryLiterals()) {
    this->queries.push_back(query);
    this->l_index[query] = this->queries.size()-1;
  }
  int n = this->queries.size();
  this->partialDerivs = vector<vector<double>> (n, vector<double> (n+1, 0.0));
}


double Influence::influenceQuery(MLN& mln, string& query, string& infl) {
  unordered_set<string> queries = mln.getQueryLiterals();
  unordered_set<string> obs = mln.getObsLiterals();
  assert(queries.find(query)!=queries.end()&&obs.find(infl)!=obs.end());
  computePartialDerivatives(mln, infl);
  solveEquations();
  int q_i = this->l_index[query];
  return this->influences[q_i];
}


void Influence::computePartialDerivatives(MLN& mln, string& infl) {
  map<string, double> probs = mln.getProb();
  vector<Clique> cliques = mln.getCliques();
  map<string, vector<int>> c_map = mln.getCMap();
  for (string numerator : this->queries) {
    // for each queried tuple, use its probability formula
    int n_i = this->l_index[numerator];
    double base = -probs[numerator]*(1-probs[numerator]);
    for (string denominator : this->queries) {
      // for each queried tuple, considered its partial derivative of tuple numerator
      int d_i = this->l_index[denominator];
      if (denominator==numerator) {
        this->partialDerivs[n_i][d_i] = 1.0;
      }
      else {
        double accu = getAccuPotential(numerator, denominator, probs, cliques, c_map);
        this->partialDerivs[n_i][d_i] = base*accu;
      }
    }
    // compute the infl's partial derivative of numerator
    int d_i = this->queries.size();
    double accu = getAccuPotential(numerator, infl, probs, cliques, c_map);
    this->partialDerivs[n_i][d_i] = base*accu;
  }
}


double Influence::getPartialDeriv(string& query, string& infl) {
  assert(this->l_index.find(query)!=this->l_index.end()&this->l_index.find(infl)!=this->l_index.end());
  int q_i = this->l_index[query];
  int i_i = this->l_index[infl];
  return this->partialDerivs[q_i][i_i];
}


vector<string> Influence::getQueries() {
  return this->queries;
}


vector<vector<double>> Influence::getPartialDeriv() {
  return this->partialDerivs;
}


double Influence::getAccuPotential(string& numerator, string& denominator, map<string, double>& probs, vector<Clique>& cliques, map<string, vector<int>> c_map) {
  double accu = 0.0;
  for (int c_i : c_map[numerator]) {
    Clique c = cliques[c_i];
    if (!c.isSingular()) {
      if (c.isRuleHead(numerator)&&c.isRuleBody(denominator)) {
        double potential = 1.0;
        potential *= c.getRuleWeight();
        for (string literal : c.getRuleBody()) {
          if (literal!=denominator) {
            potential *= probs[literal];
          }
        }
        accu += potential;
      }
      else if (c.isRuleHead(denominator)&&c.isRuleBody(numerator)) {
        double potential = 1.0;
        potential *= c.getRuleWeight();
        for (string literal : c.getRuleBody()) {
          if (literal!=numerator) {
            potential *= probs[literal];
          }
        }
        accu += potential;
      }
      else if (c.isRuleBody(numerator)&&c.isRuleBody(denominator)) {
        double potential = 1.0;
        potential *= -c.getRuleWeight();
        potential *= (1-probs[c.getRuleHead()]);
        for (string literal : c.getRuleBody()) {
          if (literal!=numerator&&literal!=denominator) {
            potential *= probs[literal];
          }
        }
        accu += potential;
      }
      else if (c.isRuleHead(numerator)&&c.isRuleName(denominator)) {
        double potential = 1.0;
        for (string literal : c.getRuleBody()) {
          potential *= probs[literal];
        }
        accu += potential;
      }
      else if (c.isRuleBody(numerator)&&c.isRuleName(denominator)) {
        double potential = -1.0;
        potential *= (1-probs[c.getRuleHead()]);
        for (string literal : c.getRuleBody()) {
          if (numerator!=literal) {
            potential *= probs[literal];
          }
        }
        accu += potential;
      }
    }
  }
  return accu;
}


void Influence::solveEquations() {
  int n = this->partialDerivs.size();
  int m = this->partialDerivs[0].size();
  vector<vector<double>> equations (this->partialDerivs);
  vector<double> solutions (n);
  for (int i=0; i<n; i++) {
    double base = equations[i][i];
    assert(abs(base)>1e-10);
    for (int j=i; j<m; j++) {
      equations[i][j] /= base;
    }
    for (int k=i+1; k<n; k++) {
      for (int j=i+1; j<m; j++) {
        equations[k][j] -= equations[k][i]*equations[i][j];
      }
      equations[k][i] = 0.0;
    }
  }
  for (int i=n-1; i>=0; i--) {
    double value = 0.0;
    for (int k=i+1; k<n; k++) {
      value += equations[i][k]*solutions[k];
    }
    value += equations[i][m-1];
    solutions[i] = -value;
  }
  this->influences = solutions;
  // for (int i=0; i<n; i++) {
  //   cout << solutions[i] << endl;
  // }
  // for (int i=0; i<n; i++) {
  //   for (int j=0; j<m; j++) {
  //     cout << equations[i][j] << ' ';
  //   }
  //   cout << endl;
  // }
}


Influence::~Influence() {
}