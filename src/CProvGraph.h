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

enum VertexType {Variable, Sum, Mul, Div, Scale};

inline std::ostream& operator<<(std::ostream& out, const VertexType value){
  const char* s = 0;
  #define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value) {
      PROCESS_VAL(Variable);     
      PROCESS_VAL(Sum);
      PROCESS_VAL(Mul);
      PROCESS_VAL(Div);
      PROCESS_VAL(Scale);
    }
  #undef PROCESS_VAL
    return out << s;
}

inline std::string vertexTypeToString(VertexType vt) {
  string s;
  switch(vt) {
    case(Variable): s = "var"; break;
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
  bool isEDB;
  double value;
  std::unordered_map<string, double> params;
  std::unordered_set<string> EDBs;
};

struct ProvEdge{
  float weight;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, ProvVertex, ProvEdge > Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_tier;

class CProv_BFS_Visitor : public boost::default_bfs_visitor {
public:
    template <typename Vertex, typename Graph>
    void discover_vertex(Vertex u, const Graph& g)
    {
      std::cout << "Visit " << g[u].vt << " \"" << g[u].name << "\"";
      if (g[u].vt==Variable) {
        std::cout << ", value=" << g[u].value << ", isEDB=" << g[u].isEDB << std::endl;
      }
      else {
        std::cout << std::endl;
      }
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

  inline unordered_set<string> getVertexEBDs(vertex_t v) {
    return g[v].EDBs;
  }


  /* write functions */
  vertex_t addVariableVertex(const VertexType vt, const std::string& name, bool isEDB, float value) {
    ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
    vertex_t v = boost::add_vertex(g);
    vertex_set.insert(std::make_pair(name, v));
    g[v].vt = vt;
    g[v].name = name;
    g[v].isEDB = isEDB;
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

  vertex_t addOperatorVertex(const VertexType vt, const std::string& name, const std::unordered_map<string, double>& params) {
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
      v_output = addVariableVertex(Variable, output_name, false, value);
      v_operator = addOperatorVertex(vt, vertexTypeToString(vt)+"_"+output_name);
      addProvEdge(v_output, v_operator);
    }
    for (auto s : input_names) {
      addProvEdge(v_operator, getVertexByName(s));
    }
  }

  void addComputingSubgraph(const std::string& output_name, const double value, VertexType vt, const std::vector<std::string>& input_names, const std::unordered_map<string, double>& params) {
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
      v_output = addVariableVertex(Variable, output_name, false, value);
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

  inline void setVertexEDBs(vertex_t v, std::unordered_set<string>& EDBs) {
    g[v].EDBs = EDBs;
  }

  

  


  /* provenance query functions */
  CProvGraph traceProvOfVariableByName(const std::string& name) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);
    
    // initialize CProvGraph of the queried vertex
    std::string new_save_path = save_path.substr(0, save_path.find("."));
    new_save_path += "-"+name+".dot";
    CProvGraph subProvG(new_save_path);

    // insert the source vertex to subProvG
    assert(g[v].vt==Variable);
    subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].isEDB, g[v].value);

    std::cout << "start provenance query" << std::endl;
    DFSProvQuery(v, subProvG);
    return subProvG;
  }

private:
  void DFSProvQuery(vertex_t s, CProvGraph& subProvG) {
    vertex_t parent = subProvG.getVertexByName(g[s].name);
    unordered_set<string> s_tmp;
    if (g[s].isEDB) {
      s_tmp.insert(g[s].name);
      subProvG.setVertexEDBs(parent, s_tmp);
      return;
    }
    // start iterating adjacent vertices
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      vertex_t child;
      if (subProvG.checkVertexExistByName(g[v].name)) {
        child = subProvG.getVertexByName(g[v].name);
        subProvG.addProvEdge(parent, child);
      }
      else {
        if (g[v].vt==Variable) child = subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].isEDB, g[v].value);
        else child = subProvG.addOperatorVertex(g[v].vt, g[v].name);
        subProvG.addProvEdge(parent, child);
        DFSProvQuery(v, subProvG);
      }
      for (auto EDB : subProvG.getVertexEBDs(child)) {
        s_tmp.insert(EDB);
      }
    }
    subProvG.setVertexEDBs(parent, s_tmp);
    return;
  }


  /* provenance-enable XAI functions */
public: 
  double computeVariableByName(const std::string& name) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);
    return DFSComputeVariable(v);
  }

private:
  double DFSComputeVariable(vertex_t s) {
    assert(g[s].vt==Variable);
    if (g[s].isEDB) return g[s].value;
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    switch (g[v_operator].vt) {
      case(Sum): return DFSComputeSum(v_operator); break;
      case(Mul): return DFSComputeMul(v_operator); break;
      case(Div): return DFSComputeDiv(v_operator); break;
      case(Scale): return DFSComputeScale(v_operator); break;
    }
  }

  double DFSComputeSum(vertex_t s) {
    assert(g[s].vt==Sum);
    double ret = 0;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret += DFSComputeVariable(v);
    }
    return ret;
  }

  double DFSComputeMul(vertex_t s) {
    assert(g[s].vt==Mul);
    double ret = 1;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret *= DFSComputeVariable(v);
    }
    return ret;
  }

  double DFSComputeDiv(vertex_t s) {
    assert(g[s].vt==Div);
    double ret = 1;
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g);
    vertex_t v_numerator = *ai;
    vertex_t v_denominator = *(ai+1);
    return DFSComputeVariable(v_numerator)/DFSComputeVariable(v_denominator);
  }

  double DFSComputeScale(vertex_t s) {
    assert(g[s].vt==Scale);
    double ret = 0;
    int index = 0;
    double numerator; 
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      double tmp = DFSComputeVariable(v);
      if (index==g[s].params["index"]) numerator = tmp;
      ret += tmp;
    }
    return numerator/ret;
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
    write_graphviz (std::cout, g, boost::make_label_writer(boost::get(&ProvVertex::name, g)));
  }

  void setSavePath(const std::string& sp) {
    save_path = sp;
  }

  void saveGraph() {
    std::stringstream ss;
    write_graphviz (ss, g, boost::make_label_writer(boost::get(&ProvVertex::name, g)));
    std::ofstream fout(save_path);
    fout << ss.str();
    fout.close();
  }

  ~CProvGraph() {};


 /* class members */
private:
  Graph g;
  std::string save_path = "/home/jz598/MLNInfer/data/CProv/raw/test.dot";
  std::unordered_map<std::string, vertex_t> vertex_set;
};

#endif