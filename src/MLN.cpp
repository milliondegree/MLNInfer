#include "MLN.h"


MLN::MLN() {
}


MLN::MLN(string prov, map<string, double> prob) {
  this->prov = prov;
  this->prob = prob;
  // build(prov);
}


void MLN::clear() {
  this->cliques = vector<Clique> ();
  this->c_map = map<string, vector<int>> ();
  this->obs = set<string> ();
  this->queries = set<string> ();
  this->prob = map<string, double> ();
  this->prov = string ();
}


void MLN::setProperty(string prov, map<string, double> prob) {
  clear();
  this->prov = prov;
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


void MLN::gibbsSampling(int round) {
  map<string, int> samples;
  for (string query : this->queries) {
    samples[query] = 0;
  }

  map<string, int> assignments;
  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0,1.0);
  for (string query : this->queries) {
    double prand = distribution(generator);
    if (prand<0.5) {
      assignments[query] = 1;
    }
    else {
      assignments[query] = 0;
    }
  }

  for (int r=0; r<round; r++) {
    for (string query : this->queries) {
      vector<int> indices = this->c_map[query];
      vector<double> potentials_0;
      vector<double> potentials_1;
      for (int i : indices) {
        Clique c = this->cliques[i];
        map<string, int> truth;
        vector<string> literals = c.getLiterals();
        for (string literal : literals) {
          if (this->obs.find(literal)!=this->obs.end()) {
            double prand = distribution(generator);
            if (prand<this->prob[literal]) {
              truth[literal] = 1;
            }
            else {
              truth[literal] = 0;
            }
          }
          else {
            truth[literal] = assignments[literal];
          }
        }
        truth[query] = 1;
        potentials_1.push_back(c.getPotential(truth));
        truth[query] = 0;
        potentials_0.push_back(c.getPotential(truth));
      }
      double sum_1 = 0;
      for (double p : potentials_1) {
        sum_1 += p;
      }
      double sum_0 = 0;
      for (double p : potentials_0) {
        sum_0 += p;
      }
      double exp_1 = exp(sum_1);
      double exp_0 = exp(sum_0);
      double p = exp_1 / (exp_1+exp_0);
      double prand = distribution(generator);
      if (prand<p) {
        samples[query] += 1;
        assignments[query] = 1;
      }
      else {
        assignments[query] = 0;
      }
    }
  }

  for (string query : this->queries) {
    this->prob[query] = ((double)samples[query]) / round;
  }
}



void MLN::gibbsSampling_v2(int round) {
  map<string, int> samples;
  for (string query : this->queries) {
    samples[query] = 0;
  }

  map<string, int> assignments;
  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0,1.0);
  for (string query : this->queries) {
    double prand = distribution(generator);
    if (prand<0.5) {
      assignments[query] = 1;
    }
    else {
      assignments[query] = 0;
    }
  }

  for (int r=0; r<round; r++) {
    for (string query : this->queries) {
      vector<int> indices = this->c_map[query];
      vector<double> potentials_0;
      vector<double> potentials_1;
      for (int i : indices) {
        Clique c = this->cliques[i];
        map<string, double> truth;
        vector<string> literals = c.getLiterals();
        for (string literal : literals) {
          if (this->obs.find(literal)!=this->obs.end()) {
            truth[literal] = this->prob[literal];
          }
          else {
            truth[literal] = (double)assignments[literal];
          }
        }
        truth[query] = 1.0;
        potentials_1.push_back(c.getPotential(truth));
        truth[query] = 0.0;
        potentials_0.push_back(c.getPotential(truth));
      }
      double sum_1 = 0;
      for (double p : potentials_1) {
        sum_1 += p;
      }
      double sum_0 = 0;
      for (double p : potentials_0) {
        sum_0 += p;
      }
      double exp_1 = exp(sum_1);
      double exp_0 = exp(sum_0);
      double p = exp_1 / (exp_1+exp_0);
      double prand = distribution(generator);
      if (prand<p) {
        samples[query] += 1;
        assignments[query] = 1;
      }
      else {
        assignments[query] = 0;
      }
    }
  }

  for (string query : this->queries) {
    this->prob[query] = ((double)samples[query]) / round;
  }
}



double MLN::queryProb(string query) {
  assert(this->prob.find(query)!=this->prob.end());
  return this->prob[query];
}


MLN::~MLN() {
}