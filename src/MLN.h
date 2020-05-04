#ifndef _MLN_H
#define _MLN_H

#define MAXTRIES 100
#define MAXFLIPS 10
#define TARGET 0.00001
#define PTARGET 1.0
#define NOISE 0.5
#define SARATIO 0.5
#define SATEMPERATURE 80


#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <math.h>
#include <float.h>
#include <assert.h>
#include "Clique.h"
#include "Load.h"
using namespace std;


class MLN {
public:
  MLN();
  MLN(string prov, map<string, double> prob);
  MLN(Load& load);
  void clear();
  void setProperty(string prov, map<string, double> prob);

  friend class Parser;
  friend class Grader;

  void removeRedundant(string& prov);
  void build(string prov);

  int numberCliques(string literal);
  vector<Clique> getCliques(string literal);
  unordered_set<string> getObsLiterals();
  unordered_set<string> getQueryLiterals();

  void setObsProb(string str, double prob);

  void gibbsSampling(int round);
  void gibbsSampling_v2(int round);
  void gibbsSampling_v3(int round);
  void gibbsSampling_v4(int round, string query);
  double queryProb(string query);

  void mcsat(int round, string query);
  void pmcsat(int round, string query);

  map<string, vector<double>> getAllProbs(int round, int times);
  unordered_map<string, double> getInfluence(string query);

  void saveToFile(ofstream& file);

  ~MLN();

private:
  string prov;
  vector<string> provs;
  vector<Clique> cliques;
  unordered_set<string> obs;
  unordered_set<string> queries;
  map<string, double> prob;
  map<string, string> sames;
  map<string, vector<int>> c_map;
  unordered_map<string, unordered_map<string, double>> pd;

  void dfsSearch(unordered_set<string>& valid_unknown, vector<bool>& visited, string& query);
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

  void parseProvenance(MLN& mln);
  void extendCliques(MLN& mln);
  void extendR1Cliques(MLN& mln);

  ~Parser();

private:
  void parseRuleHead(MLN& mln, string& prov, int& i);
  vector<vector<string>> parseRules(MLN& mln, string& prov, int& i);
  vector<string> parseRule(MLN& mln, string& prov, int& i);
  vector<string> parseRuleBody(MLN& mln, string& prov, int& i);
  bool findIn(MLN& mln, Clique& c);
  string extractName(string& s);
};



class Grader {
public:
  Grader();

  void computeGradients(MLN& mln, string query, int round);
  void computeGradients_v2(MLN& mln, string query, int round, double delta);
  void computeGradients_mcsat(MLN& mln, string query, int round, double delta);
  void computeGradient(MLN& mln, string query, string infl, int round, double delta, string mode);
  unordered_set<string> getValidObservedTuples(MLN& mln, string query);

  ~Grader();

private:
  string target;
  void dfsBuild(MLN& mln, vector<bool>& visited, string& query, double grad);
  void dfsSearch(MLN& mln, unordered_set<string>& valid_obs, vector<bool>& visited, string& query);
};


#endif