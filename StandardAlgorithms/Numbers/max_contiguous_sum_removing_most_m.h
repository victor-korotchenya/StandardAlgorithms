#pragma once
#include"../Utilities/require_utilities.h"
#include"arithmetic.h"

namespace Standard::Algorithms::Numbers
{
    // Maximum contiguous sum in array of n ints, removing at most 'remove_max' items.
    // Can select an empty subset.
    // Time O(n*min(remove_max, negatives count)), space O(n).
    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto max_contiguous_sum_removing_most_m(
        const std::vector<int_t> &arr, std::size_t remove_max) -> long_int_t
    {
        const auto size = require_positive(arr.size(), "size");

        require_positive(remove_max, "remove_max");
        require_less_equal(remove_max, size, "remove_max");

        {
            std::size_t negatives{};
            std::size_t positives{};
            long_int_t positive_sum{};

            for (const auto &item : arr)
            {
                negatives += item < 0 ? 1U : 0U;
                positives += 0 < item ? 1U : 0U;
                positive_sum += 0 < item ? item : int_t{};

                assert(0 <= positive_sum);
            }

            if (negatives <= remove_max)
            {
                return positive_sum;
            }

            if (0U == positives)
            {
                return {};
            }

            // Some >0, some <0.
            assert(0U < negatives && negatives < size && positives < size);
        }

        long_int_t result{};

        std::array<std::vector<long_int_t>, 2> buffers{ std::vector<long_int_t>(size, long_int_t{}),
            std::vector<long_int_t>(size, long_int_t{}) };

        {// No removals.
            auto &prevs = buffers[0];
            long_int_t sum{};

            for (std::size_t index{}; index < size; ++index)
            {
                const auto &item = arr[index];
                sum += item;

                if (0 < sum)
                {
                    prevs[index] = sum;

                    result = std::max(result, sum);
                }
                else
                {
                    sum = {};
                }
            }
        }

        for (std::size_t rem = 1; rem <= remove_max; ++rem)
        {
            const auto parity = rem & 1U;
            const auto &prevs = buffers[parity ^ 1U];
            auto &currs = buffers[parity];

            for (std::size_t index = 1; index < size; ++index)
            {
                const auto &item = arr[index];
                const auto &cur = currs[index - 1U];
                const auto included = static_cast<long_int_t>(cur + item);

                const auto &excluded = prevs[index - 1U];

                auto &value = currs[index];
                value = std::max(included, excluded);

                result = std::max(result, value);
            }
        }

        assert(0 < result);

        return result;
    }

    namespace Inner
    {
        template<std::signed_integral long_int_t, std::signed_integral int_t>
        requires(sizeof(int_t) <= sizeof(long_int_t))
        [[nodiscard]] constexpr auto cont_sum_rec(const std::vector<int_t> &arr, const std::size_t pos,
            std::vector<std::vector<long_int_t>> &buf, const std::size_t remove_max) -> long_int_t
        {
            auto &value = buf.at(pos).at(remove_max);

            if (const auto is_already_computed = 0 <= value; is_already_computed)
            {
                return value;
            }

            assert(0U < pos);

            const auto prev = cont_sum_rec<long_int_t, int_t>(arr, pos - 1U, buf, remove_max);

            const auto included = static_cast<long_int_t>(prev + arr[pos]);
            value = std::max(long_int_t{}, included);

            if (0U == remove_max)
            {
                return value;
            }

            const auto excluded = cont_sum_rec<long_int_t, int_t>(arr, pos - 1U, buf, remove_max - 1U);

            return value = std::max(value, excluded);
        }
    } // namespace Inner

    // Time, space O(n*remove_max).
    // Note. No negative number - take everything.
    // Note. If (neg number count < remove_max), then remove_max could be harmlessly decreased.
    // Note. The first and last items can be in- or excluded sort of free of change. Thus, when 3<=n, remove_max cannot
    // exceed n-2.
    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto max_contiguous_sum_removing_most_m_rec(
        const std::vector<int_t> &arr, const std::size_t remove_max) -> long_int_t
    {
        const auto size = require_positive(arr.size(), "size");
        {
            constexpr auto max_size = std::numeric_limits<std::size_t>::max() - 4U;

            require_less_equal(size, max_size, "size");
        }

        require_positive(remove_max, "remove_max");
        require_less_equal(remove_max, size, "remove_max");

        constexpr long_int_t one = 1;
        constexpr long_int_t minus_one = long_int_t{} - one;

        // Last potentially included index; how many excluded -> sum.
        std::vector<std::vector<long_int_t>> buf(size, std::vector<long_int_t>(remove_max + 1U, minus_one));

        buf[0][0] = std::max(int_t{}, arr[0]);

        for (std::size_t rem = 1; rem <= remove_max; ++rem)
        {
            buf[0][rem] = buf[0][0];
        }

        auto result = Inner::cont_sum_rec<long_int_t, int_t>(arr, size - 1U, buf, remove_max);

        for (std::size_t index{}; index < size; ++index)
        {
            for (std::size_t rem{}; rem <= remove_max; ++rem)
            {
                result = std::max(result, buf[index][rem]);
            }
        }

        return result;
    }

    namespace Inner
    {
        // Must include items.
        template<std::signed_integral long_int_t, std::signed_integral int_t>
        requires(sizeof(int_t) <= sizeof(long_int_t))
        [[nodiscard]] constexpr auto cont_sum_from_index(long_int_t sum, std::size_t index,
            const std::vector<int_t> &arr, const std::size_t remove_max, std::vector<int_t> &ignored,
            const std::size_t stop) -> std::pair<long_int_t, bool> // max value; is done.
        {
            constexpr int_t zero{};

            assert(zero < sum && index < stop && stop <= arr.size() && arr[index] < zero && zero < arr[stop - 1U]);

            ignored.clear();

            auto best = sum;
            auto done = true;

            for (;;)
            {
                // Non-positives.
                assert(index < stop && arr[index] < zero);

                do
                {
                    const auto &item = arr[index];
                    if (zero == item)
                    {
                        continue;
                    }

                    ignored.push_back(item);
                    std::push_heap(ignored.begin(), ignored.end());

                    if (ignored.size() <= remove_max)
                    {
                        continue;
                    }

                    done = false;

                    const auto &value_max = ignored.front();

                    assert(2U <= ignored.size() && value_max < zero);
                    sum += value_max;

                    if (sum <= zero)
                    {
                        return { best, done };
                    }

                    std::pop_heap(ignored.begin(), ignored.end());
                    ignored.pop_back();
                } while (arr.at(++index) <= zero);

                // Non-negatives.
                assert(index < stop && zero < arr[index] && zero <= sum);

                do
                {
                    sum += arr[index];
                } while (++index < stop && zero <= arr[index]);

                best = std::max(best, sum);

                if (index == stop)
                {
                    return { best, done };
                }
            }
        }
    } // namespace Inner

    // Time O(n*max(1, negatives count - remove_max)*log(remove_max)) which is not the fastest, space O(remove_max).
    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto max_contiguous_sum_removing_most_m_sensitive(
        const std::vector<int_t> &arr, const std::size_t remove_max) -> long_int_t
    {
        auto stop = require_positive(arr.size(), "size");
        require_positive(remove_max, "remove_max");
        require_less_equal(remove_max, stop, "remove_max");

        // Skip non-positive on both sides.
        std::size_t start{};

        while (start < stop && arr[start] <= 0)
        {
            ++start;
        }

        if (const auto no_positive = start == stop; no_positive)
        {
            return {};
        }

        // At least 1 positive number.
        assert(start < stop && 0 < arr[start]);

        while (arr[stop - 1U] <= 0)
        {
            assert(0U < stop);

            --stop;
        }

        assert(start < stop && 0 < arr[stop - 1U]);

        // - Take a consecutive chain of numbers, starting from a positive value.
        // - The chain might have more than remove_max negative numbers which might be not consecutive among negatives
        // e.g. {a, -8, b, -1, c, -8, d}.
        // - Can remove up to 'remove_max' negs from the chain.
        // Then, remove the maximum negative value as it is better to ignore smaller negatives.
        // -- If the sum becomes <= 0, stop; another attempt starts from the next positive value.
        // - When there are no more than remove_max negatives (flag done), the main loop can be stopped.

        std::vector<int_t> ignored;
        ignored.reserve(remove_max + 1U);

        long_int_t result{};

        for (;;)
        {
            // Sum non-negatives.
            long_int_t sum_before{};

            do
            {
                sum_before += arr[start];

                assert(0 < sum_before && 0 <= arr[start]);
            } while (++start < stop && 0 <= arr[start]);

            if (start == stop)
            {
                result = std::max(result, sum_before);
                break;
            }

            const auto [cand, done] =
                Inner::cont_sum_from_index<long_int_t, int_t>(sum_before, start, arr, remove_max, ignored, stop);

            assert(0 < sum_before && sum_before <= cand);

            result = std::max(result, cand);

            if (done || ++start == stop)
            {
                break;
            }

            while (arr[start] <= 0)
            {
                ++start;

                assert(start < stop);
            }
        }

        return result;
    }

    // Kadane's max contiguous possibly empty sub-array sum. Time O(n).
    // Can be used to find max sum of a rectangle matrix in 2D array in O(n**3):
    // - Find sums of all sub-matrices, starting at (0,0).
    // - Enumerate all pairs (left, right) of columns, left <= right:
    // -- Form an array of in between sums, then use this alg.
    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto max_contiguous_sum(const std::vector<int_t> &arr) noexcept(is_add_noexcept<long_int_t>)
        -> long_int_t
    {
        constexpr long_int_t zero{};

        auto result = zero;
        auto sum = zero;

        for (const auto &item : arr)
        {
            sum += item;

            if (sum < zero)
            {
                sum = zero;
            }
            else if (result < sum)
            {
                result = sum;
            }
        }

        assert(zero <= result);

        return result;
    }

    namespace Inner
    {
        template<std::signed_integral long_int_t, std::signed_integral int_t>
        requires(sizeof(int_t) <= sizeof(long_int_t))
        constexpr void process_mask_slow(const std::vector<int_t> &arr, const std::vector<bool> &includes,
            std::vector<int_t> &buf, long_int_t &result)
        {
            buf.clear();

            const auto size = arr.size();
            assert(size == includes.size());

            for (std::size_t index{}; index < size; ++index)
            {
                if (includes[index])
                {
                    buf.push_back(arr[index]);
                }
            }

            auto cur = max_contiguous_sum<long_int_t, int_t>(buf);
            if (result < cur)
            {
                result = std::move(cur);
            }
        }
    } // namespace Inner

    // Slow time O(n * (m choose n)),
    // where 0 < remove_max <= n,
    // and m = min(remove_max, neg number count).
    template<std::signed_integral long_int_t, std::signed_integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto max_contiguous_sum_removing_most_m_slow(
        const std::vector<int_t> &arr, const std::size_t remove_max) -> long_int_t
    {
        const auto size = require_positive(arr.size(), "size");
        {
            constexpr auto max_size = 20U;
            require_less_equal(size, max_size, "size");
        }

        require_positive(remove_max, "remove_max");
        require_less_equal(remove_max, size, "remove_max");

        std::size_t negatives{};
        {
            std::size_t zeros{};
            long_int_t positive_sum{};

            for (const auto &item : arr)
            {
                negatives += item < 0 ? 1U : 0U;
                zeros += item == 0 ? 1U : 0U;
                positive_sum += 0 < item ? item : int_t{};

                assert(0 <= positive_sum);
            }

            if (negatives <= remove_max)
            {
                return positive_sum;
            }

            if (const auto no_positive = negatives + zeros == size; no_positive)
            {
                return {};
            }

            // Some >0, some <0.
            assert(0U < negatives && negatives < size);
        }

        negatives = remove_max;

        std::vector<bool> includes(size, true);

        for (std::size_t index{}; index < negatives; ++index)
        {
            includes[index] = false;
        }

        Standard::Algorithms::require_sorted(includes, "includes");

        long_int_t result{};
        std::vector<int_t> buf(size);

        do
        {
            Inner::process_mask_slow<long_int_t, int_t>(arr, includes, buf, result);
        } while (std::next_permutation(includes.begin(), includes.end()));

        assert(0 < result);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
