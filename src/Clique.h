#ifndef _CLIQUE_H
#define _CLIQUE_H

#include <iostream> 
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <assert.h>
using namespace std;

class Clique {
public:
  Clique();
  Clique(string rule, double weight, string r_head, vector<string> r_body);

  double getPotential(map<string, int>& truth);
  double getPotential(map<string, double>& truth);
  double getPotential(unordered_map<string, double>& truth);
  vector<string> getLiterals();

  void printClique();

  ~Clique();

private:
  string rule_name;
  double rule_weight;
  string rule_head;
  vector<string> rule_body;
  vector<string> literals;
};


#endif