#include"sparse_graph_tests.h"
#include"../Numbers/hash_utilities.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"sparse_graph.h"
#include<unordered_set>

namespace
{
    template<class int_t>
    [[nodiscard]] constexpr auto create_random_sparse_graph() -> Standard::Algorithms::Graphs::sparse_graph<int_t>
    {
        constexpr auto min_vertex = 2;
        constexpr auto max_vertex = 100;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_vertex, max_vertex);

        const auto vertex_count = rnd();
        const auto edge_count = rnd(1, vertex_count / 2);

        rnd.limits(0, vertex_count - 1);

        Standard::Algorithms::Graphs::sparse_graph<int_t> result({ vertex_count,
            // Added twice
            edge_count * 2 });

        using pair_t = std::pair<int_t, int_t>;
        std::unordered_set<pair_t, Standard::Algorithms::Numbers::pair_hash> edges;

        int_t cnt{};
        int_t source{};
        int_t dest{};

        do
        {
            for (;;)
            {
                source = rnd();

                do
                {
                    dest = rnd();
                } while (source == dest);

                if (dest < source)
                {
                    std::swap(source, dest);
                }

                const auto iter = edges.emplace(source, dest);
                if (iter.second)
                {
                    break;
                }
            }

            result.add_edge(source, dest);
            result.add_edge(dest, source);
        } while (++cnt < edge_count);

        return result;
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::sparse_graph_tests()
{
    using int_t = std::int64_t;

    const auto graph = create_random_sparse_graph<int_t>();
    require_greater(graph.size(), 0U, "graph size");

    // todo(p3): finish.
}
