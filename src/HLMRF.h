#include "MLN.h"

class HLMRF : public MLN {

  int loopyBeliefPropagationWithProv() {
    map<string, vector<double> > dists;
    
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
        this->provG.addVariableVertex(Parameter, "nodeMsg_"+literal+"_"+to_string(c)+"_1_iteration_0", nodeMsg_init_value);
        this->provG.addVariableVertex(Parameter, "nodeMsg_"+literal+"_"+to_string(c)+"_0_iteration_0", nodeMsg_init_value);
      }
      dists[literal] = vector<double> (2, 0.5);
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
    }

    

    for (string literal : this->queries) {
      if (Parser::isVariable(literal)) {
      this->prob[literal] = dists[literal][1];
      }
    }
    return iteration;
  }

};