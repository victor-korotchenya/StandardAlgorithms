#include"disjoint_set_print_tests.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"disjoint_set.h"
#include<array>

namespace
{
    using int_t = std::uint32_t;
    using buffers_t = std::array<std::vector<int_t>, 2>;

    constexpr auto dsu_buffer_id = 0;
    constexpr auto slow_buffer_id = 1;

    constexpr int_t size = 10;
    constexpr std::size_t total_steps = size * 2ZU;

    [[nodiscard]] constexpr auto root_slow(const std::vector<int_t> &roots, int_t node) -> int_t
    {
        for (std::size_t iterations{};;)
        {
            const auto &root = roots.at(node);
            if (root == node)
            {
                return node;
            }

            Standard::Algorithms::require_greater(roots.size(), iterations, "root_slow iterations");
            ++iterations;

            node = root;
        }
    }

    constexpr void unite_test(std::vector<int_t> &roots, const int_t one, auto &dsu, const int_t two)
    {
        auto root_1 = root_slow(roots, one);
        auto root_2 = root_slow(roots, two);

        const auto must_unite = root_1 != root_2;
        if (must_unite)
        {
            roots.at(root_1) = root_2;
        }

        const auto actual = dsu.unite(one, two);
        ::Standard::Algorithms::ert::are_equal(must_unite, actual, "unite");
    }

    constexpr void print_set_slow(const std::vector<int_t> &roots, int_t set_id, std::vector<int_t> &buffer)
    {
        ::Standard::Algorithms::ert::are_equal(roots.size(), static_cast<std::size_t>(size), "roots size");

        set_id = root_slow(roots, set_id);

        for (int_t node{}; node < size; ++node)
        {
            const auto parent = root_slow(roots, node);
            if (parent == set_id)
            {
                buffer.push_back(node);
            }
        }
    }

    constexpr void print_test(
        const std::vector<int_t> &roots, const auto &dsu, buffers_t &buffers, const auto &print_func)
    {
        for (int_t node{}; node < size; ++node)
        {
            buffers[0].clear();
            buffers[1].clear();

            print_set_slow(roots, node, buffers[slow_buffer_id]);
            dsu.print_set(print_func, node);

            std::sort(buffers[0].begin(), buffers[0].end());
            std::sort(buffers[1].begin(), buffers[1].end());

            ::Standard::Algorithms::ert::are_equal(
                buffers[1], buffers[0], "print_set buffers at node " + std::to_string(node));
        }
    }

    constexpr void test_dsu(std::vector<std::pair<int_t, int_t>> &union_steps, auto &dsu)
    {
        auto roots = Standard::Algorithms::Utilities::iota_vector<int_t>(size);
        buffers_t buffers{ std::vector<int_t>(size), std::vector<int_t>(size) };
        Standard::Algorithms::Utilities::random_t<int_t> rnd(int_t{}, size - 1);

        const auto print_func = [&buffers](const int_t &node)
        {
            buffers[dsu_buffer_id].push_back(node);
        };

        for (std::size_t step{}; step < total_steps; ++step)
        {
            const auto one = rnd();
            const auto two = rnd();
            union_steps.emplace_back(one, two);

            unite_test(roots, one, dsu, two);
            print_test(roots, dsu, buffers, print_func);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::disjoint_set_print_tests()
{
    constexpr auto is_printable = true;

    std::vector<std::pair<int_t, int_t>> union_steps;
    union_steps.reserve(total_steps);

    Standard::Algorithms::Numbers::disjoint_set<int_t, is_printable> dsu(size);

    try
    {
        test_dsu(union_steps, dsu);
    }
    catch (const std::exception &exc)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        ::Standard::Algorithms::print("Steps", union_steps, str);
        str << ", error: " << exc.what();

        throw std::runtime_error(str.str());
    }
}
