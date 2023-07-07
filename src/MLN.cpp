#include "MLN.h"


MLN::MLN() {
}


MLN::MLN(string prov, map<string, double> prob) {
  this->prov = prov;
  this->prob = prob;
  // build(prov);
}


MLN::MLN(Load& load) {
  this->provs = load.getProv();
  this->prob = load.getProb();
}



MLN MLN::operator + (const MLN& mln) {
  for (string literal : mln.obs) {
    this->obs.insert(literal);
  }
  for (string literal : mln.queries) {
    this->queries.insert(literal);
  }
  for (Clique c : mln.cliques) {
    int ind = 0;
    for (; ind<this->cliques.size(); ind++) {
      if (this->cliques[ind]==c) break;
    }
    if (ind==this->cliques.size()) this->cliques.push_back(c);
  }
  map<string, vector<int> > new_cmap;
  for (int ci=0; ci<this->cliques.size(); ci++) {
    Clique c = this->cliques[ci];
    for (string literal : c.getLiterals()) {
      if (new_cmap.find(literal)==new_cmap.end()) {
        new_cmap[literal] = vector<int> ();
      }
      new_cmap[literal].push_back(ci);
    }
  }
  this->c_map = new_cmap;
  map<string, double> mln_prob = mln.prob;
  for (string literal : mln.obs) {
    this->prob[literal] = mln_prob[literal];
  }
  return *this;
}



void MLN::clear() {
  this->cliques = vector<Clique> ();
  this->c_map = map<string, vector<int> > ();
  this->obs = unordered_set<string> ();
  this->queries = unordered_set<string> ();
  this->prob = map<string, double> ();
  this->prov = string ();
}


void MLN::setProperty(string prov, map<string, double> prob) {
  clear();
  this->prov = prov;
  this->prob = prob;
  build(prov);
}


void MLN::setProv(string prov) {
  this->prov = prov;
}


void MLN::setProvs(vector<string> provs) {
  this->provs = provs;
}


void MLN::setCliques(vector<Clique> cliques) {
  this->cliques = cliques;
}


void MLN::setObs(unordered_set<string> obs) {
  this->obs = obs;
}


void MLN::setQueries(unordered_set<string> queries) {
  this->queries = queries;
}


void MLN::setProb(map<string, double> prob) {
  this->prob = prob;
}


void MLN::setCMap(map<string, vector<int> > c_map) {
  this->c_map = c_map;
}


void MLN::setPd(unordered_map<string, unordered_map<string, double> > pd) {
  this->pd = pd;
}


void MLN::merge() {
  vector<Clique> ncliques;
  map<string, vector<int> > nc_map;
  for (auto it=this->c_map.begin(); it!=c_map.end(); it++) {
    nc_map[it->first] = vector<int> ();
  }
  for (int i=0; i<this->cliques.size(); i++) {
    int target = -1;
    for (int j=i+1; j<this->cliques.size(); j++) {
      if (this->cliques[i].sameLiterals(this->cliques[j])) {
        target = j;
        break;
      }
    }
    if (target!=-1) {
      this->cliques[i] += this->cliques[target];
      this->prob[this->cliques[i].getRuleName()] = this->cliques[i].getRuleWeight();
      this->obs.insert(this->cliques[i].getRuleName());
      this->cliques.erase(this->cliques.begin()+target);
    }
    ncliques.push_back(this->cliques[i]);
    for (string s : this->cliques[i].getLiterals()) {
      nc_map[s].push_back(ncliques.size()-1);
    }
  }
  this->cliques = ncliques;
  this->c_map = nc_map;
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
  string neck_body = prov.substr(s, prov.length()-s);
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
        rules.push_back(neck_body.substr(s, e+1-s));
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
    string rule_body = rule.substr(rs, rule.length()-rs);
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
        bodies.push_back(rule_body.substr(rs, re-rs));
        rs = re+1;
      }
      re++;
    }
    if (rs!=rule_body.length()) {
      bodies.push_back(rule_body.substr(rs, re-rs));
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
    // build clique list of rule body@
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


int MLN::getNumberOfCliques() {
  return this->cliques.size();
}


int MLN::numberCliques(string literal) {
  return this->c_map[literal].size();
}


vector<Clique> MLN::getCliques() {
  return this->cliques;
}


vector<Clique> MLN::getCliques(string literal) {
  vector<int> indices = this->c_map[literal];
  vector<Clique> res;
  for (int i : indices) {
    res.push_back(this->cliques[i]);
  }
  return res;
}


map<string, vector<int> > MLN::getCMap() {
  return this->c_map;
}


map<string, double> MLN::getProb() {
  return this->prob;
}

unordered_set<string> MLN::getObsLiterals() {
  return this->obs;
}


unordered_set<string> MLN::getQueryLiterals() {
  return this->queries;
}


MLN MLN::getMinimalMLN(string& query) {
  MLN mmln;
  unordered_set<string> valid_unknown;
  vector<bool> visited (this->cliques.size(), false);
  dfsSearch(valid_unknown, visited, query);

  vector<Clique> mcliques;
  unordered_set<string> mobs;
  unordered_set<string> mqueries;
  map<string, vector<int> > mc_map;
  for (int i=0; i<this->cliques.size(); i++) {
    if (visited[i]) {
      mcliques.push_back(this->cliques[i]);
      // if (!this->cliques[i].isSingular()) {
      mobs.insert(this->cliques[i].getRuleName());
      // }
      for (string literal : this->cliques[i].getLiterals()) {
        if (mc_map.find(literal)==mc_map.end()) {
          mc_map[literal] = vector<int> ();
        }
        mc_map[literal].push_back(mcliques.size()-1);
        if (this->obs.find(literal)==this->obs.end()) {
          mqueries.insert(literal);
        }
        else {
          mobs.insert(literal);
        }
      }
    }
  }
  mmln.setCliques(mcliques);
  mmln.setObs(mobs);
  mmln.setQueries(mqueries);
  mmln.setCMap(mc_map);
  mmln.setProb(this->prob);
  return mmln;
}


MLN MLN::getConnectedMLN(string& query) {
  MLN mmln;
  unordered_set<string> valid_unknown;
  vector<bool> visited (this->cliques.size(), false);
  dfsSearch(valid_unknown, visited, query, false);

  vector<Clique> mcliques;
  unordered_set<string> mobs;
  unordered_set<string> mqueries;
  map<string, vector<int> > mc_map;
  for (int i=0; i<this->cliques.size(); i++) {
    if (visited[i]) {
      mcliques.push_back(this->cliques[i]);
      // if (!this->cliques[i].isSingular()) {
      mobs.insert(this->cliques[i].getRuleName());
      // }
      for (string literal : this->cliques[i].getLiterals()) {
        if (mc_map.find(literal)==mc_map.end()) {
          mc_map[literal] = vector<int> ();
        }
        mc_map[literal].push_back(mcliques.size()-1);
        if (this->obs.find(literal)==this->obs.end()) {
          mqueries.insert(literal);
        }
        else {
          mobs.insert(literal);
        }
      }
    }
  }
  mmln.setCliques(mcliques);
  mmln.setObs(mobs);
  mmln.setQueries(mqueries);
  mmln.setCMap(mc_map);
  mmln.setProb(this->prob);
  return mmln;
}


void MLN::setObsProb(string str, double p) {
  assert(this->obs.find(str)!=this->obs.end());
  this->prob[str] = p;
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
        unordered_map<string, int> truth;
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
        potentials_1.push_back(c.satisifiablity(truth)*this->prob[c.getRuleName()]);
        truth[query] = 0;
        potentials_0.push_back(c.satisifiablity(truth)*this->prob[c.getRuleName()]);
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
        if (round<10000 || r>1000) {
          samples[query] += 1;
        }
        assignments[query] = 1;
      }
      else {
        assignments[query] = 0;
      }
    }
  }
  for (string query : this->queries) {
    this->prob[query] = ((double)samples[query]) / (round<10000? round : round-1000);
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




void MLN::gibbsSampling_v3(int round) {
  unordered_map<string, int> samples;
  for (string query : this->queries) {
    samples[query] = 0;
  }

  unordered_map<string, int> assignments;
  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0,1.0);
  for (string query : this->queries) {
    if (this->prob.find(query)==this->prob.end()) {
      this->prob[query] = 0.5;
    }
  }

  vector<unordered_map<string, double> > truth_tables;
  for (Clique c : this->cliques) {
    unordered_map<string, double> m;
    vector<string> literals = c.getLiterals();
    for (string literal : literals) {
      if (this->obs.find(literal)==this->obs.end()) {
        m[literal] = (double)assignments[literal];
      }
      else {
        m[literal] = this->prob[literal];
      }
    }
    truth_tables.push_back(m);
  }

  for (int r=0; r<round; r++) {
    for (string query : this->queries) {
      vector<int> indices = this->c_map[query];
      vector<double> potentials_0;
      vector<double> potentials_1;
      for (int i : indices) {
        Clique c = this->cliques[i];
        vector<string> literals = c.getLiterals();
        for (string literal : literals) {
          if (this->obs.find(literal)==this->obs.end()) {
            truth_tables[i][literal] = (double)assignments[literal];
          }
        }
        truth_tables[i][query] = 1.0;
        potentials_1.push_back(c.getPotential(truth_tables[i]));
        truth_tables[i][query] = 0.0;
        potentials_0.push_back(c.getPotential(truth_tables[i]));
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


void MLN::gibbsSampling_v4(int round, string query) {
  unordered_set<string> valid_unknown;
  valid_unknown = this->queries;
  // vector<bool> visited (this->cliques.size(), false);
  // dfsSearch(valid_unknown, visited, query);

  unordered_map<string, int> samples;
  for (string query : valid_unknown) {
    samples[query] = 0;
  }

  unordered_map<string, int> assignments;
  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0, 1.0);
  // initialize randomly
  /*
  for (string query : valid_unknown) {
    double rand = distribution(generator);
    if (rand>0.5) {
      assignments[query] = 1;
    }
    else {
      assignments[query] = 0;
    }
  }
  */
  // intialize assignment with maxwalksat
  unordered_set<int> c_idx;
  for (auto& query_name : valid_unknown) {
    for (int id : this->c_map[query_name]) {
      c_idx.insert(id);
    }
  }
  maxWalkSAT(assignments, c_idx, MAXFLIPS, MAXTRIES, TARGET, NOISE);
  // cout << "after maxWalkSAT initialization: " << endl;
  // for (auto& query_name : valid_unknown) {
  //   cout << query_name << ' ' << assignments[query_name] << endl;
  // }


  vector<unordered_map<string, double> > truth_tables;
  for (Clique c : this->cliques) {
    unordered_map<string, double> m;
    vector<string> literals = c.getLiterals();
    for (string literal : literals) {
      if (this->obs.find(literal)==this->obs.end()) {
        m[literal] = (double)assignments[literal];
      }
      else {
        m[literal] = this->prob[literal];
      }
    }
    truth_tables.push_back(m);
  }

  unordered_map<string, vector<double> > potentials_0s;
  unordered_map<string, vector<double> > potentials_1s;
  for (string query : valid_unknown) {
    potentials_0s[query] = vector<double> (this->c_map[query].size(), 0.0);
    potentials_1s[query] = vector<double> (this->c_map[query].size(), 0.0);
  }

  vector<string> v_query;
  for (string query : valid_unknown) {
    v_query.push_back(query);
  }
  vector<int> q_indices (v_query.size());
  for (int i=0; i<v_query.size(); i++) {
    q_indices[i] = i;
  }

  auto rng = std::default_random_engine {};

  for (int r=0; r<round; r++) {
    shuffle(q_indices.begin(), q_indices.end(), rng);
    for (int qi : q_indices) {
      string query = v_query[qi];
      for (int i=0; i<this->c_map[query].size(); i++) {
        int ind = this->c_map[query][i];
        Clique c = this->cliques[ind];
        vector<string> literals = c.getLiterals();
        for (string literal : literals) {
          if (this->obs.find(literal)==this->obs.end()) {
            truth_tables[ind][literal] = (double)assignments[literal];
          }
        }
        truth_tables[ind][query] = 1.0;
        potentials_1s[query][i] = c.getPotential(truth_tables[ind]);
        // c.printClique();
        // cout << potentials_1s[query][i] << ' ';
        truth_tables[ind][query] = 0.0;
        potentials_0s[query][i] = c.getPotential(truth_tables[ind]);
        // cout << potentials_0s[query][i] << endl;
      }
      double sum_1 = 0;
      for (double p : potentials_1s[query]) {
        sum_1 += p;
      }
      double sum_0 = 0;
      for (double p : potentials_0s[query]) {
        sum_0 += p;
      }
      // cout << sum_1 << ' ' << sum_0 << endl;
      double exp_1 = exp(sum_1);
      double exp_0 = exp(sum_0);
      double p = exp_1 / (exp_1+exp_0);
      // cout << query << ' ' << p << endl;
      double prand = distribution(generator);
      if (prand<p) {
        if (round<10000 || r>1000) {
          samples[query] += 1;
        }
        assignments[query] = 1;
      }
      else {
        assignments[query] = 0;
      }
    }
  }

  int denominator = round>=10000 ? round-1000 : round;
  for (string query : valid_unknown) {
    this->prob[query] = ((double)samples[query]) / denominator;
    // cout << query << ": " << this->prob[query] << endl;
  }
}



void MLN::gibbsSampling_vp(int round, string query, double delta) {
  unordered_set<string> valid_unknown;
  valid_unknown = this->queries;

  random_device rd;
  mt19937 generator(rd());
  uniform_real_distribution<double> distribution(0.0, 1.0);

  // initialize randomly
  map<string, double> assignments;
  for (string query : valid_unknown) {
    // assignments[query] = 0.5;
    assignments[query] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    assignments[query] = distribution(generator);
    // cout << query << ' ' << assignments[query] << endl;
  }
  map<string, double> temp (assignments);

  unordered_set<int> c_idx;
  for (auto& query_name : valid_unknown) {
    for (int id : this->c_map[query_name]) {
      c_idx.insert(id);
    }
  }

  vector<unordered_map<string, double> > truth_tables;
  for (int c_i=0; c_i<this->cliques.size(); c_i++) {
    Clique c = this->cliques[c_i];
    unordered_map<string, double> m;
    vector<string> literals = c.getLiterals();
    for (string literal : literals) {
      if (this->obs.find(literal)==this->obs.end()) {
        m[literal] = assignments[literal];
      }
      else {
        m[literal] = this->prob[literal];
      }
    }
    truth_tables.push_back(m);
  }

  unordered_map<string, vector<double> > potentials_0s;
  unordered_map<string, vector<double> > potentials_1s;
  for (string query : valid_unknown) {
    potentials_0s[query] = vector<double> (this->c_map[query].size(), 0.0);
    potentials_1s[query] = vector<double> (this->c_map[query].size(), 0.0);
  }

  vector<string> v_query;
  for (string query : valid_unknown) {
    v_query.push_back(query);
  }
  vector<int> q_indices (v_query.size());
  for (int i=0; i<v_query.size(); i++) {
    q_indices[i] = i;
  }

  auto rng = std::default_random_engine {};

  vector<bool> converges (v_query.size(), false);
  for (int r=0; r<round; r++) {
    shuffle(q_indices.begin(), q_indices.end(), rng);
    for (int qi : q_indices) {
      string query = v_query[qi];
      // cout << query << endl;
      for (int i=0; i<this->c_map[query].size(); i++) {
        int ind = this->c_map[query][i];
        Clique c = this->cliques[ind];
        vector<string> literals = c.getLiterals();
        string rule_name = c.getRuleName();
        for (string literal : literals) {
          if (this->obs.find(literal)==this->obs.end()) {
            truth_tables[ind][literal] = assignments[literal];
          }
        }
        truth_tables[ind][query] = 1.0;
        potentials_1s[query][i] = -this->prob[rule_name]*c.getHingeSatisifiablity(truth_tables[ind]);
        // cout << rule_name << ' ' << this->prob[rule_name] << endl;
        // cout << c.toString() << endl;
        // cout << potentials_1s[query][i] << ' ';
        // cout << c.satisifiablity(truth_tables[ind]) << ' ' << endl;
        // cout << c.toString() << ' ' << rule_name << endl;
        truth_tables[ind][query] = 0.0;
        potentials_0s[query][i] = -this->prob[rule_name]*c.getHingeSatisifiablity(truth_tables[ind]);
        // cout << potentials_0s[query][i] << ' ';
        // cout << endl;
        // cout << c.getPotential(truth_tables[ind]) << ' ';
        // cout << c.toString() << ' ' << rule_name << endl;
      }
      double sum_1 = 0;
      for (double p : potentials_1s[query]) {
        sum_1 += p;
      }
      double sum_0 = 0;
      for (double p : potentials_0s[query]) {
        sum_0 += p;
      }
      // cout << sum_1 << ' ' << sum_0 << endl;
      double exp_1 = exp(sum_1);
      double exp_0 = exp(sum_0);
      double p = exp_1 / (exp_1+exp_0);
      temp[query] = p;
      converges[qi] = abs(temp[query]-assignments[query])<delta;
      // if (!converges[qi]) {
      //   cout << query << ": " << abs(temp[query]-assignments[query]) << endl;
      //   cout << query << ": " << assignments[query] << ' ' << temp[query] << endl;
      // }
    }
    // cout << endl << "round " << r << " finished " << endl;
    // for (string query : this->queries) {
    //   cout << query << ": " << temp[query] << endl;
    // }
    // cout << endl;
    bool converge = true;
    for (int qi : q_indices) {
      string query = v_query[qi];
      assignments[query] = temp[query];
      converge &= converges[qi];
    }
    if (converge) {
      // cout << "converge at iteration: " << r << endl;
      break;
    }
  }

  for (string query : valid_unknown) {
    this->prob[query] = assignments[query];
  }
}




double MLN::estimatedProb(string query) {
  assert(this->queries.find(query)!=this->queries.end());
  for (string literal : this->queries) {
    assert(this->prob.find(literal)!=this->prob.end());
  }
  map<string, double> truth_table (this->prob);
  double potential_1 = 0.0, potential_0 = 0.0;
  for (int c_i : this->c_map[query]) {
    Clique c = this->cliques[c_i];
    truth_table[query] = 1;
    potential_1 += c.getPotential(truth_table);
    truth_table[query] = 0;
    potential_0 += c.getPotential(truth_table);
  }
  double p = exp(potential_1) / (exp(potential_1)+exp(potential_0));
  return p;
}



double MLN::queryProb(string query) {
  assert(this->prob.find(query)!=this->prob.end());
  return this->prob[query];
}


map<string, vector<double> > MLN::getAllProbs(int round, int times) {
  map<string, vector<double> > res;
  for (string query : this->queries) {
    res[query] = vector<double> (times);
  }
  for (int i=0; i<times; i++) {
    gibbsSampling_v3(round);
    for (string query : this->queries) {
      res[query][i] = queryProb(query);
    }
  }
  return res;
}


unordered_map<string, double> MLN::getInfluence(string query) {
  assert(this->pd.find(query)!=this->pd.end());
  return this->pd[query];
}



void MLN::buildCliqueMaps() {
  for (int ci=0; ci<this->cliques.size(); ci++) {
    this->cliques[ci].buildPotentialMaps(this->obs, this->queries, this->prob);
  }
}



string MLN::toString() {
  string res;
  res += "obs: "+to_string(this->obs.size())+"\n";
  for (string literal : this->obs) {
    res += literal + " ";
  }
  res += "\nqueries: "+to_string(this->queries.size())+"\n";
  for (string literal : this->queries) {
    res += literal + " ";
  }
  res += "\ncliques: "+to_string(this->cliques.size())+"\n";
  for (Clique c : this->cliques) {
    res += c.toString()+"\n";
  }
  return res;
}


void MLN::saveToFile(ofstream& file) {
  for (string provenance : this->provs) {
    file << "provenance: " << provenance << endl;
  }
  file << "observed: ";
  for (string s : this->obs) {
    file << s << ' ';
  }
  file << endl;
  file << "queried: ";
  for (string s : this->queries) {
    file << s << ' ';
  }
  file << endl;
  for (int i=0; i<this->cliques.size(); i++) {
    this->cliques[i].saveToFile(file);
  }
  for (auto it=this->c_map.begin(); it!=this->c_map.end(); it++) {
    file << "c_map: " << it->first << ' ';
    for (int i=0; i<it->second.size(); i++) {
      file << it->second[i] << ' ';
    }
    file << endl;
  }
  for (string query : this->queries) {
    if (this->prob.find(query)!=this->prob.end()) {
      file << "prob: " << query << ' ' << this->prob[query];
    }
    file << endl;
  }
  for (string literal : this->obs) {
    if (this->prob.find(literal)!=this->prob.end()) {
      file << "prob: " << literal << ' ' << this->prob[literal];
    }
    file << endl;
  }
}


void MLN::saveToDotFile(ofstream& file) {
  file << "graph {\n";
  file << "layout=\"neato\"\n";
  file << "ratio=\"0.5\"\n";
  unordered_map<string, int> node_map;
  for (string literal : this->obs) {
    if (Parser::isVariable(literal)) {
      if (node_map.find(literal)==node_map.end()) {
          node_map.insert({literal, node_map.size()+1});
        }
      int l_i = node_map[literal];
      // file << l_i << " [label=\"" << literal << "\", color=\"goldenrod1\"]\n";
      // file << l_i << " [shape=\"point\", color=\"goldenrod1\"]\n";
      file << l_i << " [label=\"\", shape=circle, width=.3, color=\"goldenrod1\"]\n";
    }
  }
  for (string literal : this->queries) {
    if (Parser::isVariable(literal)) {
      if (node_map.find(literal)==node_map.end()) {
          node_map.insert({literal, node_map.size()+1});
        }
      int l_i = node_map[literal];
      // file << l_i << " [label=\"" << literal << "\", color=\"darkgreen\"]\n";
      file << l_i << " [label=\"\", shape=circle, width=.3, color=\"darkgreen\"]\n";
    }
  }

  unordered_set<string> edge_set;
  for (int i=0; i<this->cliques.size(); i++) {
    Clique c = this->cliques[i];
    if (c.isSingular()) continue;
    vector<string> literals = c.getLiterals();
    for (int i=0; i<literals.size(); i++) {
      string l = literals[i];
      int l_i = node_map[l];
      for (int j=i+1; j<literals.size(); j++) {
        string r = literals[j];
        int r_i = node_map[r];
        string edge1 = to_string(l_i)+"_"+to_string(r_i);
        string edge2 = to_string(r_i)+"_"+to_string(l_i);
        if (edge_set.find(edge1)==edge_set.end() && edge_set.find(edge2)==edge_set.end()) {
          file << l_i << " -- " << r_i << "\n";
          edge_set.insert(edge1);
          edge_set.insert(edge2);
        }
      }
    }
  }
  file << "}\n";
}


void MLN::dfsSearch(unordered_set<string>& valid_unknown, vector<bool>& visited, string& query, bool stopObserved) {
  if (this->obs.find(query)!=this->obs.end()) {
    if (stopObserved) return;
  }
  else {
    valid_unknown.insert(query);
  }
  for (int ind : this->c_map[query]) {
    if (!visited[ind]) {
      visited[ind] = true;
      for (string s : this->cliques[ind].getLiterals()) {
        if (s!=query) {
          dfsSearch(valid_unknown, visited, s);
        }
      }
    }
  }
}


bool MLN::connectivity(string& query) {
  if (this->queries.find(query)==this->queries.end()) {
    return false;
  }
  unordered_set<string> valid_unknown;
  vector<bool> visited (this->cliques.size(), false);
  dfsSearch(valid_unknown, visited, query);
  bool res = true;
  for (bool b : visited) {
    res &= b;
  }
  return res;
}


map<string, double> MLN::computeGradients(const string& target, 
                                          const vector<string>& query_names, 
                                          map<string, double>& original_probs,
                                          Regulation regulation, 
                                          double theta,
                                          int rule_name) {
  assert(find(query_names.begin(), query_names.end(), target)!=query_names.end());
  map<string, double> softmax;
  map<string, double> gradients;
  double presum = 0;
  for (string query_name : query_names) {
    assert(this->prob.find(query_name)!=this->prob.end());
    softmax[query_name] = exp(this->prob[query_name]);
    presum += softmax[query_name];
  }
  for (string query_name : query_names) {
    softmax[query_name] /= presum;
  }
  for (string literal : this->obs) {
    if (Parser::isRuleName(literal) && rule_name==0) continue;
    gradients[literal] = 0;
    for (string query_name : query_names) {
      if (query_name==target) {
        gradients[literal] += (softmax[query_name]-1)*this->pd[query_name][literal];
      }
      else {
        gradients[literal] += softmax[query_name]*this->pd[query_name][literal];
      }
    }
  }
  for (string literal : this->obs) {
    if (Parser::isRuleName(literal) && rule_name==0) continue;
    switch (regulation)
    {
    case l2: {
      double reg_grad = theta*2*(this->prob[literal]-original_probs[literal]);
      // cout << literal << ' ' << gradients[literal] << ' ' << reg_grad << endl;
      gradients[literal] += reg_grad;
      break;
    }
    case l1: {
      // double square = pow(this->prob[literal]-original_probs[literal], 2);
      // double reg_grad = theta / sqrt(square+1e-9) * (this->prob[literal]-original_probs[literal]);
      // // cout << literal << ' ' << gradients[literal] << ' ' << reg_grad << endl;
      // gradients[literal] += reg_grad;
      double diff = max(0.0, min(1.0, this->prob[literal]-gradients[literal]))-original_probs[literal];
      if (diff>theta) gradients[literal] += theta;
      else if (diff<-theta) gradients[literal] -= theta;
      else gradients[literal] = 0;
      break;
    }
    default:
      break;
    }
  }
  return gradients;
}


double MLN::computeCrossEntropyLoss(const string& target, 
                                    const vector<string>& query_names,
                                    map<string, double>& original_probs,
                                    Regulation regulation, 
                                    double theta) {
  assert(find(query_names.begin(), query_names.end(), target)!=query_names.end());
  double loss = 0.0;
  map<string, double> softmax;
  double presum = 0;
  for (string query_name : query_names) {
    assert(this->prob.find(query_name)!=this->prob.end());
    softmax[query_name] = exp(this->prob[query_name]);
    presum += softmax[query_name];
  }
  softmax[target] /= presum;
  loss += -log(softmax[target]);

  double reg = 0.0;
  for (string literal : this->obs) {
    if (Parser::isRuleName(literal)) continue;
    switch (regulation)
    {
    case l2:
      reg += theta * pow(fabs(this->prob[literal]-original_probs[literal]), 2);
      break;
    case l1: {
      double square = pow(this->prob[literal]-original_probs[literal], 2);
      reg += theta * sqrt(square+1e-9);
      break;
    }
    default:
      break;
    }
  }
  cout << "origin loss: " << loss << ", reg: " << reg << endl;
  loss += reg;
  return loss;
}



void MLN::updateObs(vector<pair<string, double> >& gradients, double delta, size_t number_of_changes) {
  number_of_changes = std::min(number_of_changes, gradients.size());
  size_t count = 0;
  for (auto it : gradients) {
    string obs_literal = it.first;
    double obs_gradient = it.second;
    double tmp = this->prob[obs_literal]-delta*obs_gradient;
    tmp = max(0.0, min(1.0, tmp));
    cout << "new " << obs_literal << ": " << tmp << endl;
    if (this->prob[obs_literal]==tmp) continue;
    this->prob[obs_literal] = max(0.0, min(1.0, tmp));
    count++;
    if (count>=number_of_changes) break;
  }
  return;
}



MLN::~MLN() {
}