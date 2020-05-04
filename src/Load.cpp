
#include "Load.h"

Load::Load() {
}

Load::Load(string pfile, string ofile) {
  Load::setProv(pfile);
  Load::setProb(ofile);
}

Load::~Load() {
}

vector<string> Load::getProv() {
  return this->prov;
}

map<string, double> Load::getProb() {
  return this->prob;
}


map<string, string> Load::getSames() {
  return this->sames;
}


void Load::setProv(string pfile) {
  ifstream ofin(pfile);
  string line;

  while (getline(ofin, line)) {
    if (!line.empty()) {
      line.erase(0, line.find_first_not_of(" \t\r\n"));
      line.erase(line.find_last_not_of(" \t\r\n")+1);
      this->prov.push_back(line);
    }
  }
}


void Load::setProb(string ofile) {
  ifstream ofin(ofile);
  string line;

  while (getline(ofin, line)) {
    if (!line.empty()) {
      line.erase(0, line.find_first_not_of(" \t\r\n"));
      line.erase(line.find_last_not_of(" \t\r\n")+1);
      vector<string> split;
      boost::split(split, line, boost::is_any_of(" "));
      string literal = split[0];
      string sp = split[1];
      if (!isalpha(sp[0])) {
        this->prob[literal] = stof(sp);
      }
      else {
        this->sames[literal] = sp;
        this->sames[sp] = literal;
      }
    }
  }
}

