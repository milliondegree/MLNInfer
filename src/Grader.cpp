#include "MLN.h"
#include "Clique.h"


Grader::Grader() {
  this->target = "";
}



void Grader::computeGradients_v2(MLN& mln, string query, int round, double delta) {
  if (mln.queries.find(query)==mln.queries.end()) {
    cout << "cannot compute influence to a observed tuple" << endl;
    return;
  }
  if (mln.pd.find(query)==mln.pd.end()) {
    mln.pd[query] = unordered_map<string, double> ();
  }
  unordered_set<string> valid_obs;
  vector<bool> visited (mln.cliques.size(), false);
  dfsSearch(mln, valid_obs, visited, query);
  cout << "tuples which have direct influence: ";
  for (string s : valid_obs) {
    cout << s << ' ';
  }
  cout << endl;
  for (string observe : valid_obs) {
    double prev = mln.prob[observe];
    double upper = min(1.0, mln.prob[observe]+delta);
    double lower = max(0.0, mln.prob[observe]-delta);
    mln.setObsProb(observe, upper);
    mln.gibbsSampling_v4(round, query);
    double upper_prob = mln.queryProb(query);
    mln.setObsProb(observe, lower);
    mln.gibbsSampling_v4(round, query);
    double lower_prob = mln.queryProb(query);
    mln.setObsProb(observe, prev);
    mln.pd[query][observe] = (upper_prob-lower_prob) / (upper-lower);
  }
}




void Grader::computeGradient(MLN& mln, string query, string infl, int round, double delta, string mode) {
  assert(mln.queries.find(query)!=mln.queries.end() && mln.obs.find(infl)!=mln.obs.end());
  if (mln.pd.find(query)==mln.pd.end()) {
    mln.pd[query] = unordered_map<string, double> ();
  }
  clock_t t1 = clock();
  if (mln.obs.find(infl)==mln.obs.end()) {
    cout << infl << endl;
    mln.pd[query][infl] = 0.0;
    return;
  }
  double prev = mln.prob[infl];
  double upper, lower;
  if (prev>=0&&prev<=1) {
    upper = min(1.0, mln.prob[infl]+delta);
    lower = max(0.0, mln.prob[infl]-delta);
  }
  else {
    upper = prev+delta;
    lower = prev-delta;
  }
  mln.setObsProb(infl, upper);
  if (mode=="mcsat") {
    mln.mcsat(round, query);
  }
  else if (mode=="gibbs") {
    mln.gibbsSampling(round);
  }
  else if (mode=="pgibbs") {
    mln.gibbsSampling_vp(round, query, 1e-7);
  }
  else if (mode=="lbp") {
    mln.loopyBeliefPropagation(query);
  }
  else if (mode=="mclbp") {
    mln.loopyBeliefPropagationMCS(query, round);
  }
  double upper_prob = mln.queryProb(query);
  mln.setObsProb(infl, lower);
  if (mode=="mcsat") {
    mln.mcsat(round, query);
  }
  else if (mode=="gibbs") {
    mln.gibbsSampling(round);
  }
  else if (mode=="pgibbs") {
    mln.gibbsSampling_vp(round, query, 1e-7);
  }
  else if (mode=="lbp") {
    mln.loopyBeliefPropagation(query);
  }
  else if (mode=="mclbp") {
    mln.loopyBeliefPropagationMCS(query, round);
  }
  double lower_prob = mln.queryProb(query);
  mln.setObsProb(infl, prev);
  mln.pd[query][infl] = (upper_prob-lower_prob) / (upper-lower);
  clock_t t2 = clock();
}



unordered_set<string> Grader::getValidObservedTuples(MLN& mln, string query) {
  unordered_set<string> valid_obs;
  vector<bool> visited (mln.cliques.size(), false);
  dfsSearch(mln, valid_obs, visited, query);
  return valid_obs;
}



void Grader::dfsSearch(MLN& mln, unordered_set<string>& valid_obs, vector<bool>& visited, string& query) {
  if (mln.obs.find(query)!=mln.obs.end()) {
    valid_obs.insert(query);
    return;
  }
  for (int ind : mln.c_map[query]) {
    if (!visited[ind]) {
      visited[ind] = true;
      for (string& s : mln.cliques[ind].getLiterals()) {
        if (s!=query) {
          dfsSearch(mln, valid_obs, visited, s);
        }
      }
    }
  }
}


Grader::~Grader() {
}