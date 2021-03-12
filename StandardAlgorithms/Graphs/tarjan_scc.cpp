#include <algorithm>
#include <cassert>

#include "tarjan_scc.h"

namespace
{
    constexpr int si = 1 * 1001;

    //Input: a directed graph.
    std::vector<int> graph[si];
    int n,
        //stack
        st_size, st[si];

    //Output:
    int comp_cnt, vert_to_comp[si];
    //often only one needed of {vert_to_comp, components}.
    std::vector<std::vector<int>> components;

    //Temporary variables:
    int visit_cnt, discovery_time[si], lower_index[si];
    bool in_stack[si];

    void visit(const int pare)
    {
        discovery_time[pare] = lower_index[pare] = ++visit_cnt;
        st[st_size++] = pare;
        in_stack[pare] = true;

        for (const auto& v : graph[pare])
        {
            if (0 == discovery_time[v])
            {
                visit(v);
                lower_index[pare] = std::min(lower_index[pare], lower_index[v]);
            }
            else if (in_stack[v])
                lower_index[pare] = std::min(lower_index[pare], discovery_time[v]);
        }

        if (lower_index[pare] != discovery_time[pare])
            return;

        auto& compo = components[comp_cnt];
        compo.clear();

        int u;
        do
        {
            u = st[st_size - 1];
            in_stack[u] = false;
            vert_to_comp[u] = comp_cnt;
            compo.push_back(u);
            assert(0 < st_size);
            --st_size;
        }     while (pare != u);

        ++comp_cnt;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            //for testing only.
            //
            //input.
            std::vector<int>* get_tarjan_graph_ptr() { return graph; }
            int* get_tarjan_graph_size() { return &n; }
            //
            //output.
            int get_tarjan_comp_cnt() { return comp_cnt; }
            const int* get_tarjan_vertex_to_component() { return vert_to_comp; }
            const std::vector<std::vector<int>>* get_tarjan_components() { return &components; }

            void tarjan_scc()
            {
                assert(0 <= n);

                // clear
                comp_cnt = 0;
                visit_cnt = 0;
                memset(discovery_time, 0, sizeof(int) * (n + 2));
                //memset(lower_index, 0, sizeof(int) * (n + 2));
                memset(in_stack, 0, sizeof(bool) * (n + 2));
                st_size = 0;//st.clear();
                components.clear();
                components.resize(n);

                for (auto i = 0; i < n; i++)
                {
                    if (0 == discovery_time[i])
                        visit(i);
                }

                components.resize(comp_cnt);
                components.shrink_to_fit();
            }
        }
    }
}