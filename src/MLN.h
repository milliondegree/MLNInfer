#ifndef _MLN_H
#define _MLN_H

#define MAXTRIES 100
#define MAXFLIPS 10
#define TARGET 0.00001
#define PTARGET 1.0
#define NOISE 0.5
#define SARATIO 0.5
#define SATEMPERATURE 0.01
#define DROP_RATE 0.98
#define DEFAULT_ROUNDS 500
#define MAX_VISIT 1000
#define NUM_THREADS 4


#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <queue>
#include <thread>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <math.h>
#include <float.h>
#include <assert.h>
#include "Clique.h"
#include "Load.h"
#include "CProvGraph.h"

using namespace std;


enum Regulation {
  l0, l2, l1, none
};


class MLN {
public:
  MLN();
  MLN(string prov, map<string, double> prob);
  MLN(Load& load);

  string prov;
  vector<string> provs;
  vector<Clique> cliques;
  unordered_set<string> obs;
  unordered_set<string> queries;
  map<string, double> prob;
  map<string, vector<int> > c_map;
  unordered_map<string, unordered_map<string, double> > pd;

  CProvGraph provG;

  friend class Parser;
  friend class Grader;

  MLN operator + (const MLN& mln );

  void clear();
  void setProperty(string prov, map<string, double> prob);
  void setProv(string prov);
  void setProvs(vector<string> provs);
  void setCliques(vector<Clique> cliques);
  void setObs(unordered_set<string> obs);
  void setQueries(unordered_set<string> queries);
  void setProb(map<string, double> prob);
  void setCMap(map<string, vector<int> > c_map);
  void setPd(unordered_map<string, unordered_map<string, double> > pd);

  void merge();

  void removeRedundant(string& prov);
  void build(string prov);

  int getNumberOfCliques();
  int numberCliques(string literal);
  vector<Clique> getCliques();
  vector<Clique> getCliques(string literal);
  map<string, vector<int> > getCMap();
  map<string, double> getProb();
  unordered_set<string> getObsLiterals();
  unordered_set<string> getQueryLiterals();
  MLN getMinimalMLN(string& query);
  MLN getConnectedMLN(string& query);

  void setObsProb(string str, double prob);

  void gibbsSampling(int round);
  void gibbsSampling_v2(int round);
  void gibbsSampling_v3(int round);
  void gibbsSampling_v4(int round, string query);
  void gibbsSampling_vp(int round, string query, double delta);

  double estimatedProb(string query);
  
  double queryProb(string query);

  void mcsat(int round, string query);
  void pmcsat(int round, string query);

  void naiveBPInfluence(string query, string infl);
  void naiveLBPInfluence(string query, string infl);

  void naiveBeliefPropagation(string query);
  void nonProbabilisticBeliefPropagation(string query);
  void advanceBeliefPropagation(string query);
  void loopyBeliefPropagation(string query);
  int loopyBeliefPropagation();
  void pLoopyBeliefPropagation(string query);
  void loopyBeliefPropagationMCS(string query, int rounds);

  void loopyBeliefPropagationWithProv(string query);
  int loopyBeliefPropagationWithProv();
  int loopyBeliefPropagationWithCoarseProv();

  map<string, vector<double> > getAllProbs(int round, int times);
  unordered_map<string, double> getInfluence(string query);

  string toString();
  void saveToFile(ofstream& file);
  void saveToDotFile(ofstream& file);

  MLN approximateSubGraph(string query, double target, int rounds, double delta, int max_clique_number=100);
  MLN approximateSubGraph_v2(string query, double target, int rounds, double delta, int max_clique_number);
  MLN approximateSubGraph_v3(string query, double target, int rounds, double delta, int max_clique_number);

  bool connectivity(string& query);

  double computeCrossEntropyLoss(const string& target, const vector<string>& query_names, map<string, double>& original_probs, Regulation regulation, double theta);
  map<string, double> computeGradients(const string& target, const vector<string>& query_names, map<string, double>& original_probs, Regulation regulation, double theta, int rule_name);
  void updateObs(vector<pair<string, double> >& gradients, double delta, size_t number_of_changes);

  void buildCliqueMaps();

  ~MLN();

private:

  void dfsSearch(unordered_set<string>& valid_unknown, vector<bool>& visited, string& query, bool stopObserved=true);
  void sampleSAT(unordered_map<string, int>& state, unordered_set<int>& c_idx, int maxFlips, int maxTries, double target, double p);
  void maxWalkSAT(unordered_map<string, int>& state, unordered_set<int>& c_idx, int maxFlips, int maxTries, double target, double p);
  void pSampleSAT(unordered_map<string, double>& state, unordered_set<int>& c_idx, int maxFlips, int maxTries, double target, double p);

  // double getCost(unordered_map<string, int>& state, unordered_set<int>& c_idx);
  double getCost(unordered_map<string, int>& state, unordered_set<int>& c_idx, string& mode);
  double pGetCost(unordered_map<string, double>& state, unordered_set<int>& c_idx, string& mode);
  // double getSampleSATCost(unordered_map<string, int>& state, unordered_set<int>& c_idx);
  string randomPick(Clique& c);
  string lowestPick(Clique& c, unordered_map<string, int>& state, unordered_set<int>& c_idx, string& mode);
  string optimalPick(unordered_map<string, int>& state, unordered_set<int>& c_idx, string& mode);
  string ssPick(unordered_map<string, int>& state, unordered_set<int>& c_idx, string& mode);
  string pSsPick(unordered_map<string, double>& state, unordered_set<int>& c_idx, string& mode);

  
};



class Parser {
public:
  Parser();
  Parser(int rule_name);

  void parseProvenance(MLN& mln);
  void extendCliques(MLN& mln);

  static string extractName(string& s);
  static bool isRuleName(string& s);
  static bool isVariable(string& s);
  // void extendR1Cliques(MLN& mln);

  ~Parser();

private:
  void parseRuleHead(MLN& mln, string& prov, int& i);
  vector<vector<string> > parseRules(MLN& mln, string& prov, int& i);
  vector<string> parseRule(MLN& mln, string& prov, int& i);
  vector<string> parseRuleBody(MLN& mln, string& prov, int& i);
  bool findIn(MLN& mln, Clique& c);
  int rule_name_obs;
};



class Grader {
public:
  Grader();

  void computeGradients_v2(MLN& mln, string query, int round, double delta);
  void computeGradient(MLN& mln, string query, string infl, int round, double delta, string mode);
  void computeGradient(MLN& mln, vector<string>& query_names, string infl, int round, double delta);
  unordered_set<string> getValidObservedTuples(MLN& mln, string query);

  ~Grader();

private:
  string target;
  void dfsSearch(MLN& mln, unordered_set<string>& valid_obs, vector<bool>& visited, string& query);
};



#endif