#include"page_rank_tests.h"
#include"../Utilities/test_utilities.h"
#include"page_rank.h"

namespace
{
    using edge_t = std::int32_t;
    using floating_t = Standard::Algorithms::floating_point_type;
    using graph_t = std::vector<std::vector<edge_t>>;

    constexpr floating_t epsilon = 1.0e-6;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, graph_t &&graph, const floating_t &damping_factor,
            std::vector<floating_t> &&expected) noexcept
            : base_test_case(std::move(name))
            , Graph(std::move(graph))
            , Damping_factor(damping_factor)
            , Expected(std::move(expected))
        {
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const graph_t &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto damping_factor() const &noexcept -> const floating_t &
        {
            return Damping_factor;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::vector<floating_t> &
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::append_separator(str);
            ::Standard::Algorithms::print_value("damping factor", str, Damping_factor);
            // todo(p3):  ::Standard::Algorithms::print("graph", Graph.cbegin(), Graph.cend());

            ::Standard::Algorithms::append_separator(str);
            ::Standard::Algorithms::print("expected", Expected.cbegin(), Expected.cend(), str);
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::throw_if_empty("graph", Graph);
            Standard::Algorithms::throw_if_empty("expected", Expected);

            ::Standard::Algorithms::ert::are_equal(Graph.size(), Expected.size(), "graph size");
        }

private:
        graph_t Graph;
        floating_t Damping_factor;
        std::vector<floating_t> Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr auto nodes = 4;

        constexpr floating_t damping_factor = 0.8;
        constexpr auto rest = static_cast<floating_t>((1 - damping_factor) / nodes);

        const auto graph_gena = [] [[nodiscard]] ()
        {
            // a has only in-edges;
            // b -> a, c
            // c -> a
            // d -> {a,b,c}
            return graph_t{ {}, { 0, 2 }, { 0 }, { 0, 1, 2 } };
        };

        test_cases.push_back({ "Simple", graph_gena(), damping_factor, { 1 - rest * 3, rest, rest, rest } });

        test_cases.push_back({ "Damping factor 1", graph_gena(), 1, { 1, 0, 0, 0 } });
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<floating_t> actual;
        std::vector<floating_t> buffer;

        Standard::Algorithms::Graphs::page_rank_simple(test.graph(), buffer, epsilon, actual, test.damping_factor());

        ::Standard::Algorithms::ert::are_equal_with_epsilon(test.expected(), actual, "page_rank_simple");
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::page_rank_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
