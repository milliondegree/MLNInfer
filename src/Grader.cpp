#include "MLN.h"
#include "Clique.h"


Grader::Grader() {
  this->target = "";
}


// void Grader::computeGradients(MLN& mln, string query, int round) {
//   if (mln.pd.find(query)==mln.pd.end()) {
//     mln.pd[query] = unordered_map<string, double> ();
//   }
//   else {
//     cout << "influence of " << query << "has already been computed" << endl;
//     return;
//   }
//   mln.gibbsSampling_v3(round);
//   this->target = query;
//   vector<bool> visited (mln.cliques.size(), false);
//   this->dfsBuild(mln, visited, query, 1.0);
// }



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


void Grader::computeGradients_mcsat(MLN& mln, string query, int round, double delta) {
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
    mln.mcsat(round, query);
    double upper_prob = mln.queryProb(query);
    mln.setObsProb(observe, lower);
    mln.mcsat(round, query);
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
  // unordered_set<string> valid_obs;
  // vector<bool> visited (mln.cliques.size(), false);
  // dfsSearch(mln, valid_obs, visited, query);
  if (mln.obs.find(infl)==mln.obs.end()) {
    cout << infl << endl;
    mln.pd[query][infl] = 0.0;
    return;
    // consider rule tuple
    // if (infl.start)
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
    mln.gibbsSampling_v4(round, query);
  }
  else if (mode=="pgibbs") {
    mln.gibbsSampling_vp(round, query, 1e-7);
  }
  double upper_prob = mln.queryProb(query);
  mln.setObsProb(infl, lower);
  if (mode=="mcsat") {
    mln.mcsat(round, query);
  }
  else if (mode=="gibbs") {
    mln.gibbsSampling_v4(round, query);
  }
  else if (mode=="pgibbs") {
    mln.gibbsSampling_vp(round, query, 1e-7);
  }
  double lower_prob = mln.queryProb(query);
  mln.setObsProb(infl, prev);
  mln.pd[query][infl] = (upper_prob-lower_prob) / (upper-lower);
}



unordered_set<string> Grader::getValidObservedTuples(MLN& mln, string query) {
  unordered_set<string> valid_obs;
  vector<bool> visited (mln.cliques.size(), false);
  dfsSearch(mln, valid_obs, visited, query);
  return valid_obs;
}



// void Grader::dfsBuild(MLN& mln, vector<bool>& visited, string& query, double grad) {
//   vector<int> c_list = mln.c_map[query];
//   for (int id : c_list) {
//     if (!visited[id]) {
//       visited[id] = true;
//       Clique c = mln.cliques[id];
//       vector<string> literals = c.getLiterals();
//       for (string s : literals) {
//         if (s!=query && mln.pd[this->target].find(s)==mln.pd[this->target].end()) {
//           double pd_value = c.getPartialDerivative(mln.prob, query, s);
//           double n_grad = pd_value * grad;
//           mln.pd[this->target][s] = n_grad;
//         } 
//       }
//       for (string s : literals) {
//         if (s!=query) {
//           dfsBuild(mln, visited, s, mln.pd[this->target][s]);
//         }
//       }
//     }
//   }
// }


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