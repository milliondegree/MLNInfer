
#include <iostream>
#include <vector>
#include <string>
#include "Load.h"
#include "MLN.h"
#include "Clique.h"
using namespace std;


#define provFile \
"./data/prov/cancer5.txt"

#define ObsFile \
"./data/observe/smokeTest.db"


void test(string& s) {
  s = s.substr(0, s.length()-1);
  cout << s.length();
}


int main() {

  Load l(provFile, ObsFile);
  string prov = l.getProv();
  cout << prov << endl;
  map<string, double> prob = l.getProb();
  for (auto it=prob.begin(); it!=prob.end(); it++) {
    cout << it->first << ' ' << it->second << endl;
  }
  cout << endl;

  MLN mln(l.getProv(), l.getProb());
  set<string> obs = mln.getObsLiterals();
  set<string> queries = mln.getQueryLiterals();
  for (string s : obs) {
    cout << s << ' ';
  }
  cout << endl;
  for (string s : queries) {
    cout << s << ' ';
  }
  cout << endl;

  for (string s : obs) {
    int nc = mln.numberCliques(s);
    cout << s << ' ' << nc << endl;
  }

  vector<Clique> cs = mln.getCliques("smoke5");
  for (Clique c : cs) {
    c.printClique();
  }

}