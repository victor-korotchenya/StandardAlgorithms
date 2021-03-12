#include "segm_tree_sum_invert_slow.h"
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Trees
{
    segm_tree_sum_invert_slow::segm_tree_sum_invert_slow(size_t size)
        : su(RequirePositive(size, "size"))
    {
    }

    void segm_tree_sum_invert_slow::clear()
    {
        su.assign(su.size(), false);
    }

    void segm_tree_sum_invert_slow::set1(const int pos, const bool b)
    {
        su[pos] = b;
    }

    void segm_tree_sum_invert_slow::invert8(int le, int ri_x)
    {
        for (int i = le; i < ri_x; ++i)
            su[i] = !su[i];
    }

    int segm_tree_sum_invert_slow::get8(int le, int ri_x) const
    {
        int r = 0;
        for (int i = le; i < ri_x; ++i)
            r += su[i];

        return r;
    }
}