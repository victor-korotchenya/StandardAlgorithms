#pragma once
#include <vector>

namespace Privet::Algorithms::Trees
{
    //It is slow - see "segm_tree_sum_invert".
    // Given bool[n]:
    // - get sum over [b, e).
    // - invert [b, e) i.e. false => true and vv.
    class segm_tree_sum_invert_slow final
    {
        std::vector<bool> su;

    public:
        explicit segm_tree_sum_invert_slow(size_t size);

        void clear();

        //Set the given bit.
        void set1(const int pos, const bool b);

        int get8(int le, int ri_x) const;

        void invert8(int le, int ri_x);
    };
}