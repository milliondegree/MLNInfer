
#include <iostream>
#include <vector>
#include <string>
#include "Load.h"
using namespace std;


#define provFile \
"./data/prov/cancer5.txt"

#define ObsFile \
"./data/observe/smokeTest.db"


int main() {

  Load l(provFile, ObsFile);
  string prov = l.getProv();
  cout << prov << endl;
  map<string, double> prob = l.getProb();
  for (auto it=prob.begin(); it!=prob.end(); it++) {
    cout << it->first << ' ' << it->second << endl;
  }

}