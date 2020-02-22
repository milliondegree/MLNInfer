#include "MLN.h"


Parser::Parser() {
}


Parser::~Parser() {
}


void Parser::parseProvenance(MLN& mln) {
  // cout << "parsing with class Parser" << endl;
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
  cout << prov.substr(0, i) << endl;
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
  cout << prov.substr(0, i) << endl;
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
  // cout << prov.substr(0, i) << endl;
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
      // cout << prov.substr(0, i) << endl;
      parseRuleHead(mln, prov, i);
      // cout << prov.substr(0, i) << endl;
      // cout << i << ' ' << prov.length() << endl;
      assert(prov[i]=='*'|prov[i]==')');
      if (prov[i]=='*') {
        i++;
      }
      else {
        break;
      }
    }
    else if (prov[i]=='*') {
      i++;
    }
  }
  i++;
  return res;
}