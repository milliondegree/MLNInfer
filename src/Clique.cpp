#include "Clique.h"


Clique::Clique() {
}


// Clique::Clique(string rule, double weight) {
//   this->rule_name = rule;
//   this->rule_weight = weight;
//   this->literals.push_back(rule);
//   buildLiterals();
// }


Clique::Clique(string rule_name, string rule_head, double weight) {
  this->rule_name = rule_name;
  this->rule_head = rule_head;
  this->rule_weight = weight;
  this->literals.push_back(rule_head);
  buildLiterals();
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
  buildLiterals();
}


bool Clique:: operator == (Clique& c) {
  return this->toString()==c.toString();
}


double Clique::getPotential(map<string, int>& truth) {
  if (this->literals.size()==1) {
    return this->rule_weight * truth[this->rule_head];
  }
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


double Clique::getPotential(map<string, double>& truth) {
  if (this->literals.size()==1) {
    return this->rule_weight * truth[this->rule_head];
  }
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


double Clique::getPotential(unordered_map<string, double>& truth) {
  if (this->literals.size()==1) {
    return this->rule_weight * truth[this->rule_head];
  }
  double p_head = 1 - truth[this->rule_head];
  if (labs(p_head)<1e-10) {
    return this->rule_weight;
  }
  double p_body = 1.0;
  for (string rule_b : this->rule_body) {
    if (labs(truth[rule_b])<1e-10) {
      return this->rule_weight;
    }
    p_body *= truth[rule_b];
  }
  double potential = 1.0 - p_head*p_body;
  return potential*this->rule_weight;
}


// double Clique::getPartialDerivative(map<string, double>& truth, string p_f, string p_v) {
//   assert(p_f!=p_v);
//   double res;
//   if (p_f==this->rule_head || p_v==this->rule_head) {
//     double k = this->rule_weight;
//     for (string body : this->rule_body) {
//       if (body!=p_v) {
//         k *= truth[body];
//       }
//     }
//     res = k * truth[p_f] * (1-truth[p_f]);
//   }
//   else {
//     // when f and v are all rule body
//     double k = this->rule_weight * (1-truth[this->rule_head]);
//     for (string body : this->rule_body) {
//       if (body!=p_f && body!=p_v) {
//         k *= truth[body];
//       }
//     }
//     res = k * truth[p_f] * (1-truth[p_f]);
//   }
//   return res;
// }



bool Clique::satisifiablity(unordered_map<string, int>& truth) {
  bool res = false;
  for (Literal& l : this->sliterals) {
    string lname = l.name;
    if (l.nag) {
      if (!truth[lname]) {
        res = true;
        break;
      }
    }
    else {
      if (truth[lname]) {
        res = true;
        break;
      }
    }
  }
  if (this->rule_weight<0) {
    res = !res;
  }
  return res;
}


double Clique::satisifiablity(unordered_map<string, double>& truth) {
  double res = 1.0;
  for (Literal& l : this->sliterals) {
    if (l.nag) {
      res *= truth[l.name];
    }
    else {
      res *= 1-truth[l.name];
    }
  }
  res = 1-res;
  // if (this->rule_weight<0) {
  //   res = (1-res);
  // }
  return res;
}


string Clique::toString() {
  string res = "";
  if (this->literals.size()==1) {
    res = res + rule_name + " " + to_string(rule_weight) + " " + rule_head;
  }
  else {
    res += rule_name + " " + to_string(rule_weight) + " " + rule_head + " :- ";
    for (int i=1; i<literals.size(); i++) {
      res += literals[i];
      if (i!=literals.size()-1) {
        res += ", ";
      }
    }
  }
  return res;
}


void Clique::printClique() {
  if (this->literals.size()==1) {
    cout << this->rule_name << ' ' << this->rule_weight << ' ' << this->rule_head << endl;
    return;
  }
  cout << this->rule_name << ' ' << this->rule_weight << ' ' << this->rule_head << " :- ";
  for (size_t i=0; i<this->rule_body.size(); i++) {
    if (i!=this->rule_body.size()-1) {
      cout << this->rule_body[i] << ", ";
    }
    else {
      cout << this->rule_body[i] << endl;
    }
  }
}


void Clique::saveToFile(ofstream& file) {
  if (this->literals.size()==1) {
    file << "clique: ";
    file << this->rule_name << endl;
    return;
  }
  file << "clique: ";
  file << this->rule_name << ' ' << this->rule_head << ' ';
  for (int i=0; i<this->rule_body.size(); i++) {
    file << this->rule_body[i] << ' ';
  }
  file << endl;
}


vector<string> Clique::getLiterals() {
  return this->literals;
}


vector<string> Clique::getRuleBody() {
  return this->rule_body;
}


string Clique::getRuleName() {
  return this->rule_name;
}


string Clique::getRuleHead() {
  return this->rule_head;
}


double Clique::getRuleWeight() {
  return this->rule_weight;
}


double Clique::getCost(string& mode) {
  double res = 0.0;
  if (mode=="mws") {
    res = labs(this->rule_weight);
  }
  else if (mode=="ss") {
    res = 1.0;
  }
  return res;
}



bool Clique::isHard() {
  return labs(this->rule_weight-DBL_MAX)<1e-10;
}


bool Clique::isSingular() {
  return this->literals.size()==1;
}


bool Clique::isRuleHead(string& literal) {
  return literal==this->rule_head;
}


bool Clique::isRuleBody(string& literal) {
  for (string body : this->rule_body) {
    if (body==literal) {
      return true;
    }
  }
  return false;
}


bool Clique::isRuleName(string& literal) {
  assert(!this->isSingular());
  return literal==this->rule_name;
}


void Clique::buildLiterals() {
  if (this->literals.size()==1) {
    Literal l(this->literals[0], false);
    this->sliterals.push_back(l);
  }
  else {
    Literal head(this->rule_head, false);
    this->sliterals.push_back(head);
    for (string body : this->rule_body) {
      Literal lb(body, true);
      this->sliterals.push_back(lb);
    }
  }
}


Clique::~Clique() {
}