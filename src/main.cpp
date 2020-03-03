
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <math.h>
#include "Load.h"
#include "MLN.h"
#include "Clique.h"
#include "Influence.h"
using namespace std;


#define provFile \
"./data/prov/cancer5.txt"

#define ObsFile \
"./data/observe/smokeTest.db"

#define provFileTrain \
"./data/prov/cancer2Train.txt"

#define ObsFileTrain \
"./data/observe/smokeTrain.db"


double calcVar(vector<double> values) {
  double mean = 0;
  for (int i=0; i<values.size(); i++) {
    mean += values[i];
  }
  mean /= values.size();
  double var = 0;
  for (int i=0; i<values.size(); i++) {
    var += pow((values[i]-mean), 2);
  }
  var /= values.size();
  return var;
}


int main(int argc, char* argv[]) {

  string query_name (argv[1]);
  string obsfile = ObsFile;
  if (argc>=3) {
    obsfile = string (argv[2]);
  }
  string provfile = provFile;
  if (argc>=4) {
    provfile = string (argv[3]);
  }

  Load l(provfile, obsfile);
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
  cout << "optimized parsing time: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;

  clock_t t3 = clock();
  mln.setProperty(l.getProv(), l.getProb());
  clock_t t4 = clock();
  cout << "original parsing time: " << ((double)(t4-t3))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << endl;

  set<string> obs = mln.getObsLiterals();
  set<string> queries = mln.getQueryLiterals();
  cout << "observed tuples: ";
  for (string s : obs) {
    cout << s << ' ';
  }
  cout << endl;
  cout << "unknown tuples: ";
  for (string s : queries) {
    cout << s << ' ';
  }
  cout << endl;
  cout << endl;

  cout << "cliques of queried tuple: " << endl;
  vector<Clique> cliques = mln.getCliques(query_name);
  for (Clique c : cliques) {
    c.printClique();
  }
  cout << endl;


  t1 = clock();
  mln.gibbsSampling(10000);
  t2 = clock();
  cout << "gibbs sample v1: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << "gibbs sample v1: " << mln.queryProb(query_name) << endl;

  t3 = clock();
  mln.gibbsSampling_v2(10000);
  t4 = clock();
  cout << "gibbs sample v2: " << ((double)(t4-t3))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << "gibbs sample v2: " << mln.queryProb(query_name) << endl;

  clock_t t5 = clock();
  mln.gibbsSampling_v3(10000);
  clock_t t6 = clock();
  cout << "gibbs sample v3: " << ((double)(t6-t5))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << "gibbs sample v3: " << mln.queryProb(query_name) << endl;

  cout << endl;


  t1 = clock();
  vector<double> p1;
  for (int i=0; i<1000; i++) {
    mln.gibbsSampling(1000);
    p1.push_back(mln.queryProb(query_name));
  }
  t2 = clock();
  cout << "gibbs sample v1: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  double var1 = calcVar(p1);
  cout << "variance of v1: " << var1 << endl;

  t3 = clock();
  vector<double> p2;
  for (int i=0; i<1000; i++) {
    mln.gibbsSampling_v2(1000);
    p2.push_back(mln.queryProb(query_name));
  }
  t4 = clock();
  cout << "gibbs sample v2: " << ((double)(t4-t3))/CLOCKS_PER_SEC << " seconds" << endl;
  double var2 = calcVar(p2);
  cout << "variance of v2: " << var2 << endl;

  t5 = clock();
  vector<double> p3;
  for (int i=0; i<1000; i++) {
    mln.gibbsSampling_v3(1000);
    p3.push_back(mln.queryProb(query_name));
  }
  t6 = clock();
  cout << "gibbs sample v3: " << ((double)(t6-t5))/CLOCKS_PER_SEC << " seconds" << endl;
  double var3 = calcVar(p3);
  cout << "variance of v3: " << var3 << endl;

}