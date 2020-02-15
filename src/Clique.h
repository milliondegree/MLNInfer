#ifndef _CLIQUE_H
#define _CLIQUE_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <assert.h>
using namespace std;

class Clique {
public:
  Clique();
  Clique(string rule, double weight, string r_head, vector<string> r_body);

  // double getPotential();
  // void setValue(string literal, int value);

  ~Clique();

private:
  string rule_name;
  double rule_weight;
  map<string, int> literals;
  string rule_head;
  vector<string> rule_body;
};


#endif