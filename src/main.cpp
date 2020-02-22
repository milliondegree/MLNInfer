
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include "Load.h"
#include "MLN.h"
#include "Clique.h"
using namespace std;


#define provFile \
"./data/prov/cancer5.txt"

#define ObsFile \
"./data/observe/smokeTest.db"


int main(int argc, char* argv[]) {

  string query_name (argv[1]);

  Load l(provFile, ObsFile);
  string prov = l.getProv();
  cout << prov << endl;

  map<string, double> prob = l.getProb();
  for (auto it=prob.begin(); it!=prob.end(); it++) {
    cout << it->first << ' ' << it->second << endl;
  }
  cout << endl;

  MLN mln(l.getProv(), l.getProb());

  Parser parser;
  clock_t t1 = clock();
  parser.parseProvenance(mln);
  clock_t t2 = clock();
  cout << "parsing time: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << endl;


  clock_t t3 = clock();
  mln.setProperty(l.getProv(), l.getProb());
  clock_t t4 = clock();
  cout << "parsing time: " << ((double)(t4-t3))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << endl;

  set<string> obs = mln.getObsLiterals();
  set<string> queries = mln.getQueryLiterals();
  for (string s : obs) {
    cout << s << ' ';
  }
  cout << endl;
  for (string s : queries) {
    cout << s << ' ';
  }
  cout << endl;

  vector<Clique> cliques = mln.getCliques(query_name);
  for (Clique c : cliques) {
    c.printClique();
  }

  mln.gibbsSampling_v2(10000);
  double p = mln.queryProb(query_name);
  cout << p << endl;

}