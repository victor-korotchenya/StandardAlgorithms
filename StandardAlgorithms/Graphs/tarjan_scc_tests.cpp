#include"tarjan_scc_tests.h"
#include"../Utilities/test_utilities.h"
#include"tarjan_scc.h"
#include<initializer_list>

namespace
{
    void add_edges(const std::initializer_list<std::int32_t> &edges)
    {
        const auto size = static_cast<std::int32_t>(edges.size());
        Standard::Algorithms::require_greater(size, 1, "edges size");

        auto &graf = Standard::Algorithms::Graphs::tarjan_graph_ptr();

        auto prev = *edges.begin();
        auto skip_first = true;

        for (const auto &cur : edges)
        {
            if (skip_first)
            {
                skip_first = false;
                continue;
            }

            graf.at(prev).push_back(cur);
            prev = cur;
        }
    }

    void init_digraph()
    {
        constexpr auto gr_size = 14;

        Standard::Algorithms::Graphs::tarjan_graph_size() = gr_size;
        auto &graf = Standard::Algorithms::Graphs::tarjan_graph_ptr();

        for (std::int32_t vertex{}; vertex <= gr_size; ++vertex)
        {
            graf.at(vertex).clear();
        }

        // 0 -> 1 -> 2 -> 3 -> 1
        // 4 -> 5 -> 4
        // 8 -> 7 -> 5 -> 6 -> 7
        // 13 -> 12 -> 10 -> 11 -> 12
        //       10 -> 12

        // NOLINTNEXTLINE
        add_edges({ 0, 1, 2, 3, 1 });
        // NOLINTNEXTLINE
        add_edges({ 4, 5, 4 });
        // NOLINTNEXTLINE
        add_edges({ 8, 7, 5, 6, 7 });
        // NOLINTNEXTLINE
        add_edges({ 13, 12, 10, 11, 12 });
        // NOLINTNEXTLINE
        add_edges({ 10, 12 });
    }

    void check_scc()
    {
        const std::vector<std::vector<std::int32_t>> expected_scc{ { 0 }, { 1, 2, 3 },
            // NOLINTNEXTLINE
            { 4, 5, 6, 7 },
            // NOLINTNEXTLINE
            { 8 },
            // NOLINTNEXTLINE
            { 9 },
            // NOLINTNEXTLINE
            { 10, 11, 12 },
            // NOLINTNEXTLINE
            { 13 } };

        const auto expected_size = static_cast<std::int32_t>(expected_scc.size());
        ::Standard::Algorithms::ert::are_equal(
            expected_size, Standard::Algorithms::Graphs::tarjan_component_cnt(), "number of components");

        auto &actual_components = Standard::Algorithms::Graphs::tarjan_components();

        for (auto &actual : actual_components)
        {
            std::sort(actual.begin(), actual.end());
        }

        std::sort(actual_components.begin(), actual_components.end());

        ::Standard::Algorithms::ert::are_equal(expected_scc, actual_components, "components");
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::tarjan_scc_tests()
{
    init_digraph();
    tarjan_scc();
    check_scc();
}
