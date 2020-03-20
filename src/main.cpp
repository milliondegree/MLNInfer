
#include <iostream>
#include <fstream>
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
  unordered_set<string> obs = mln.getObsLiterals();
  unordered_set<string> queries = mln.getQueryLiterals();
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


void saveToFile(MLN& mln, string file_name) {
  cout << "saving to file " << file_name << endl;
  clock_t t1 = clock();
  ofstream file;
  file.open(file_name);
  mln.gibbsSampling_v3(100000);
  mln.saveToFile(file);
  file.close();
  clock_t t2 = clock();
  cout << "saving finished, time cost: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
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


void probabilityQuery(MLN& mln, int round, string query_name) {
  clock_t t1 = clock();
  // mln.gibbsSampling_v3(round);
  mln.gibbsSampling_v4(round, query_name);
  clock_t t2 = clock();
  cout << "gibbs sample: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  double prob = mln.queryProb(query_name);
  cout << "probability of " << query_name << " is " << prob << endl;
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


map<string, vector<double>> boxplotTest(MLN& mln, int round, int times) {
  map<string, vector<double>> values;
  unordered_set<string> queries = mln.getQueryLiterals();
  for (string s : queries) {
    values[s] = vector<double> ();
  }
  for (int t=0; t<times; t++) {
    mln.gibbsSampling_v3(round);
    for (string s : queries) {
      values[s].push_back(mln.queryProb(s));
    }
  }
  return values;
}


void boxplotTestSave(MLN mln, string file_name, int times) {
  ofstream file;
  file.open(file_name);
  vector<int> rounds = {1000, 2000, 5000, 10000, 50000, 100000};
  unordered_set<string> queries = mln.getQueryLiterals();
  for (int round : rounds) {
    // cout << round << endl;
    file << "rounds: " << round << ' ' << "times: " << times << endl;
    // map<string, vector<double>> values = boxplotTest(mln, round, times);
    map<string, vector<double>> values = mln.getAllProbs(round, times);
    for (string s : queries) {
      file << s << ": ";
      for (double v : values[s]) {
        file << v << ' ';
      }
      file << endl;
    }
    file << endl;
  }
  file.close();
}


void influenceTest(MLN mln, string target, int n) {
  clock_t t1 = clock();
  Influence influence(mln);
  vector<double> probs;
  for (int i=0; i<=n; i++) {
    probs.push_back((1.0/n)*i);
  }
  influence.computeObsInfluence(target, probs);
  unordered_set<string> queries = mln.getQueryLiterals();
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


void influenceQuery(MLN& mln, string query, int round, double delta) {
  cout << "compute influence of " << query << endl;
  Grader grader;
  clock_t t1 = clock();
  grader.computeGradients_v2(mln, query, round, delta);
  // grader.computeGradients(mln, query, round);
  clock_t t2 = clock();
  cout << "influence compute time: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  unordered_map<string, double> influs = mln.getInfluence(query);
  for (auto it=influs.begin(); it!=influs.end(); it++) {
    cout << it->first << ' ' << it->second << endl;
  }
  cout << endl;
}


void setDefaultArgs(unordered_map<string, string>& args) {
  args["observe_file"] = "./data/observe/smokeTest.db";
  args["provenance_file"] = "./data/prov/cancer2.txt";
  args["round"] = "100000";
  args["delta"] = "0.1";
}


void save3DPlot(MLN& mln, string file_name, string query, int round, double delta) {
  ofstream file;
  file.open(file_name);
  Grader grader;
  vector<double> smokes;
  vector<double> friends;
  vector<double> smokes_inf;
  vector<double> friends_inf;
  for (int i=0; i<=10; i++) {
    for (int j=0; j<=10; j++) {
      smokes.push_back(i*0.1);
      friends.push_back(j*0.1);
      mln.setObsProb("smoke1", i*0.1);
      mln.setObsProb("friends1_2", j*0.1);
      grader.computeGradients_v2(mln, query, round, delta);
      unordered_map<string, double> influs = mln.getInfluence(query);
      smokes_inf.push_back(influs["smoke1"]);
      friends_inf.push_back(influs["friends1_2"]);
    }
  }
  file << "smoke1: ";
  for (double v : smokes) {
    file << v << ' ';
  }
  file << endl;
  file << "inf: ";
  for (double v : smokes_inf) {
    file << v << ' ';
  }
  file << endl;
  file << "friend1_2: ";
  for (double v : friends) {
    file << v << ' ';
  }
  file << endl;
  file << "inf: ";
  for (double v : friends_inf) {
    file << v << ' ';
  }
  file << endl;
}


int main(int argc, char* argv[]) {

  unordered_map<string, string> args;
  setDefaultArgs(args);
  vector<string> argvs;
  for (int i=0; i<argc; i++) {
    argvs.push_back(string(argv[i]));
  }
  for (int i=1; i<argc; i+=2) {
    assert(i+1<argc);
    if (argvs[i]=="-q" || argvs[i]=="-query_name") {
      args["query_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-t" || argvs[i]=="-target_name") {
      args["target_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-o" || argvs[i]=="-observe_file") {
      args["observe_file"] = argvs[i+1];
    }
    else if (argvs[i]=="-p" || argvs[i]=="-provenance_file") {
      args["provenance_file"] = argvs[i+1];
    }
    else if (argvs[i]=="-s" || argvs[i]=="-save") {
      args["save"] = argvs[i+1];
    }
    else if (argvs[i]=="-i" || argvs[i]=="-influence_name") {
      args["influence_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-r" || argvs[i]=="-round") {
      args["round"] = argvs[i+1];
    }
    else if (argvs[i]=="-d" || argvs[i]=="delta") {
      args["delta"] = argvs[i+1];
    }
  }

  // assert(args.find("query_name")!=args.end());
  // assert(args.find("target_name")!=args.end());
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

  printLiterals(mln);

  if (args.find("query_name")!=args.end()) {
    cliqueTest(mln, args["query_name"]);
    probabilityQuery(mln, stoi(args["round"]), args["query_name"]);
    // gibbsTest(mln, 10000, args["query_name"]);
    // varianceTest(mln, args["query_name"]);
  }

  if (args.find("save")!=args.end()) {
    saveToFile(mln, args["save"]);
  }

  // boxplotTestSave(mln, "./data/record/cancer8_2.txt", 100);

  if (args.find("influence_name")!=args.end()) {
    influenceQuery(mln, args["influence_name"], stoi(args["round"]), stod(args["delta"]));
    // save3DPlot(mln, "./data/record/test.txt", args["influence_name"], stoi(args["round"]), stod(args["round"]));
  }

  if (args.find("target_name")!=args.end()) {
    // influenceTest(mln, args["target_name"], 10);
  }
}