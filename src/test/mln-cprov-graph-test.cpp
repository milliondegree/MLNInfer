#include "MLN.h"

void printMLNStatistic(MLN& mln) {
  cout << "observed tuple number: " << mln.obs.size() << endl;
  cout << "unobserved tuple number: " << mln.queries.size() << endl;
  cout << "clique number: " << mln.cliques.size() << endl;
}

void probabilityQuery(MLN& mln, int round, string query_name, string mode, double delta) {
  clock_t t1 = clock();
  if (mode=="gibbs") {
    mln.gibbsSampling(round);
  }
  else if (mode=="pgibbs") {
    // wrong results
    mln.gibbsSampling_vp(round, query_name, delta);
  }
  else if (mode=="mcsat"){
    mln.mcsat(round, query_name);
  }
  else if (mode=="pmcsat") {
    mln.pmcsat(round, query_name);
  }
  else if (mode=="bp") {
    mln.naiveBeliefPropagation(query_name);
  }
  else if (mode=="abp") {
    mln.advanceBeliefPropagation(query_name);
  }
  else if (mode=="lbp") {
    mln.loopyBeliefPropagation(query_name);
  }
  else if (mode=="mclbp") {
    mln.loopyBeliefPropagationMCS(query_name, round);
  }
  clock_t t2 = clock();
  cout << mode+" sample time: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  double prob = mln.queryProb(query_name);
  cout << "probability of " << query_name << " is " << setprecision(9) << prob << endl;
  cout << endl;
}

int main(int argc, char* argv[]) {  
  
  string provenance_file = "/home/jz598/MLNInfer/data/smoke/prov/test2.txt";
  string observe_file = "/home/jz598/MLNInfer/data/smoke/observe/test2.db";

  Load l(provenance_file, observe_file);
  vector<string> prov = l.getProv();

  MLN mln(l);
  Parser parser;
  parser.parseProvenance(mln);
  mln.merge();

  cout << "original MLN: " << endl;
  printMLNStatistic(mln);
  cout << endl;
  
  MLN mmln;
  MLN amln;

  string query_name = "smoke1";

  mmln = mln.getMinimalMLN(query_name);
  cout << "minumum MLN: " << endl;
  printMLNStatistic(mmln);
  cout << endl;

}