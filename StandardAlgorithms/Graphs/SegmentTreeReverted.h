#pragma once
#include <vector>
#include "BinaryTreeUtilities.h"

namespace Privet::Algorithms::Trees
{
    //Set value on an interval [leftInclusive, rightExclusive);
    // get "sum" at a point.
    template <typename Number, typename Operation>
    class SegmentTreeReverted final
    {
        std::vector<Number> _Data;

        Operation _Operation;
        const Number _Zero;
        const size_t _MaxIndexExclusive;

    public:

        explicit SegmentTreeReverted(size_t initialSize,
            Operation operation = {},
            //E.g. 0 for addition, 1 for multiplication.
            Number zero = {});

        explicit SegmentTreeReverted(const std::vector<Number>& data,
            Operation operation = {},
            Number zero = {});

        inline size_t max_index_exclusive() const
        {
            return  _MaxIndexExclusive;
        }

        //Return the aggregate at a point index.
        Number get(size_t index) const;

        void set(size_t leftInclusive, size_t rightExclusive, const Number& value);

        //To inspect all the items in O(n), push all the changes to the leaves
        // and then get the items starting with the index max_index_exclusive().
        //
        //It works only! if
        //  the order of element modifications doesn't affect the result.
        // Assignment, for example, isn't good.
        //Use the lazy propagation!
        const std::vector<Number>& push_changes_to_leaves();
    };

    template <typename Number, typename Operation>
    SegmentTreeReverted<Number, Operation>::SegmentTreeReverted(
        size_t size, Operation operation, Number zero)
        : _Data(BinaryTreeUtilities<>::calc_tree_size(size)),
        _Operation(operation),
        _Zero(zero),
        _MaxIndexExclusive(size)
    {
    }

    template <typename Number, typename Operation>
    SegmentTreeReverted<Number, Operation>::SegmentTreeReverted(
        const std::vector<Number>& data, Operation operation, Number zero)
        : SegmentTreeReverted(data.size(), operation, zero)
    {
        std::copy(data.begin(), data.end(), _Data.begin() + data.size());

        //All data are ready already.
    }

    //          1
    //       /     \
            //      2       3
            //    /  \      /\
            //   4    5    6  7
            //  /\
            // 8  9
    template <typename Number, typename Operation>
    void SegmentTreeReverted<Number, Operation>::set(
        size_t leftInclusive, size_t rightExclusive, const Number& value)
    {
        BinaryTreeUtilities<>::check_indexes(
            leftInclusive, rightExclusive,
            _MaxIndexExclusive, "SegmentTreeReverted::set");

        leftInclusive += _MaxIndexExclusive;
        rightExclusive += _MaxIndexExclusive;

        while (leftInclusive < rightExclusive)
        {
            if (leftInclusive & 1) //"leftInclusive" is right child.
            {//Thus, its left sibling must not be included.
                _Data[leftInclusive] = _Operation(_Data[leftInclusive], value);
                ++leftInclusive; //Go to the right sibling parent.
            } // else both are included - go to the parent.
            leftInclusive >>= 1;

            if (rightExclusive & 1) //"rightExclusive" is right child.
            {//Include its left sibling.
                _Data[rightExclusive ^ 1] = _Operation(value, _Data[rightExclusive ^ 1]);
            } // Just go to the parent as "rightExclusive" means not inclusive.
            rightExclusive >>= 1;
        }
    }

    template <typename Number, typename Operation>
    Number SegmentTreeReverted<Number, Operation>::get(size_t index) const
    {
        BinaryTreeUtilities<>::check_index(
            index, _MaxIndexExclusive, "SegmentTreeReverted::get");

        Number result = _Zero;
        index += _MaxIndexExclusive;

        do
        {
            result = _Operation(result, _Data[index]);
            index >>= 1;
        } while (index);

        return result;
    }

    template <typename Number, typename Operation>
    const std::vector<Number>&
        SegmentTreeReverted<Number, Operation>::push_changes_to_leaves()
    {
        for (size_t i = 1; i < _MaxIndexExclusive; ++i)
        {
            const auto left = BinaryTreeUtilities<>::left_child(i);
            _Data[left] = _Operation(_Data[left], _Data[i]);

            const auto right = BinaryTreeUtilities<>::right_child(i);
            _Data[right] = _Operation(_Data[i], _Data[right]);

            _Data[i] = _Zero;
        }

        return _Data;
    }
}