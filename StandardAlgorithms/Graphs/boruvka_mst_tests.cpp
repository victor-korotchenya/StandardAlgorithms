#include"boruvka_mst_tests.h"
#include"../Utilities/test_utilities.h"
#include"boruvka_mst.h"
#include<cstdint>

namespace
{
    using weight_t = std::uint32_t;
    using from_to_weight_t = std::tuple<std::int32_t, std::int32_t, weight_t>;

    constexpr void add_edge(std::vector<from_to_weight_t> &graph, const std::int32_t from, const weight_t weight,
        const std::int32_t tod, const bool add_rev_edge = true)
    {
        assert(from != tod);
        graph.emplace_back(from, tod, weight);

        if (add_rev_edge)
        {
            graph.emplace_back(tod, from, weight);
        }
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::boruvka_mst_tests()
{
    // todo(p2): test with other algs.

    constexpr auto vertex_count = 5;
    std::vector<from_to_weight_t> graph;
    std::vector<from_to_weight_t> actual_tree;
    // a   10    b   400   d
    //  20   30    500  200
    //    c           e
    // MST: ab, ac, bd, de. W = 10 + 20 + 400 + 200 = 630

    constexpr auto weight01 = 10;
    add_edge(graph, 0, weight01, 1);
    constexpr auto weight02 = 20;
    add_edge(graph, 0, weight02, 2);
    constexpr auto weight12 = 30;
    add_edge(graph, 1, weight12, 2);
    constexpr auto weight13 = 400;
    add_edge(graph, 1, weight13, 3);
    constexpr auto weight14 = 500;
    add_edge(graph, 1, weight14, 4);
    constexpr auto weight34 = 200;
    add_edge(graph, 3, weight34, 4);
    constexpr auto expected = static_cast<weight_t>(weight01 + weight02 + weight13 + weight34);

    {
        const auto actual = boruvka_mst<weight_t, weight_t>(graph, vertex_count, actual_tree);
        ::Standard::Algorithms::ert::are_equal(expected, actual, "weight");
    }

    std::vector<from_to_weight_t> expected_tree;
    add_edge(expected_tree, 0, weight01, 1, false);
    add_edge(expected_tree, 0, weight02, 2, false);
    add_edge(expected_tree, 1, weight13, 3, false);
    add_edge(expected_tree, 3, weight34, 4, false);

    std::sort(expected_tree.begin(), expected_tree.end());
    std::sort(actual_tree.begin(), actual_tree.end());
    ::Standard::Algorithms::ert::are_equal(expected_tree, actual_tree, "minimum spanning tree");
}
