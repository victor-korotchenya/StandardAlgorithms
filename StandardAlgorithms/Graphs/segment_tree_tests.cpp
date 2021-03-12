#include <memory>
#include <tuple>
#include "segment_tree_tests.h"
#include "segment_tree_max.h"
#include "segment_tree_assign_sum.h"
#include "segment_tree_add_sum.h"
#include "../Utilities/Random.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    void gen_range(
        vector<tuple<int, int, int>>& ops, IntRandom& r, const int n,
        int& value, int& left, int& right)
    {
        constexpr auto cutter = 1 << 28;
        value = r(-cutter, cutter);

        left = r(0, n - 1);
        right = r(0, n - 1);
        if (right < left) swap(left, right);

        ops.emplace_back(value, left, right);
    }

    void segment_tree_tests_impl()
    {
        constexpr auto size_log = 10,
            n = 100;
        constexpr auto max_ops = 20;

        vector<tuple<int, int, int>> ops;
        vector<int64_t> v(n), v_assign(n);
        IntRandom r;

        auto tree_max_ptr = make_unique<segment_tree_max<int64_t, size_log>>();
        auto& tree_max = *tree_max_ptr;

        auto tree_max2_ptr = make_unique<segment_tree_get_max_add_range<int64_t, size_log>>(n);
        auto& tree_max2 = *tree_max2_ptr;

        auto tree_ass_sum_ptr = make_unique<segment_tree_assign_sum<int64_t>>(size_log);
        auto& tree_ass_sum = *tree_ass_sum_ptr;

        auto tree_add_sum_ptr = make_unique<segment_tree_add_sum<int64_t>>(size_log);
        auto& tree_add_sum = *tree_add_sum_ptr;

        int left, right, value;
        for (auto i = 0; i < max_ops; ++i)
        {
            // get
            gen_range(ops, r, n, value, left, right);

            const auto max_m = tree_max.get_max(left, right + 1);
            const auto max_m3 = tree_max2.get_max(left, right);
            Assert::AreEqual(max_m, max_m3, "segment_tree_max get");

            const auto ass_sum_m = tree_ass_sum.get_sum(left, right + 1);
            const auto add_sum_m = tree_add_sum.get_sum(left, right + 1);

            auto max_m2 = numeric_limits<int64_t>::min();
            int64_t ass_sum_m2 = 0, add_sum_m2 = 0;
            for (auto k = left; k <= right; ++k)
            {
                max_m2 = max(max_m2, v[k]);
                ass_sum_m2 += v_assign[k];
                add_sum_m2 += v[k];
            }

            if (max_m != max_m2)
                Assert::AreEqual(max_m, max_m2, "max_segment_tree");

            if (add_sum_m != add_sum_m2)
                Assert::AreEqual(add_sum_m, add_sum_m2, "segment_tree_add_sum");

            if (ass_sum_m != ass_sum_m2)
                Assert::AreEqual(ass_sum_m, ass_sum_m2, "segment_tree_assign_sum");

            // add
            gen_range(ops, r, n, value, left, right);

            tree_max.add_range(value, left, right + 1);
            tree_max2.add_range(value, left, right);
            tree_ass_sum.add(value, left, right + 1);
            tree_add_sum.add(value, left, right + 1);

            for (auto k = left; k <= right; ++k)
            {
                v[k] += value;
                v_assign[k] = value;
            }
        }
    }
}

void Privet::Algorithms::Trees::Tests::segment_tree_tests()
{
    segment_tree_tests_impl();
}