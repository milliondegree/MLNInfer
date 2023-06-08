#include "../MLN.h"

using namespace std;


int main(int argc, char* argv[]) {  
  
  string provenance_file = "/home/jz598/MLNInfer/data/smoke/prov/test2.txt";
  string observe_file = "/home/jz598/MLNInfer/data/smoke/observe/test2.db";

  Load l(provenance_file, observe_file);
  vector<string> prov = l.getProv();

  MLN mln(l);
  Parser parser;
  parser.parseProvenance(mln);
  mln.merge();
  
  MLN mmln;
  MLN amln;

  string query_name = "cancer_1";

  mmln = mln.getMinimalMLN(query_name);
  cout << "minumum MLN: " << endl;
  cout << "observed tuple number: " << mmln.obs.size() << endl;
  cout << "unobserved tuple number: " << mmln.queries.size() << endl;
  cout << "clique number: " << mmln.cliques.size() << endl;
  cout << endl;

  cout << mmln.toString();

  mmln.loopyBeliefPropagationWithProv(query_name);
  double prob = mmln.queryProb(query_name);
  cout << "probability of " << query_name << " is " << setprecision(9) << prob << endl;

  /* print out the provGraph*/
  mmln.provG.setSavePath("/home/jz598/MLNInfer/data/CProv/raw/mln-cprov-graph-test.dot");
  mmln.provG.saveGraph();

  /* trace provenance */
  string output_name = "cancer_1_iteration_3";
  CProvGraph query_of_output = mmln.provG.traceProvOfVariableByName(output_name);
  query_of_output.saveGraph();
}