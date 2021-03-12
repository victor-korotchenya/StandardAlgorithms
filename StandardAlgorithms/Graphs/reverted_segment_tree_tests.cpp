#include"reverted_segment_tree_tests.h"
#include"../Numbers/standard_operations.h"
#include"../Utilities/ert.h"
#include"segment_tree_lazy.h"
#include"segment_tree_reverted.h"
#include<tuple>

namespace
{
    using int_t = std::uint16_t;
    using lambda_t = int_t (*)(const int_t &, const int_t &);

    template<class t_tree_factory>
    constexpr void test_helper(t_tree_factory tree_factory, const std::pair<std::size_t, std::size_t> &range,
        lambda_t lambda, const std::vector<int_t> &data, const std::tuple<int_t, int_t, int_t> &value_index_expected,
        const std::string &name, const int_t zero = {})
    {
        const auto &left_inclusive = range.first;
        const auto &right_exclusive = range.second;

        const auto &some_value = std::get<0>(value_index_expected);
        const auto &index = std::get<1>(value_index_expected);
        const auto &expected = std::get<2>(value_index_expected);

        auto tree = tree_factory(data, lambda, zero);
        const auto max_index = tree.max_index_exclusive();
        ::Standard::Algorithms::ert::are_equal(data.size(), max_index, "max_index_exclusive_" + name);

        tree.set(left_inclusive, right_exclusive, some_value);
        {
            const auto actual = tree.get(index);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "Get " + name);
        }

        //  tree.push_changes_to_leaves();
        // const auto leaves = tree.data();
        //{
        //    const auto actual = tree.get(index);
        //    ::Standard::Algorithms::ert::are_equal(expected, actual, "push_" + name);
        //}
        //{
        //    const auto actual = leaves[index + tree.max_index_exclusive()];
        //    ::Standard::Algorithms::ert::are_equal(expected, actual, "leaves_" + name);
        //}
    }

    template<class t_tree_factory>
    constexpr void test(const std::string &test_name, t_tree_factory &tree_factory)
    {
        const std::vector<int_t> data{// NOLINTNEXTLINE
            1, 3, 7, 9, 12
        };

        constexpr std::size_t left_inclusive{};
        constexpr std::size_t right_exclusive = 3;
        constexpr std::size_t some_index = 1;
        constexpr const int_t some_value = 101;
        {
            const auto name = test_name + " Add";
            auto lambda = Standard::Algorithms::Numbers::sum_unchecked<int_t>;
            const auto expected = static_cast<int_t>(lambda(some_value, data[some_index]));

            test_helper<t_tree_factory>(tree_factory, { left_inclusive, right_exclusive }, lambda, data,
                { some_value, some_index, expected }, name);
        }
        {
            const auto name = test_name + " XOR";
            auto lambda = Standard::Algorithms::Numbers::xorr<int_t>;
            const auto expected = static_cast<int_t>(lambda(some_value, data[some_index]));

            test_helper<t_tree_factory>(tree_factory, { left_inclusive, right_exclusive }, lambda, data,
                { some_value, some_index, expected }, name);
        }
        {
            const auto name = test_name + " Max";
            auto lambda = Standard::Algorithms::Numbers::max<int_t>;
            const auto expected = static_cast<int_t>(lambda(some_value, data[some_index]));

            test_helper<t_tree_factory>(tree_factory, { left_inclusive, right_exclusive }, lambda, data,
                { some_value, some_index, expected }, name);
        }
        {
            const auto name = test_name + " GCD";
            auto lambda = Standard::Algorithms::Numbers::gcd<int_t>;
            const std::vector<int_t> data2{// NOLINTNEXTLINE
                12 * 19, 36, 256 * 3 * 3
            };

            constexpr std::size_t some_index2 = 2;
            constexpr int_t some_value2 = 21 * 33;
            constexpr int_t expected = 3 * 3;

            test_helper<t_tree_factory>(tree_factory, { left_inclusive, right_exclusive }, lambda, data2,
                { some_value2, some_index2, expected }, name);
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::reverted_segment_tree_tests()
{
    {
        auto factory = [] [[nodiscard]] (const std::vector<int_t> &data, lambda_t lambda, const int_t &zero)
        {
            segment_tree_lazy<int_t, lambda_t> result1(data, lambda, zero, lambda);

            return result1;
        };

        test("segment_tree_lazy_", factory);
    }
    {
        auto factory = [] [[nodiscard]] (const std::vector<int_t> &data, lambda_t lambda, const int_t &zero)
        {
            segment_tree_reverted<int_t, lambda_t> result1(data, lambda, zero);
            return result1;
        };

        test("segment_tree_reverted_", factory);
    }
}
