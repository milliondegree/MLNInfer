
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <math.h>
#include "../Load.h"
#include "../MLN.h"
#include "../Clique.h"
#include "../Influence.h"
#include "../Optimizer.h"
using namespace std;


bool sortByValue(const pair<string, double>& p1, const pair<string, double>& p2) {
  return fabs(p1.second) > fabs(p2.second);
}

void printMLNStatistic(MLN& mln) {
  cout << "observed tuple number: " << mln.obs.size() << endl;
  cout << "unobserved tuple number: " << mln.queries.size() << endl;
  cout << "clique number: " << mln.cliques.size() << endl;
}

void influenceQuery(MLN& mln, string query, string infl, int round, double delta, string mode) {
  clock_t t1, t2;
  double infValue;
  t1 = clock();
  if (mode=="equations") {
    Influence influence (mln);
    infValue = influence.influenceQuery(mln, query, infl);
  }
  else {
    Grader grader;
    grader.computeGradient(mln, query, infl, round, delta, mode);
    infValue = mln.getInfluence(query)[infl];
  }
  t2 = clock();
  // cout << "mode " << mode << ": influence compute time (" << mode << "): " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  // cout << "influence of " << infl << " on " << query << " is " << infValue << endl;
  // cout << endl;
}


void probabilityQuery(MLN& mln, int round, string query_name, string mode, double delta) {
  clock_t t1 = clock();
  if (mode=="gibbs") {
    mln.gibbsSampling(round);
  }
  else if (mode=="pgibbs") {
    // wrong results
    mln.gibbsSampling_vp(round, query_name, delta);
  }
  else if (mode=="mcsat"){
    mln.mcsat(round, query_name);
  }
  else if (mode=="pmcsat") {
    mln.pmcsat(round, query_name);
  }
  else if (mode=="bp") {
    mln.naiveBeliefPropagation(query_name);
  }
  else if (mode=="abp") {
    mln.advanceBeliefPropagation(query_name);
  }
  else if (mode=="lbp") {
    mln.loopyBeliefPropagation(query_name);
  }
  else if (mode=="mclbp") {
    mln.loopyBeliefPropagationMCS(query_name, round);
  }
  clock_t t2 = clock();
  cout << mode+" sample time: " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  double prob = mln.queryProb(query_name);
  cout << "probability of " << query_name << " is " << setprecision(9) << prob << endl;
  cout << endl;
}


int main(int argc, char* argv[]) {

  unordered_map<string, string> args;
  vector<string> argvs;
  for (int i=0; i<argc; i++) {
    argvs.push_back(string(argv[i]));
  }
  for (int i=1; i<argc; i+=2) {
    assert(i+1<argc);
    if (argvs[i]=="-q" || argvs[i]=="-query_name") {
      args["query_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-t" || argvs[i]=="-target_name") {
      args["target_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-o" || argvs[i]=="-observe_file") {
      args["observe_file"] = argvs[i+1];
    }
    else if (argvs[i]=="-p" || argvs[i]=="-provenance_file") {
      args["provenance_file"] = argvs[i+1];
    }
    else if (argvs[i]=="-s" || argvs[i]=="-save") {
      args["save"] = argvs[i+1];
    }
    else if (argvs[i]=="-i" || argvs[i]=="-influence_name") {
      args["influence_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-r" || argvs[i]=="-round") {
      args["round"] = argvs[i+1];
    }
    else if (argvs[i]=="-a") {
      args["approx"] = argvs[i+1];
    }
    else if (argvs[i]=="-d" || argvs[i]=="-delta") {
      args["delta"] = argvs[i+1];
    }
    else if (argvs[i]=="-m" || argvs[i]=="-mode") {
      args["mode"] = argvs[i+1];
    }
    else if (argvs[i]=="-cn" || argvs[i]=="-clique_number") {
      args["clique_number"] = argvs[i+1];
    }
    else if (argvs[i]=="-im" || argvs[i]=="-influ_mode") {
      args["influ_mode"] = argvs[i+1];
    }
    else if (argvs[i]=="-op" || argvs[i]=="-optimize") {
      args["optimize"] = argvs[i+1];
    }
    else if (argvs[i]=="-update_rate") {
      args["update_rate"] = argvs[i+1];
    }
    else if (argvs[i]=="-regularization") {
      args["regularization"] = argvs[i+1];
    }
    else if (argvs[i]=="-optimizer") {
      args["optimizer"] = argvs[i+1];
    }
    else if (argvs[i]=="-l") {
      args["norm"] = argvs[i+1];
    }
    else if (argvs[i]=="-rule_name") {
      args["rule_name"] = argvs[i+1];
    }
  }


  Load l(args["provenance_file"], args["observe_file"]);
  vector<string> prov = l.getProv();

  MLN mln(l);
  Parser parser = Parser(1);
  parser.parseProvenance(mln);
  // mln.merge();

  cout << "original MLN: " << endl;
  printMLNStatistic(mln);
  // ofstream file ("/home/jz598/MLNInfer/data/hypertext-class/sample7/graphviz/raw/full.txt");
  // mln.saveToDotFile(file);
  // file.close();

  // for (string literal : mln.getQueryLiterals()) {
  //   if (Parser::isVariable(literal)) {
  //     MLN cmln = mln.getConnectedMLN(literal);
  //     ofstream file_c ("/home/jz598/MLNInfer/data/hypertext-class/sample7/graphviz/raw/"+literal+"_c.txt");
  //     cmln.saveToDotFile(file_c);
  //     file_c.close();
  //     MLN mmln = mln.getMinimalMLN(literal);
  //     ofstream file_q ("/home/jz598/MLNInfer/data/hypertext-class/sample7/graphviz/raw/"+literal+".txt");
  //     mmln.saveToDotFile(file_q);
  //     file_q.close();
  //   }
  // }

  if (args.find("query_name")!=args.end()) {
    MLN mmln = mln.getMinimalMLN(args["query_name"]);

    if (args.find("influence_name")!=args.end()) {
      string query_name = args["query_name"];
      probabilityQuery(mmln, stoi(args["round"]), query_name, args["mode"], 0.1);
      if (args["influence_name"]=="all") {
        unordered_set<string> obs = mmln.obs;
        vector<pair<string, double> > vp;
        clock_t t1 = clock();
        for (string literal : obs) {
          // if (Parser::isRuleName(literal) && stoi(args["rule_name"])>0) {
            // influenceQuery(mmln, query_name, literal, 10000, stod(args["delta"]), "gibbs");
            // vp.push_back(pair<string, double>({literal, mmln.getInfluence(query_name)[literal]}));
          // }
          if (Parser::isVariable(literal)) {
            // continue;
            influenceQuery(mmln, query_name, literal, stoi(args["round"]), 1.0, args["influ_mode"]);
            vp.push_back(pair<string, double>({literal, mmln.getInfluence(query_name)[literal]}));
          }
        }
        cout << "influence query time of " << query_name << ": " << (clock()-t1)*1.0/CLOCKS_PER_SEC << endl; 
        sort(vp.begin(), vp.end(), sortByValue);
        cout << "sorted influences:" << endl;
        for (auto it=vp.begin(); it<vp.end(); it++) {
          cout << "influence of " << it->first << " on " << query_name << " is " << it->second << endl;
        }
      }
      else {
        influenceQuery(mmln, query_name, args["influence_name"], stoi(args["round"]), stod(args["delta"]), args["influ_mode"]);
      }
    }

    ofstream file ("/home/jz598/MLNInfer/data/hypertext-class/sample7/graphviz/raw/"+args["query_name"]+".txt");
    
    file << "graph {\n";
    file << "layout=\"neato\"\n";
    file << "ratio=\"0.5\"\n";
    // file << "splines=\"curved\"\n";
    file << "overlap=false\n";
    unordered_map<string, int> node_map;
    for (string literal : mmln.obs) {
      if (Parser::isVariable(literal)) {
        if (node_map.find(literal)==node_map.end()) {
            node_map.insert({literal, node_map.size()+1});
          }
        int l_i = node_map[literal];
        // file << l_i << " [label=\"" << literal << "\", color=\"goldenrod1\"]\n";
        // file << l_i << " [shape=\"point\", color=\"goldenrod1\"]\n";
        if (abs(mmln.getInfluence(args["query_name"])[literal])>0) {
          double width = abs(mmln.getInfluence(args["query_name"])[literal]);
          width = max(0.01, min(width, 0.02))/0.02 * 0.7;
          file << l_i << " [label=\"\", xlabel=\"" << literal << "\", shape=circle, penwidth=2.0 width=" << width << ", color=\"goldenrod1\", fontsize=\"20pt\"]\n";
        }
        else file << l_i << " [label=\"\", shape=circle, penwidth=2.0, width=" << 0.2 << ", color=\"goldenrod1\", style=\"dashed\"]\n";
      }
    }
    for (string literal : mmln.queries) {
      if (Parser::isVariable(literal)) {
        if (node_map.find(literal)==node_map.end()) {
            node_map.insert({literal, node_map.size()+1});
          }
        int l_i = node_map[literal];
        if (literal==args["query_name"]) {
          file << l_i << " [label=\"\", xlabel=\"" << literal << "\", shape=circle, penwidth=2.0, width=.2, color=\"lightcoral\", fontsize=\"20pt\"]\n";
        }
        else file << l_i << " [label=\"\", shape=circle, penwidth=2.0, width=.2, color=\"darkgreen\", style=\"dashed\"]\n";
      }
    }

    unordered_set<string> edge_set;
    for (int i=0; i<mmln.cliques.size(); i++) {
      Clique c = mmln.cliques[i];
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
            file << l_i << " -- " << r_i << "[style=\"dotted\", penwidth=1.0]\n";
            edge_set.insert(edge1);
            edge_set.insert(edge2);
          }
        }
      }
    }
    file << "}\n";
    file.close();
  }
  else {
    cout << "missing query name or probability inference mode" << endl;
    exit(0);
  }

}
