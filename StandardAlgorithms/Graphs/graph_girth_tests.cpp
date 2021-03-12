#include"graph_girth_tests.h"
#include"../Utilities/ert.h"
#include"graph_girth.h"

namespace
{
    constexpr auto vertex_count = 10;

    [[nodiscard]] constexpr auto build_graph() -> Standard::Algorithms::Graphs::graph<std::int32_t, std::uint32_t>
    {
        Standard::Algorithms::Graphs::graph<std::int32_t, std::uint32_t> graf(vertex_count);

        // Edges: 0-1-2-3-4-5-6-7-8-9-0, 0-4, 0-6.
        // Some cycles: 0-1-2-3-4-0, 0-1-2-3-4-5-6-0, 0-1-2-3-4-5-6-7-8-9-0,
        //  0-4-5-6-0, 0-4-5-6-7-8-9-0,
        //  0-6-7-8-9-0.
        //
        // Girth path: 0-4-5-6-0.

        for (std::int32_t index{}; index < vertex_count; ++index)
        {
            graf.add_edge(index, (index + 1) % vertex_count);
        }

        // NOLINTNEXTLINE
        graf.add_edge(0, 4);

        // NOLINTNEXTLINE
        graf.add_edge(0, 6);

        return graf;
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::graph_girth_tests()
{
    const auto graf = build_graph();
    std::vector<std::size_t> actual_cycle_vertices;

    const auto actual_girth =
        Standard::Algorithms::Graphs::graph_girth::find_girth_unweighted(graf, actual_cycle_vertices);

    const std::vector<std::size_t> expected_cycle_vertices
        // NOLINTNEXTLINE
        { 6, 5, 4, 0 };

    ::Standard::Algorithms::ert::are_equal(expected_cycle_vertices, actual_cycle_vertices, "actual cycle vertices");

    const auto expected_girth = expected_cycle_vertices.size();

    ::Standard::Algorithms::ert::are_equal(expected_girth, actual_girth, "Girth");
}
