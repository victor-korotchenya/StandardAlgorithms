#pragma once
#include"../Numbers/to_unsigned.h"
#include"../Utilities/project_constants.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"multiway_point.h"
#include<vector>

namespace Standard::Algorithms::Trees::Tests
{
    using key_t = std::uint8_t;
    using tuple_t = std::vector<key_t>;

    constexpr auto dimensions = 5;

    constexpr key_t min_value = ::Standard::Algorithms::Utilities::min_char;
    constexpr key_t max_value = ::Standard::Algorithms::Utilities::max_char;

    static_assert(min_value <= max_value);
} // namespace Standard::Algorithms::Trees::Tests

namespace Standard::Algorithms::Trees::Tests::Inner
{
    constexpr auto max_steps = 37U;

    static_assert(
        max_steps < ::Standard::Algorithms::Numbers::to_unsigned(::Standard::Algorithms::Utilities::stack_max_size));

    [[nodiscard]] auto build_random_multi_dim_point(
        Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd) noexcept(false) -> tuple_t;

    template<class tree_t>
    constexpr void choose_expected_slow(const std::vector<tuple_t> &points,
        const std::pair<const tuple_t &, const tuple_t &> limits, std::vector<tuple_t> &expected)
    {
        expected.clear();

        for (const auto &point : points)
        {
            if (is_between<dimensions, tuple_t>(point, limits))
            {
                expected.push_back(point);
            }
        }
    }

    void ensure_non_empty_range(tuple_t &low, tuple_t &high) noexcept;

    template<class tree_t, class node_t>
    constexpr void find_subtest(std::vector<tuple_t> &points, const tree_t &tree,
        Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd, std::vector<tuple_t> &expected,
        std::function<void(const node_t &)> &visit, std::vector<tuple_t> &actual, const bool can_swap)
    {
        auto low = build_random_multi_dim_point(rnd);
        auto high = build_random_multi_dim_point(rnd);

        if (can_swap)
        {
            ensure_non_empty_range(low, high);
        }

        {
            const std::pair<const tuple_t &, const tuple_t &> limits{ low, high };
            choose_expected_slow<tree_t>(points, limits, expected);

            std::sort(expected.begin(), expected.end());
        }

        try
        {
            actual.clear();
            tree.find(low, visit, high);

            std::sort(actual.begin(), actual.end());

            ::Standard::Algorithms::ert::are_equal(expected, actual, "tree find");
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            ::Standard::Algorithms::print("low", low, str);
            ::Standard::Algorithms::print("high", high, str);
            str << "\nFind error: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }

    template<class tree_t>
    constexpr void insert_subtest(
        std::vector<tuple_t> &points, tree_t &tree, Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd)
    {
        points.push_back(build_random_multi_dim_point(rnd));

        // todo(p3): prepare all the data, then sort, select a median, ..
        const auto &cur = points.back();
        tree.insert(cur);
        ::Standard::Algorithms::ert::are_equal(points.size(), tree.size(), "tree size");
    }

    template<class tree_t, class node_t>
    void random_test(std::vector<tuple_t> &points)
    {
        std::vector<tuple_t> actual;

        std::function<void(const node_t &)> visit = [&actual](const node_t &node)
        {
            actual.push_back(node.point);
        };

        std::vector<tuple_t> expected;
        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_value, max_value);
        tree_t tree;

        for (std::uint32_t step{}; step < max_steps; ++step)
        {
            const auto can_swap = (step & 1U) == 0U;

            find_subtest<tree_t, node_t>(points, tree, rnd, expected, visit, actual, can_swap);

            insert_subtest<tree_t>(points, tree, rnd);
        }
    }
} // namespace Standard::Algorithms::Trees::Tests::Inner

namespace Standard::Algorithms::Trees::Tests
{
    template<class tree_t, class node_t = typename tree_t::node_t>
    void multiway_search_tree_tests()
    {
        std::vector<tuple_t> points;
        points.reserve(Inner::max_steps);

        try
        {
            Inner::random_test<tree_t, node_t>(points);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            ::Standard::Algorithms::print("points", points, str);
            str << "\nError: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }
} // namespace Standard::Algorithms::Trees::Tests
