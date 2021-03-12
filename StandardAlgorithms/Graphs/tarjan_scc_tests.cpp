#include <initializer_list>
#include <stdexcept>
#include "../test_utilities.h"
#include "tarjan_scc.h"
#include "tarjan_scc_tests.h"

using namespace std;
using namespace Privet::Algorithms::Graphs::Tests;
using namespace Privet::Algorithms::Graphs;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    void addm(const initializer_list<int>& edges)
    {
        const auto size = static_cast<int>(edges.size());
        if (size < 2)
            throw runtime_error("The size must be at least 2");

        auto gr = get_tarjan_graph_ptr();
        auto it = edges.begin();
        auto prev = *it;
        for (;;)
        {
            ++it;
            if (edges.end() == it)
                break;

            const auto& curr = *it;
            gr[prev].push_back(curr);
            prev = curr;
        }
    }

    constexpr auto gr_size = 14;

    void init_digraph()
    {
        *get_tarjan_graph_size() = gr_size;
        vector<int>* gr = get_tarjan_graph_ptr();

        for (int i = 0; i <= gr_size; ++i)
            gr[i].clear();

        // 0 -> 1 -> 2 -> 3 -> 1
        // 4 -> 5 -> 4
        // 8 -> 7 -> 5 -> 6 -> 7
        // 13 -> 12 -> 10 -> 11 -> 12
        //       10 -> 12
        addm({ 0, 1, 2, 3, 1 });
        addm({ 4, 5, 4 });
        addm({ 8, 7, 5, 6, 7 });
        addm({ 13, 12, 10, 11, 12 });
        addm({ 10, 12 });
    }

    void check_scc()
    {
        const vector<vector<int>> expected_scc = {
          {0},
          {1,2,3},
          {4,5,6,7},
          {8},
          {9},
          {10,11,12},
          {13}
        };

        const auto expected_size = static_cast<int>(expected_scc.size());
        Assert::AreEqual(expected_size, get_tarjan_comp_cnt(), "number of components");

        auto& actual = *const_cast<vector<vector<int>>*>(get_tarjan_components());
        for (auto i = 0; i < expected_size; ++i)
        {
            sort(actual[i].begin(), actual[i].end());
        }
        sort(actual.begin(), actual.end());

        for (int i = 0; i < expected_size; ++i)
        {
            Assert::AreEqual(expected_scc[i], actual[i], "components at " + to_string(i));
        }
    }
}

void Privet::Algorithms::Graphs::Tests::tarjan_scc_tests()
{
    init_digraph();
    tarjan_scc();
    check_scc();
}