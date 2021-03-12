#include"min_cost_perfect_matching_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"min_cost_perfect_matching.h"
#include<iostream>

namespace
{
    using weight_t = std::int16_t;
    using weight_t2 = std::int64_t;

    constexpr weight_t2 not_computed = -1;
    constexpr auto print_mc_mm = false;

    using graph_t = std::vector<std::vector<weight_t>>;
    using matching_t = std::vector<std::size_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, graph_t &&graph, matching_t &&matching, weight_t2 expected = not_computed) noexcept
            : base_test_case(std::move(name))
            , Graph(std::move(graph))
            , Matching(std::move(matching))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const graph_t &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto matching() const &noexcept -> const matching_t &
        {
            return Matching;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const weight_t2 &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(Graph.size(), "graph size");
            ::Standard::Algorithms::ert::are_equal(0U, size & 1U, "even size");

            Standard::Algorithms::Graphs::require_full_graph(Graph, true);

            for (std::size_t from{}; from < size; ++from)
            {
                const auto &edges = Graph[from];
                const auto prefix = std::to_string(from) + ", ";
                ::Standard::Algorithms::ert::are_equal(weight_t{}, edges[from], prefix + prefix);

                for (std::size_t tod{}; tod < size; ++tod)
                {
                    Standard::Algorithms::require_non_negative(edges[tod], prefix + std::to_string(tod));
                }
            }

            ::Standard::Algorithms::ert::greater_or_equal(Expected, not_computed, "expected");

            if (Expected < 0)
            {
                return;
            }

            ::Standard::Algorithms::ert::are_equal(size, Matching.size(), "matching.size");
            Standard::Algorithms::require_unique(Matching, "matching");

            {
                const auto [min_it, max_it] = std::minmax_element(Matching.cbegin(), Matching.cend());
                ::Standard::Algorithms::ert::are_equal(0U, *min_it, "matching.min");
                ::Standard::Algorithms::ert::are_equal(size - 1U, *max_it, "matching.max");
            }

            for (std::size_t from{}; from < size; ++from)
            {
                ::Standard::Algorithms::ert::not_equal(from, Matching[from], "matching at " + std::to_string(from));
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("graph", Graph, str);
            ::Standard::Algorithms::print("matching", Matching, str);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        graph_t Graph;
        matching_t Matching;
        weight_t2 Expected;
    };

    template<class random_t>
    constexpr void random_full_symmetric_graph(graph_t &graph, const std::size_t size, random_t &rnd,
        const weight_t min_value = 0, const weight_t max_value = 20)
    {
        graph.clear();

        if (size == 0U)
        {
            return;
        }

        graph.resize(size);

        for (std::size_t from{}; from < size; ++from)
        {
            graph[from].resize(size);
        }

        for (std::size_t from{}; from < size - 1; ++from)
        {
            auto &edges = graph[from];

            for (auto tod = from + 1U; tod < size; ++tod)
            {
                edges[tod] = graph[tod][from] = rnd(min_value, max_value);
            }
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("base2", // NOLINTNEXTLINE
            graph_t{ { 0, 20 }, { 20, 0 } }, // NOLINTNEXTLINE
            matching_t{ 1, 0 }, 20);

        test_cases.emplace_back("base4", // NOLINTNEXTLINE
            graph_t{ { 0, 20, 30, 40 }, // NOLINTNEXTLINE
                { 20, 0, 70, 80 }, // NOLINTNEXTLINE
                { 30, 70, 0, 10 }, // NOLINTNEXTLINE
                { 40, 80, 10, 0 } }, // NOLINTNEXTLINE
            matching_t{ 1, 0, 3, 2 }, 30);

        constexpr auto min_value = 1;

        constexpr auto max_value = ::Standard::Algorithms::is_debug ? 5 : 10;

        Standard::Algorithms::Utilities::random_t<std::int16_t> rnd(min_value, max_value);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto size = 2 * rnd();
            matching_t matching(size);

            for (std::int32_t from{}; from < size - 1; ++from)
            {
                matching[from] = static_cast<weight_t>(from + 1);
            }

            matching.back() = 0;

            graph_t graph;
            random_full_symmetric_graph(graph, size, rnd);

            test_cases.emplace_back("Random" + std::to_string(att), std::move(graph), std::move(matching));
        }
    }

    void run_test_case(const test_case &test)
    {
        const Standard::Algorithms::elapsed_time_ns ti0;
        const auto &graf = test.graph();
        matching_t matching;

        const auto fast = Standard::Algorithms::Graphs::min_cost_perfect_matching<weight_t2, weight_t>(graf, matching);

        [[maybe_unused]] const auto elapsed0 = ti0.elapsed();

        if (!(test.expected() < 0))
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "min_cost_perfect_matching");
        }

        const Standard::Algorithms::elapsed_time_ns ti1;

        const auto other = Standard::Algorithms::Graphs::min_cost_perfect_matching_other<weight_t2, weight_t>(graf);

        [[maybe_unused]] const auto elapsed1 = ti1.elapsed();

        ::Standard::Algorithms::ert::are_equal(fast, other, "min_cost_perfect_matching_other");

        if constexpr (print_mc_mm)
        {
            const auto ratio = ::Standard::Algorithms::ratio_compute(elapsed0, elapsed1);

            constexpr auto mid_size = 10U;

            const auto graf_size = graf.size();
            auto pus = std::string(graf_size < mid_size ? "0" : "").append(std::to_string(graf_size));

            std::cout << " n "
                      << pus
                      //<< ", defa " << elapsed0
                      //<< ", th " << elapsed1
                      << ", th/defa " << ratio << '\n';
        }

        if (constexpr auto large = 5U; large < graf.size())
        {
            return;
        }

        matching_t matching_slow;

        const auto slow =
            Standard::Algorithms::Graphs::min_cost_perfect_matching_slow<weight_t2, weight_t>(graf, matching_slow);

        ::Standard::Algorithms::ert::are_equal(fast, slow, "min_cost_perfect_matching_slow");

        // todo(p3): compare the matchings.
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::min_cost_perfect_matching_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
