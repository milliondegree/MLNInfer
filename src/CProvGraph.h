#ifndef _CPROVGRAPH_H
#define _CPROVGRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <string>


enum VertexType {Variable, Operator};

struct ProvVertex {
  VertexType vt;
  std::string name;
  bool isEDB;
  float value;
};

struct ProvEdge{
  float weight;
};

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, ProvVertex, ProvEdge > Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;

class CProv_BFS_Visitor : public boost::default_bfs_visitor {
public:
    // template <typename Vertex, typename Graph>
    void discover_vertex(vertex_t u, const Graph& g)
    {
      std::cout << "Visit vertex " << g[u].name;
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

  size_t getVertexIndexByName(std::string& name) {
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

  vertex_iter getVariableVertexByName(std::string& name) {
    vertex_iter vi, vi_end;
    for (boost::tie(vi, vi_end)=boost::vertices(g); vi!=vi_end; vi++) {
      if (g[*vi].name.compare(name)==0) {
        return vi;
      }
    }
    return vi_end;
  }

  vertex_t addVariableVertex(const VertexType vt, std::string& name, bool isEDB, float value) {
    assert(getVariableVertexByName(name)==boost::vertices(g).second);
    vertex_t v = boost::add_vertex(g);
    g[v].vt = vt;
    g[v].name = name;
    g[v].isEDB = isEDB;
    g[v].value = value;
    return v;
  }

  vertex_t addOperatorVertex(const VertexType vt, std::string& name) {
    vertex_t v = boost::add_vertex(g);
    g[v].vt = vt;
    g[v].name = name;
    return v;
  }

  void addProvEdge(vertex_t v1, vertex_t v2, bool reverse) {
    if (!reverse) boost::add_edge(v1, v2, g);
    else boost::add_edge(v2, v2, g);
  }

  void traceProvOfVariableByName(std::string& name) {
    vertex_iter vi = getVariableVertexByName(name);
    assert(vi!=boost::vertices(g).second);
    vertex_t v = *vi;
    CProv_BFS_Visitor vis;
    boost::breadth_first_search(g, v, boost::visitor(vis));
  }

  void printVertex(vertex_t v) {
    std::cout << g[v].name << std::endl;
  }

  void printGraph() {
    write_graphviz (std::cout, g, boost::make_label_writer(boost::get(&ProvVertex::name, g)));
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