#pragma once
#include <vector>

#ifdef _DEBUG
#include "../Utilities/VectorUtilities.h"
#endif

namespace Privet::Algorithms::Numbers
{
    //Given a sorted array and a "sum",
    // finds two distinct indexes of elements,
    // which sum to "sum", some indexes.
    //If it is impossible, a pair of (-1) is returned.
    //
    //Note that if the array were not sorted,
    // a hash table could be used to achieve average O(n) time.
    // I.e. given number k, is there (sum-k) number in the table?
    class SortedArraySumOfTwoElements final
    {
        SortedArraySumOfTwoElements() = delete;

    public:
        using Pair = std::pair<size_t, size_t>;

        //Use "Fast" which is faster.
        //O(n*n) time.
        template <typename Number>
        static Pair Slow(
            const std::vector<Number>& data, const Number& sum);

        //O(n) time.
        template <typename Number>
        static Pair Fast(
            const std::vector<Number>& data, const Number& sum, const size_t offset = 0);

        static Pair NotFoundPair();
    };

    inline SortedArraySumOfTwoElements::Pair SortedArraySumOfTwoElements::NotFoundPair()
    {
        const size_t minusOne = size_t(0) - size_t(1);

        const Pair result = Pair(minusOne, minusOne);
        return result;
    }

    template <typename Number>
    SortedArraySumOfTwoElements::Pair SortedArraySumOfTwoElements::Slow(
        const std::vector<Number>& data,
        const Number& sum)
    {
#ifdef _DEBUG
        Privet::Algorithms::VectorUtilities::RequireArrayIsSorted(data, "data");
#endif
        const size_t size = data.size();
        if (1 < size)
        {
            for (size_t index1 = 0; index1 < size - 1; index1++)
            {
                for (size_t index2 = index1 + 1; index2 < size; index2++)
                {
                    const Number actual = data[index1] + data[index2];
                    if (sum == actual)
                    {
                        return Pair(index1, index2);
                    }
                }
            }
        }

        return NotFoundPair();
    }

    template <typename Number>
    SortedArraySumOfTwoElements::Pair SortedArraySumOfTwoElements::Fast(
        const std::vector<Number>& data,
        const Number& sum,
        const size_t offset)
    {
#ifdef _DEBUG
        Privet::Algorithms::VectorUtilities::RequireArrayIsSorted(data, "data");
#endif
        const size_t size = data.size();
        if (1 < size)
        {
            size_t low = offset;
            size_t high = size - 1;

            while (low < high)
            {
                const Number actual = data[low] + data[high];
                if (sum == actual)
                {
                    return Pair(low, high);
                }

                if (actual < sum)
                {
                    ++low;
                }
                else
                {//The "high" can never be 0.
                    --high;
                }
            }
        }

        return NotFoundPair();
    }
}