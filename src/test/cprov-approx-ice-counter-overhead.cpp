#include "../MLN.h"

using namespace std;

void enumerateTruthTest(Clique& c, 
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
      enumerateTruthTest(c, toSearch, truth, res, pos+1);
    }
  }
}


double computeInfluence(MLN& mln, CProvGraph& query_of_output, string& output_variable_name, string& literal) {
  unordered_map<string, float> changedEDBs;
  double prev_value = query_of_output.getVertexValueByName(output_variable_name);
  mln.prob[literal] = 1-mln.prob[literal];
  for (int ci : mln.c_map[literal]) {
    Clique c = mln.cliques[ci];
    vector<string> toSearch;
    for (string l : c.getLiterals()) {
      if (mln.queries.find(l)!=mln.queries.end()) {
        toSearch.push_back(l);
      }
    }
    map<string, int> truth;
    for (string l : c.getLiterals()) {
      if (mln.obs.find(l)!=mln.obs.end()) {
        truth[l] = mln.prob[l];
      }
    }
    vector<double> potential;
    enumerateTruthTest(c, toSearch, truth, potential, 0);
    for (int i=0; i<potential.size(); i++) {
      float new_potential = potential[i];
      string potential_variable_name = "potential_"+to_string(ci)+"_"+to_string(i);
      if (new_potential!=query_of_output.getVertexValueByName(potential_variable_name)) {
        changedEDBs[potential_variable_name] = new_potential;
      }
    }
  }
  query_of_output.computeVariableWithChangedEDBs(output_variable_name, changedEDBs);
  double after_value = query_of_output.getVertexByName(output_variable_name);
  mln.prob[literal] = 1-mln.prob[literal];
  return after_value-prev_value;
}


int main(int argc, char* argv[]) {  

  vector<string> provenance_file_list = {"/home/jz598/MLNInfer/data/hypertext-class/sample7/prov/sample71.txt",
                                        "/home/jz598/MLNInfer/data/hypertext-class/sample7/prov/sample72.txt",
                                        "/home/jz598/MLNInfer/data/hypertext-class/sample7/prov/sample73.txt",
                                        "/home/jz598/MLNInfer/data/hypertext-class/sample7/prov/sample74.txt",
                                        "/home/jz598/MLNInfer/data/hypertext-class/sample7/prov/sample75.txt",
                                        "/home/jz598/MLNInfer/data/hypertext-class/sample7/prov/sample76.txt",
                                        "/home/jz598/MLNInfer/data/hypertext-class/sample7/prov/sample77.txt",
                                        "/home/jz598/MLNInfer/data/hypertext-class/sample7/prov/sample78.txt",
                                        "/home/jz598/MLNInfer/data/hypertext-class/sample7/prov/sample79.txt",
                                        "/home/jz598/MLNInfer/data/hypertext-class/sample7/prov/sample710.txt"
  };
  vector<string> observe_file_list = {"/home/jz598/MLNInfer/data/hypertext-class/sample7/sample71.obs",
                                      "/home/jz598/MLNInfer/data/hypertext-class/sample7/sample72.obs",
                                      "/home/jz598/MLNInfer/data/hypertext-class/sample7/sample73.obs",
                                      "/home/jz598/MLNInfer/data/hypertext-class/sample7/sample74.obs",
                                      "/home/jz598/MLNInfer/data/hypertext-class/sample7/sample75.obs",
                                      "/home/jz598/MLNInfer/data/hypertext-class/sample7/sample76.obs",
                                      "/home/jz598/MLNInfer/data/hypertext-class/sample7/sample77.obs",
                                      "/home/jz598/MLNInfer/data/hypertext-class/sample7/sample78.obs",
                                      "/home/jz598/MLNInfer/data/hypertext-class/sample7/sample79.obs",
                                      "/home/jz598/MLNInfer/data/hypertext-class/sample7/sample710.obs"
  };
  vector<double> ice_time_list, approx_time_list, ice_time_list_on_approx;

  for (int i=0; i<10; i++) {
    cout << "start sample " << i+1 << endl;
    Load l(provenance_file_list[i], observe_file_list[i]);
    vector<string> prov = l.getProv();

    MLN mln(l);
    Parser parser;
    parser.parseProvenance(mln);
    mln.merge();
    cout << "clique size: " << mln.cliques.size() << endl;

    clock_t t1, t2;
    int round = mln.loopyBeliefPropagationWithProv();
    cout << "round " << round << endl;

    double ice_time_sum = 0;
    double approx_time_sum = 0;
    double ice_time_approx_sum = 0;
    int count = 0;
    for (string query : mln.queries) {
      if (Parser::isVariable(query)) {
        string output_variable_name = query+"_iteration_"+to_string(round-1);
        CProvGraph query_of_output = mln.provG.ProvenanceQuery(output_variable_name);

        MLN mmln = mln.getMinimalMLN(query);

        /* start to compute ICEs */
        // t1 = clock();
        // for (string literal : mmln.obs) {
        //   computeInfluence(mln, query_of_output, output_variable_name, literal);
        // }
        // t2 = clock();
        // ice_time_sum += (t2-t1)*1.0/CLOCKS_PER_SEC;

        /* start to compute approx subgraph*/
        t1 = clock();
        // MLN amln = mmln.approximateSubGraph(query, mln.prob[query], 1, 0.001);
        // CProvGraph approx_subgraph = query_of_output.ApproximateSubGraphQuery(output_variable_name, 0.01, 0.01);
        CProvGraph approx_subgraph = query_of_output.ApproximateSubGraphQueryPrune(output_variable_name, 0.01, 0.01);
        // approx_subgraph.saveGraph();
        t2 = clock();
        approx_time_sum += (t2-t1)*1.0/CLOCKS_PER_SEC;

        /* start to compute ice on approx subgraph*/
        t1 = clock();
        for (string literal : mmln.obs) {
          computeInfluence(mln, approx_subgraph, output_variable_name, literal);
        }
        t2 = clock();
        ice_time_approx_sum += (t2-t1)*1.0/CLOCKS_PER_SEC;

        // add a query count
        count += 1;
        if (count>50) break;
      }
    }
    ice_time_list.push_back(ice_time_sum/count);
    approx_time_list.push_back(approx_time_sum/count);
    ice_time_list_on_approx.push_back(ice_time_approx_sum/count);
  }

  for (double t : ice_time_list) {
    cout << t << ", ";
  }
  cout << endl;
  for (double t : approx_time_list) {
    cout << t << ", ";
  }
  cout << endl;
  for (double t : ice_time_list_on_approx) {
    cout << t << ", ";
  }
  cout << endl;

}