#include"dag_longest_weighted_path_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"dag_longest_weighted_path.h"
#include"weighted_vertex.h"
#include<algorithm>
#include<cassert>

namespace
{
    using weight_t = std::int32_t;
    using long_int_t = std::int64_t;

    using edge_t = Standard::Algorithms::weighted_vertex<std::int32_t, weight_t>;
    using dag_t = std::vector<std::vector<edge_t>>;

    constexpr void add_weighted_edge(
        const std::int32_t source_vertex, dag_t &dag, const std::int32_t destination_vertex, const weight_t weight)
    {
        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (source_vertex == destination_vertex)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The source vertex (" << (source_vertex) << ") == destination (" << destination_vertex << ").";
                Standard::Algorithms::throw_exception(str);
            }
        }

        dag[source_vertex].emplace_back(destination_vertex, weight);
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::dag_longest_weighted_path_tests()
{
    constexpr auto vertex_count = 5;
    dag_t dag(vertex_count);

    constexpr weight_t bas = 0xFFFFF;
    static_assert(weight_t{} < bas);

    Standard::Algorithms::Utilities::random_t<weight_t> rnd(0, bas);

    const auto path02 = rnd();
    const auto path21 = rnd();
    const auto path31 = rnd();
    const auto path32 = rnd();
    const auto path43 = rnd();

    add_weighted_edge(0, dag, 2, path02);
    add_weighted_edge(2, dag, 1, path21);
    add_weighted_edge(3, dag, 1, path31);
    add_weighted_edge(3, dag, 2, path32);
    add_weighted_edge(4, dag, 3, path43);
    // 0 -> 2 -> 1
    //      ^  /
    // 4 -> 3

    constexpr auto source_vertex = 3;
    constexpr auto destination_vertex = 1;
    const auto expected =
        std::max<long_int_t>(path31, static_cast<long_int_t>(static_cast<long_int_t>(path32) + path21));

    {
        const auto actual_slow = Standard::Algorithms::Graphs::dag_longest_weighted_path_slow<long_int_t>(
            source_vertex, dag, destination_vertex);

        ::Standard::Algorithms::ert::are_equal(expected, actual_slow, "Error in dag_longest_weighted_path_slow(s, t).");
    }
    {
        const auto actual = Standard::Algorithms::Graphs::dag_longest_weighted_path_source<long_int_t>(
            source_vertex, dag, destination_vertex);

        ::Standard::Algorithms::ert::are_equal(expected, actual, "dag_longest_weighted_path_source destination");
    }

    const auto expected_from = std::max<long_int_t>(expected, path32);
    {
        const auto actual_from =
            Standard::Algorithms::Graphs::dag_longest_weighted_path_source<long_int_t>(source_vertex, dag);

        ::Standard::Algorithms::ert::are_equal(expected_from, actual_from, "dag_longest_weighted_path_source any");
    }
    {
        const auto actual_from_slow =
            Standard::Algorithms::Graphs::dag_longest_weighted_path_slow<long_int_t>(source_vertex, dag);

        ::Standard::Algorithms::ert::are_equal(expected_from, actual_from_slow, "dag_longest_weighted_path_slow(s).");
    }

    const auto expected_whole = std::max<long_int_t>({
        path02,
        path21,
        path31,
        path32,
        path43,
        // 2 segments
        static_cast<long_int_t>(path02) + path21,
        static_cast<long_int_t>(path32) + path21,
        static_cast<long_int_t>(path43) + path31,
        static_cast<long_int_t>(path43) + path32,
        // 3 segments
        static_cast<long_int_t>(path43) + path32 + path21,
    });

    const auto actual_whole = Standard::Algorithms::Graphs::dag_longest_weighted_path<long_int_t>(dag);

    ::Standard::Algorithms::ert::are_equal(expected_whole, actual_whole, "dag_longest_weighted_path");
}
