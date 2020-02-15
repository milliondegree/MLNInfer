
#include <iostream>
#include <vector>
#include <string>
#include "Load.h"
#include "MLN.h"
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

  MLN mln(l.getProv(), l.getProb());
  
}