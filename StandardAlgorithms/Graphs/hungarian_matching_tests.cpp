#include"hungarian_matching_tests.h"
#include"../Numbers/permutation.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"hungarian_matching.h"

namespace
{
    using int_t = std::int16_t;
    using long_int_t = std::int64_t;
    using random_t = Standard::Algorithms::Utilities::random_t<int_t>;
    using graph_t = std::vector<std::vector<int_t>>;

    constexpr void check_matchings(const std::string &name, const graph_t &graph, const long_int_t &expected_weight,
        const std::vector<std::int32_t> &matchings)
    {
        assert(!name.empty());

        const auto size = graph.size();
        ::Standard::Algorithms::ert::are_equal(size, matchings.size(), name + " matching size");

        {
            constexpr auto is_zero_start = true;

            const auto is_perm = ::Standard::Algorithms::Numbers::is_permutation_slow2(matchings, is_zero_start);

            ::Standard::Algorithms::ert::are_equal(true, is_perm, name + " is matching a permutation");
        }
        {
            const auto weight = Standard::Algorithms::Graphs::Inner::matching_weight<long_int_t>(graph, matchings);

            ::Standard::Algorithms::ert::are_equal(expected_weight, weight, name + " matching weight");
        }
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, graph_t &&graph, std::vector<std::int32_t> &&expected_matchings = {},
            long_int_t min_weight = {}) noexcept
            : base_test_case(std::move(name))
            , Graph(std::move(graph))
            , Expected_matchings(std::move(expected_matchings))
            , Min_weight(min_weight)
        {
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const graph_t &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto empty_expected() const noexcept -> bool
        {
            return Expected_matchings.empty();
        }

        [[nodiscard]] constexpr auto expected_matchings() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Expected_matchings;
        }

        [[nodiscard]] constexpr auto min_weight() const noexcept -> long_int_t
        {
            return Min_weight;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();
            Standard::Algorithms::Graphs::require_full_graph(Graph);

            if (empty_expected())
            {
                return;
            }

            check_matchings("validate expected matchings", Graph, Min_weight, Expected_matchings);
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print(", graph ", Graph, str);
            ::Standard::Algorithms::print(", expected matchings ", Expected_matchings, str);
            str << " min weight " << Min_weight;
        }

private:
        graph_t Graph;
        std::vector<std::int32_t> Expected_matchings;
        long_int_t Min_weight;
    };

    void generate_random(std::vector<test_case> &tests)
    {
        constexpr auto min_size = 1; // NOLINTNEXTLINE
        constexpr auto max_size = ::Standard::Algorithms::is_debug ? 5 : 8;

        random_t rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto size = rnd();

            graph_t graph(size);

            for (std::int32_t from{}; from < size; ++from)
            {
                Standard::Algorithms::Utilities::fill_random(graph[from], size);
            }

            tests.emplace_back("Random" + std::to_string(att), std::move(graph));
        }
    }

    void generate_test_cases(std::vector<test_case> &tests)
    {
        tests.emplace_back("K33minus",
            // NOLINTNEXTLINE
            graph_t{ { -20, -3, 1 },
                // NOLINTNEXTLINE
                { -5, -4, -6 },
                // NOLINTNEXTLINE
                { -16, -15, -9 } },
            // NOLINTNEXTLINE
            std::vector<std::int32_t>{ 0, 2, 1 }, -20 - 6 - 15);

        tests.emplace_back("K33",
            // NOLINTNEXTLINE
            graph_t{ { 20, 3, -1 },
                // NOLINTNEXTLINE
                { 5, 4, 6 },
                // NOLINTNEXTLINE
                { 16, 15, 9 } },
            // NOLINTNEXTLINE
            std::vector<std::int32_t>{ 1, 0, 2 }, 3 + 5 + 9);

        tests.emplace_back("K33 v0",
            // NOLINTNEXTLINE
            graph_t{ { 21, 4, 0 },
                // NOLINTNEXTLINE
                { 1, 0, 2 },
                // NOLINTNEXTLINE
                { 7, 6, 0 } },
            // NOLINTNEXTLINE
            std::vector<std::int32_t>{ 1, 0, 2 }, 4 + 1 + 0);

        tests.emplace_back("K22",
            // NOLINTNEXTLINE
            graph_t{ { 20, 3 },
                // NOLINTNEXTLINE
                { 5, 4 } },
            // NOLINTNEXTLINE
            std::vector<std::int32_t>{ 1, 0 }, 3 + 5);

        tests.emplace_back("K22same",
            // NOLINTNEXTLINE
            graph_t{ { 10, 10 },
                // NOLINTNEXTLINE
                { 10, 10 } },
            // NOLINTNEXTLINE
            std::vector<std::int32_t>{ 0, 1 }, 20);

        tests.emplace_back("K22 same minus",
            // NOLINTNEXTLINE
            graph_t{ { -10, -10 },
                // NOLINTNEXTLINE
                { -10, -10 } },
            // NOLINTNEXTLINE
            std::vector<std::int32_t>{ 0, 1 }, -20);

        tests.emplace_back("K22minus",
            // NOLINTNEXTLINE
            graph_t{ { -20, -3 },
                // NOLINTNEXTLINE
                { -5, -4 } },
            // NOLINTNEXTLINE
            std::vector<std::int32_t>{ 0, 1 }, -20 - 4);

        tests.emplace_back("K11",
            // NOLINTNEXTLINE
            graph_t{ { 30 } },
            // NOLINTNEXTLINE
            std::vector<std::int32_t>{ 0 }, 30);

        tests.emplace_back("K11minus",
            // NOLINTNEXTLINE
            graph_t{ { -3 } },
            // NOLINTNEXTLINE
            std::vector<std::int32_t>{ 0 }, -3);

        generate_random(tests);
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<std::int32_t> matchings;

        const auto min_weight =
            Standard::Algorithms::Graphs::hungarian_matching<long_int_t, graph_t>(test.graph(), matchings);

        Standard::Algorithms::require_positive(matchings.size(), "hungarian_matching size");

        if (!test.empty_expected())
        {
            ::Standard::Algorithms::ert::are_equal(test.min_weight(), min_weight, "hungarian_matching weight");
        }

        if (test.expected_matchings() != matchings)
        {
            check_matchings("hungarian_matching matchings", test.graph(), min_weight, matchings);
        }

        matchings.clear();
        {
            const auto slow = Standard::Algorithms::Graphs::min_cost_perfect_matching_slow<long_int_t, graph_t>(
                test.graph(), matchings);

            Standard::Algorithms::require_positive(matchings.size(), "min_cost_perfect_matching_slow size");

            ::Standard::Algorithms::ert::are_equal(min_weight, slow, "min_cost_perfect_matching_slow weight");
        }

        if (test.expected_matchings() != matchings)
        {
            check_matchings("min_cost_perfect_matching_slow matchings", test.graph(), min_weight, matchings);
        }
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::hungarian_matching_tests()
{
    ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
