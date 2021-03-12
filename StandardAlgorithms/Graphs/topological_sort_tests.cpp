#include"topological_sort_tests.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"topological_sort.h"

namespace
{
    using vertex_t = std::size_t;
    using edges_t = std::vector<vertex_t>;
    using graph_t = std::vector<edges_t>;
    using many_edges_t = std::vector<edges_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, graph_t, many_edges_t>;

    constexpr void simple_validate(const test_case &test)
    {
        const auto &input = test.input();
        const auto size = Standard::Algorithms::require_positive(input.size(), "graph size.");

        const auto &output = test.output();
        Standard::Algorithms::require_positive(output, "output");

        for (std::size_t index{}; index < output.size(); ++index)
        {
            const auto &sub = output[index];
            const auto name = "expected indexes size at " + std::to_string(index);

            ::Standard::Algorithms::ert::are_equal(size, sub.size(), name);
            Standard::Algorithms::require_unique(sub, name);

            const auto [min_it, max_it] = std::minmax_element(sub.cbegin(), sub.cend());
            ::Standard::Algorithms::ert::are_equal(0U, *min_it, name + " min vertex");
            ::Standard::Algorithms::ert::are_equal(size - 1U, *max_it, name + " max vertex");
        }
    }

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("Many variants",
            // 0 -> 1 -> 3
            // 0 -> 2
            graph_t{ { 1, 2 }, { 3 }, {}, {}, {}, {} },
            // NOLINTNEXTLINE
            many_edges_t{ edges_t{ 0, 4, 5, 1, 2, 3 },
                // Many variants are possible here. NOLINTNEXTLINE
                edges_t{ 5, 4, 0, 2, 1, 3 } });

        test_cases.emplace_back("simple",
            // 0 -> 1 -> 2
            graph_t{ { 1 }, { 2 }, {} }, many_edges_t{ edges_t{ 0, 1, 2 } });

        test_cases.emplace_back("trivial graph", graph_t{ {} }, many_edges_t{ edges_t{ 0 } });

        test_cases.emplace_back("one edge", graph_t{ { 1 }, {} }, many_edges_t{ edges_t{ 0, 1 } });

        test_cases.emplace_back("one edge reverted", graph_t{ {}, { 0 } }, many_edges_t{ edges_t{ 1, 0 } });
    }

    // todo(p3): move out
    constexpr void require_one_of(const std::string &name, const auto &expected_many, const auto &actual_one)
    {
        assert(!name.empty());

        if (const auto iter = std::find(expected_many.cbegin(), expected_many.cend(), actual_one);
            iter != expected_many.cend()) [[likely]]
        {
            return;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "require_one_of failed. " << name << " ";
        ::Standard::Algorithms::print("actual one", actual_one, str);
        ::Standard::Algorithms::print("expected many", expected_many, str);

        throw std::runtime_error(str.str());
    }

    constexpr void run_test_case(const test_case &test)
    {
        if (const auto actual = Standard::Algorithms::Graphs::topological_sort(test.input()); actual.has_value())
            [[likely]]
        {
            require_one_of("topological_sort", test.output(), actual.value());
        }
        else
        {
            throw std::invalid_argument("The dag must have been acyclic in topological_sort.");
        }

        if (const auto actual = Standard::Algorithms::Graphs::topological_sort_via_dfs(test.input()); actual.has_value())
            [[likely]]
        {
            require_one_of("topological_sort_via_dfs", test.output(), actual.value());
        }
        else
        {
            throw std::invalid_argument("The dag must have been acyclic in topological_sort_via_dfs.");
        }
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::topological_sort_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
