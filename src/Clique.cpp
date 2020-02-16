#include "Clique.h"


Clique::Clique() {
}

Clique::Clique(string rule, double weight, string r_head, vector<string> r_body) {
  this->rule_name = rule;
  this->rule_weight = weight;
  this->rule_head = r_head;
  this->rule_body = r_body;
  this->literals.push_back(this->rule_head);
  for (string body : this->rule_body) {
    this->literals.push_back(body);
  }
}


double Clique::getPotential(map<string, int> truth) {
  assert(truth.find(this->rule_head)!=truth.end());
  int rule_head_value = (1-truth[this->rule_head]);
  int rule_body_value = 1;
  for (string body : this->rule_body) {
    assert(truth.find(body)!=truth.end());
    rule_body_value &= truth[body];
  }
  int bool_res = rule_head_value | rule_body_value;
  double res = this->rule_weight * bool_res;
  return res;
}


void Clique::printClique() {
  cout << this->rule_name << ' ' << this->rule_head << " :- ";
  for (size_t i=0; i<this->rule_body.size(); i++) {
    if (i!=this->rule_body.size()-1) {
      cout << this->rule_body[i] << ", ";
    }
    else {
      cout << this->rule_body[i] << endl;
    }
  }
}


vector<string> Clique::getLiterals() {
  return this->literals;
}

Clique::~Clique() {
}