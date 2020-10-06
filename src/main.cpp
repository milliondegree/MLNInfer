
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


bool sortByValue(const pair<string, double>& p1, const pair<string, double>& p2) {
  return p1.second > p2.second;
}


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


double calcStdVar(vector<double> values) {
  double mean = 0;
  for (int i=0; i<values.size(); i++) {
    mean += values[i];
  }
  mean /= values.size();
  double var = 0;
  for (int i=0; i<values.size(); i++) {
    var += abs(values[i]-mean);
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
  /*
  clock_t t3 = clock();
  mln.setProperty(l.getProv(), l.getProb());
  clock_t t4 = clock();
  cout << "original parsing time: " << ((double)(t4-t3))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << endl;
  */
}


void printLiterals(MLN mln) {
  // print out the observed literals and unknown literals after parsing
  unordered_set<string> obs = mln.getObsLiterals();
  unordered_set<string> queries = mln.getQueryLiterals();
  cout << "observed tuples: ";
  for (string s : obs) {
    cout << s << ' ';
  }
  // cout << endl;
  // cout << "unknown tuples: ";
  // for (string s : queries) {
  //   cout << s << ' ';
  // }
  cout << endl;
  cout << "size of observed tuples: " << obs.size() << endl;
  cout << "size of unobserved tuples: " << queries.size() << endl;
  cout << endl;
  // for (Clique c : mln.getCliques()) {
  //   cout << c.toString() << endl;
  // }
  // cout << endl;
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
  // mln.gibbsSampling_v3(100000);
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


void probabilityQuery_mcsat(MLN& mln, int round, string query_name) {
  clock_t t1 = clock();
  mln.mcsat(round, query_name);
  clock_t t2 = clock();
  cout << "mcsat: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  double prob = mln.queryProb(query_name);
  cout << "probability of " << query_name << " is " << prob << endl;
}


void probabilityQuery(MLN& mln, int round, string query_name, string mode, double delta) {
  clock_t t1 = clock();
  // mln.gibbsSampling_v3(round);
  if (mode=="gibbs") {
    mln.gibbsSampling_v4(round, query_name);
  }
  else if (mode=="pgibbs") {
    mln.gibbsSampling_vp(round, query_name, delta);
  }
  else if (mode=="mcsat"){
    mln.mcsat(round, query_name);
  }
  else if (mode=="pmcsat") {
    mln.pmcsat(round, query_name);
  }
  clock_t t2 = clock();
  cout << mode+" sample time: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  double prob = mln.queryProb(query_name);
  cout << "probability of " << query_name << " is " << prob << endl;
  cout << endl;
}


void probabilityQueryAll(MLN& mln, int round, string mode) {
  clock_t t1 = clock();
  // mln.gibbsSampling_v3(round);
  unordered_set<string> queries = mln.getQueryLiterals();
  for (string query : queries) {
    MLN mmln = mln.getMinimalMLN(query);
    printLiterals(mmln);
    if (mode=="gibbs") {
      mmln.gibbsSampling_v4(round, query);
    }
    else if (mode=="pgibbs") {
      mmln.gibbsSampling_vp(round, query, 0.000001);
    }
    else if (mode=="mcsat"){
      mmln.mcsat(round, query);
    }
    else if (mode=="pmcsat") {
      mmln.pmcsat(round, query);
    }
    double prob = mmln.queryProb(query);
    cout << "probability of " << query << " is " << prob << endl;
  }
  clock_t t2 = clock();
  cout << mode+" sample time: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << endl;
}


void verifyProb(MLN& mln) {
  unordered_set<string> queries = mln.getQueryLiterals();
  map<string, double> probs = mln.getProb();
  for (string query : queries) {
    double p = mln.estimatedProb(query);
    cout << query << ": " << abs(p-probs[query]) << endl;
  }
}


void varianceTest(MLN& mln, int round, string query_name) {
  /*
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
  */
  clock_t t1 = clock();
  vector<double> p1;
  for (int i=0; i<50; i++) {
    mln.gibbsSampling_v4(round, query_name);
    p1.push_back(mln.queryProb(query_name));
  }
  clock_t t2 = clock();
  cout << "gibbs sample time: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  double var1 = calcStdVar(p1);
  cout << "standard variance of gibbs sampling: " << sqrt(var1) << endl;
  /*
  clock_t t3 = clock();
  vector<double> p2;
  for (int i=0; i<100; i++) {
    mln.mcsat(1000, query_name);
    p2.push_back(mln.queryProb(query_name));
  }
  clock_t t4 = clock();
  cout << "mcsat time: " << ((double)(t4-t3))/CLOCKS_PER_SEC << " seconds" << endl;
  double var2 = calcVar(p2);
  cout << "standard variance of masat: " << sqrt(var2) << endl;
  */

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


void boxplotTestSave(MLN& mln, string query_name, string file_name, int times) {
  ofstream file;
  file.open(file_name);
  vector<int> rounds = {1, 2, 4, 6, 8, 10};
  unordered_set<string> queries = mln.getQueryLiterals();
  for (int round : rounds) {
    // cout << round << endl;
    file << "rounds: " << round << ' ' << "times: " << times << endl;
    file << query_name << ": ";
    for (int i=0; i<times; i++) {
      cout << round << ' ' << i << endl;
      // mln.gibbsSampling_v4(round, query_name);
      mln.gibbsSampling_vp(round, query_name, 1e-10);
      double prob = mln.queryProb(query_name);
      file << prob << ' ';
    }
    file << endl;
  }
  file.close();
}


/*
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
*/


void influenceTest(MLN& mln, string& query, string& infl, int round) {
  Influence influence (mln);
  double infValue = influence.influenceQuery(mln, query, infl);
  cout << "influence of " << infl << " on " << query << " is " << infValue << endl;
  // influence.computePartialDerivatives(mln, infl);
  // double pd = influence.getPartialDeriv(query, infl);
  // vector<vector<double>> pds = influence.getPartialDeriv();
  // vector<string> queries = influence.getQueries();
  // cout << endl;
  // for (int i=0; i<queries.size(); i++) {
  //   cout << queries[i] << ' ';
  // }
  // cout << endl;
  // for (int i=0; i<pds.size(); i++) {
  //   for (int j=0; j<pds[i].size(); j++) {
  //     cout << pds[i][j] << ' ';
  //   }
  //   cout << endl;
  // }
  // cout << endl;
}


void influenceQuery(MLN& mln, string query, int round, double delta) {
  cout << "compute influence of " << query << endl;
  Grader grader;
  clock_t t1 = clock();
  grader.computeGradients_v2(mln, query, round, delta);
  // grader.computeGradients(mln, query, round);
  clock_t t2 = clock();
  cout << "influence compute time (gibbs sample): " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  unordered_map<string, double> influs = mln.getInfluence(query);
  for (auto it=influs.begin(); it!=influs.end(); it++) {
    cout << it->first << ' ' << it->second << endl;
  }
  cout << endl;
}


void influenceQuery_mcsat(MLN& mln, string query, int round, double delta) {
  cout << "compute influence of " << query << endl;
  Grader grader;
  clock_t t1 = clock();
  grader.computeGradients_mcsat(mln, query, round, delta);
  // grader.computeGradients(mln, query, round);
  clock_t t2 = clock();
  cout << "influence compute time (mcsat): " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  unordered_map<string, double> influs = mln.getInfluence(query);
  for (auto it=influs.begin(); it!=influs.end(); it++) {
    cout << it->first << ' ' << it->second << endl;
  }
  cout << endl;
}


void influenceQuery(MLN& mln, string query, string infl, int round, double delta, string mode) {
  clock_t t1, t2;
  double infValue;
  if (mode=="equations") {
    t1 = clock();
    Influence influence (mln);
    infValue = influence.influenceQuery(mln, query, infl);
    t2 = clock();
  }
  else {
    t1 = clock();
    Grader grader;
    grader.computeGradient(mln, query, infl, round, delta, mode);
    infValue = mln.getInfluence(query)[infl];
    t2 = clock();
  }
  cout << "mode " << mode << ": influence compute time (" << mode << "): " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << "influence of " << infl << " on " << query << " is " << infValue << endl;
  cout << endl;
}


void influenceQueryAll(MLN& mln, string query, int round, double delta, string mode) {
  Grader grader;
  // unordered_set<string> observedTuples = grader.getValidObservedTuples(mln, query);
  unordered_set<string> observedTuples = mln.getObsLiterals();
  clock_t t1 = clock();
  cout << "mode " << mode << endl;
  vector<pair<string, double>> vp;
  if (mode=="equations") {
    Influence influence (mln);
    for (string observed : observedTuples) {
      double infValue = influence.influenceQuery(mln, query, observed);
      vp.push_back(pair<string, double>({observed, infValue}));
    }
    sort(vp.begin(), vp.end(), sortByValue);
    for (auto it=vp.begin(); it<vp.end(); it++) {
      cout << "influence of " << it->first << " on " << query << " is " << it->second << endl;
    }
  }
  else {
    for (string observed : observedTuples) {
      grader.computeGradient(mln, query, observed, round, delta, mode);
      double influs = mln.getInfluence(query)[observed];
      vp.push_back(pair<string, double>({observed, influs}));
    }
    sort(vp.begin(), vp.end(), sortByValue);
    for (auto it=vp.begin(); it<vp.end(); it++) {
      cout << "influence of " << it->first << " on " << query << " is " << it->second << endl;
    }
  }
  clock_t t2 = clock();
  cout << "influence compute time (" << mode << "): " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  cout << endl;
}


void setDefaultArgs(unordered_map<string, string>& args) {
  args["observe_file"] = "./data/observe/smokeTest.db";
  args["provenance_file"] = "./data/prov/cancer2.txt";
  args["round"] = "100000";
  args["delta"] = "0.01";
  args["approx"] = "1e-7";
  args["equation"] = "0";
  args["mode"] = "pgibbs";
}


void save3DPlot(MLN& mln, string file_name, string query, int round, double delta) {
  ofstream file;
  file.open(file_name);
  Grader grader;
  vector<double> smokes;
  vector<double> friends;
  vector<double> smokes_inf;
  vector<double> friends_inf;
  clock_t t1 = clock();
  for (int i=1; i<10; i++) {
    for (int j=1; j<10; j++) {
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
  clock_t t2 = clock();
  cout << "time cost: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
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
    else if (argvs[i]=="-a") {
      args["approx"] = argvs[i+1];
    }
    else if (argvs[i]=="-d" || argvs[i]=="-delta") {
      args["delta"] = argvs[i+1];
    }
    else if (argvs[i]=="-e" || argvs[i]=="-equation") {
      args["equation"] = argvs[i+1];
    }
    else if (argvs[i]=="-m" || argvs[i]=="-mode") {
      args["mode"] = argvs[i+1];
    }
  }

  Load l(args["provenance_file"], args["observe_file"]);
  vector<string> prov = l.getProv();

  // printObservation(l);

  MLN mln(l);
  Parser parser;
  parser.parseProvenance(mln);
  // parser.extendCliques(mln);
  // parser.extendR1Cliques(mln);

  /*
  map<string, double> prob = mln.getProb();
  for (auto it : prob) {
    cout << it.first << ' ' << it.second << endl;
  }
  */

  printLiterals(mln);
  
  MLN mmln;

  if (args.find("query_name")!=args.end()) {
    if (args["query_name"]!="all") {
      mmln = mln.getMinimalMLN(args["query_name"]);
      // mmln = mln;
      printLiterals(mmln);
      probabilityQuery(mmln, stoi(args["round"]), args["query_name"], args["mode"], stod(args["approx"]));
      // verifyProb(mmln);
    }
    else {
      // probabilityQueryAll(mln, stoi(args["round"]), args["mode"]);
      unordered_set<string> queries = mln.getQueryLiterals();
      for (string query : queries) {
        mmln = mln.getMinimalMLN(query);
        probabilityQuery(mmln, stoi(args["round"]), query, args["mode"], stod(args["approx"]));
        unordered_set<string> obs = mmln.getObsLiterals();
        string obs_tuple = *(obs.begin());
        influenceQuery(mmln, query, obs_tuple, stoi(args["round"]), stod(args["delta"]), "equations");
      }
    }
  }

  if (args.find("save")!=args.end()) {
    cout << mmln.toString() << endl;
    saveToFile(mln, args["save"]);
  }

  // boxplotTestSave(mmln, args["query_name"], "./data/hypertext-class/sample3/record/topic_Faculty_8_pgibbs.txt", 100);
  // save3DPlot(mln, "./data/record/test1.txt", args["query_name"], stoi(args["round"]), stod(args["delta"]));

  if (args.find("influence_name")!=args.end()) {
    if (args["influence_name"]=="all") {
      if (stoi(args["equation"])) {
        influenceQueryAll(mmln, args["query_name"], stoi(args["round"]), stod(args["delta"]), "equations");
      }
      else {
        influenceQueryAll(mmln, args["query_name"], stoi(args["round"]), stod(args["delta"]), "pgibbs");
      }
    }
    else {
      if (stoi(args["equation"])) {
        influenceQuery(mmln, args["query_name"], args["influence_name"], stoi(args["round"]), stod(args["delta"]), "equations");
      }
      else {
        influenceQuery(mmln, args["query_name"], args["influence_name"], stoi(args["round"]), stod(args["delta"]), "pgibbs");
      }
    }
  }
}