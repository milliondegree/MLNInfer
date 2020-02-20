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
  int rule_head_value = truth[this->rule_head];
  int rule_body_value = 1;
  for (string body : this->rule_body) {
    assert(truth.find(body)!=truth.end());
    rule_body_value &= truth[body];
  }
  rule_body_value = 1-rule_body_value;
  int bool_res = rule_head_value | rule_body_value;
  double res = this->rule_weight * bool_res;
  return res;
}


double Clique::getPotential(map<string, double> truth) {
  assert(truth.find(this->rule_head)!=truth.end());
  for (string rule_b : this->rule_body) {
    assert(truth.find(rule_b)!=truth.end());
  }
  double p_head = 1 - truth[this->rule_head];
  double p_body = 1.0;
  for (string rule_b : this->rule_body) {
    p_body *= truth[rule_b];
  }
  double potential = 1.0 - p_head*p_body;
  return potential*this->rule_weight;
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