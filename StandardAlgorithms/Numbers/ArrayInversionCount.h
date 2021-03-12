#pragma once
#include <vector>
#include "Arithmetic.h"

namespace Privet::Algorithms::Numbers
{
    // Given an array of numbers,
    // return the number of inversions,
    // where an inversion is a pair (x, y) of two array items having (y < x).
    //
    // E.g. for array  {1, 234, 56 }, there is one inversion: pair(234, 56).
    struct ArrayInversionCount final
    {
        ArrayInversionCount() = delete;

        // Slow O(n*n) time.
        template <typename Number >
        static size_t Slow(const std::vector< Number >& data)
        {
            const auto size = data.size();
            if (size <= 1)
                return 0;

            size_t result{}, index{};
            do
            {
                auto index2 = index + 1u;
                do
                {
                    if (data[index2] < data[index])
                        ++result;
                } while (++index2 < size);
            } while (++index < size - 1u);

            return result;
        }

        // Using merge sort, the array will be sorted, and thus modified!
        // O(n*log(n)) time and O(n) space.
        template <typename Number >
        static size_t FastUsingMergeSort(std::vector< Number >& data)
        {
            const auto size = data.size();
            if (size <= 1)
                return 0;

            size_t result{};

            //Size|maxSubsetSize = 2|1,
            // 3|2, 4|2,
            // 5|4, 6|4, 7|4, 8|4,
            // 9|8, 10|8, 11|8, 12|8.
            const auto maxSubsetSize = static_cast<size_t>(RoundToGreaterPowerOfTwo_t(size) >> 1);

            size_t subsetSize = 1;
            std::vector<Number> buffer(
                // todo: p2. maxSubsetSize which is less than size.
                size
            );

            do
            {
                ProcessSubsets< Number >(data, buffer, result, subsetSize);

                subsetSize <<= 1;
            } while (subsetSize <= maxSubsetSize);

            return result;
        }

    private:
        template <class Number >
        static void ProcessSubsets(std::vector< Number >& data, std::vector< Number >& buffer,
            size_t& result,
            const size_t subsetSize)
        {
            const auto size = data.size(),
                twoSubsets = subsetSize << 1;

            assert(subsetSize && subsetSize < size);

            size_t start{};
            do
            {
                const auto stop = std::min(start + twoSubsets, size),
                    mid = start + subsetSize - 1u;

                assert(start <= mid && mid + 1u < stop);

                if (data[mid + 1u] < data[mid])
                    Merge<Number>(data, buffer, result, start, mid, stop);
#if _DEBUG
                else// Skip the already sorted array merge.
                    assert(std::is_sorted(data.begin() + start, data.begin() + stop));
#endif

                start += twoSubsets;
            } while (start < size - subsetSize);
        }

        // Subset1 [start, mid].
        // Subset2 [mid + 1, stop).
        template <class Number >
        static void Merge(std::vector< Number >& data, std::vector< Number >& buffer,
            size_t& result,
            const size_t start, const size_t mid, const size_t stop)
        {
            size_t end1 = mid,
                i = stop - 1u,
                end2 = i,
                j;

            for (;;)
            {
                assert(end2 > mid && i < stop);
                if (data[end2] < data[end1])
                {
                    result += end2 - mid;
                    buffer[i] = data[end1];

                    if (end1 == start)
                    {//Copy remainder2.
                        for (size_t k{}; k < end2 - mid; ++k)
                            data[start + k] = data[k + mid + 1];

                        j = start + end2 - mid;
                        break;
                    }

                    --end1;
                }
                else
                {
                    buffer[i] = data[end2];

                    if (--end2 == mid)
                    {//Skip copying the remainder1.
                        j = end1 + 1;
                        break;
                    }
                }

                --i;
            }

            std::copy(buffer.begin() + i,
                buffer.begin() + stop,
                data.begin() + j);
        }
    };
}