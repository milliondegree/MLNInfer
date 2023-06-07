#include "../CProvGraph.h"
#include <iostream>
#include <boost/graph/graphviz.hpp>

using namespace std;

int main() {
  string sp = "/home/jz598/MLNInfer/data/CProv/raw/test.dot";
  CProvGraph provG(sp);
  
  string input_1_name = "input_1";
  vertex_t input_1 = provG.addVariableVertex(Variable, input_1_name, true, 0.5);

  string input_2_name = "input_2";
  vertex_t input_2 = provG.addVariableVertex(Variable, input_2_name, true, 0.5);

  string operator_1_name = "sum_output";
  vertex_t operator_1 = provG.addOperatorVertex(Sum, operator_1_name);

  provG.addProvEdge(operator_1, input_1, false);
  provG.addProvEdge(operator_1, input_2, false);
  
  string output_name = "output";
  vertex_t output = provG.addVariableVertex(Variable, output_name, false, 1.0);
  
  provG.addProvEdge(output, operator_1, false);

  /* test saveGraph */
  provG.saveGraph();


  /* test getVariableVertexByName */
  // vertex_iter v_tmp = provG.getVariableVertexByName(input_1_name);
  // provG.printVertex(*v_tmp);
  // cout << provG.getVertexIndexByName(input_1_name) << endl;

  /* test get children vertices */
  // provG.printChildrenVerticesByName(output_name);

  /* test traceProvOfVariableByName */
  cout << endl << "start tracing provenance of " << output_name << endl;
  CProvGraph subProvG = provG.traceProvOfVariableByName(output_name);
  subProvG.saveGraph();
  

  return 0;
}