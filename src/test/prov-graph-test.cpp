#include "../CProvGraph.h"
#include <iostream>
#include <boost/graph/graphviz.hpp>

using namespace std;

int main() {
  Graph g;
  vertex_t v1 = boost::add_vertex(g);
  g[v1].vt=Variable;
  g[v1].name="v1";
  g[v1].isEDB=true;
  g[v1].value=0.5;

  vertex_t v2 = boost::add_vertex(g);
  g[v2].vt=Operator;
  g[v2].name="v2";
  g[v2].isEDB=true;
  g[v2].value=0.6;

  boost::add_edge(v1, v2, ProvEdge{1.0}, g);

  write_graphviz (std::cout, g, boost::make_label_writer(boost::get(&ProvVertex::name, g)));
  // write_graphviz (std::cout, g, boost::make_label_writer(boost::get(boost::vertex_bundle, g)), boost::default_writer{});
}