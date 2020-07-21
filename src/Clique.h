#ifndef _CLIQUE_H
#define _CLIQUE_H

#include <iostream> 
#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <assert.h>
#include <float.h>
using namespace std;


struct Literal {
  string name;
  bool nag;
  Literal(string literal_name, bool literal_nag) {
    name = literal_name;
    nag = literal_nag;
  }
  bool operator == (Literal& l) {
    return (name==l.name) & (nag==l.nag);
  }
};


class Clique {
public:
  Clique();
  // Clique(string rule, double weight);
  Clique(string rule_name, string rule_head, double weight);
  Clique(string rule, double weight, string r_head, vector<string> r_body);

  bool operator == (Clique& c);

  double getPotential(map<string, int>& truth);
  double getPotential(map<string, double>& truth);
  double getPotential(unordered_map<string, double>& truth);
  // double getPartialDerivative(map<string, double>& truth, string p_f, string p_v);
  bool satisifiablity(unordered_map<string, int>& truth);
  double satisifiablity(unordered_map<string, double>& truth);

  vector<string> getLiterals();
  vector<string> getRuleBody();
  string getRuleName();
  string getRuleHead();
  double getRuleWeight();
  double getCost(string& mode);
  double getMinCost();
  bool isHard();
  bool isSingular();
  bool isRuleHead(string& literal);
  bool isRuleBody(string& literal);
  bool isRuleName(string& literal);

  string toString();
  void printClique();
  void saveToFile(ofstream& file);

  ~Clique();

private:
  string rule_name;
  double rule_weight;
  string rule_head;
  vector<string> rule_body;
  vector<string> literals;
  vector<Literal> sliterals;

  void buildLiterals();
};


#endif