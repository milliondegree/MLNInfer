#include "MLN.h"


MLN::MLN() {
}


MLN::MLN(string prov, map<string, double> prob) {
  this->prob = prob;
  build(prov);
}


void MLN::removeRedundant(string& prov) {
  while (true) {
    if (prov.length()>0 && prov[0]!='(') {
      return;
    }
    int count = 0;
    for (int i=0; i<prov.length(); i++) {
      if (prov[i]=='(') {
        count++;
      }
      else if (prov[i]==')') {
        count--;
        if (count==0) {
          if (i==prov.length()-1) {
            prov = prov.substr(1, prov.length()-2);
          }
          else {
            return;
          }
        }
      }
    }
  }
}


void MLN::build(string prov) {
  removeRedundant(prov);
  // find rule head
  int s = 0;
  while (s<prov.length() && prov[s]!='(') {
    s++;
  }
  string head = prov.substr(0, s);
  if (this->c_map.find(head)==this->c_map.end()) {
    this->c_map[head] = vector<int> ();
  }
  if (this->prob.find(head)==this->prob.end()) {
    this->queries.insert(head);
  }
  else {
    this->obs.insert(head);
  }

  // find vector of rules
  string neck_body = prov.substr(s, prov.length());
  removeRedundant(neck_body);
  vector<string> rules;
  int count = 0;
  s = 0;
  int e = s;
  while (e<neck_body.length()) {
    if (neck_body[e]=='(') {
      count++;
    }
    else if (neck_body[e]==')') {
      count--;
      if (count==0) {
        rules.push_back(neck_body.substr(s, e+1));
        if (e!=neck_body.length()-1) {
          assert(neck_body[e+1]=='+');
          s = e+2;
          e = s-1;
        }
      }
    }
    e++;
  }

  // parse each rule, get rule name and rule body
  for (string rule : rules) {
    removeRedundant(rule);
    // get rule name
    int rs = 0;
    while (rule[rs]!='@') {
      rs++;
    }
    string rule_name = rule.substr(0, rs);
    while (rs<rule.length() && rule[rs]!='(') {
      rs++;
    }
    // get vector of rule bodies
    string rule_body = rule.substr(rs, rule.length());
    removeRedundant(rule_body);
    vector<string> bodies;
    int rcount = 0;
    rs = 0;
    int re = rs;
    while (re<rule_body.length()) {
      if (rule_body[re]=='(') {
        rcount++;
      }
      else if (rule_body[re]==')') {
        rcount--;
      }
      else if (rule_body[re]=='*' && rcount==0) {
        bodies.push_back(rule_body.substr(rs, re));
        rs = re+1;
      }
      re++;
    }
    if (rs!=rule_body.length()) {
      bodies.push_back(rule_body.substr(rs, re));
    }

    //parse each body, get body list
    vector<string> r_body;
    for (string body : bodies) {
      removeRedundant(body);
      int bs = 0;
      while (bs<body.length() && body[bs]!='(') {
        bs++;
      }
      if (bs==body.length()) {
        r_body.push_back(body);
      }
      else {
        string body_name = body.substr(0, bs);
        r_body.push_back(body_name);
        build(body);
      }
    }

    // build clique given rule head, rule name, rule body list
    Clique c(rule_name, prob[rule_name], head, r_body);
    this->cliques.push_back(c);
    int index = this->cliques.size()-1;
    this->c_map[head].push_back(index);
    // build clique list of rule body
    for (string body : r_body) {
      if (this->c_map.find(body)==this->c_map.end()) {
        this->c_map[body] = vector<int> ();
      }
      if (this->prob.find(body)==this->prob.end()) {
        this->queries.insert(body);
      }
      else {
        this->obs.insert(body);
      }
      this->c_map[body].push_back(index);
    }
  }
}


int MLN::numberCliques(string literal) {
  return this->c_map[literal].size();
}


vector<Clique> MLN::getCliques(string literal) {
  vector<int> indices = this->c_map[literal];
  vector<Clique> res;
  for (int i : indices) {
    res.push_back(this->cliques[i]);
  }
  return res;
}


set<string> MLN::getObsLiterals() {
  return this->obs;
}


set<string> MLN::getQueryLiterals() {
  return this->queries;
}


MLN::~MLN() {
}