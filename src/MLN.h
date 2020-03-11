#ifndef _MLN_H
#define _MLN_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <math.h>
#include <assert.h>
#include "Clique.h"
using namespace std;


class MLN {
public:
  MLN();
  MLN(string prov, map<string, double> prob);
  void clear();
  void setProperty(string prov, map<string, double> prob);

  friend class Parser;
  friend class Grader;

  void removeRedundant(string& prov);
  void build(string prov);

  int numberCliques(string literal);
  vector<Clique> getCliques(string literal);
  set<string> getObsLiterals();
  set<string> getQueryLiterals();

  void setObsProb(string str, double prob);

  void gibbsSampling(int round);
  void gibbsSampling_v2(int round);
  void gibbsSampling_v3(int round);
  double queryProb(string query);

  map<string, vector<double>> getAllProbs(int round, int times);

  unordered_map<string, double> getInfluence(string query);

  void saveToFile(ofstream& file);

  ~MLN();

private:
  string prov;
  vector<Clique> cliques;
  set<string> obs;
  set<string> queries;
  map<string, double> prob;
  map<string, vector<int>> c_map;
  unordered_map<string, unordered_map<string, double>> pd;
};



class Parser {
public:
  Parser();

  void parseProvenance(MLN& mln);

  ~Parser();

private:
  void parseRuleHead(MLN& mln, string& prov, int& i);
  vector<vector<string>> parseRules(MLN& mln, string& prov, int& i);
  vector<string> parseRule(MLN& mln, string& prov, int& i);
  vector<string> parseRuleBody(MLN& mln, string& prov, int& i);
};



class Grader {
public:
  Grader();

  void computeGradients(MLN& mln, string query);

  ~Grader();

private:
  string target;
  void dfsBuild(MLN& mln, vector<bool>& visited, string query, double grad);
};


#endif