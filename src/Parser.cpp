#include "MLN.h"


Parser::Parser() {
}


Parser::~Parser() {
}


void Parser::parseProvenance(MLN& mln) {
  int i = 0;
  this->parseRuleHead(mln, mln.prov, i);
}


void Parser::parseRuleHead(MLN& mln, string& prov, int& i) {
  int start = i;
  while (prov[i]!='(') {
    i++;
  }
  string rule_head = prov.substr(start, i-start);
  if (mln.c_map.find(rule_head)==mln.c_map.end()) {
    mln.c_map[rule_head] = vector<int> ();
  }
  if (mln.prob.find(rule_head)==mln.prob.end()) {
    mln.queries.insert(rule_head);
  }
  else {
    mln.obs.insert(rule_head);
  }

  // deal with cliques only contain one literal (single clique)
  string predName = extractName(rule_head);
  assert(mln.prob.find(predName)!=mln.prob.end());
  Clique s_c = Clique(rule_head, mln.prob[predName]);
  mln.cliques.push_back(s_c);
  mln.c_map[rule_head].push_back(mln.cliques.size()-1);

  vector<vector<string>> rules = parseRules(mln, prov, i);

  for (vector<string> rule : rules) {
    string rule_name = rule[0];
    vector<string> r_body;
    for (int j=1; j<rule.size(); j++) {
      r_body.push_back(rule[j]);
    }
    Clique c(rule_name, mln.prob[rule_name], rule_head, r_body);
    mln.cliques.push_back(c);
    int index = mln.cliques.size()-1;
    mln.c_map[rule_head].push_back(index);
    for (string body : r_body) {
      if (mln.c_map.find(body)==mln.c_map.end()) {
        mln.c_map[body] = vector<int> ();
      }
      if (mln.prob.find(body)==mln.prob.end()) {
        mln.queries.insert(body);
      }
      else {
        mln.obs.insert(body);
      }
      mln.c_map[body].push_back(index);
    }
  }
}


vector<vector<string>> Parser::parseRules(MLN& mln, string& prov, int& i) {
  vector<vector<string>> res;
  if (prov[i+1]!='(') {
    vector<string> rule = parseRule(mln, prov, i);
    res.push_back(rule);
  }
  else {
    i++;
    while (true) {
      vector<string> rule = parseRule(mln, prov, i);
      res.push_back(rule);
      assert(prov[i]=='+'|prov[i]==')');
      if (prov[i]=='+') {
        i++;
      }
      else {
        break;
      }
    }
    i++;
  }
  return res;
}


vector<string> Parser::parseRule(MLN& mln, string& prov, int& i) {
  i++;
  int start = i;
  while (prov[i]!='@') {
    i++;
  }
  string rule_name = prov.substr(start, i-start);
  while (prov[i]!='(') {
    i++;
  }
  vector<string> rule_body = parseRuleBody(mln, prov, i);
  rule_body.insert(rule_body.begin(), rule_name);
  i++;
  return rule_body;
}


vector<string> Parser::parseRuleBody(MLN& mln, string& prov, int& i) {
  i++;
  vector<string> res;
  int start = i;
  while (prov[i]!=')') {
    int start = i;
    while (prov[i]!='('&&prov[i]!='*'&&prov[i]!=')') {
      i++;
    }
    string body = prov.substr(start, i-start);
    res.push_back(body);
    if (prov[i]=='(') {
      i = start;
      parseRuleHead(mln, prov, i);
      assert(prov[i]=='*'|prov[i]==')');
      if (prov[i]=='*') {
        i++;
      }
      else {
        break;
      }
    }
    else {
      string predName = extractName(body);
      assert(mln.prob.find(predName)!=mln.prob.end());
      Clique s_c = Clique(body, mln.prob[predName]);
      if (mln.c_map.find(body)==mln.c_map.end()) {
        mln.c_map[body] = vector<int> ();
      }
      mln.cliques.push_back(s_c);
      mln.c_map[body].push_back(mln.cliques.size()-1);
      if (prov[i]=='*') {
        i++;
      }
    }
  }
  i++;
  return res;
}


string Parser::extractName(string& s) {
  int i = 0;
  while (!(s[i]>='0' && s[i]<='9')) {
    i++;
  }
  return s.substr(0, i);
}