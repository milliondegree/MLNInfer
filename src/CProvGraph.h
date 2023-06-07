#ifndef _CPROVGRAPH_H
#define _CPROVGRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <string>


enum VertexType {Variable, Sum, Mul, Div, Scale};

inline std::ostream& operator<<(std::ostream& out, const VertexType value){
  const char* s = 0;
  #define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
      PROCESS_VAL(Variable);     
      PROCESS_VAL(Sum);
      PROCESS_VAL(Mul);
      PROCESS_VAL(Scale);
    }
  #undef PROCESS_VAL
    return out << s;
}

struct ProvVertex {
  VertexType vt;
  std::string name;
  bool isEDB;
  double value;
};

struct ProvEdge{
  float weight;
};

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, ProvVertex, ProvEdge > Graph;
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
  CProvGraph() {
  }

  CProvGraph(std::string& sp): save_path(sp) {}

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

  vertex_iter getVertexByName(const std::string& name) {
    vertex_iter vi, vi_end;
    for (boost::tie(vi, vi_end)=boost::vertices(g); vi!=vi_end; vi++) {
      if (g[*vi].name.compare(name)==0) {
        return vi;
      }
    }
    return vi_end;
  }

  bool checkVertexExistByName(const std::string& name) {
    return getVertexByName(name)!=boost::vertices(g).second;
  }

  vertex_t addVariableVertex(const VertexType vt, const std::string& name, bool isEDB, float value) {
    assert(!checkVertexExistByName(name));
    vertex_t v = boost::add_vertex(g);
    g[v].vt = vt;
    g[v].name = name;
    g[v].isEDB = isEDB;
    g[v].value = value;
    return v;
  }

  vertex_t addOperatorVertex(const VertexType vt, const std::string& name) {
    vertex_t v = boost::add_vertex(g);
    g[v].vt = vt;
    g[v].name = name;
    return v;
  }

  void addProvEdge(vertex_t v1, vertex_t v2, bool reverse) {
    if (!reverse) boost::add_edge(v1, v2, g);
    else boost::add_edge(v2, v2, g);
  }

  void addProvEdge(vertex_t v1, vertex_t v2) {
    boost::add_edge(v1, v2, g);
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
      v_output = *getVertexByName(output_name);
      v_operator = *getVertexByName("op_"+output_name);
      g[v_output].value = value;
    }
    else {
      v_output = addVariableVertex(Variable, output_name, false, value);
      v_operator = addOperatorVertex(vt, "op_"+output_name);
      addProvEdge(v_output, v_operator);
    }
    for (auto s : input_names) {
      addProvEdge(v_operator, *getVertexByName(s));
    }
  }

  void printChildrenVerticesByName(const std::string& name) {
    assert(checkVertexExistByName(name));
    vertex_t v = *getVertexByName(name);
    auto neighbors = boost::adjacent_vertices(v, g);
    for (auto vi : boost::make_iterator_range(neighbors)) {
      std::cout << g[vi].name << std::endl;
    }
  }

  CProvGraph traceProvOfVariableByName(const std::string& name) {
    assert(checkVertexExistByName(name));
    vertex_t v = *getVertexByName(name);
    
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

  void DFSProvQuery(vertex_t s, CProvGraph& subProvG) {
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      vertex_t child;
      if (subProvG.checkVertexExistByName(g[v].name)) child = *(subProvG.getVertexByName(g[v].name));
      else {
        if (g[v].vt==Variable) child = subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].isEDB, g[v].value);
        else child = subProvG.addOperatorVertex(g[v].vt, g[v].name);
      }
      vertex_t parent = *(subProvG.getVertexByName(g[s].name));
      subProvG.addProvEdge(parent, child);
      DFSProvQuery(v, subProvG);
    }
    return;
  }

  void printVertex(vertex_t v) {
    std::cout << g[v].vt << " vertex ";
    std::cout << g[v].name << ", value=" << g[v].value << std::endl;
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

private:
  Graph g;
  std::string save_path = "/home/jz598/MLNInfer/data/CProv/raw/test.dot";
};

#endif