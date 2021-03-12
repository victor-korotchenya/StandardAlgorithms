#include <memory> // unique_ptr
#include <stdexcept>
#include "../Utilities/Random.h"
#include "../Utilities/RandomGenerator.h"
#include "segm_tree_sum_invert.h"
#include "segm_tree_sum_invert_slow.h"
#include "../test_utilities.h"
#include "segm_tree_sum_invert_tests.h"

using namespace std;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    constexpr int max_log = 7;

    void init(
        const int size,
        vector<bool>& data,
        segm_tree_sum_invert_slow& slow,
        segm_tree_sum_invert<max_log>& fast)
    {
        fast.clear();

        FillRandom(data, static_cast<size_t>(size));
        for (int i = 0; i < size; ++i)
        {
            slow.set1(i, data[i]);
            fast.init1(i, data[i]);
        }
        fast.build();
    }

    void init_ranges(const int size, IntRandom& gen, int ar[4])
    {
        for (int k = 0; k < 4; ++k)
            ar[k] = gen.operator()(0, size - 1);

        if (ar[1] < ar[0])
            swap(ar[1], ar[0]);
        if (ar[3] < ar[2])
            swap(ar[3], ar[2]);
        ++ar[1], ++ar[3];
    }

    void invert(
        const int begin,
        const int end,
        segm_tree_sum_invert_slow& slow,
        segm_tree_sum_invert<max_log>& fast)
    {
        slow.invert8(begin, end);
        fast.invert8(begin, end);
    }

    void test_get(
        const int size,
        const int begin,
        const int end,
        const segm_tree_sum_invert_slow& slow,
        segm_tree_sum_invert<max_log>& fast)
    {
        const auto s = slow.get8(begin, end);
        const auto f = fast.get8(begin, end);
        if (s != f)
        {
            const auto error = "segm_tree_sum_invert failed, size="
                + to_string(size)
                + ", range [" + to_string(begin) + ", " + to_string(end)
                + ").";
            throw runtime_error(error);
        }
    }

    void test_size(
        const int logar,
        vector<bool>& data,
        IntRandom& gen,
        segm_tree_sum_invert<max_log>& fast)
    {
        const auto max_size = 1 << logar,
            ops = max(max_size, 100);
        const auto size = gen.operator()(1, max_size);

        segm_tree_sum_invert_slow slow(size);
        init(size, data, slow, fast);

        int ar[4] = { 0, size, 0, size };
        for (int j = 0; j < ops; ++j)
        {
            if (j)
                init_ranges(size, gen, ar);
            else
                test_get(size, ar[2], ar[3], slow, fast);

            invert(ar[0], ar[1], slow, fast);
            test_get(size, ar[2], ar[3], slow, fast);
        }
    }
}

void Privet::Algorithms::Trees::Tests::segm_tree_sum_invert_tests()
{
    auto ptr = make_unique<segm_tree_sum_invert<max_log>>();
    auto& fast = *ptr;
    vector<bool> data;
    IntRandom gen;

    for (auto logar = 3; logar <= max_log; ++logar)
    {
        test_size(logar, data, gen, fast);
    }
}