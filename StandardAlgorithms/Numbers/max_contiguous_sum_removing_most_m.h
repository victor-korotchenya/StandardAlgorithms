#pragma once
#include <algorithm> // push_heap
#include <cassert>
#include <type_traits>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/VectorUtilities.h"

namespace
{
    template <class long_int_t, class int_t>
    void process_mask_slow(const std::vector<int_t>& ar,
        const std::vector<bool>& includes,
        std::vector<int_t>& buf,
        long_int_t& result)
    {
        buf.clear();

        const auto size = static_cast<int_t>(ar.size());

        for (int_t i = 0; i < size; ++i)
            if (includes[i])
                buf.push_back(ar[i]);

        const auto cur = max_contiguous_sum<long_int_t, int_t>(buf);
        if (result < cur)
            result = cur;
    }

    // Must include items.
    template <class long_int_t, class int_t>
    long_int_t run_from_index(const std::vector<int_t>& ar,
        std::vector<int_t>& ignored,
        const long_int_t sum_before,
        const int_t remove_max,
        int_t i,
        const int_t stop,
        bool& enough_negatives)
    {
        assert(sum_before > 0 && i < stop&& ar[i] < 0 && ar[stop - 1] > 0 && stop > 0);
        ignored.clear();

        long_int_t best = sum_before, sum = sum_before;
        enough_negatives = false;
        for (;;)
        {
            // Non-positives
            assert(i < stop&& ar[i] < 0);
            do
            {
                if (!ar[i])
                    continue;

                ignored.push_back(ar[i]);
                std::push_heap(ignored.begin(), ignored.end());

                if (static_cast<int_t>(ignored.size()) <= remove_max)
                    continue;

                assert(ignored.size() >= 2);
                enough_negatives = true;

                const auto& value_max = ignored.front();
                assert(value_max < 0);

                sum += value_max;
                if (sum <= 0)
                {
                    assert(best > 0);
                    return best;
                }

                std::pop_heap(ignored.begin(), ignored.end());
                ignored.pop_back();
            } while (ar[++i] <= 0);

            if (i == stop)
            {
                assert(best > 0);
                return best;
            }

            // Non-negatives
            assert(i < stop&& ar[i] > 0 && sum >= 0);
            do
            {
                sum += ar[i];
            } while (++i < stop && ar[i] >= 0);

            if (best < sum)
                best = sum;

            if (i == stop)
            {
                assert(best > 0);
                return best;
            }
        }
    }

    template<class long_int_t, class int_t>
    long_int_t max_contiguous_sum_removing_most_m_they_rec(
        const std::vector<int_t>& ar,
        std::vector<std::vector<long_int_t>>& buf,
        const int_t remove_max,
        const int_t pos)
    {
        assert(pos >= 0 && remove_max >= 0);
        auto& value = buf[pos][remove_max];
        if (value >= 0)
            return value;

        const auto next = max_contiguous_sum_removing_most_m_they_rec<long_int_t, int_t>(
            ar, buf, remove_max, pos - 1);
        const auto included = std::max<long_int_t>(0, ar[pos] + next);

        if (!remove_max) // Nothing to remove
            return value = included;

        const auto excluded = max_contiguous_sum_removing_most_m_they_rec<long_int_t, int_t>(
            ar, buf, remove_max - 1, pos - 1);
        return value = std::max(included, excluded);
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Kadane's max contiguous possibly empty subarray sum.
            // Can be used to find max sum of a rectangle matrix in 2D array in O(n**3):
            // - Find sums of all sub-matrices, starting at (0,0).
            // - Enumerate all pairs (left, right) of columns, left <= right:
            // -- Form an array of in between sums, then use Kadane alg.
            template<class long_int_t = int64_t, class int_t = int>
            long_int_t max_contiguous_sum(const std::vector<int_t>& ar)
            {
                static_assert(std::is_signed_v<int_t> && std::is_signed_v<long_int_t> && sizeof(int_t) <= sizeof(long_int_t));

                const auto size = static_cast<int_t>(ar.size());
                long_int_t result = 0, sum = 0;

                for (int_t i = 0; i < size; ++i)
                {
                    sum += ar[i];
                    if (sum < 0)
                        sum = 0;
                    else if (result < sum)
                        result = sum;
                }

                assert(result >= 0);
                return result;
            }

            // Maximum contiguous sum in array of n ints, removing at most 'remove_max' items.
            // Can select empty subset.
            // Time O(n*n*log(remove_max)), space O(remove_max).
            template<class long_int_t = int64_t, class int_t = int>
            long_int_t max_contiguous_sum_removing_most_m(const std::vector<int_t>& ar, const int_t remove_max)
            {
                static_assert(std::is_signed_v<int_t> && std::is_signed_v<long_int_t> && sizeof(int_t) <= sizeof(long_int_t));

                RequirePositive(ar.size(), "size");
                RequirePositive(remove_max, "remove_max");

                // Skip non-positive at both ends.
                int_t start = 0, stop = static_cast<int_t>(ar.size());
                while (start < stop && ar[start] <= 0)
                    ++start;

                if (start == stop) // All <= 0
                    return 0;

                // At least 1 positive number.
                assert(start >= 0 && start < stop&& ar[start] > 0);
                while (ar[stop - 1] <= 0)
                {
                    assert(stop > 0);
                    --stop;
                }

                assert(start < stop&& stop > 0 && ar[start] > 0 && ar[stop - 1] > 0);

                std::vector<int_t> ignored;
                ignored.reserve(stop - start);

                long_int_t result = 0;
                auto enough_negatives = false;
                for (auto i = start; i < stop; ++i)
                {
                    // - Take a consecutive chain of numbers, starting from a positive value.
                    // - Remove 'remove_max' <0 from the chain - deleting fewer is a smaller result.
                    // -- Once not 'enough_negatives', stop the main loop.
                    // - The chain can have more than remove_max negative numbers which might be not consecutive among negatives e.g. {a, -8, b, -1, c, -8, d}.
                    // -- Remove max negative value as it is better to ignore smaller negatives.
                    // -- If the sum becomes <= 0, stop - another attempt starts from further point.
                    while (ar[i] <= 0)
                    {
                        ++i;
                        assert(i < stop);
                    }

                    long_int_t sum_before = 0;
                    do
                    {
                        sum_before += ar[i];
                        assert(sum_before > 0 && ar[i] >= 0);
                    } while (++i < stop && ar[i] >= 0);

                    if (i < stop)
                    {
                        const auto cand = run_from_index<long_int_t, int_t>(ar, ignored,
                            sum_before, remove_max, i, stop, enough_negatives);
                        assert(cand >= sum_before);
                        if (result < cand)
                            result = cand;

                        if (!enough_negatives)
                            break; // Take all - not enough negatives.
                    }
                    else if (result < sum_before)
                        result = sum_before;

                    assert(result > 0);
                }

                assert(result > 0);
                return result;
            }

            // They.
            // Time O(n*remove_max), space O(n*remove_max).
            template<class long_int_t = int64_t, class int_t = int>
            long_int_t max_contiguous_sum_removing_most_m_they(const std::vector<int_t>& ar, const int_t remove_max)
            {
                static_assert(std::is_signed_v<int_t> && std::is_signed_v<long_int_t> && sizeof(int_t) <= sizeof(long_int_t));

                const auto size = static_cast<int_t>(ar.size());
                RequirePositive(size, "size");
                RequirePositive(remove_max, "remove_max");

                // Last potentially included index; how many excluded.
                std::vector<std::vector<long_int_t>> buf(size,
                    std::vector<long_int_t>(remove_max + 1, -1));

                buf[0][0] = std::max<int_t>(0, ar[0]);
                for (int_t rem = 1; rem <= remove_max; ++rem)
                    buf[0][rem] = buf[0][0];

                max_contiguous_sum_removing_most_m_they_rec<long_int_t, int_t>(ar, buf, remove_max, size - 1);

                long_int_t result = 0;
                for (int_t i = 0; i < size; ++i)
                    for (int_t j = 0; j <= remove_max; ++j)
                        result = std::max(result, buf[i][j]);

                return result;
            }

            // Slow.
            template<class long_int_t = int64_t, class int_t = int>
            long_int_t max_contiguous_sum_removing_most_m_slow(const std::vector<int_t>& ar, const int_t remove_max)
            {
                static_assert(std::is_signed_v<int_t> && std::is_signed_v<long_int_t> && sizeof(int_t) <= sizeof(long_int_t));

                const auto size = static_cast<int_t>(ar.size());
                assert(size <= 20);
                RequirePositive(size, "size");
                RequirePositive(remove_max, "remove_max");

                int_t negatives = 0;
                {
                    long_int_t sum = 0;
                    int_t z = 0;
                    for (const auto& a : ar)
                        negatives += a < 0, z += !a, sum += a;

                    if (!negatives) // All >= 0
                        return sum;

                    if (negatives + z == size) // All <= 0
                        return 0;

                    // Some >0, some <0.
                    assert(negatives > 0 && negatives < size);
                }

                if (remove_max < negatives)
                    negatives = remove_max;
                assert(negatives > 0);

                std::vector<bool> includes(size, true);
                for (int_t i = 0; i < negatives; ++i) includes[i] = false;

                VectorUtilities::RequireArrayIsSorted(includes, "masks");

                long_int_t result = 0;
                std::vector<int_t> buf(size);
                do
                {
                    process_mask_slow<long_int_t, int_t>(ar, includes, buf, result);
                }                 while (std::next_permutation(includes.begin(), includes.end()));

                assert(result >= 0);
                return result;
            }
        }
    }
}