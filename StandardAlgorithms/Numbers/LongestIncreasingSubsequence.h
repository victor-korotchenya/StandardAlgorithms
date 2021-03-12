#pragma once

#include <vector>
#include <tuple>
#include <utility>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // It can be used to find the longest bitonic subsequence
            // which first increases then decreases e.g. 10,20,30 is max top,29,5,2.
            // The idea is to find lefts = LIS for every i=0..n-1,
            // and that in the reversed array;
            // return max(lefts[i] + revs[i] - 1).
            template <typename TItem,
                typename TContainer = std::vector<TItem>,
                typename TComparer = std::less<TItem>>
                class LongestIncreasingSubsequence final
            {
                LongestIncreasingSubsequence() = delete;

            public:

                using Indexes = std::vector<size_t>;

                //O(n*n) time.
                static Indexes Slow(const TContainer& source, TComparer comparer = {});

                //O(n*log(n)) time.
                static Indexes Fast(const TContainer& source, TComparer comparer = {});

            private:

                using MatrixPair_Slow = std::pair<Indexes, size_t>;
                static MatrixPair_Slow CalculateMatix_Slow(
                    const TContainer& source, TComparer comparer);

                static Indexes ReconstructResult_Slow(
                    const size_t size, const MatrixPair_Slow& matrixTuple);

                using MatrixTuple_Fast = std::tuple<Indexes, Indexes, size_t>;
                static MatrixTuple_Fast CalculateMatix_Fast(
                    const TContainer& source, TComparer comparer);

                static size_t FindLargestPrevious(const TContainer& source,
                    const Indexes& increasingSubsequence,
                    const size_t resultLength, const size_t index, TComparer comparer);

                static Indexes ReconstructResult_Fast(
                    const Indexes& previous,
                    const size_t initialIndex,
                    const size_t resultLength);
            };

            template <typename TItem, typename TContainer, typename TComparer>
            typename LongestIncreasingSubsequence<TItem, TContainer, TComparer>::Indexes
                LongestIncreasingSubsequence<TItem, TContainer, TComparer>::Slow(
                    const TContainer& source, TComparer comparer)
            {
                if (!source.empty())
                {
                    const auto matrixTuple = CalculateMatix_Slow(source, comparer);
                    const auto result = ReconstructResult_Slow(source.size(), matrixTuple);
                    return result;
                }

                return{};
            }

            template <typename TItem, typename TContainer, typename TComparer>
            typename LongestIncreasingSubsequence<TItem, TContainer, TComparer>::MatrixPair_Slow
                LongestIncreasingSubsequence<TItem, TContainer, TComparer>::CalculateMatix_Slow(
                    const TContainer& source, TComparer comparer)
            {
                const auto size = source.size();

                Indexes lengths(size);
                size_t resultLength{};

                for (size_t i = 0; i < size; ++i)
                {
                    lengths[i] = 1; //The item by itself.
                    for (size_t j = 0; j < i; ++j)
                    {
                        if (comparer(source[j], source[i]))
                        {
                            const auto newLength = 1 + lengths[j];
                            if (lengths[i] < newLength)
                            {
                                lengths[i] = newLength;
                            }
                        }
                    }

                    if (resultLength < lengths[i])
                    {
                        resultLength = lengths[i];
                    }
                }

                return MatrixPair_Slow(lengths, resultLength);
            }

            template <typename TItem, typename TContainer, typename TComparer>
            typename LongestIncreasingSubsequence<TItem, TContainer, TComparer>::Indexes
                LongestIncreasingSubsequence<TItem, TContainer, TComparer>::ReconstructResult_Slow(
                    const size_t size,
                    const MatrixPair_Slow& matrixTuple)
            {
                const auto& lengths = matrixTuple.first;
                auto resultLength = matrixTuple.second;

                Indexes result(resultLength);

                //Start from the end.
                size_t index = size - 1;
                do
                {
                    while (resultLength != lengths[index])
                    {//Find the first match.
                        --index;
                    }

                    --resultLength;
                    result[resultLength] = index;
                } while (resultLength);

                return result;
            }

            template <typename TItem, typename TContainer, typename TComparer>
            typename LongestIncreasingSubsequence<TItem, TContainer, TComparer>::Indexes
                LongestIncreasingSubsequence<TItem, TContainer, TComparer>::Fast(
                    const TContainer& source, TComparer comparer)
            {
                if (!source.empty())
                {
                    const auto matrixTuple = CalculateMatix_Fast(source, comparer);

                    const auto& previous = std::get<0>(matrixTuple);
                    const auto& increasingSubsequence = std::get<1>(matrixTuple);
                    const auto resultLength = std::get<2>(matrixTuple);

                    const auto initialIndex = increasingSubsequence[resultLength];
                    const auto result = ReconstructResult_Fast(previous, initialIndex, resultLength);
                    return result;
                }

                return{};
            }

            template <typename TItem, typename TContainer, typename TComparer>
            typename LongestIncreasingSubsequence<TItem, TContainer, TComparer>::MatrixTuple_Fast
                LongestIncreasingSubsequence<TItem, TContainer, TComparer>::CalculateMatix_Fast(
                    const TContainer& source, TComparer comparer)
            {
                const auto size = source.size();

                Indexes previous(size);

                //The "increasingSubsequence" actually starts at index 1.
                Indexes increasingSubsequence(size + 1);
                size_t resultLength = 0;

                for (size_t i = 0; i < size; ++i)
                {
                    //Binary search for the largest positive "searchIndex",
                    // such that 1 <= searchIndex <= resultLength
                    // and source[increasingSubsequence[searchIndex - 1]] < source[i]
                    const auto searchIndex = FindLargestPrevious(
                        source, increasingSubsequence, resultLength, i, comparer);

                    //After searching, searchIndex is 1 greater
                    // than the length of the longest prefix of source[i].
                    //The predecessor of source[i] is
                    //  the last index of the subsequence of length searchIndex - 1.
                    previous[i] = increasingSubsequence[searchIndex - 1];
                    increasingSubsequence[searchIndex] = i;

                    if (resultLength < searchIndex)
                    {// A longer subsequence is found.
                        resultLength = searchIndex;
                    }
                }

                return MatrixTuple_Fast(previous, increasingSubsequence, resultLength);
            }

            template <typename TItem, typename TContainer, typename TComparer>
            size_t LongestIncreasingSubsequence<TItem, TContainer, TComparer>::FindLargestPrevious(
                const TContainer& source, const Indexes& increasingSubsequence,
                const size_t resultLength, const size_t index, TComparer comparer)
            {
                size_t searchIndex = 1;
                auto high = resultLength;

                while (searchIndex <= high)
                {
                    const auto middle = searchIndex + ((high - searchIndex) >> 1);
                    const auto previousIndex = increasingSubsequence[middle];

                    if (comparer(source[previousIndex], source[index]))
                    {//Found a sub-sequence ending with smaller value.
                        searchIndex = middle + 1;
                    }
                    else
                    {
                        high = middle - 1;
                    }
                }

                return searchIndex;
            }

            template <typename TItem, typename TContainer, typename TComparer>
            typename LongestIncreasingSubsequence<TItem, TContainer, TComparer>::Indexes
                LongestIncreasingSubsequence<TItem, TContainer, TComparer>::ReconstructResult_Fast(
                    const Indexes& previous, const size_t initialIndex, const size_t resultLength)
            {
                Indexes result;

                const size_t someNonZeroSymbol = 0 - size_t(1);
                result.resize(resultLength, someNonZeroSymbol);

                for (size_t j = resultLength - 1, index = initialIndex;
                    //(-1) is the max value for an unsigned type.
                    j < resultLength;
                    --j, index = previous[index])
                {
                    result[j] = index;
                }

                return result;
            }
        }
    }
}