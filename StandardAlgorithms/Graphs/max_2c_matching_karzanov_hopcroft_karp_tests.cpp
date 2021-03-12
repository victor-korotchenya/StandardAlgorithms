#include"max_2c_matching_karzanov_hopcroft_karp_tests.h"
#include"../Numbers/hash_utilities.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"graph_helper.h"
#include"max_2c_matching_karzanov_hopcroft_karp.h"
#include<unordered_set>

namespace
{
    template<class int_t, class edge_t>
    constexpr void demand_bipartite_graph(
        int_t left_size, const std::vector<edge_t> &edges, int_t right_size, const bool is_one_based = false)
    {
        if (is_one_based)
        {
            ++left_size, ++right_size;

            Standard::Algorithms::require_positive(left_size, "left part size + 1");
            Standard::Algorithms::require_positive(right_size, "right part size + 1");
        }

        std::unordered_set<edge_t, Standard::Algorithms::Numbers::pair_hash> unique_edges;

        for (const auto &edge : edges)
        {
            const auto &from = edge.first;
            if (!(from < left_size)) [[unlikely]]
            {
                throw std::invalid_argument("The edge left from vertex " + std::to_string(from) +
                    " must be less than size " + std::to_string(left_size) + ".");
            }

            const auto &tod = edge.second;
            if (!(tod < right_size)) [[unlikely]]
            {
                throw std::invalid_argument("The edge right to vertex " + std::to_string(tod) +
                    " must be less than size " + std::to_string(right_size) + ".");
            }

            if (!unique_edges.insert(edge).second) [[unlikely]]
            {
                throw std::invalid_argument("An edge {" + std::to_string(from) + ", " + std::to_string(tod) +
                    "} must not repeat in a bipartite graph.");
            }
        }
    }

    template<class int_t, class edge_t>
    constexpr void demand_bipartite_matching(
        int_t left_size, const std::vector<edge_t> &matching, int_t right_size, const bool is_one_based = false)
    {
        if (is_one_based)
        {
            ++left_size, ++right_size;

            Standard::Algorithms::require_positive(left_size, "left part size + 1");
            Standard::Algorithms::require_positive(right_size, "right part size + 1");
        }

        std::unordered_set<int_t> lefts;
        std::unordered_set<int_t> rights;

        for (const auto &edge : matching)
        {
            {
                const auto &from = edge.first;
                if (!(from < left_size)) [[unlikely]]
                {
                    throw std::invalid_argument("The matching left from vertex " + std::to_string(from) +
                        " must be less than size " + std::to_string(left_size) + ".");
                }

                if (!lefts.insert(from).second) [[unlikely]]
                {
                    throw std::invalid_argument(
                        "The left from vertex " + std::to_string(from) + " must not repeat in a matching.");
                }
            }
            {
                const auto &tod = edge.second;
                if (!(tod < right_size)) [[unlikely]]
                {
                    throw std::invalid_argument("The matching right to vertex " + std::to_string(tod) +
                        " must be less than size " + std::to_string(right_size) + ".");
                }

                if (!rights.insert(tod).second) [[unlikely]]
                {
                    throw std::invalid_argument(
                        "The right to vertex " + std::to_string(tod) + " must not repeat in a matching.");
                }
            }
        }
    }

    using int_t = std::uint32_t;
    using max_matching_t = Standard::Algorithms::Graphs::max_2c_matching_karzanov_hopcroft_karp<int_t>;
    using edge_t = typename max_matching_t::edge_t;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, int_t left_size, std::vector<edge_t> &&edges, int_t right_size,
            std::vector<edge_t> &&expected_matchings) noexcept
            : base_test_case(std::move(name))
            , Left_size(left_size)
            , Right_size(right_size)
            , Edges(std::move(edges))
            , Expected_matchings(std::move(expected_matchings))
        {
        }

        [[nodiscard]] constexpr auto left_size() const noexcept
        {
            return Left_size;
        }

        [[nodiscard]] constexpr auto right_size() const noexcept
        {
            return Right_size;
        }

        [[nodiscard]] constexpr auto edges() const &noexcept -> const std::vector<edge_t> &
        {
            return Edges;
        }

        [[nodiscard]] constexpr auto expected_matchings() const &noexcept -> const std::vector<edge_t> &
        {
            return Expected_matchings;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();
            Standard::Algorithms::require_positive(Left_size, "left size");
            Standard::Algorithms::require_positive(Right_size, "right size");

            constexpr auto is_one_based = true;

            demand_bipartite_graph(Left_size, Edges, Right_size, is_one_based);

            demand_bipartite_matching(Left_size, Expected_matchings, Right_size, is_one_based);
        }

        void print(std::ostream &str) const override
        {
            str << ", left size " << Left_size;
            str << ", right size " << Right_size;
            ::Standard::Algorithms::print(", edges", Edges, str);
            ::Standard::Algorithms::print(", expected matchings", Expected_matchings, str);
        }

private:
        int_t Left_size;
        int_t Right_size;
        std::vector<edge_t> Edges;
        std::vector<edge_t> Expected_matchings;
    };

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        // Left      Right
        // 1 ------- 1
        //     \     /;
        //       X
        //       /  \;
        //    / /     \;
        // 2  /        2
        //    /;
        // 3 ------- 3
        //              4
        //
        // Max matching: {1,2}, {2,1}, {3,3}
        tests.push_back({ "base-34", 3,
            // NOLINTNEXTLINE
            { { 1, 1 }, { 1, 2 }, { 2, 1 }, { 3, 1 }, { 3, 3 } },
            // NOLINTNEXTLINE
            4,
            // NOLINTNEXTLINE
            { { 1, 2 }, { 2, 1 }, { 3, 3 } } });
    }

    void run_test_case(const test_case &test)
    {
        max_matching_t graph({ test.left_size(), test.right_size() });

        for (const auto &edge : test.edges())
        {
            graph.add_edge({ edge.first, edge.second });
        }

        std::vector<edge_t> matchings;
        graph.max_matching(matchings);

        ::Standard::Algorithms::ert::are_equal(test.expected_matchings(), matchings, "max matching");
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::max_2c_matching_karzanov_hopcroft_karp_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
