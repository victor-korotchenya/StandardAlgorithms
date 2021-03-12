#pragma once
#include "BinaryTreeUtilities.h"

namespace Privet::Algorithms::Trees
{
    // Set value at a point,
    // get "sum/max" over an interval [leftInclusive, rightExclusive).
    //
    // Twice the linear space is required.
    // The actual data are stored at the higher indexes.
    // And the "sums" are stored at the lower indexes.
    // 0 based.
    template <typename Number, typename Operation>
    class SegmentTree final
    {
        std::vector<Number> _Data;

        Operation _Operation;
        const Number _Zero;
        const size_t _MaxIndexExclusive;

    public:
        explicit SegmentTree(size_t size,
            Operation operation = {},
            //E.g. 0 for addition, 1 for multiplication.
            Number zero = {}) : _Data(BinaryTreeUtilities<>::calc_tree_size(size), zero),
            _Operation(operation),
            _Zero(zero),
            _MaxIndexExclusive(size)
        {
        }

        // Don't use it if possible - use data(), then init().
        explicit SegmentTree(const std::vector<Number>& datas,
            Operation operation = {},
            Number zero = {}) : SegmentTree(datas.size(), operation, zero)
        {
            std::copy(datas.begin(), datas.end(), _Data.begin() + datas.size());
            init();
        }

        void init()
        {
            for (auto i = _MaxIndexExclusive - 1; i; --i)
            {
                _Data[i] = _Operation(
                    _Data[BinaryTreeUtilities<>::left_child(i)],
                    _Data[BinaryTreeUtilities<>::right_child(i)]);
            }
        }

        size_t max_index_exclusive() const noexcept
        {
            return _MaxIndexExclusive;
        }

        std::vector<Number>& data()
        {
            return _Data;
        }

        //_MaxIndexExclusive = 5
        //[0, 1, 2, 3, 4] are stored at indexes [5, 6, 7, 8, 9].
        //          1
        //       /     \  _
        //      2       3
        //    /  \      /\  _
        //   4    5    6  7
        //  /\
                // 8  9
                //
                //In the constructor(), "i" goes from 4 down to 1:
                // a[4] = a[8]+a[9]; then [3] = [6]+[7]; [2] = [4]+[5]; [1] = [2]+[3];
                //
                //Assume, get(0, 5) is being called.
                // After adding _MaxIndexExclusive: left=5, right=10.
                //  result_sum += [5]; left=(5+1)/2=3; right=10/2=5;
                // left=3; right=5;
                //  result_sum += [3] + [5-1] == [3] + [4];
                // Stop as left == right == 2.
                // That is, result_sum = [5]+ ([3] + 4).

                // Return the aggregate on the interval [leftInclusive, rightExclusive).
                // The "index" starts from 0.
        Number get(size_t leftInclusive, size_t rightExclusive) const
        {
            BinaryTreeUtilities<>::check_indexes(
                leftInclusive, rightExclusive,
                _MaxIndexExclusive, "SegmentTree::get");

            Number result = _Zero;

            leftInclusive += _MaxIndexExclusive;
            rightExclusive += _MaxIndexExclusive;

            while (leftInclusive < rightExclusive)
            {
                if (leftInclusive & 1) //"leftInclusive" is right child.
                {//Thus, its left sibling must not be included.
                    result = _Operation(result, _Data[leftInclusive]);
                    ++leftInclusive; //Go to the right sibling parent.
                } // else both are included - go to the parent.
                leftInclusive >>= 1;

                if (rightExclusive & 1) //"rightExclusive" is right child.
                {//Include its left sibling.
                    result = _Operation(_Data[rightExclusive ^ 1], result);
                }
                // Go to the parent as "rightExclusive" means not inclusive.
                rightExclusive >>= 1;
            }

            return result;
        }

        //The "index" starts from 0.
        void set(size_t index, const Number& value)
        {
            BinaryTreeUtilities<>::check_index(
                index, _MaxIndexExclusive, "SegmentTree::set");

            index += _MaxIndexExclusive;
            _Data[index] = value;

            while (1 < index)
            {
                index = BinaryTreeUtilities<>::parent(index);
                _Data[index] = _Operation(
                    _Data[BinaryTreeUtilities<>::left_child(index)],
                    _Data[BinaryTreeUtilities<>::right_child(index)]);
            }
        }
    };
}