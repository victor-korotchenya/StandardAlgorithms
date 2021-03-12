#pragma once
#include <vector>
#include <stdexcept>
#include "../Utilities/StreamUtilities.h"

namespace Privet::Algorithms::Trees
{
    //Fenwick tree running time is O(log(N)), where N is the input size.
    //It is ideal when 1) adding a number to a slot,
    // and then 2) calculating the sum between consecutive slots i to j, where i <= j.
    //It cannot be directly used to compute GCD, LCM, MIN, MAX.
    //A linear space is required.
    //
    //A node stores (the value of itself) + (its left subtree).
    //
    //Note: The indexes start from 1.
    //Get up, while(0 != index), index decreases.
    // Set, while (index < size), - down (add the right-most 1-bit).
    //               8
    //           /       \
            //       4              12
            //     /   \          /   \
            //    2     6       10     14
            //   / \   / \     / \    / \
            //  1  3   5  7   9  11  13  15
            //
            //      7 values:
            //[5]  [1]  [15]  [11]  [52]  [28] [0]
            //      Sums:
            //[5]  [6]  [21]  [32]  [84]  [112] [112]
            //[+5] [+1] [+15] [+11] [+52] [+28] [+0]
            //  1   2     3     4     5     6     7
            //
            //After:
            //       4 [+32]
            //       /      \
            //   2 [+6]     6 [+80]
            //    /   \       /     \
            // 1[+5] 3[+15]  5[+52] 7[+0]
    template <typename Number,
        typename Operation, typename ReverseOperation = Operation>
        class BinaryIndexedTree final
    {
        std::vector<Number> _Data;

        Operation _Operation;
        ReverseOperation _ReverseOperation;

        const Number _Zero;

    public:
        static constexpr unsigned InitialIndex = 1;

        explicit BinaryIndexedTree(
            size_t initialSize,
            Operation operation = {},
            ReverseOperation reverseOperation = operation,
            //E.g. 0 for addition, 1 for multiplication.
            Number zero = {})
            : _Data(0 == initialSize ?
                0 // For empty collections.
                : initialSize + InitialIndex, zero),
            _Operation(operation),
            _ReverseOperation(reverseOperation),
            _Zero(zero)
        {
        }

        //Return the sum from 1 to the "index".
        Number get(size_t index) const
        {
            check_index(index);

            Number result{ _Zero };
            do
            {
                result = _Operation(result, _Data[index]);
                index &= index - 1; //Remove the right-most 1-bit.
            } while (0 != index);

            return result;
        }

        void set(size_t index, const Number& value = Number(1))
        {
            check_index(index);

            const auto size = _Data.size();
            do
            {
                _Data[index] = _Operation(_Data[index], value);
                index += index & (0 - index);//Add the right-most 1-bit.
            } while (index < size);
        }

        //Return the maximum supported index.
        size_t max_index() const
        {
            return _Data.size() - InitialIndex;
        }

        //When "leftInclusive" is ether 0 or 1,
        // the sum is taken from the beginning to the "rightInclusive".
        //Otherwise, the returned sum is taken between indexes inclusively.
        Number get(size_t leftInclusive, size_t rightInclusive) const
        {
#ifdef _DEBUG
            if (rightInclusive < leftInclusive)
            {
                std::ostringstream ss;
                ss << "The rightInclusive (" << rightInclusive
                    << ") cannot be smaller than leftInclusive (" << leftInclusive
                    << "), size=" << (_Data.size()) << ".";
                StreamUtilities::throw_exception<std::out_of_range>(ss);
            }
#endif
            auto val = get(rightInclusive);
            const auto result = leftInclusive <= InitialIndex
                ? val
                : _ReverseOperation(val, get(leftInclusive - InitialIndex));
            return result;
        }

        //Return the scalar value at "index",
        // which is semantically equivalent to get(index, index),
        // but can run faster.
        Number value_at(size_t index) const
        {
            check_index(index);

            Number result = _Data[index];

            const auto stopIndex = index & (index - 1); //Remove the right-most 1-bit.
            --index;

            while (stopIndex != index)
            {
                result = _ReverseOperation(result, _Data[index]);
                index &= index - 1;
            }

            return result;
        }

    private:
        void check_index(const size_t index) const
        {
            if (!index || _Data.size() <= index)
            {
                std::ostringstream ss;
                ss << "The index (" << index
                    << ") must be between " << InitialIndex
                    << " and " << (_Data.size() - InitialIndex) << ".";
                StreamUtilities::throw_exception<std::out_of_range>(ss);
            }
        }
    };
}