#include"segm_tree_sum_invert_tests.h"
#include"../Numbers/shift.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"segm_tree_sum_invert.h"
#include"segm_tree_sum_invert_slow.h"
#include<memory> // unique_ptr

namespace
{
    constexpr auto max_log = 7;

    using range_t = std::pair<std::int32_t, std::int32_t>;

    void init_trees(const std::int32_t size, std::vector<bool> &data,
        Standard::Algorithms::Trees::segm_tree_sum_invert_slow &slow,
        Standard::Algorithms::Trees::segm_tree_sum_invert<max_log> &fast)
    {
        fast.clear();

        Standard::Algorithms::Utilities::fill_random(data, static_cast<std::size_t>(size));

        for (std::int32_t ind{}; ind < size; ++ind)
        {
            const auto flag = data[ind];

            slow.set1(ind, flag);
            fast.set1(ind, flag);
        }

        fast.build();
    }

    constexpr void init_range(
        const std::int32_t size, Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd, range_t &range)
    {
        assert(0 < size);

        auto &left = range.first;
        auto &ri_exclusive = range.second;

        {
            const auto max_val = size - 1;

            left = rnd.operator() (0, max_val);
            ri_exclusive = rnd.operator() (0, max_val);
        }

        if (ri_exclusive < left)
        {
            std::swap(ri_exclusive, left);
        }

        ++ri_exclusive;

        Standard::Algorithms::require_non_negative(left, "left");

        Standard::Algorithms::require_greater(ri_exclusive, left, "ri_exclusive");

        Standard::Algorithms::require_less_equal(ri_exclusive, size, "ri_exclusive");
    }

    void invert(const range_t &range, Standard::Algorithms::Trees::segm_tree_sum_invert_slow &slow,
        Standard::Algorithms::Trees::segm_tree_sum_invert<max_log> &fast)
    {
        const auto &left = range.first;
        const auto &ri_exclusive = range.second;

        slow.invert_in_range(left, ri_exclusive);
        fast.invert_in_range(left, ri_exclusive);
    }

    void test_ghetto(const std::int32_t size, const range_t &range,
        const Standard::Algorithms::Trees::segm_tree_sum_invert_slow &slow,
        Standard::Algorithms::Trees::segm_tree_sum_invert<max_log> &fast)
    {
        const auto &left = range.first;
        const auto &ri_exclusive = range.second;

        const auto slow_sum = slow.count_in_range(left, ri_exclusive);

        const auto fast_sum = static_cast<std::size_t>(fast.count_in_range(left, ri_exclusive));

        ::Standard::Algorithms::ert::are_equal(slow_sum, fast_sum,
            "count_in_range[" + std::to_string(left) + ", " + std::to_string(ri_exclusive) + ") size" +
                std::to_string(size));
    }

    void test_size(const std::int32_t logar, std::vector<bool> &data,
        Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd,
        Standard::Algorithms::Trees::segm_tree_sum_invert<max_log> &fast)
    {
        constexpr auto min_size = 1;
        const auto max_size = ::Standard::Algorithms::Numbers::shift_left(1, logar);

        const auto size = Standard::Algorithms::require_positive(rnd(min_size, max_size), "size");

        Standard::Algorithms::Trees::segm_tree_sum_invert_slow slow(size);

        init_trees(size, data, slow, fast);

        range_t range_one{ 0, size };
        range_t range_two{ 0, size };

        constexpr auto upper = 100;
        const auto ops = std::max(max_size, upper);

        for (std::int32_t index{}; index < ops; ++index)
        {
            if (index == 0)
            {
                test_ghetto(size, range_two, slow, fast);
            }
            else
            {
                init_range(size, rnd, range_one);
                init_range(size, rnd, range_two);
            }

            invert(range_one, slow, fast);
            test_ghetto(size, range_two, slow, fast);
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::segm_tree_sum_invert_tests()
{
    auto uni = std::make_unique<Standard::Algorithms::Trees::segm_tree_sum_invert<max_log>>();

    auto &fast = *uni;
    std::vector<bool> data;
    Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

    for (auto logar = 3; logar <= max_log; ++logar)
    {
        test_size(logar, data, rnd, fast);
    }
}
