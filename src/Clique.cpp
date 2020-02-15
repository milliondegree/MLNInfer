#include "Clique.h"


Clique::Clique() {
}

Clique::Clique(string rule, double weight, string r_head, vector<string> r_body) {
  this->rule_name = rule;
  this->rule_weight = weight;
  this->rule_head = r_head;
  this->rule_body = r_body;
  /*
  if (obs.find(rule_head)==obs.end()) {
    this->literals[rule_head] = -1;
  }
  else {
    this->literals[rule_head] = 1;
  }
  for (string body : this->rule_body) {
    if (obs.find(body)==obs.end()) {
      this->literals[body] = -1;
    }
    else {
      this->literals[body] = 1;
    }
  }
  */
}

/*
void Clique::setValue(string literal, int value) {
  assert(value==1 || value==0);
  assert(this->literals.find(literal)!=this->literals.end());
  this->literals[literal] = value;
}



double Clique::getPotential() {
  int body = 1;
  for (string literal : this->rule_body) {
    body &= this->literals[literal];
  }
  int whole = body | this->literals[this->rule_head];
  double res = this->rule_weight * whole;
  return res;
}
*/


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

Clique::~Clique() {
}