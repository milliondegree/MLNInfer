#ifndef _CPROVGRAPH_H
#define _CPROVGRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <string>
#include <queue>
#include <unordered_set>
#include <unordered_map>


#ifndef NDEBUG
#define ASSERT_EX(condition, statement) \
    do { \
        if (!(condition)) { statement; assert(condition); } \
    } while (false)
#else
#define ASSERT_EX(condition, statement) ((void)0)
#endif

#define MAX_NUM_OF_SEARCH 100
#define ALPHA 0.5
#define BETA 0.5

enum VertexType {Input, Derived, Parameter, Sum, Mul, Div, Scale, NodeToClique, CliqueToNode, BeliefPropagation};

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
      PROCESS_VAL(NodeToClique)
      PROCESS_VAL(CliqueToNode)
      PROCESS_VAL(BeliefPropagation)
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
    case(NodeToClique): s = "nodetoclique"; break;
    case(CliqueToNode): s = "cliquetonode"; break;
    case(BeliefPropagation): s = "beliefpropagation"; break;
  }
  return s;
}

struct ProvVertex {
  VertexType vt;
  std::string name;
  float value;
  float contribution;
  float derivative;
  std::unordered_map<std::string, std::string> params;
  std::unordered_set<std::string> EDBs;
};

struct ProvEdge{
  float contribution;
  float derivative;
  float importance;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, ProvVertex, ProvEdge> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
typedef boost::graph_traits<Graph>::edge_iterator edge_iter;
typedef boost::graph_traits<Graph>::in_edge_iterator in_edge_iter;
typedef boost::graph_traits<Graph>::out_edge_iterator out_edge_iter;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_tier;

struct my_node_writer {
  my_node_writer(Graph& g_) : g (g_) {};
  template <class Vertex>
  void operator()(std::ostream& out, Vertex v) {
    if (g[v].vt==Input) 
      out << " [label=\"" << g[v].name << ": " << g[v].value << ", d: " << g[v].derivative << "\", shape=oval, color=dodgerblue]";
    else if (g[v].vt==Derived) 
      out << " [label=\"" << g[v].name << ": " << g[v].value << ", d: " << g[v].derivative << "\", shape=oval, color=limegreen]";
    else if (g[v].vt==Parameter) 
      out << " [label=\"" << g[v].name << ": " << g[v].value << ", d: " << g[v].derivative << "\", shape=oval, color=darkviolet]";
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
    out << " [label=\"";
    if (abs(g[e].contribution)>1e-6&&abs(g[e].contribution)<1) {
      out << "Con: " << g[e].contribution << ' ';
    }
    if (abs(g[e].derivative)>1e-6&&abs(g[e].derivative)<1) {
      out << "Dev: " << g[e].derivative;
    }
    out << "\"]";
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

struct edge_comparer {
  Graph g;
  edge_comparer(Graph& c_g): g(c_g) {}
  bool operator() (edge_t e1, edge_t e2) const {
    return g[e1].importance < g[e2].importance;
  }
};

class ApproxSubGraphDiff {
public: 
  std::unordered_set<std::string> EDBs;
  float value_diff;
  float derivative_diff;
  ApproxSubGraphDiff() {}
  ApproxSubGraphDiff(const std::unordered_set<std::string>& s, float v_d, float d_d): EDBs(s), value_diff(d_d), derivative_diff(d_d) {}

  bool operator == (const ApproxSubGraphDiff& asgd2) const {
    return this->EDBs==asgd2.EDBs;
  }

  inline float getDiff() {
    return value_diff*ALPHA+derivative_diff*BETA;
  }
};

class ApproxSubGraphDiffComparison {
public: 
  ApproxSubGraphDiffComparison() {};
  bool operator () (const ApproxSubGraphDiff& sd1, const ApproxSubGraphDiff& sd2) {
    return sd1.value_diff*ALPHA+sd1.derivative_diff*BETA>sd2.value_diff*ALPHA+sd2.derivative_diff*BETA; 
  }
};

class EDBSetHash {
public:
  size_t operator () (const std::unordered_set<std::string>& EDBs) const {
    std::hash<std::string> hasher;
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

  CProvGraph(const std::string& sp): save_path(sp) {}

  /* read functions */
  inline vertex_t getVertexByName(const std::string& name) {
    return vertex_set[name];
  }

  inline std::string edgeToString(const edge_t e) {
    return std::to_string(e.m_source)+"_"+std::to_string(e.m_target);
  }

  inline vertex_t checkVertexExistByName(const std::string& name) {
    return vertex_set.find(name)!=vertex_set.end();
  }

  inline std::unordered_set<std::string> getVertexEBDs(vertex_t v) {
    return g[v].EDBs;
  }

  inline float getVertexDerivativeByName(const std::string& name) {
    return g[getVertexByName(name)].derivative;
  }

  inline std::unordered_set<std::string> getVertexEBDsByName(const std::string& name) {
    return g[getVertexByName(name)].EDBs;
  }

  inline float getVertexValueByName(const std::string& name) {
    return g[getVertexByName(name)].value;
  }


  /* write functions */
  vertex_t addVariableVertex(const VertexType vt, const std::string& name, float value) {
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

  void addComputingSubgraph(const std::string& output_name, const float value, VertexType vt, const std::vector<std::string>& input_names) {
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

  void addComputingSubgraph(const std::string& output_name, const float value, VertexType vt, const std::vector<std::string>& input_names, const std::unordered_map<std::string, std::string>& params) {
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

  inline void addProvEdge(vertex_t v1, vertex_t v2, float contribution) {
    edge_t e;
    bool b;
    boost::tie(e, b) = boost::add_edge(v1, v2, g);
    g[e].contribution = contribution;
  }

  inline void setVertexValue(vertex_t v, float value) {
    g[v].value = value;
  }

  inline void setVertexEDBs(vertex_t v, std::unordered_set<std::string>& EDBs) {
    g[v].EDBs = EDBs;
  }

  inline void setChangedEDBs(std::unordered_map<std::string, float>& m) {
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

    // std::cout << "start provenance query" << std::endl;
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
        child = subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].value);
        subProvG.addProvEdge(a_operator, child);
        DFSProvQuery(v, subProvG);
      }
    }
    subProvG.updateVertexEDBs(parent);
    return;
  }

public:
  void computeDerivative(const std::string& name) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);

    // insert derivative of the source vertex
    g[v].derivative = 1.0;
    
    // std::cout << "start to compute derivative" << std::endl;
    std::unordered_set<std::string> visited;
    DFSComputeDerivative(v, visited);
  }

private: 
  void DFSComputeDerivative(vertex_t s, std::unordered_set<std::string>& visited) {
    if (g[s].vt==Input || g[s].vt==Parameter) return;
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    switch (g[v_operator].vt) {
      case Sum: DFSComputeSumDerivative(v_operator, g[s].derivative, visited); break;
      case Mul: DFSComputeMulDerivative(v_operator, g[s].derivative, visited); break;
      case Div: DFSComputeDivDerivative(v_operator, g[s].derivative, visited); break;
      case Scale: DFSComputeScaleDerivative(v_operator, g[s].derivative, visited); break;
      default: std::cout << "this is not an operator vertex\n"; exit(1);
    }
  }

  void DFSComputeSumDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited) {
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      edge_t e = boost::edge(s, v, g).first;
      g[e].derivative = 1.0*d;
      visited.insert(edgeToString(e));
      bool all_visited = true;
      in_edge_iter ei, ei_end;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
        if (visited.find(edgeToString(*ei))==visited.end()) {
          all_visited = false;
          break;
        }
      }
      if (all_visited) {
        g[v].derivative = 0.0;
        for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
          g[v].derivative += g[*ei].derivative;
        // std::cout << "derivative of " << g[v].name << " is " << g[v].derivative << std::endl;
        DFSComputeDerivative(v, visited);
      }
    }
  }

  void DFSComputeMulDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited) {
    adjacency_tier ai, ai_end;
    float value = 1.0;
    int count = 0;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      value *= g[v].value;
      count += 1;
    }
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      edge_t e = boost::edge(s, v, g).first;
      if (count==1) g[e].derivative = d;
      else g[e].derivative = d*value/g[v].value;
      visited.insert(edgeToString(e));
      bool all_visited = true;
      in_edge_iter ei, ei_end;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
        if (visited.find(edgeToString(*ei))==visited.end()) {
          all_visited = false;
          break;
        }
      }
      if (all_visited) {
        g[v].derivative = 0.0;
        for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
          g[v].derivative += g[*ei].derivative;
        // std::cout << "derivative of " << g[v].name << " is " << g[v].derivative << std::endl;
        DFSComputeDerivative(v, visited);
      }
    }
  }

  void DFSComputeDivDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited) {
    adjacency_tier ai, ai_end;
    float numerator = 0;
    float denominator = 1;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      if (g[v].name==g[s].params["numerator_name"]) 
        numerator = g[v].value;
      else 
        denominator = g[v].value;
    }
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      edge_t e = boost::edge(s, v, g).first;
      if (g[v].name==g[s].params["numerator_name"]) 
        g[e].derivative = d*(1.0/denominator);
      else 
        g[e].derivative = d*(-1.0*numerator/std::pow(denominator, 2));
      visited.insert(edgeToString(e));
      bool all_visited = true;
      in_edge_iter ei, ei_end;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
        if (visited.find(edgeToString(*ei))==visited.end()) {
          all_visited = false;
          break;
        }
      }
      if (all_visited) {
        g[v].derivative = 0.0;
        for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
          g[v].derivative += g[*ei].derivative;
        // std::cout << "derivative of " << g[v].name << " is " << g[v].derivative << std::endl;
        DFSComputeDerivative(v, visited);
      }
    }
  }

  void DFSComputeScaleDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited) {
    float denominator = 0;
    float numerator = 0; 
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      denominator += g[v].value;
      if (g[v].name==g[s].params["numerator_name"])
        numerator = g[v].value;
    }
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      edge_t e = boost::edge(s, v, g).first;
      if (g[v].name==g[s].params["numerator_name"]) {
        g[e].derivative = d*((1.0/denominator)-(numerator/std::pow(denominator, 2)));
      }
      else {
        g[e].derivative = d*(-1.0*numerator/std::pow(denominator, 2));
      }
      visited.insert(edgeToString(e));
      bool all_visited = true;
      in_edge_iter ei, ei_end;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
        if (visited.find(edgeToString(*ei))==visited.end()) {
          all_visited = false;
          break;
        }
      }
      if (all_visited) {
        g[v].derivative = 0.0;
        for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
          g[v].derivative += g[*ei].derivative;
        // std::cout << "derivative of " << g[v].name << " is " << g[v].derivative << std::endl;
        DFSComputeDerivative(v, visited);
      }
    }
  }

public:
  void computeContributions(const std::string& name) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);
    float previous_value = g[v].value;
    edge_iter ei_begin, ei_end, ei;
    boost::tie(ei_begin, ei_end) = boost::edges(g);
    std::vector<edge_t> edge_list;
    for (edge_iter ei=ei_begin; ei!=ei_end; ei++) edge_list.push_back(*ei);
    for (edge_t e : edge_list) {
      vertex_t v_source, v_target;
      v_source = boost::source(e, g);
      v_target = boost::target(e, g);
      if (g[v_source].vt!=Derived) {
        boost::remove_edge(v_source, v_target, g);
        float new_value = computeVariable(name);
        addProvEdge(v_source, v_target, previous_value-new_value);
      }
    }
  }




  /* provenance-enabled model inference */
public: 
  inline float computeVariableWithChangedEDBs(const std::string& name, const std::unordered_map<std::string, float>& EDBs) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);
    changedEDBs = EDBs;
    std::unordered_set<vertex_t> visited;
    return DFSComputeVariable(v, visited);
  }

  inline float computeVariable(const std::string& name) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);
    std::unordered_set<vertex_t> visited;
    return DFSComputeVariableNoEDB(v, visited);
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

  bool hasIntersection(const std::unordered_map<std::string, float>& l_map, const std::unordered_set<std::string>& r_set) {
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

  float DFSComputeVariable(vertex_t s, std::unordered_set<vertex_t>& visited) {
    // std::cout << "entering " << g[s].name << std::endl;
    if (visited.find(s)!=visited.end()) return g[s].value;
    if (g[s].vt==Input) {
      if (changedEDBs.find(g[s].name)!=changedEDBs.end()) {
        // std::cout << "find changed edb: " << g[s].name << ", previous value: " << g[s].value << ", changed value: " << changedEDBs[g[s].name] << std::endl;
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
    // std::cout << "first time " << g[s].name << std::endl;
    // if the current vertex is a derived veriable that depends on changed inputs
    float ret;
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    if (boost::out_degree(v_operator, g)==0) {
      g[s].value = 0;
      return 0;
    }
    switch (g[v_operator].vt) {
      case Sum: ret = DFSComputeSum(v_operator, visited); break;
      case Mul: ret = DFSComputeMul(v_operator, visited); break;
      case Div: ret = DFSComputeDiv(v_operator, visited); break;
      case Scale: ret = DFSComputeScale(v_operator, visited); break;
      default: std::cout << "this is not an operator vertex\n"; exit(1);
    }
    // std::cout << "finish " << g[s].name << std::endl;
    g[s].value = ret;
    visited.insert(s);
    return ret;
  }

  float DFSComputeVariableNoEDB(vertex_t s, std::unordered_set<vertex_t>& visited) {
    if (visited.find(s)!=visited.end()) return g[s].value;
    if (g[s].vt==Input) {
      visited.insert(s);
      return g[s].value;
    }
    else if (g[s].vt==Parameter) {
      visited.insert(s);
      return g[s].value;
    }
    // if the current vertex is a derived veriable that depends on changed inputs
    float ret;
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    if (boost::out_degree(v_operator, g)==0) {
      g[s].value = 0;
      return 0;
    }
    switch (g[v_operator].vt) {
      case Sum: {
        ret = 0;
        adjacency_tier ai, ai_end;
        for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
          vertex_t v = *ai;
          ret += DFSComputeVariableNoEDB(v, visited);
        }
        break;
      }
      case Mul: {
        ret = 1;
        adjacency_tier ai, ai_end;
        for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
          vertex_t v = *ai;
          ret *= DFSComputeVariableNoEDB(v, visited);
        }
        break;
      }
      case Div: {
        float numerator = 0, denominator = 1;
        adjacency_tier ai, ai_end;
        for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
          vertex_t v = *ai;
          if (g[v].name==g[v_operator].params["numerator_name"]) {
            numerator = DFSComputeVariableNoEDB(v, visited);
          }
          else {
            denominator = DFSComputeVariableNoEDB(v, visited);
          }
        }
        if (denominator==0) ret = 0;
        else ret = numerator/denominator;
        break;
      }
      case Scale: {
        float denominator = 0, numerator = 0; 
        adjacency_tier ai, ai_end;
        for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
          vertex_t v = *ai;
          float tmp = DFSComputeVariableNoEDB(v, visited);
          if (g[v].name==g[v_operator].params["numerator_name"]) {
            numerator = tmp;
          }
          denominator += tmp;
        }
        if (denominator==0) ret = 0;
        else ret = numerator/denominator;
        break;
      }
      default: std::cout << "this is not an operator vertex\n"; exit(1);
    }
    g[s].value = ret;
    visited.insert(s);
    return ret;
  }

  float DFSComputeSum(vertex_t s, std::unordered_set<vertex_t>& visited) {
    float ret = 0;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret += DFSComputeVariable(v, visited);
    }
    return ret;
  }

  float DFSComputeMul(vertex_t s, std::unordered_set<vertex_t>& visited) {
    float ret = 1;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret *= DFSComputeVariable(v, visited);
    }
    return ret;
  }

  float DFSComputeDiv(vertex_t s, std::unordered_set<vertex_t>& visited) {
    float numerator=0, denominator=0;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      float tmp = DFSComputeVariable(v, visited);
      if (g[v].name==g[s].params["numerator_name"]) {
        numerator = tmp;
      }
      else {
        denominator = tmp;
      }
    }
    if (denominator==0) return 0;
    return numerator/denominator;
  }

  float DFSComputeScale(vertex_t s, std::unordered_set<vertex_t>& visited) {
    float denominator = 0;
    float numerator = 0; 
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      float tmp = DFSComputeVariable(v, visited);
      if (g[v].name==g[s].params["numerator_name"]) {
        numerator = tmp;
      }
      denominator += tmp;
    }
    if (denominator==0) return 0;
    return numerator/denominator;
  }





  /* provenance-enabled approximate subgraph query */
public:
  CProvGraph ApproximateSubGraphQuery(std::string& name, float epsilon, float lambda) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);
    ASSERT_EX(g[v].vt==Derived, std::cout << name << " is a " << vertexTypeToString(g[v].vt) << " vertex");
    float target = g[v].value;
    computeDerivative(name);
    std::unordered_map<std::string, float> target_derivatives;
    for (std::string edb : getVertexEBDsByName(name)) {
      target_derivatives[edb] = getVertexDerivativeByName(edb);
    }
    // std::cout << "target edb size: " << target_derivatives.size() << std::endl;
    
    // initialize CProvGraph of the the approximate query result
    std::string new_save_path = save_path.substr(0, save_path.find("."));
    new_save_path += "-approx.dot";

    std::priority_queue<ApproxSubGraphDiff, std::vector<ApproxSubGraphDiff>, ApproxSubGraphDiffComparison> pq;
    std::unordered_set<std::unordered_set<std::string>, EDBSetHash> visited;

    float min_diff = 1.0;
    float min_value_diff = 1.0;
    float min_derivative_diff = 1.0;
    std::unordered_set<std::string> min_set;
    std::unordered_set<std::string> includedEDBs;
    for (std::string EDB : g[v].EDBs) {
      std::unordered_set<std::string> includedEDBs;
      includedEDBs.insert(EDB);
      CProvGraph approxSubProvG(new_save_path);
      approxSubProvG.addVariableVertex(Derived, name, 0);
      DFSApproximateSubGraphQuery(v, approxSubProvG, includedEDBs);
      float value_diff = std::abs(target-approxSubProvG.getVertexValueByName(name));
      approxSubProvG.computeDerivative(name);
      // std::cout << "approx derivative finish" << std::endl;
      std::unordered_map<std::string, float> approx_derivatives;
      for (std::string edb : approxSubProvG.getVertexEBDsByName(name)) {
        approx_derivatives[edb] = approxSubProvG.getVertexDerivativeByName(edb);
      }
      // std::cout << "approx edb size: " << approx_derivatives.size() << std::endl;
      float derivative_diff = computeDerivativeDiff(target_derivatives, approx_derivatives);
      ApproxSubGraphDiff asgd(includedEDBs, value_diff, derivative_diff);
      if (asgd.getDiff()<min_diff) {
        min_set = asgd.EDBs;
        min_diff = asgd.getDiff();
        min_value_diff = asgd.value_diff;
        min_derivative_diff = asgd.derivative_diff;
      }
      if (min_value_diff<epsilon && min_derivative_diff<lambda) {
        return approxSubProvG;
      }
      pq.push(asgd);
      visited.insert(includedEDBs);
    }

    int iteration = 1;
    while (!pq.empty() && visited.size()<MAX_NUM_OF_SEARCH) {
      ApproxSubGraphDiff asgd = pq.top();
      pq.pop();
      if (asgd.getDiff()<min_diff) {
        min_set = asgd.EDBs;
        min_diff = asgd.getDiff();
        min_value_diff = asgd.value_diff;
        min_derivative_diff = asgd.derivative_diff;
      }
      if (min_value_diff<epsilon && min_derivative_diff<lambda) {
        break;
      }
      // search neighbors of asgd.EDBs
      std::unordered_set<std::string> includedEDBs = asgd.EDBs;
      for (std::string EDB : g[v].EDBs) {
        if (includedEDBs.find(EDB)==includedEDBs.end()) {
          includedEDBs.insert(EDB);
          if (visited.find(includedEDBs)!=visited.end()) continue;
          CProvGraph approxSubProvG(new_save_path);
          approxSubProvG.addVariableVertex(Derived, name, 0);
          DFSApproximateSubGraphQuery(v, approxSubProvG, includedEDBs);
          float value_diff = std::abs(target-approxSubProvG.getVertexValueByName(name));
          approxSubProvG.computeDerivative(name);
          std::unordered_map<std::string, float> approx_derivatives;
          for (std::string edb : approxSubProvG.getVertexEBDsByName(name)) {
            approx_derivatives[edb] = approxSubProvG.getVertexDerivativeByName(edb);
          }
          float derivative_diff = computeDerivativeDiff(target_derivatives, approx_derivatives);
          ApproxSubGraphDiff new_asgd(includedEDBs, value_diff, derivative_diff);
          pq.push(new_asgd);
          visited.insert(includedEDBs);
          includedEDBs.erase(EDB);
        }
      }
      iteration++;
    }

    // std::cout << "iteration: " << iteration << ", min value diff: " << min_value_diff << ", min derivative diff: " << min_derivative_diff << std::endl;
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
      default: std::cout << "this is not an operator vertex\n"; exit(1);
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

  float computeSum(vertex_t s) {
    float ret = 0;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret += g[v].value;
    }
    return ret;
  }

  float computeMul(vertex_t s) {
    float ret = 1;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret *= g[v].value;
    }
    return ret;
  }

  float computeDiv(vertex_t s) {
    float ret = 1;
    float numerator = 0;
    float denominator = 1;
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

  float computeScale(vertex_t s) {
    float sum = 0;
    float numerator = 0; 
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

  float computeDerivativeDiff(std::unordered_map<std::string, float>& target_derivatives, std::unordered_map<std::string, float>& approx_derivatives) {
    float ret = 0;
    for (auto it : approx_derivatives) {
      std::string edb = it.first;
      ret += std::pow(std::abs(target_derivatives[edb]-it.second), 2);
    }
    return std::pow(ret, 0.5);
  }


public:
  CProvGraph ApproximateSubGraphQueryPrune(std::string& name, float epsilon, float lambda) {
    computeContributions(name);
    computeDerivative(name);

    vertex_t v = getVertexByName(name);
    float target = g[v].value;
    std::unordered_map<std::string, float> target_derivatives;
    for (std::string edb : getVertexEBDsByName(name)) {
      target_derivatives[edb] = getVertexDerivativeByName(edb);
    }

    edge_iter ei_begin, ei_end, ei;
    boost::tie(ei_begin, ei_end) = boost::edges(g);
    std::vector<edge_t> edge_list;
    for (edge_iter ei=ei_begin; ei!=ei_end; ei++) {
      vertex_t v_source = boost::source(*ei, g);
      vertex_t v_target = boost::target(*ei, g);
      if (g[v_source].vt!=Derived) {
        // std::cout << abs(g[*ei].contribution) << ' ' << std::abs(g[*ei].derivative) << std::endl;
        g[*ei].importance = ALPHA*std::abs(g[*ei].contribution)+BETA*std::abs(g[*ei].derivative);
        edge_list.push_back(*ei);
      }
    }
    edge_comparer ec(g);
    std::sort(edge_list.begin(), edge_list.end(), ec);
    int count = 0;
    for (edge_t e : edge_list) {
      vertex_t v_source = boost::source(e, g);
      vertex_t v_target = boost::target(e, g);
      boost::remove_edge(v_source, v_target, g);
      count += 1;
      if (count%10==0) {
        CProvGraph approxSubProvG = ProvenanceQuery(name);
        float approxiValue = approxSubProvG.computeVariable(name);
        float value_diff = std::abs(target-approxSubProvG.getVertexValueByName(name));
        approxSubProvG.computeDerivative(name);
        std::unordered_map<std::string, float> approx_derivatives;
        for (std::string edb : approxSubProvG.getVertexEBDsByName(name)) {
          approx_derivatives[edb] = approxSubProvG.getVertexDerivativeByName(edb);
        }
        float derivative_diff = computeDerivativeDiff(target_derivatives, approx_derivatives);
        if (value_diff>epsilon||derivative_diff>lambda) break;
      }
    }
    CProvGraph ret = ProvenanceQuery(name);
    std::string new_save_path = save_path.substr(0, save_path.find("."));
    new_save_path += "-approx.dot";
    ret.setSavePath(new_save_path);
    // std::cout << count << std::endl;
    return ret;
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
    std::cout << std::endl;
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
    write_graphviz (fout, g, my_node_writer(g), my_edge_writer(g));
    fout.close();
  }

  ~CProvGraph() {};


 /* class members */
private:
  Graph g;
  std::string save_path = "/home/jz598/MLNInfer/data/CProv/raw/test.dot";
  std::unordered_map<std::string, vertex_t> vertex_set;
  std::unordered_map<std::string, float> changedEDBs;
};

#endif