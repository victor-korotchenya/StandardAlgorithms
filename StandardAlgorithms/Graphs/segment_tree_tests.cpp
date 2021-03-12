#include"segment_tree_tests.h"
#include"../Numbers/shift.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"segment_tree_add_sum.h"
#include"segment_tree_assign_sum.h"
#include"segment_tree_get_max_add_range.h"
#include"segment_tree_max.h"
#include<memory>
#include<tuple>

namespace
{
    using val_range_incl_t = std::tuple<std::int32_t, std::uint32_t, std::uint32_t>;

    constexpr auto size_log = 10U;
    constexpr auto size = 100;

    struct segm_tree_test_context final
    {
        std::vector<val_range_incl_t> &ops;

        Standard::Algorithms::Trees::segment_tree_max<std::int64_t, size_log> &tree_max;

        Standard::Algorithms::Trees::segment_tree_get_max_add_range<std::int64_t, size_log> &tree_max2;

        Standard::Algorithms::Trees::segment_tree_assign_sum<std::int64_t> &tree_ass_sum;

        Standard::Algorithms::Trees::segment_tree_add_sum<std::int64_t> &tree_add_sum;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{ 0, size - 1 };

        std::vector<std::int64_t> vals_sum = std::vector<std::int64_t>(size);

        std::vector<std::int64_t> vals_assign = std::vector<std::int64_t>(size);
    };

    constexpr auto gen_range(Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd,
        std::vector<val_range_incl_t> &ops) -> const val_range_incl_t &
    {
        constexpr auto cutter = ::Standard::Algorithms::Numbers::shift_left(static_cast<std::int32_t>(1), 28);

        const auto value = rnd(-cutter, cutter);
        auto left = static_cast<std::uint32_t>(rnd());
        auto right = static_cast<std::uint32_t>(rnd());

        if (right < left)
        {
            std::swap(left, right);
        }

        ops.emplace_back(value, left, right);

        return ops.back();
    }

    constexpr void max_test(segm_tree_test_context &context)
    {
        const auto [value, left, right] = gen_range(context.rnd, context.ops);

        const auto max_m = context.tree_max.max(left, right + 1U);
        const auto max_m3 = context.tree_max2.max(left, right);
        ::Standard::Algorithms::ert::are_equal(max_m, max_m3, "segment_tree_max get");

        const auto ass_sum_m = context.tree_ass_sum.sum(left, right + 1U);
        const auto add_sum_m = context.tree_add_sum.sum(left, right + 1U);

        auto max_m2 = std::numeric_limits<std::int64_t>::min();
        std::int64_t ass_sum_m2{};
        std::int64_t add_sum_m2{};

        for (auto powered = left; powered <= right; ++powered)
        {
            max_m2 = std::max(max_m2, context.vals_sum[powered]);
            ass_sum_m2 += context.vals_assign[powered];
            add_sum_m2 += context.vals_sum[powered];
        }

        ::Standard::Algorithms::ert::are_equal(max_m, max_m2, "max_segment_tree");

        ::Standard::Algorithms::ert::are_equal(add_sum_m, add_sum_m2, "segment_tree_add_sum");

        ::Standard::Algorithms::ert::are_equal(ass_sum_m, ass_sum_m2, "segment_tree_assign_sum");
    }

    constexpr void add_test(segm_tree_test_context &context)
    {
        const auto [value, left, right] = gen_range(context.rnd, context.ops);

        context.tree_max.add(value, left, right + 1U);
        context.tree_max2.add(value, left, right);
        context.tree_ass_sum.add(value, left, right + 1U);
        context.tree_add_sum.add(value, left, right + 1U);

        for (auto powered = left; powered <= right; ++powered)
        {
            context.vals_sum[powered] += value;
            context.vals_assign[powered] = value;
        }
    }

    void segment_tree_tests_impl(std::vector<val_range_incl_t> &ops)
    {
        auto tree_max_ptr = std::make_unique<Standard::Algorithms::Trees::segment_tree_max<std::int64_t, size_log>>();

        auto &tree_max = *tree_max_ptr;

        auto tree_max2_ptr =
            std::make_unique<Standard::Algorithms::Trees::segment_tree_get_max_add_range<std::int64_t, size_log>>(size);

        auto &tree_max2 = *tree_max2_ptr;

        auto tree_ass_sum_ptr =
            std::make_unique<Standard::Algorithms::Trees::segment_tree_assign_sum<std::int64_t>>(size_log);

        auto &tree_ass_sum = *tree_ass_sum_ptr;

        auto tree_add_sum_ptr =
            std::make_unique<Standard::Algorithms::Trees::segment_tree_add_sum<std::int64_t>>(size_log);

        auto &tree_add_sum = *tree_add_sum_ptr;

        segm_tree_test_context context{ ops, tree_max, tree_max2, tree_ass_sum, tree_add_sum };

        constexpr auto max_ops = 20;

        for (std::int32_t step{}; step < max_ops; ++step)
        {
            max_test(context);
            add_test(context);
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::segment_tree_tests()
{
    std::vector<val_range_incl_t> ops;

    try
    {
        segment_tree_tests_impl(ops);
    }
    catch (const std::exception &exc)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << " Ops size: " << ops.size();
        ::Standard::Algorithms::print(" ops: ", ops, str);
        str << ". Error: " << exc.what();

        throw std::runtime_error(str.str());
    }
}
