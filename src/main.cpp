
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


void printObservation(Load l) {
  // print out observed literals and their probabilities
  map<string, double> prob = l.getProb();
  for (auto it=prob.begin(); it!=prob.end(); it++) {
    cout << it->first << ' ' << it->second << endl;
  }
  cout << endl;
}


void parseTest(MLN mln, Load l) {
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
}


void printLiterals(MLN mln) {
  // print out the observed literals and unknown literals after parsing
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
}


void cliqueTest(MLN& mln, string query_name) {
  cout << "cliques of queried tuple: " << endl;
  vector<Clique> cliques = mln.getCliques(query_name);
  for (Clique c : cliques) {
    c.printClique();
  }
  cout << endl;
}


void gibbsTest(MLN& mln, int round, string query_name) {
  clock_t t1 = clock();
  mln.gibbsSampling(round);
  clock_t t2 = clock();
  cout << "gibbs sample v1: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << "gibbs sample v1: " << mln.queryProb(query_name) << endl;

  clock_t t3 = clock();
  mln.gibbsSampling_v2(round);
  clock_t t4 = clock();
  cout << "gibbs sample v2: " << ((double)(t4-t3))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << "gibbs sample v2: " << mln.queryProb(query_name) << endl;

  clock_t t5 = clock();
  mln.gibbsSampling_v3(round);
  clock_t t6 = clock();
  cout << "gibbs sample v3: " << ((double)(t6-t5))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << "gibbs sample v3: " << mln.queryProb(query_name) << endl;
  cout << endl;
}


void varianceTest(MLN& mln, string query_name) {
  clock_t t1 = clock();
  vector<double> p1;
  for (int i=0; i<1000; i++) {
    mln.gibbsSampling(1000);
    p1.push_back(mln.queryProb(query_name));
  }
  clock_t t2 = clock();
  cout << "gibbs sample v1: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  double var1 = calcVar(p1);
  cout << "variance of v1: " << var1 << endl;

  clock_t t3 = clock();
  vector<double> p2;
  for (int i=0; i<1000; i++) {
    mln.gibbsSampling_v2(1000);
    p2.push_back(mln.queryProb(query_name));
  }
  clock_t t4 = clock();
  cout << "gibbs sample v2: " << ((double)(t4-t3))/CLOCKS_PER_SEC << " seconds" << endl;
  double var2 = calcVar(p2);
  cout << "variance of v2: " << var2 << endl;

  clock_t t5 = clock();
  vector<double> p3;
  for (int i=0; i<1000; i++) {
    mln.gibbsSampling_v3(1000);
    p3.push_back(mln.queryProb(query_name));
  }
  clock_t t6 = clock();
  cout << "gibbs sample v3: " << ((double)(t6-t5))/CLOCKS_PER_SEC << " seconds" << endl;
  double var3 = calcVar(p3);
  cout << "variance of v3: " << var3 << endl;
}


void influenceTest(MLN mln, string target, int n) {
  clock_t t1 = clock();
  Influence influence(mln);
  vector<double> probs;
  for (int i=0; i<=n; i++) {
    probs.push_back((1.0/n)*i);
  }
  influence.computeObsInfluence(target, probs);
  set<string> queries = mln.getQueryLiterals();
  for (string q : queries) {
    vector<double> q_p = influence.getProbs(q);
    cout << q << ": ";
    for (double p : q_p) {
      cout << p << ' ';
    }
    cout << endl;
  }
  cout << endl;
  clock_t t2 = clock();
  cout << "influence compute time: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
}


int main(int argc, char* argv[]) {

  unordered_map<string, string> args;
  vector<string> argvs;
  for (int i=0; i<argc; i++) {
    argvs.push_back(string(argv[i]));
  }
  for (int i=1; i<argc; i+=2) {
    assert(i+1<argc);
    if (argvs[i]=="-q" || argvs[i+1]=="-query_name") {
      args["query_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-t" || argvs[i+1]=="-target_name") {
      args["target_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-o" || argvs[i+1]=="-observe_file") {
      args["observe_file"] = argvs[i+1];
    }
    else if (argvs[i]=="-p" || argvs[i+1]=="-provenance_file") {
      args["provenance_file"] = argvs[i+1];
    }
  }

  assert(args.find("query_name")!=args.end());
  assert(args.find("target_name")!=args.end());
  if (args.find("observe_file")==args.end()) {
    args["observe_file"] = ObsFile;
  }
  if (args.find("provenance_file")==args.end()) {
    args["provenance_file"] = provFile;
  }

  Load l(args["provenance_file"], args["observe_file"]);
  string prov = l.getProv();
  // cout << prov << endl;

  // printObservation(l);

  MLN mln(l.getProv(), l.getProb());
  Parser parser;
  parser.parseProvenance(mln);

  // parseTest(mln, l);

  printLiterals(mln);

  cliqueTest(mln, args["query_name"]);

  gibbsTest(mln, 10000, args["query_name"]);

  // varianceTest(mln, args["query_name"]);

  influenceTest(mln, args["target_name"], 10);
}