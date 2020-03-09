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
using namespace std;

class Clique {
public:
  Clique();
  Clique(string rule, double weight, string r_head, vector<string> r_body);

  double getPotential(map<string, int>& truth);
  double getPotential(map<string, double>& truth);
  double getPotential(unordered_map<string, double>& truth);
  double getPartialDerivative(unordered_map<string, double>& truth, string p_f, string p_v);
  vector<string> getLiterals();

  void printClique();
  void saveToFile(ofstream& file);

  ~Clique();

private:
  string rule_name;
  double rule_weight;
  string rule_head;
  vector<string> rule_body;
  vector<string> literals;
};


#endif