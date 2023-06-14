#include "../MLN.h"

using namespace std;


int main(int argc, char* argv[]) {  

  unordered_map<string, string> args;
  vector<string> argvs;
  for (int i=0; i<argc; i++) {
    argvs.push_back(string(argv[i]));
  }
  for (int i=1; i<argc; i+=2) {
    assert(i+1<argc);
    if (argvs[i]=="-q" || argvs[i]=="-query_name") {
      args["query_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-v" || argvs[i]=="-variable_name") {
      args["variable_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-e") {
      args["epsilon"] = argvs[i+1];
    }
  }
  
  string provenance_file = "/home/jz598/MLNInfer/data/smoke/prov/train.txt";
  string observe_file = "/home/jz598/MLNInfer/data/smoke/observe/smokeTrain.db";
  // string provenance_file = "/home/jz598/MLNInfer/data/hypertext-class/sample7/prov/sample710.txt";
  // string observe_file = "/home/jz598/MLNInfer/data/hypertext-class/sample7/sample710.obs";

  Load l(provenance_file, observe_file);
  vector<string> prov = l.getProv();

  MLN mln(l);
  Parser parser;
  parser.parseProvenance(mln);
  mln.merge();
  
  MLN mmln;
  MLN amln;

  string query_name = "cancer_2";
  // string query_name = "topic_Department_29";
  if (args.find("query_name")!=args.end()) query_name = args["query_name"];
  clock_t t1, t2;

  // mmln = mln.getMinimalMLN(query_name);
  mmln = mln;
  cout << "minumum MLN: " << endl;
  cout << "observed tuple number: " << mmln.obs.size() << endl;
  cout << "unobserved tuple number: " << mmln.queries.size() << endl;
  cout << "clique number: " << mmln.cliques.size() << endl;
  cout << endl;

  cout << mmln.toString();

  t1 = clock();
  mmln.loopyBeliefPropagation(query_name);
  t2 = clock();
  cout << "time cost of loopy belief propagation: " << (t2-t1)*1.0/CLOCKS_PER_SEC << endl;

  t1 = clock();
  mmln.loopyBeliefPropagationWithProv(query_name);
  t2 = clock();
  cout << "time cost of loopy belief propagation with provenance: " << (t2-t1)*1.0/CLOCKS_PER_SEC << endl;
  double prob = mmln.queryProb(query_name);
  cout << "probability of " << query_name << " is " << setprecision(9) << prob << endl;

  /* print out the provGraph*/
  mmln.provG.setSavePath("/home/jz598/MLNInfer/data/CProv/raw/mln-cprov-graph-test.dot");
  mmln.provG.saveGraph();

  /* trace provenance */
  string output_name = "cancer_2_iteration_2";
  // string output_name = "topic_Department_29_iteration_3";
  if (args.find("variable_name")!=args.end()) output_name = args["variable_name"];
  t1 = clock();
  CProvGraph query_of_output = mmln.provG.ProvenanceQuery(output_name);
  t2 = clock();
  cout << "time cost of provenance query: " << (t2-t1)*1.0/CLOCKS_PER_SEC << endl;
  // query_of_output.printVertex(query_of_output.getVertexByName(output_name));
  query_of_output.saveGraph();

  /* compute variable using provenance */
  // cout << "compute record: " << query_of_output.getVertexValueByName(output_name) << endl;
  // unordered_map<string, double> changedEDBs = {{"potential_2657_0", 1.1}};
  // // unordered_map<string, double> changedEDBs;
  // t1 = clock();
  // double changedResult = query_of_output.computeVariableWithChangedEDBs(output_name, changedEDBs);
  // cout << "compute result: " << changedResult << endl;;
  // t2 = clock();
  // cout << "time cost of computing variable using provenance: " << (t2-t1)*1.0/CLOCKS_PER_SEC << endl;

  /* PXAI-influence */
  // Grader grader;
  // unordered_set<string> observedTuples = mln.getObsLiterals();

  /* PXAI - approximate subgraph*/
  double epsilon = 0.01;
  if (args.find("epsilon")!=args.end()) epsilon = stod(args["epsilon"]);
  cout << "start approximate subgraph query, epsilon: " << epsilon << endl; 
  t1 = clock();
  CProvGraph approx_subgraph = query_of_output.ApproximateSubGraphQuery(output_name, epsilon);
  t2 = clock();
  approx_subgraph.saveGraph();
  cout << "approximate subgraph query time: " << (t2-t1)*1.0/CLOCKS_PER_SEC << endl;
  cout << "approximate subgraph size: " << approx_subgraph.getVertexEBDsByName(output_name).size() << "(# of input features)" << endl;
  cout << "approximate compute result: " << approx_subgraph.getVertexValueByName(output_name) << endl;
}