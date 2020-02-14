
#include <iostream>
#include <vector>
#include <string>
using namespace std;


#define provFile \
"./data/prov/cancer5.txt"

#define ObsFile \
"./data/observe/smokeTest.db"


int main() {

  vector<string> msg {"Hello", "C++", "World", "from", "VS Code!"};

  for (const string& word : msg)
  {
    cout << word << " ";
  }
  cout << endl;

}