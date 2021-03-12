#include"segment_tree_lazy_tests.h"
#include"../Numbers/standard_operations.h"
#include"../Utilities/ert.h"
#include"segment_tree.h"
#include"segment_tree_lazy.h"
#include<numeric> // accumulate

namespace
{
    using int_t = std::uint16_t;
    using lambda_t = int_t (*)(const int_t &, const int_t &);

    template<class t_tree>
    constexpr void test_helper(const std::vector<int_t> &data, lambda_t lambda,
        const std::pair<std::size_t, int_t> &index_value, const int_t expected, const std::string &name,
        const int_t zero = {})
    {
        const auto &some_index = index_value.first;
        const auto &some_value = index_value.second;

        t_tree tree(data, lambda, zero);

        const auto max_index = tree.max_index_exclusive();
        ::Standard::Algorithms::ert::are_equal(data.size(), max_index, "max_index_exclusive_" + name);

        tree.set(some_index, some_value);

        const auto actual = tree.get(0, max_index);
        ::Standard::Algorithms::ert::are_equal(expected, actual, "Get " + name);
    }

    template<class t_tree>
    constexpr void test(const std::string &test_name)
    {
        const std::vector<int_t> data{// NOLINTNEXTLINE
            1, 3, 7, 9, 12
        };

        constexpr std::size_t some_index = 2;
        Standard::Algorithms::require_greater(data.size(), some_index, "someIndex");

        constexpr int_t some_value = 101;

        // Sum, XOR, min/max, count, GCD/LCM.
        // 2D, set/get a segment.
        {
            const auto name = test_name + " Add";
            auto lambda = Standard::Algorithms::Numbers::sum_unchecked<int_t>;

            const auto acc = std::accumulate(data.cbegin(), data.cend(), int_t{}, lambda);
            const auto expected = static_cast<int_t>(lambda(some_value, acc) - data[some_index]);

            test_helper<t_tree>(data, lambda, { some_index, some_value }, expected, name);
        }
        {
            const auto name = test_name + " XOR";
            auto lambda = Standard::Algorithms::Numbers::xorr<int_t>;

            const auto acc = std::accumulate(data.cbegin(), data.cend(), int_t{}, lambda);
            const auto expected = static_cast<int_t>(lambda(lambda(some_value, acc), data[some_index]));

            test_helper<t_tree>(data, lambda, { some_index, some_value }, expected, name);
        }
        {
            const auto name = test_name + " Max";
            auto lambda = Standard::Algorithms::Numbers::max<int_t>;

            const auto acc = std::accumulate(data.cbegin(), data.cend(), data[0], lambda);

            test_helper<t_tree>(data, lambda, { some_index, data[some_index + 1U] }, acc, name);

            const auto expected = static_cast<int_t>(lambda(lambda(some_value, acc), data[some_index]));

            test_helper<t_tree>(data, lambda, { some_index, some_value }, expected, name);
        }
        {
            const auto name = test_name + " GCD";
            const std::vector<int_t> data2{// NOLINTNEXTLINE
                12 * 19, 36, 256
            };

            auto lambda = Standard::Algorithms::Numbers::gcd<int_t>;

            constexpr int_t some_value2 = 21 * 33;
            constexpr int_t expected = 3;

            test_helper<t_tree>(data2, lambda, { some_index, some_value2 }, expected, name);
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::segment_tree_lazy_tests()
{
    test<Standard::Algorithms::Trees::segment_tree_lazy<int_t, lambda_t>>("segment_tree_lazy_");

    test<Standard::Algorithms::Trees::segment_tree<int_t, lambda_t>>("segment_tree _ ");
}
