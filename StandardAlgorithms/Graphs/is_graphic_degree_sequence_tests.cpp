#include"is_graphic_degree_sequence_tests.h"
#include"../Utilities/test_utilities.h"
#include"graph_helper.h"
#include"is_graphic_degree_sequence.h"
#include<algorithm>
#include<unordered_set>

namespace
{
    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<std::int32_t> &&degree_sorted_desc,
            std::vector<std::vector<std::int32_t>> &&graph = {}, bool expected = {}) noexcept
            : base_test_case(std::move(name))
            , Degree_sorted_desc(std::move(degree_sorted_desc))
            , Graph(std::move(graph))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto degree_sorted_desc() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Degree_sorted_desc;
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const std::vector<std::vector<std::int32_t>> &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> bool
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            if (!Expected)
            {
                return;
            }

            const auto size = static_cast<std::int32_t>(Graph.size());
            if (size == 0)
            {
                return;
            }

            Standard::Algorithms::require_positive(size, "graph size.");
            Standard::Algorithms::Graphs::require_simple_graph(Graph);

            std::vector<std::int32_t> degrees(size);

            std::transform(Graph.cbegin(), Graph.cend(), degrees.begin(),
                [] [[nodiscard]] (const std::vector<std::int32_t> &vec)
                {
                    return static_cast<std::int32_t>(vec.size());
                });

            std::sort(degrees.begin(), degrees.end(), std::greater<>{});

            ::Standard::Algorithms::ert::are_equal(Degree_sorted_desc, degrees, "degree sorted desc");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("degree sorted desc", Degree_sorted_desc, str);
            ::Standard::Algorithms::print("graph", Graph, str);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::vector<std::int32_t> Degree_sorted_desc;
        std::vector<std::vector<std::int32_t>> Graph;
        bool Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "test1", // NOLINTNEXTLINE
            { 3, 3, 3, 3, 3, 3 }, // NOLINTNEXTLINE
            { { 1, 4, 3 }, { 0, 2, 3 }, { 5, 1, 4 }, { 5, 1, 0 }, { 5, 0, 2 }, { 4, 3, 2 } }, true });

        test_cases.push_back({ "los parasitos", // NOLINTNEXTLINE
            { 6, 6, 5, 4, 3, 3, 1 } });

        test_cases.push_back({ "P2-4", { 4, 1, 1 } });

        test_cases.push_back({ "P2", { 2, 1, 1 }, { { 2, 1 }, { 0 }, { 0 } }, true });

        test_cases.push_back({ "pair", { 1, 1 }, { { 1 }, { 0 } }, true });

        test_cases.push_back({ "three", { 1, 1, 1 } });

        test_cases.push_back({ "empty", {}, {}, true });

        test_cases.push_back({ "trivial", { 0 }, { {} }, true });

        test_cases.push_back({ "not trivial 1", { 1 } });

        test_cases.push_back({ "not trivial 2", { 2 } });
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<std::vector<std::int32_t>> graph;

        const auto actual = Standard::Algorithms::Graphs::is_graphic_degree_sequence(test.degree_sorted_desc(), graph);

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "result");
        if (!actual)
        {
            return;
        }

        ::Standard::Algorithms::ert::are_equal(test.graph(), graph, "graph");
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::is_graphic_degree_sequence_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
