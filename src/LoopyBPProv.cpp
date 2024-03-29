#include "MLN.h"
// #include "CProvGraph.h"

/* rules to define varaible names:
potential: potential_cliqueNumber_truthValue
nodeMsg: nodeMsg_literal_cliqueNumber_truthValue_iteration_iterationNumber
cliqueMsg: cliqueMsg_cliqueNumber_literal_truthValue_iteration_iterationNumber
*/


void enumerateTruthWithProv(Clique& c, 
                    vector<string>& toSearch,
                    map<string, int>& truth, 
                    vector<double>& res,
                    int pos) {
  if (pos==toSearch.size()) {
    res.push_back(exp(c.getPotential(truth)));
  }
  else {
    for (int i=0; i<=1; i++) {
      truth[toSearch[pos]] = i;
      enumerateTruthWithProv(c, toSearch, truth, res, pos+1);
    }
  }
}


vector<double> loopyBPRunWithProv(MLN* mln, string query) {
  // total initialization
  map<string, map<int, vector<double> > > nodeMsgs;
  map<int, map<string, vector<double> > > cliqueMsgs;
  map<int, vector<double> > potentials;
  map<string, vector<double> > dists;
  double nodeMsg_init_value = 1;
  double cliqueMsg_init_value = 0;
  for (string literal : mln->queries) {
    for (int c : mln->c_map[literal]) {
      nodeMsgs[literal][c] = vector<double> (2, nodeMsg_init_value);
      cliqueMsgs[c][literal] = vector<double> (2, cliqueMsg_init_value);
      /* build EDB vertices of initial nodeMsgs */
      mln->provG.addVariableVertex(Parameter, "nodeMsg_"+literal+"_"+to_string(c)+"_1_iteration_0", nodeMsg_init_value);
      mln->provG.addVariableVertex(Parameter, "nodeMsg_"+literal+"_"+to_string(c)+"_0_iteration_0", nodeMsg_init_value);
    }
    dists[literal] = vector<double> (2, 0.5);
  }
  for (auto it1 : cliqueMsgs) {
    Clique c = mln->cliques[it1.first];
    vector<string> toSearch;
    for (auto it2 : it1.second) {
      toSearch.push_back(it2.first);
    }
    map<string, int> truth;
    for (string literal : c.getLiterals()) {
      if (mln->obs.find(literal)!=mln->obs.end()) {
        truth[literal] = mln->prob[literal];
      }
    }
    vector<double> potential;
    enumerateTruthWithProv(c, toSearch, truth, potential, 0);
    potentials[it1.first] = potential;
  }

  /* build EDB vertices of potentials */
  for (auto it : potentials) {
    for (int i=0; i<it.second.size(); i++) {
      string vertex_name = "potential_"+to_string(it.first)+"_"+to_string(i);
      mln->provG.addVariableVertex(Input, vertex_name, it.second[i]);
    }
  }

  // the loopy bp begins
  bool converge = false;
  int iteration = 1;
  while (!converge&&iteration<4) {
    
    // initialization newcliqueMsgs
    map<int, map<string, vector<double> > > newcliqueMsgs;
    for (auto it1 : nodeMsgs) {
      for (auto it2 : it1.second) {
        newcliqueMsgs[it2.first][it1.first] = vector<double> (2, 0);
      }
    }

    // pass node messages to cliques:
    for (auto it1 : cliqueMsgs) {
      // for each clique
      int c = it1.first;
      vector<string> toQuery;
      for (auto it : cliqueMsgs[c]) {
        toQuery.push_back(it.first);
      }
      for (int i=0; i<potentials[c].size(); i++) {
        int tmp = i;

        double value = potentials[c][i];
        /* add computing of variable value */
        string value_name = "value_"+to_string(c)+"_"+to_string(i)+"_iteration_"+to_string(iteration);
        vector<string> value_input_names {"potential_"+to_string(c)+"_"+to_string(i)};
        mln->provG.addComputingSubgraph(value_name, value, Mul, value_input_names);
        
        for (int s=toQuery.size()-1; s>=0; s--) {
          // for each unknown vertex
          int truth_value = tmp%2;
          value *= nodeMsgs[toQuery[s]][c][truth_value];
          tmp /= 2;
          /* update computing of value */
          vector<string> value_input_names {"nodeMsg_"+toQuery[s]+"_"+to_string(c)+"_"+to_string(truth_value)+"_iteration_"+to_string(iteration-1)};
          mln->provG.addComputingSubgraph(value_name, value, Mul, value_input_names);
        }

        tmp = i;
        for (int s=toQuery.size()-1; s>=0; s--) {
          int truth_value = tmp%2;

          double valueByNodeMsg = value/nodeMsgs[toQuery[s]][c][truth_value];
          /* add computing of valueByNodeMsg */
          string valueByNodeMsg_name = "valueByNodeMsg_"+to_string(c)+"_"+to_string(i)+"_"+toQuery[s]+"_"+to_string(truth_value)+"_iteration_"+to_string(iteration);
          vector<string> valueByNodeMsg_input_names {"value_"+to_string(c)+"_"+to_string(i)+"_iteration_"+to_string(iteration),
                                                    "nodeMsg_"+toQuery[s]+"_"+to_string(c)+"_"+to_string(truth_value)+"_iteration_"+to_string(iteration-1)};
          unordered_map<string, string> params = {{"numerator_name", "value_"+to_string(c)+"_"+to_string(i)+"_iteration_"+to_string(iteration)}};
          mln->provG.addComputingSubgraph(valueByNodeMsg_name, valueByNodeMsg, Div, valueByNodeMsg_input_names, params);

          newcliqueMsgs[c][toQuery[s]][truth_value] += valueByNodeMsg;
          /* update computing of cliqueMsg */
          string cliqueMsg_name = "cliqueMsg_"+to_string(c)+"_"+toQuery[s]+"_"+to_string(truth_value)+"_iteration_"+to_string(iteration);
          vector<string> cliqueMsg_input_names {"valueByNodeMsg_"+to_string(c)+"_"+to_string(i)+"_"+toQuery[s]+"_"+to_string(truth_value)+"_iteration_"+to_string(iteration)};
          mln->provG.addComputingSubgraph(cliqueMsg_name, newcliqueMsgs[c][toQuery[s]][truth_value], Sum, cliqueMsg_input_names);

          tmp /= 2;
        }
      }
    }
    cliqueMsgs = newcliqueMsgs;

    // initialize dists
    map<string, vector<double> > newDists;
    for (string literal : mln->queries) {
      newDists[literal] = vector<double> (2, 1);
      for (int c : mln->c_map[literal]) {
        newDists[literal][0] *= cliqueMsgs[c][literal][0];
        newDists[literal][1] *= cliqueMsgs[c][literal][1];
      }
      /* add dist computing subgraph */
      string dist_1_name = "dist_"+literal+"_1_iteration_"+to_string(iteration);
      vector<string> dist_1_input_names;
      for (int cc : mln->c_map[literal]) {
        dist_1_input_names.push_back("cliqueMsg_"+to_string(cc)+"_"+literal+"_1_iteration_"+to_string(iteration));
      }
      mln->provG.addComputingSubgraph(dist_1_name, newDists[literal][1], Mul, dist_1_input_names);
      string dist_0_name = "dist_"+literal+"_0_iteration_"+to_string(iteration);
      vector<string> dist_0_input_names;
      for (int cc : mln->c_map[literal]) {
        dist_0_input_names.push_back("cliqueMsg_"+to_string(cc)+"_"+literal+"_0_iteration_"+to_string(iteration));
      }
      mln->provG.addComputingSubgraph(dist_0_name, newDists[literal][0], Mul, dist_0_input_names);
    }

    // pass clique messages to nodes
    for (string literal : mln->queries) {
      for (int c : mln->c_map[literal]) {
        nodeMsgs[literal][c][0] = newDists[literal][0]/cliqueMsgs[c][literal][0];
        nodeMsgs[literal][c][1] = newDists[literal][1]/cliqueMsgs[c][literal][1];
      }
      /* add subgraph of computing new nodeMsgs */
      for (int c : mln->c_map[literal]) {
        string nodeMsg_1_name = "nodeMsg_"+literal+"_"+to_string(c)+"_1_iteration_"+to_string(iteration);
        vector<string> nodeMsg_1_input_names {"dist_"+literal+"_1_iteration_"+to_string(iteration),
                                              "cliqueMsg_"+to_string(c)+"_"+literal+"_1_iteration_"+to_string(iteration)};
        unordered_map<string, string> params_1 = {{"numerator_name", "dist_"+literal+"_1_iteration_"+to_string(iteration)}};
        mln->provG.addComputingSubgraph(nodeMsg_1_name, nodeMsgs[literal][c][1], Div, nodeMsg_1_input_names, params_1);
        string nodeMsg_0_name = "nodeMsg_"+literal+"_"+to_string(c)+"_0_iteration_"+to_string(iteration);
        vector<string> nodeMsg_0_input_names {"dist_"+literal+"_0_iteration_"+to_string(iteration),
                                              "cliqueMsg_"+to_string(c)+"_"+literal+"_0_iteration_"+to_string(iteration)};
        unordered_map<string, string> params_0 = {{"numerator_name", "dist_"+literal+"_0_iteration_"+to_string(iteration)}};
        mln->provG.addComputingSubgraph(nodeMsg_0_name, nodeMsgs[literal][c][0], Div, nodeMsg_0_input_names, params_0);
      }
    }

    for (string literal : mln->queries) {
      // compute distribution by scaling
      double z = newDists[literal][0]+newDists[literal][1];
      newDists[literal][0] /= z;
      newDists[literal][1] /= z;
      /* add subgraph of computing probabilities */
      string prob_name = literal+"_iteration_"+to_string(iteration);
      vector<string> prob_input_names {"dist_"+literal+"_0_iteration_"+to_string(iteration),
                                      "dist_"+literal+"_1_iteration_"+to_string(iteration)};
      unordered_map<string, string> params = {{"numerator_name", "dist_"+literal+"_1_iteration_"+to_string(iteration)}};
      mln->provG.addComputingSubgraph(prob_name, newDists[literal][1], Scale, prob_input_names, params);
    }

    // check convergence
    converge = true;
    for (auto it : dists) {
      if (abs(it.second[1]-newDists[it.first][1])>1e-3) {
        converge = false;
      }
    }
    dists = newDists;
    iteration += 1;
  }
  return dists[query];
}


pair<map<string, vector<double> >, int > loopyBPRunWithProv(MLN* mln) {
  // total initialization
  map<string, map<int, vector<double> > > nodeMsgs;
  map<int, map<string, vector<double> > > cliqueMsgs;
  map<int, vector<double> > potentials;
  map<string, vector<double> > dists;
  double nodeMsg_init_value = 1;
  double cliqueMsg_init_value = 0;
  for (string literal : mln->queries) {
    for (int c : mln->c_map[literal]) {
      nodeMsgs[literal][c] = vector<double> (2, nodeMsg_init_value);
      cliqueMsgs[c][literal] = vector<double> (2, cliqueMsg_init_value);
      /* build EDB vertices of initial nodeMsgs */
      mln->provG.addVariableVertex(Parameter, "nodeMsg_"+literal+"_"+to_string(c)+"_1_iteration_0", nodeMsg_init_value);
      mln->provG.addVariableVertex(Parameter, "nodeMsg_"+literal+"_"+to_string(c)+"_0_iteration_0", nodeMsg_init_value);
    }
    dists[literal] = vector<double> (2, 0.5);
  }
  for (auto it1 : cliqueMsgs) {
    Clique c = mln->cliques[it1.first];
    vector<string> toSearch;
    for (auto it2 : it1.second) {
      toSearch.push_back(it2.first);
    }
    map<string, int> truth;
    for (string literal : c.getLiterals()) {
      if (mln->obs.find(literal)!=mln->obs.end()) {
        truth[literal] = mln->prob[literal];
      }
    }
    vector<double> potential;
    enumerateTruthWithProv(c, toSearch, truth, potential, 0);
    potentials[it1.first] = potential;
  }

  /* build EDB vertices of potentials */
  for (auto it : potentials) {
    for (int i=0; i<it.second.size(); i++) {
      string vertex_name = "potential_"+to_string(it.first)+"_"+to_string(i);
      mln->provG.addVariableVertex(Input, vertex_name, it.second[i]);
    }
  }

  // the loopy bp begins
  bool converge = false;
  int iteration = 1;
  while (!converge&&iteration<4) {
    
    // initialization newcliqueMsgs
    map<int, map<string, vector<double> > > newcliqueMsgs;
    for (auto it1 : nodeMsgs) {
      for (auto it2 : it1.second) {
        newcliqueMsgs[it2.first][it1.first] = vector<double> (2, 0);
      }
    }

    // pass node messages to cliques:
    for (auto it1 : cliqueMsgs) {
      // for each clique
      int c = it1.first;
      vector<string> toQuery;
      for (auto it : cliqueMsgs[c]) {
        toQuery.push_back(it.first);
      }
      for (int i=0; i<potentials[c].size(); i++) {
        int tmp = i;

        double value = potentials[c][i];
        /* add computing of variable value */
        string value_name = "value_"+to_string(c)+"_"+to_string(i)+"_iteration_"+to_string(iteration);
        vector<string> value_input_names {"potential_"+to_string(c)+"_"+to_string(i)};
        mln->provG.addComputingSubgraph(value_name, value, Mul, value_input_names);
        
        for (int s=toQuery.size()-1; s>=0; s--) {
          // for each unknown vertex
          int truth_value = tmp%2;
          value *= nodeMsgs[toQuery[s]][c][truth_value];
          tmp /= 2;
          /* update computing of value */
          vector<string> value_input_names {"nodeMsg_"+toQuery[s]+"_"+to_string(c)+"_"+to_string(truth_value)+"_iteration_"+to_string(iteration-1)};
          mln->provG.addComputingSubgraph(value_name, value, Mul, value_input_names);
        }

        tmp = i;
        for (int s=toQuery.size()-1; s>=0; s--) {
          int truth_value = tmp%2;

          double valueByNodeMsg = value/nodeMsgs[toQuery[s]][c][truth_value];
          /* add computing of valueByNodeMsg */
          string valueByNodeMsg_name = "valueByNodeMsg_"+to_string(c)+"_"+to_string(i)+"_"+toQuery[s]+"_"+to_string(truth_value)+"_iteration_"+to_string(iteration);
          vector<string> valueByNodeMsg_input_names {"value_"+to_string(c)+"_"+to_string(i)+"_iteration_"+to_string(iteration),
                                                    "nodeMsg_"+toQuery[s]+"_"+to_string(c)+"_"+to_string(truth_value)+"_iteration_"+to_string(iteration-1)};
          unordered_map<string, string> params = {{"numerator_name", "value_"+to_string(c)+"_"+to_string(i)+"_iteration_"+to_string(iteration)}};
          mln->provG.addComputingSubgraph(valueByNodeMsg_name, valueByNodeMsg, Div, valueByNodeMsg_input_names, params);

          newcliqueMsgs[c][toQuery[s]][truth_value] += valueByNodeMsg;
          /* update computing of cliqueMsg */
          string cliqueMsg_name = "cliqueMsg_"+to_string(c)+"_"+toQuery[s]+"_"+to_string(truth_value)+"_iteration_"+to_string(iteration);
          vector<string> cliqueMsg_input_names {"valueByNodeMsg_"+to_string(c)+"_"+to_string(i)+"_"+toQuery[s]+"_"+to_string(truth_value)+"_iteration_"+to_string(iteration)};
          mln->provG.addComputingSubgraph(cliqueMsg_name, newcliqueMsgs[c][toQuery[s]][truth_value], Sum, cliqueMsg_input_names);

          tmp /= 2;
        }
      }
    }
    cliqueMsgs = newcliqueMsgs;

    // initialize dists
    map<string, vector<double> > newDists;
    for (string literal : mln->queries) {
      newDists[literal] = vector<double> (2, 1);
      for (int c : mln->c_map[literal]) {
        newDists[literal][0] *= cliqueMsgs[c][literal][0];
        newDists[literal][1] *= cliqueMsgs[c][literal][1];
      }
      /* add dist computing subgraph */
      string dist_1_name = "dist_"+literal+"_1_iteration_"+to_string(iteration);
      vector<string> dist_1_input_names;
      for (int cc : mln->c_map[literal]) {
        dist_1_input_names.push_back("cliqueMsg_"+to_string(cc)+"_"+literal+"_1_iteration_"+to_string(iteration));
      }
      mln->provG.addComputingSubgraph(dist_1_name, newDists[literal][1], Mul, dist_1_input_names);
      string dist_0_name = "dist_"+literal+"_0_iteration_"+to_string(iteration);
      vector<string> dist_0_input_names;
      for (int cc : mln->c_map[literal]) {
        dist_0_input_names.push_back("cliqueMsg_"+to_string(cc)+"_"+literal+"_0_iteration_"+to_string(iteration));
      }
      mln->provG.addComputingSubgraph(dist_0_name, newDists[literal][0], Mul, dist_0_input_names);
    }

    // pass clique messages to nodes
    for (string literal : mln->queries) {
      for (int c : mln->c_map[literal]) {
        nodeMsgs[literal][c][0] = newDists[literal][0]/cliqueMsgs[c][literal][0];
        nodeMsgs[literal][c][1] = newDists[literal][1]/cliqueMsgs[c][literal][1];
      }
      /* add subgraph of computing new nodeMsgs */
      for (int c : mln->c_map[literal]) {
        string nodeMsg_1_name = "nodeMsg_"+literal+"_"+to_string(c)+"_1_iteration_"+to_string(iteration);
        vector<string> nodeMsg_1_input_names {"dist_"+literal+"_1_iteration_"+to_string(iteration),
                                              "cliqueMsg_"+to_string(c)+"_"+literal+"_1_iteration_"+to_string(iteration)};
        unordered_map<string, string> params_1 = {{"numerator_name", "dist_"+literal+"_1_iteration_"+to_string(iteration)}};
        mln->provG.addComputingSubgraph(nodeMsg_1_name, nodeMsgs[literal][c][1], Div, nodeMsg_1_input_names, params_1);
        string nodeMsg_0_name = "nodeMsg_"+literal+"_"+to_string(c)+"_0_iteration_"+to_string(iteration);
        vector<string> nodeMsg_0_input_names {"dist_"+literal+"_0_iteration_"+to_string(iteration),
                                              "cliqueMsg_"+to_string(c)+"_"+literal+"_0_iteration_"+to_string(iteration)};
        unordered_map<string, string> params_0 = {{"numerator_name", "dist_"+literal+"_0_iteration_"+to_string(iteration)}};
        mln->provG.addComputingSubgraph(nodeMsg_0_name, nodeMsgs[literal][c][0], Div, nodeMsg_0_input_names, params_0);
      }
    }

    for (string literal : mln->queries) {
      // compute distribution by scaling
      double z = newDists[literal][0]+newDists[literal][1];
      newDists[literal][0] /= z;
      newDists[literal][1] /= z;
      /* add subgraph of computing probabilities */
      string prob_name = literal+"_iteration_"+to_string(iteration);
      vector<string> prob_input_names {"dist_"+literal+"_0_iteration_"+to_string(iteration),
                                      "dist_"+literal+"_1_iteration_"+to_string(iteration)};
      unordered_map<string, string> params = {{"numerator_name", "dist_"+literal+"_1_iteration_"+to_string(iteration)}};
      mln->provG.addComputingSubgraph(prob_name, newDists[literal][1], Scale, prob_input_names, params);
    }

    // check convergence
    converge = true;
    for (auto it : dists) {
      if (abs(it.second[1]-newDists[it.first][1])>1e-3) {
        converge = false;
      }
    }
    dists = newDists;
    iteration += 1;
  }
  pair<map<string, vector<double> >, int > ret = {dists, iteration};
  return ret;
}

void enumerateLoopyBeliefPropagationWithProv(MLN* mln,
                                    string& query,
                                    vector<string> prob_obs,
                                    map<string, int> prob_obs_truth,
                                    map<string, double> prev_probs,
                                    double& prob,
                                    int pos
                                    ) {
  /*
  prob_obs: probabilistic observed tuples
  prob_obs_truth: the truth table of prob_obs to enumerate
  prev_probs: the original probabilities of prob_obs
  prob: the probability of query to estimate
  */
  if (pos==prob_obs.size()) {
    double tmp = 1.0;
    for (string literal : prob_obs) {
      mln->setObsProb(literal, prob_obs_truth[literal]);
      tmp *= prob_obs_truth[literal]==0 ? (1-prev_probs[literal]) : prev_probs[literal];
    }
    vector<double> dist = loopyBPRunWithProv(mln, query);
    prob += tmp*dist[1];
  }
  else {
    for (int i=0; i<=1; i++) {
      prob_obs_truth[prob_obs[pos]] = i;
      enumerateLoopyBeliefPropagationWithProv(mln, query, prob_obs, prob_obs_truth, prev_probs, prob, pos+1);
    }
  }
}

void MLN::loopyBeliefPropagationWithProv(string query) {
  assert(this->queries.find(query)!=this->queries.end());
  // save the original probs first
  map<string, double> prev_probs = this->prob;
  // determine the probabilistic observed tuples
  vector<string> prob_obs;
  for (string literal : this->obs) {
    if (this->prob[literal]>0&&
        this->prob[literal]<1&&
        !Parser::isRuleName(literal)) {
      prob_obs.push_back(literal);
    }
  }
  map<string, int> prob_obs_truth;
  double res = 0;
  enumerateLoopyBeliefPropagationWithProv(this, query, prob_obs, prob_obs_truth, prev_probs, res, 0);
  this->prob[query] = res;
  for (string literal : this->obs) {
    this->setObsProb(literal, prev_probs[literal]);
  }
  return;
}

int MLN::loopyBeliefPropagationWithProv() {
  map<string, vector<double> > dists;
  int round;
  tie(dists, round) = loopyBPRunWithProv(this);
  for (string literal : this->queries) {
    if (Parser::isVariable(literal)) {
      this->prob[literal] = dists[literal][1];
    }
  }
  return round;
}

int MLN::loopyBeliefPropagationWithCoarseProv() {
  // total initialization
  map<string, map<int, vector<double> > > nodeMsgs;
  map<int, map<string, vector<double> > > cliqueMsgs;
  map<int, vector<double> > potentials;
  map<string, vector<double> > dists;
  double nodeMsg_init_value = 1;
  double cliqueMsg_init_value = 0;
  for (string literal : this->queries) {
    for (int c : this->c_map[literal]) {
      nodeMsgs[literal][c] = vector<double> (2, nodeMsg_init_value);
      cliqueMsgs[c][literal] = vector<double> (2, cliqueMsg_init_value);
      /* build EDB vertices of initial nodeMsgs */
      // this->provG.addVariableVertex(Parameter, "nodeMsg_"+literal+"_"+to_string(c)+"_1_iteration_0", nodeMsg_init_value);
      // this->provG.addVariableVertex(Parameter, "nodeMsg_"+literal+"_"+to_string(c)+"_0_iteration_0", nodeMsg_init_value);
    }
    dists[literal] = vector<double> (2, 1);
  }
  for (auto it1 : cliqueMsgs) {
    Clique c = this->cliques[it1.first];
    vector<string> toSearch;
    for (auto it2 : it1.second) {
      toSearch.push_back(it2.first);
    }
    map<string, int> truth;
    for (string literal : c.getLiterals()) {
      if (this->obs.find(literal)!=this->obs.end()) {
        truth[literal] = this->prob[literal];
      }
    }
    vector<double> potential;
    enumerateTruthWithProv(c, toSearch, truth, potential, 0);
    potentials[it1.first] = potential;
  }

  /* build EDB vertices of potentials */
  for (auto it : potentials) {
    for (int i=0; i<it.second.size(); i++) {
      string vertex_name = "potential_"+to_string(it.first)+"_"+to_string(i);
      this->provG.addVariableVertex(Input, vertex_name, it.second[i]);
    }
  }

  // the loopy bp begins
  bool converge = false;
  int iteration = 1;
  while (!converge&&iteration<4) {
    
    // initialization newcliqueMsgs
    map<int, map<string, vector<double> > > newcliqueMsgs;
    for (auto it1 : nodeMsgs) {
      for (auto it2 : it1.second) {
        newcliqueMsgs[it2.first][it1.first] = vector<double> (2, 0);
      }
    }

    // pass node messages to cliques:
    std::unordered_map<std::string, std::unordered_set<std::string> > clique_variable_names; 
    for (auto it1 : cliqueMsgs) {
      int c = it1.first;
      for (auto it2 : cliqueMsgs[c]) {
        std::stringstream sts1, sts2;
        sts1 << "cliqueMsg_" << c << "_" << it2.first << "_" << 0 << "_iteration_" << iteration;
        clique_variable_names[sts1.str()] = std::unordered_set<std::string> {};
        sts2 << "cliqueMsg_" << c << "_" << it2.first << "_" << 1 << "_iteration_" << iteration;
        clique_variable_names[sts2.str()] = std::unordered_set<std::string> {};
      }
    }
    for (auto it1 : cliqueMsgs) {
      // for each clique
      int c = it1.first;
      vector<string> toQuery;
      for (auto it : cliqueMsgs[c]) {
        toQuery.push_back(it.first);
      }
      for (int i=0; i<potentials[c].size(); i++) {
        int tmp = i;
        double value = potentials[c][i];
        std::stringstream psts;
        psts << "potential_" << c << "_" << i;

        for (int s=toQuery.size()-1; s>=0; s--) {
          // for each unknown vertex
          int truth_value = tmp%2;
          value *= nodeMsgs[toQuery[s]][c][truth_value];
          std::stringstream sts_c, sts_n;
          sts_c << "cliqueMsg_" << c << "_" << toQuery[s] << "_" << truth_value << "_iteration_" << iteration;
          sts_n << "nodeMsg_" << toQuery[s] << "_" << c << "_" << truth_value << "_iteration_" << iteration-1;
          if (iteration>1) {
            clique_variable_names[sts_c.str()].insert(sts_n.str());
          }
          if (s==0) {
            clique_variable_names[sts_c.str()].insert(psts.str());
          }
          tmp /= 2;
        }

        tmp = i;
        for (int s=toQuery.size()-1; s>=0; s--) {
          int truth_value = tmp%2;
          double valueByNodeMsg = value/nodeMsgs[toQuery[s]][c][truth_value];
          newcliqueMsgs[c][toQuery[s]][truth_value] += valueByNodeMsg;
          tmp /= 2;
        }
      }
    }
    cliqueMsgs = newcliqueMsgs;

    // initialize dists
    map<string, vector<double> > newDists;
    for (string literal : this->queries) {
      newDists[literal] = vector<double> (2, 1);
      for (int c : this->c_map[literal]) {
        newDists[literal][0] *= cliqueMsgs[c][literal][0];
        newDists[literal][1] *= cliqueMsgs[c][literal][1];
      }
      /* add dist computing subgraph */
      string dist_1_name = "dist_"+literal+"_1_iteration_"+to_string(iteration);
      vector<string> dist_1_input_names;
      for (int cc : this->c_map[literal]) {
        for (std::string input_name : clique_variable_names["cliqueMsg_"+to_string(cc)+"_"+literal+"_"+to_string(1)+"_iteration_"+to_string(iteration)]) {
          dist_1_input_names.push_back(input_name);
        }
      }
      this->provG.addComputingSubgraph(dist_1_name, newDists[literal][1], BeliefPropagation, dist_1_input_names);
      string dist_0_name = "dist_"+literal+"_0_iteration_"+to_string(iteration);
      vector<string> dist_0_input_names;
      for (int cc : this->c_map[literal]) {
        for (std::string input_name : clique_variable_names["cliqueMsg_"+to_string(cc)+"_"+literal+"_"+to_string(0)+"_iteration_"+to_string(iteration)]) {
          dist_0_input_names.push_back(input_name);
        }
      }
      this->provG.addComputingSubgraph(dist_0_name, newDists[literal][0], BeliefPropagation, dist_0_input_names);
    }

    // pass clique messages to nodes
    for (string literal : this->queries) {
      for (int c : this->c_map[literal]) {
        nodeMsgs[literal][c][0] = newDists[literal][0]/cliqueMsgs[c][literal][0];
        nodeMsgs[literal][c][1] = newDists[literal][1]/cliqueMsgs[c][literal][1];
      }
      /* add subgraph of computing new nodeMsgs */
      for (int c : this->c_map[literal]) {
        string nodeMsg_1_name = "nodeMsg_"+literal+"_"+to_string(c)+"_1_iteration_"+to_string(iteration);
        vector<string> nodeMsg_1_input_names {"dist_"+literal+"_1_iteration_"+to_string(iteration),
                                              "cliqueMsg_"+to_string(c)+"_"+literal+"_1_iteration_"+to_string(iteration)};
        unordered_map<string, string> params_1 = {{"numerator_name", "dist_"+literal+"_1_iteration_"+to_string(iteration)}};
        this->provG.addComputingSubgraph(nodeMsg_1_name, nodeMsgs[literal][c][1], Div, nodeMsg_1_input_names, params_1);
        string nodeMsg_0_name = "nodeMsg_"+literal+"_"+to_string(c)+"_0_iteration_"+to_string(iteration);
        vector<string> nodeMsg_0_input_names {"dist_"+literal+"_0_iteration_"+to_string(iteration),
                                              "cliqueMsg_"+to_string(c)+"_"+literal+"_0_iteration_"+to_string(iteration)};
        unordered_map<string, string> params_0 = {{"numerator_name", "dist_"+literal+"_0_iteration_"+to_string(iteration)}};
        this->provG.addComputingSubgraph(nodeMsg_0_name, nodeMsgs[literal][c][0], Div, nodeMsg_0_input_names, params_0);
      }
    }

    for (string literal : this->queries) {
      // compute probabilities by scaling
      double z = newDists[literal][0]+newDists[literal][1];
      newDists[literal][0] /= z;
      newDists[literal][1] /= z;
      /* add subgraph of computing probabilities */
      string prob_name = literal+"_iteration_"+to_string(iteration);
      vector<string> prob_input_names {"dist_"+literal+"_0_iteration_"+to_string(iteration),
                                      "dist_"+literal+"_1_iteration_"+to_string(iteration)};
      unordered_map<string, string> params = {{"numerator_name", "dist_"+literal+"_1_iteration_"+to_string(iteration)}};
      this->provG.addComputingSubgraph(prob_name, newDists[literal][1], Scale, prob_input_names, params);
    }

    // check convergence
    converge = true;
    for (auto it : dists) {
      if (abs(it.second[1]-newDists[it.first][1])>1e-3) {
        converge = false;
      }
    }
    dists = newDists;
    iteration += 1;
  }
  for (string literal : this->queries) {
    if (Parser::isVariable(literal)) {
      this->prob[literal] = dists[literal][1];
    }
  }
  return iteration;
}
