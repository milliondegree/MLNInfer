#ifndef _CPROVGRAPH_H
#define _CPROVGRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <string>


#ifndef NDEBUG
#define ASSERT_EX(condition, statement) \
    do { \
        if (!(condition)) { statement; assert(condition); } \
    } while (false)
#else
#define ASSERT_EX(condition, statement) ((void)0)
#endif

#define MAX_NUM_OF_SEARCH 100

enum VertexType {Input, Derived, Parameter, Sum, Mul, Div, Scale};

inline std::ostream& operator<<(std::ostream& out, const VertexType value){
  const char* s = 0;
  #define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value) {
      PROCESS_VAL(Input)
      PROCESS_VAL(Derived)     
      PROCESS_VAL(Parameter)
      PROCESS_VAL(Sum)
      PROCESS_VAL(Mul)
      PROCESS_VAL(Div)
      PROCESS_VAL(Scale)
    }
  #undef PROCESS_VAL
    return out << s;
}

inline std::string vertexTypeToString(VertexType vt) {
  std::string s;
  switch(vt) {
    case(Input): s = "input"; break;
    case(Derived): s = "derived"; break;
    case(Parameter): s = "param"; break;
    case(Sum): s = "sum"; break;
    case(Mul): s = "mul"; break;
    case(Div): s = "div"; break;
    case(Scale): s = "scale"; break;
  }
  return s;
}

struct ProvVertex {
  VertexType vt;
  std::string name;
  double value;
  std::unordered_map<std::string, std::string> params;
  std::unordered_set<std::string> EDBs;
};

struct ProvEdge{
  double weight;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, ProvVertex, ProvEdge > Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_tier;

struct my_node_writer {
  my_node_writer(Graph& g_) : g (g_) {};
  template <class Vertex>
  void operator()(std::ostream& out, Vertex v) {
    if (g[v].vt==Input) 
      out << " [label=\"" << g[v].name << "\", shape=oval, color=dodgerblue]";
    else if (g[v].vt==Derived) 
      out << " [label=\"" << g[v].name << "\", shape=oval, color=limegreen]";
    else if (g[v].vt==Parameter) 
      out << " [label=\"" << g[v].name << "\", shape=oval, color=darkviolet]";
    else 
      out << " [label=\"" << g[v].name << "\", shape=box, color=tomato]";
  };
  Graph g;
};

struct my_edge_writer {
  my_edge_writer(Graph& g_) : g (g_) {};
  template <class Edge>
  void operator()(std::ostream& out, Edge e) {
    out << " [color=purple]" << std::endl;
    out << " [label=\"" << e  <<":" << g[e].miles << "\"]";
  };
  Graph g;
};

struct my_graph_writer {
  void operator()(std::ostream& out) const {
    out << "graph [bgcolor=lightgrey]";
    out << "node [shape=circle color=blue]";
    out << "edge [color=red]";
  }
};

class ApproxSubGraphDiff {
public: 
  std::unordered_set<std::string> EDBs;
  double diff;
  ApproxSubGraphDiff() {}
  ApproxSubGraphDiff(const std::unordered_set<std::string>& s, double d): EDBs(s), diff(d) {}

  bool operator == (const ApproxSubGraphDiff& asgd2) const {
    return this->EDBs==asgd2.EDBs;
  }
};

class ApproxSubGraphDiffComparison {
public: 
  ApproxSubGraphDiffComparison() {};
  bool operator () (const ApproxSubGraphDiff& sd1, const ApproxSubGraphDiff& sd2) {
    return sd1.diff>sd2.diff; 
  }
};

class EDBSetHash {
public:
  size_t operator () (const std::unordered_set<std::string>& EDBs) const {
    hash<std::string> hasher;
    size_t seed = EDBs.size();
    for (auto& i : EDBs) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};


class CProvGraph {
public:
  /* initializer*/
  CProvGraph() {
  }

  CProvGraph(std::string& sp): save_path(sp) {}

  /* read functions */
  size_t getVertexIndexByName(const std::string& name) {
    typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
    IndexMap index = get(boost::vertex_index, g);
    vertex_iter vi, vi_end;
    for (boost::tie(vi, vi_end)=boost::vertices(g); vi!=vi_end; vi++) {
      if (g[*vi].name.compare(name)==0) {
        return index[*vi];
      }
    }
    return -1;
  }

  inline vertex_t getVertexByName(const std::string& name) {
    return vertex_set[name];
  }

  inline vertex_t checkVertexExistByName(const std::string& name) {
    return vertex_set.find(name)!=vertex_set.end();
  }

  inline std::unordered_set<std::string> getVertexEBDs(vertex_t v) {
    return g[v].EDBs;
  }

  inline std::unordered_set<std::string> getVertexEBDsByName(const std::string& name) {
    return g[getVertexByName(name)].EDBs;
  }

  inline double getVertexValueByName(const std::string& name) {
    return g[getVertexByName(name)].value;
  }


  /* write functions */
  vertex_t addVariableVertex(const VertexType vt, const std::string& name, double value) {
    ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
    vertex_t v = boost::add_vertex(g);
    vertex_set.insert(std::make_pair(name, v));
    g[v].vt = vt;
    g[v].name = name;
    g[v].value = value;
    return v;
  }

  vertex_t addOperatorVertex(const VertexType vt, const std::string& name) {
    ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
    vertex_t v = boost::add_vertex(g);
    vertex_set.insert(std::make_pair(name, v));
    g[v].vt = vt;
    g[v].name = name;
    return v;
  }

  vertex_t addOperatorVertex(const VertexType vt, const std::string& name, const std::unordered_map<std::string, std::string>& params) {
    ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
    vertex_t v = boost::add_vertex(g);
    vertex_set.insert(std::make_pair(name, v));
    g[v].vt = vt;
    g[v].name = name;
    g[v].params.insert(params.begin(), params.end());
    return v;
  }

  void addComputingSubgraph(const std::string& output_name, const double value, VertexType vt, const std::vector<std::string>& input_names) {
    /* add subgraph of computing provenance, it is based on an asumption that every output only relies on one operator 
      output_name: name of the output variable
      value: value of output variable
      vt: vertex type of operator
      input_names: names of the input variables that are connected to the operator vertex
    */
    vertex_t v_output, v_operator;
    if (checkVertexExistByName(output_name)) {
      v_output = getVertexByName(output_name);
      v_operator = getVertexByName(vertexTypeToString(vt)+"_"+output_name);
      g[v_output].value = value;
    }
    else {
      v_output = addVariableVertex(Derived, output_name, value);
      v_operator = addOperatorVertex(vt, vertexTypeToString(vt)+"_"+output_name);
      addProvEdge(v_output, v_operator);
    }
    for (auto s : input_names) {
      addProvEdge(v_operator, getVertexByName(s));
    }
  }

  void addComputingSubgraph(const std::string& output_name, const double value, VertexType vt, const std::vector<std::string>& input_names, const std::unordered_map<std::string, std::string>& params) {
    /* add subgraph of computing provenance, it is based on an asumption that every output only relies on one operator 
      output_name: name of the output variable
      value: value of output variable
      vt: vertex type of operator
      input_names: names of the input variables that are connected to the operator vertex
    */
    vertex_t v_output, v_operator;
    if (checkVertexExistByName(output_name)) {
      v_output = getVertexByName(output_name);
      v_operator = getVertexByName(vertexTypeToString(vt)+"_"+output_name);
      g[v_output].value = value;
    }
    else {
      v_output = addVariableVertex(Derived, output_name, value);
      v_operator = addOperatorVertex(vt, vertexTypeToString(vt)+"_"+output_name, params);
      addProvEdge(v_output, v_operator);
    }
    for (auto s : input_names) {
      addProvEdge(v_operator, getVertexByName(s));
    }
  }

  inline void addProvEdge(vertex_t v1, vertex_t v2, bool reverse) {
    if (!reverse) boost::add_edge(v1, v2, g);
    else boost::add_edge(v2, v2, g);
  }

  inline void addProvEdge(vertex_t v1, vertex_t v2) {
    boost::add_edge(v1, v2, g);
  }

  inline void setVertexValue(vertex_t v, double value) {
    g[v].value = value;
  }

  inline void setVertexEDBs(vertex_t v, std::unordered_set<std::string>& EDBs) {
    g[v].EDBs = EDBs;
  }

  inline void setChangedEDBs(std::unordered_map<std::string, double>& m) {
    changedEDBs = m;
  }

  


  /* provenance query functions */
public:
  CProvGraph ProvenanceQuery(const std::string& name) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);
    
    // initialize CProvGraph of the queried vertex
    std::string new_save_path = save_path.substr(0, save_path.find("."));
    new_save_path += "-"+name+".dot";
    CProvGraph subProvG(new_save_path);

    // insert the source vertex to subProvG
    ASSERT_EX(g[v].vt==Derived, std::cout << g[v].name << " is a " << vertexTypeToString(g[v].vt) << " vertex" << std::endl);
    subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].value);

    std::cout << "start provenance query" << std::endl;
    DFSProvQuery(v, subProvG);
    return subProvG;
  }

private:
  void DFSProvQuery(vertex_t s, CProvGraph& subProvG) {
    vertex_t parent = subProvG.getVertexByName(g[s].name);
    if (g[s].vt==Input || g[s].vt==Parameter) {
      subProvG.updateVertexEDBs(parent);
      return;
    }
    // start iterating adjacent vertices
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    vertex_t a_operator = subProvG.addOperatorVertex(g[v_operator].vt, g[v_operator].name, g[v_operator].params);
    subProvG.addProvEdge(parent, a_operator);
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      vertex_t child;
      if (subProvG.checkVertexExistByName(g[v].name)) {
        child = subProvG.getVertexByName(g[v].name);
        subProvG.addProvEdge(a_operator, child);
      }
      else {
        if (g[v].vt==Input) child = subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].value);
        else if (g[v].vt==Derived) child = subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].value);
        else child = subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].value);
        subProvG.addProvEdge(a_operator, child);
        DFSProvQuery(v, subProvG);
      }
    }
    subProvG.updateVertexEDBs(parent);
    return;
  }



  /* provenance-enabled influence query */
public: 
  double computeVariableWithChangedEDBs(const std::string& name, const std::unordered_map<std::string, double>& EDBs) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);
    changedEDBs = EDBs;
    std::unordered_set<vertex_t> visited;
    return DFSComputeVariable(v, visited);
  }

  double computeVariable(const std::string& name) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);
    std::unordered_set<vertex_t> visited;
    return DFSComputeVariable(v, visited);
  }

private:
  bool hasIntersection(const std::unordered_set<std::string>& l_set, const std::unordered_set<std::string>& r_set) {
    if (l_set.size()<r_set.size()) {
      for (std::string EDB : l_set) {
        if (r_set.find(EDB)!=r_set.end()) 
          return true;
      }
    }
    else {
      for (std::string EDB : r_set) {
        if (l_set.find(EDB)!=l_set.end()) 
          return true;
      }
    }
    return false;
  }

  bool hasIntersection(const std::unordered_map<std::string, double>& l_map, const std::unordered_set<std::string>& r_set) {
    if (l_map.size()<r_set.size()) {
      for (auto it=l_map.begin(); it!=l_map.end(); it++) {
        if (r_set.find(it->first)!=r_set.end()) 
          return true;
      }
    }
    else {
      for (auto it : r_set) {
        if (l_map.find(it)!=l_map.end()) 
          return true;
      }
    }
    return false;
  }

  double DFSComputeVariable(vertex_t s, std::unordered_set<vertex_t>& visited) {
    if (visited.find(s)!=visited.end()) return g[s].value;
    if (g[s].vt==Input) {
      if (changedEDBs.find(g[s].name)!=changedEDBs.end()) {
        std::cout << "find changed edb: " << g[s].name << ", previous value: " << g[s].value << ", changed value: " << changedEDBs[g[s].name] << std::endl;
        g[s].value = changedEDBs[g[s].name];
      }
      visited.insert(s);
      return g[s].value;
    }
    else if (g[s].vt==Parameter) {
      visited.insert(s);
      return g[s].value;
    }
    else if (!hasIntersection(changedEDBs, g[s].EDBs)) {
      visited.insert(s);
      return g[s].value;
    }
    // if the current vertex is a derived veriable that depends on changed inputs
    double ret;
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    switch (g[v_operator].vt) {
      case(Sum): ret = DFSComputeSum(v_operator, visited); break;
      case(Mul): ret = DFSComputeMul(v_operator, visited); break;
      case(Div): ret = DFSComputeDiv(v_operator, visited); break;
      case(Scale): ret = DFSComputeScale(v_operator, visited); break;
    }
    g[s].value = ret;
    visited.insert(s);
    return ret;
  }

  double DFSComputeSum(vertex_t s, std::unordered_set<vertex_t>& visited) {
    double ret = 0;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret += DFSComputeVariable(v, visited);
    }
    return ret;
  }

  double DFSComputeMul(vertex_t s, std::unordered_set<vertex_t>& visited) {
    double ret = 1;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret *= DFSComputeVariable(v, visited);
    }
    return ret;
  }

  double DFSComputeDiv(vertex_t s, std::unordered_set<vertex_t>& visited) {
    double ret = 1;
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g);
    vertex_t v_numerator, v_denominator;
    if (g[*ai].name==g[s].params["numerator_name"]) {
      v_numerator = *ai;
      v_denominator = *(ai+1);
    }
    else {
      v_numerator = *(ai+1);
      v_denominator = *ai;
    }
    double numerator = DFSComputeVariable(v_numerator, visited);
    double denominator = DFSComputeVariable(v_denominator, visited);
    return numerator/denominator;
  }

  double DFSComputeScale(vertex_t s, std::unordered_set<vertex_t>& visited) {
    double ret = 0;
    double numerator; 
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      double tmp = DFSComputeVariable(v, visited);
      if (g[v].name==g[s].params["numerator_name"]) {
        numerator = tmp;
      }
      ret += tmp;
    }
    return numerator/ret;
  }



  /* provenance-enabled approximate subgraph query */
public:
  CProvGraph ApproximateSubGraphQuery(std::string& name, double epsilon) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);
    ASSERT_EX(g[v].vt==Derived, std::cout << name << " is a " << vertexTypeToString(g[v].vt) << " vertex");
    double target = g[v].value;
    
    // initialize CProvGraph of the the approximate query result
    std::string new_save_path = save_path.substr(0, save_path.find("."));
    new_save_path += "-approx.dot";

    std::priority_queue<ApproxSubGraphDiff, std::vector<ApproxSubGraphDiff>, ApproxSubGraphDiffComparison> pq;
    std::unordered_set<std::unordered_set<std::string>, EDBSetHash> visited;

    double min_diff = 1.0;
    std::unordered_set<std::string> min_set;
    std::unordered_set<std::string> includedEDBs;
    for (std::string EDB : g[v].EDBs) {
      std::unordered_set<std::string> includedEDBs;
      includedEDBs.insert(EDB);
      CProvGraph approxSubProvG(new_save_path);
      approxSubProvG.addVariableVertex(Derived, name, 0);
      DFSApproximateSubGraphQuery(v, approxSubProvG, includedEDBs);
      double diff = std::abs(target-approxSubProvG.getVertexValueByName(name));
      ApproxSubGraphDiff asgd(includedEDBs, diff);
      if (asgd.diff<min_diff) {
        min_set = asgd.EDBs;
        min_diff = asgd.diff;
      }
      if (min_diff<epsilon) {
        return approxSubProvG;
      }
      pq.push(asgd);
      visited.insert(includedEDBs);
    }

    int iteration = 1;
    while (!pq.empty() && visited.size()<MAX_NUM_OF_SEARCH) {
      ApproxSubGraphDiff asgd = pq.top();
      pq.pop();
      if (asgd.diff<min_diff) {
        min_set = asgd.EDBs;
        min_diff = asgd.diff;
      }
      std::cout << "iteration: " << iteration << ", queue size: " << pq.size() << ", current min diff: " << min_diff << std::endl;
      if (min_diff<epsilon) {
        break;
      }
      // search neighbors of asgd.EDBs
      unordered_set<std::string> includedEDBs = asgd.EDBs;
      for (std::string EDB : g[v].EDBs) {
        if (includedEDBs.find(EDB)==includedEDBs.end()) {
          includedEDBs.insert(EDB);
          if (visited.find(includedEDBs)!=visited.end()) continue;
          CProvGraph approxSubProvG(new_save_path);
          approxSubProvG.addVariableVertex(Derived, name, 0);
          DFSApproximateSubGraphQuery(v, approxSubProvG, includedEDBs);
          ApproxSubGraphDiff new_asgd(includedEDBs, std::abs(approxSubProvG.getVertexValueByName(name)-target));
          pq.push(new_asgd);
          visited.insert(includedEDBs);
          includedEDBs.erase(EDB);
        }
      }
      iteration++;
    }

    CProvGraph approxSubProvG(new_save_path);
    approxSubProvG.addVariableVertex(Derived, name, 0);
    DFSApproximateSubGraphQuery(v, approxSubProvG, min_set);
    return approxSubProvG;
  }

  void updateVertexValue(vertex_t s) {
    if (g[s].vt==Input || g[s].vt==Parameter) return;
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    switch (g[v_operator].vt) {
      case(Sum): g[s].value = computeSum(v_operator); break;
      case(Mul): g[s].value = computeMul(v_operator); break;
      case(Div): g[s].value = computeDiv(v_operator); break;
      case(Scale): g[s].value = computeScale(v_operator); break;
    }
  }

  void updateVertexEDBs(vertex_t s) {
    std::unordered_set<std::string> ret;
    if (g[s].vt==Input) {
      ret.insert(g[s].name);
      g[s].EDBs = ret;
      return;
    }
    else if (g[s].vt==Parameter) {
      g[s].EDBs = ret;
      return;
    }
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      for (std::string EDB : g[v].EDBs) {
        ret.insert(EDB);
      }
    }
    g[s].EDBs = ret;
    g[v_operator].EDBs = ret;
  }


private:
  void DFSApproximateSubGraphQuery(vertex_t s, CProvGraph& approxSubProvG, std::unordered_set<std::string>& includedEDBs) {
    vertex_t parent = approxSubProvG.getVertexByName(g[s].name);
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    vertex_t a_operator = approxSubProvG.addOperatorVertex(g[v_operator].vt, g[v_operator].name, g[v_operator].params);
    approxSubProvG.addProvEdge(parent, a_operator);
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      vertex_t child;
      if (approxSubProvG.checkVertexExistByName(g[v].name)) {
        child = approxSubProvG.getVertexByName(g[v].name);
        approxSubProvG.addProvEdge(a_operator, child);
      } 
      else {
        if (g[v].vt==Input) {
          if (includedEDBs.find(g[v].name)!=includedEDBs.end()) {
            child = approxSubProvG.addVariableVertex(Input, g[v].name, g[v].value);
            approxSubProvG.addProvEdge(a_operator, child);
            approxSubProvG.updateVertexEDBs(child);
          }
        }
        else if (g[v].vt==Parameter) {
          child = approxSubProvG.addVariableVertex(Parameter, g[v].name, g[v].value);
          approxSubProvG.addProvEdge(a_operator, child);
        }
        else {
          if (hasIntersection(g[v].EDBs, includedEDBs)) {
            child = approxSubProvG.addVariableVertex(Derived, g[v].name, 0);
            DFSApproximateSubGraphQuery(v, approxSubProvG, includedEDBs);
            approxSubProvG.addProvEdge(a_operator, child);
          }
        }
      }
    }
    approxSubProvG.updateVertexEDBs(parent);
    approxSubProvG.updateVertexValue(parent);
    return;
  }

  double computeSum(vertex_t s) {
    double ret = 0;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret += g[v].value;
    }
    return ret;
  }

  double computeMul(vertex_t s) {
    double ret = 1;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret *= g[v].value;
    }
    return ret;
  }

  double computeDiv(vertex_t s) {
    double ret = 1;
    double numerator = 0;
    double denominator = 1;
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g);
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      if (g[v].name==g[s].params["numerator_name"]) {
        numerator = g[v].value;
      }
      else {
        denominator = 1.0/g[v].value;
      }
    }
    ret = numerator*denominator;
    return ret;
  }

  double computeScale(vertex_t s) {
    double sum = 0;
    double numerator = 0; 
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      if (g[v].name==g[s].params["numerator_name"]) {
        numerator = g[v].value;
      }
      sum += g[v].value;
    }
    return numerator/sum;
  }
  



 /* print and save functions */
public:
  void printVertex(vertex_t v) {
    std::cout << g[v].vt << " vertex ";
    std::cout << g[v].name << ", value=" << g[v].value << std::endl;
    std::cout << "EDBs: ";
    for (auto EDB : g[v].EDBs) {
      std::cout << EDB << " ";
    }
    cout << std::endl;
  }

  void printGraph() {
    // write_graphviz (std::cout, g, boost::make_label_writer(boost::get(&ProvVertex::name, g)));
    write_graphviz (std::cout, g, my_node_writer(g));
  }

  void setSavePath(const std::string& sp) {
    save_path = sp;
  }

  void saveGraph() {
    std::ofstream fout(save_path);
    write_graphviz (fout, g, my_node_writer(g));
    fout.close();
  }

  ~CProvGraph() {};


 /* class members */
private:
  Graph g;
  std::string save_path = "/home/jz598/MLNInfer/data/CProv/raw/test.dot";
  std::unordered_map<std::string, vertex_t> vertex_set;
  std::unordered_map<std::string, double> changedEDBs;
};

#endif