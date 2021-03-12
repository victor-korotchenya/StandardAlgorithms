#pragma once
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            //for testing only.
            //
            extern std::vector<int>* get_tarjan_graph_ptr();
            extern int* get_tarjan_graph_size();
            //
            //output.
            extern int get_tarjan_comp_cnt();//number of components.
            extern const int* get_tarjan_vertex_to_component();
            extern const std::vector<std::vector<int>>* get_tarjan_components();

            //SCC Tarjan strongly connected components can be used to solve 2-SAT in 2-CNF.
            //The input is a directed graph,
            // where the nodes indexes are from 0 to (n-1).
            //Self loops are not allowed.
            extern void tarjan_scc();
        }
    }
}
