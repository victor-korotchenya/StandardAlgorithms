#pragma once
#include"../Utilities/require_utilities.h"
#include"bit_utilities.h"
#include<numeric>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::signed_integral int_t>
    constexpr void cas_insert_sum_slow(const std::unordered_set<int_t> &pre, const bool negative_start,
        const std::unordered_set<int_t> &mid, std::unordered_set<int_t> &proc)
    {
        const int_t sig = negative_start ? -1 : 1;

        // 2 parts: [from, start) [start, tod);
        // 2 parts: [0, 2) [2, 4); negatives: 0011
        // 0+1- (2-3)
        for (const auto &val0 : pre)
        {
            for (const auto &val1 : mid)
            {
                const auto sum = static_cast<int_t>(val1 * sig + val0);
                proc.insert(sum);
            }
        }
    }

    template<std::signed_integral int_t>
    constexpr void cas_slow_rec(const std::vector<bool> &negatives, const std::int32_t from,
        std::vector<std::vector<std::unordered_set<int_t>>> &processed, const std::int32_t tod)
    {
        assert(0 <= from && from + 1 < tod && tod <= static_cast<std::int32_t>(processed.size()));

        auto &proc = processed[from][tod - 1];
        assert(proc.empty());

        // No sense to calc (5-1+..) ..
        // as it is the same as 5-1+.. ..
        for (auto start = from + 1; start < tod - 1; ++start)
        {
            for (auto stop = start + 1; stop <= tod; ++stop)
            {
                const auto &pre = processed[from][start - 1];

                if (pre.empty())
                {
                    cas_slow_rec<int_t>(negatives, from, processed, start);
                }

                const auto &mid = processed[start][stop - 1];
                if (mid.empty())
                {
                    cas_slow_rec<int_t>(negatives, start, processed, stop);
                }

                // [from, start) [start, stop) [stop tod); last can miss
                if (stop != tod)
                {
                    const auto &part3 = processed[stop][tod - 1];
                    if (part3.empty())
                    {
                        cas_slow_rec<int_t>(negatives, stop, processed, tod);
                    }

                    continue;
                }

                cas_insert_sum_slow<int_t>(pre, negatives[start], mid, proc);
            }
        }

        assert(!proc.empty());
    }

    template<class int_t>
    struct cas_ppd final
    {
        const std::vector<int_t> &arr;
        const std::vector<bool> &negatives;

        const std::int32_t size{};
        const int_t total_sum{};

        std::vector<bool> has{};

        // index, # of '(', wheter sum at [sum] is reachable.
        // [index, open] do NOT define the position - must use the sum.
        std::vector<std::vector<std::vector<bool>>> processed{};
    };

    template<std::signed_integral int_t>
    constexpr void count_asp_rec(cas_ppd<int_t> &context, const int_t &sum, const std::int32_t index = {},
        // At the end, there can be as many ')' as needed open_pares.
        const std::int32_t open_pares = {})
    {
        assert(0 <= index && index <= context.size && 0 <= open_pares && open_pares <= index && !(sum < 0) &&
            !context.processed[index][open_pares][sum]);

        context.processed[index][open_pares][sum] = true;

        if (index == context.size)
        {
            context.has.at(sum) = true;
            return;
        }

        const auto sum2 = static_cast<int_t>(
            context.arr[index] * (context.negatives[index] ? -1 : 1) * (has_zero_bit(open_pares) ? -1 : 1) + sum);

        assert(0 <= sum2 && sum2 <= (context.total_sum << 1U));

        if (!context.processed[index + 1][open_pares][sum2])
        {// Do not put '('.
            count_asp_rec<int_t>(context, sum2, index + 1, open_pares);
        }

        if (context.negatives[index] && !context.processed[index + 1][open_pares + 1][sum2])
        {// Place '(' only before '-'
            count_asp_rec<int_t>(context, sum2, index + 1, open_pares + 1);
        }

        if (0 < open_pares && !context.processed[index + 1][open_pares - 1][sum2])
        {// Put ')'
            count_asp_rec<int_t>(context, sum2, index + 1, open_pares - 1);
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Count how many distinct numbers can be obtained after
    // adding any number of parentheses to an arithmetic expression of n <= 30 ints
    // a0 + a1 - a2 - a3 etc.
    // provided that the expression is valid, and 0 <= ai <= 100.
    // e.g. 1 - 10 - 50 = -59,
    // 1 - (10 - 50) = -39; thus there are 2 distinct numbers.
    // Time, space O(n*n).
    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto count_add_sub_put_parentheses_distinct(
        const std::vector<int_t> &arr, const std::vector<bool> &negatives) -> std::int32_t
    {
        auto size = require_positive(static_cast<std::int32_t>(arr.size()), "size");

        {
            constexpr auto greater1 = 31;

            require_greater(greater1, size, "size");
        }

        if (negatives.size() != arr.size()) [[unlikely]]
        {
            auto err = "negatives size " + std::to_string(negatives.size()) + " must be " + std::to_string(arr.size());

            throw std::invalid_argument(err);
        }

        // Skip 0s at the end.
        while (0 < size && arr[size - 1] == 0)
        {
            --size;
        }

        if (size <= 2)
        {
            return 1;
        }

        {
            const auto last = negatives.cbegin() + size;
            const auto iter = std::find(negatives.cbegin(), last, true);

            if (iter == last)
            {
                return 1;
            }

            assert(!negatives[0]);
        }

        const auto total_sum =
            require_positive(std::accumulate(arr.cbegin(), arr.cbegin() + size, int_t{}), "total sum");

        const auto has_size = (total_sum << 1U) | 1LL;

        Inner::cas_ppd<int_t> context{ arr, negatives, size, total_sum, std::vector<bool>(has_size),
            std::vector<std::vector<std::vector<bool>>>(
                size + 1LL, std::vector<std::vector<bool>>(size + 1LL, std::vector<bool>(has_size, false))) };

        Inner::count_asp_rec<int_t>(context, context.total_sum); // Avoid negative values.

        auto result = std::accumulate(context.has.cbegin(), context.has.cend(), 0);

        assert(0 < result);

        return result;
    }

    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto count_add_sub_put_parentheses_distinct_slow(
        const std::vector<int_t> &arr, const std::vector<bool> &negatives) -> std::int32_t
    {
        const auto size = static_cast<std::int32_t>(arr.size());
        if (size <= 2)
        {
            return 1;
        }

        if (negatives.size() != arr.size()) [[unlikely]]
        {
            throw std::invalid_argument(
                "negatives.size " + std::to_string(negatives.size()) + " must be " + std::to_string(arr.size()));
        }

        require_all_non_negative(arr, "arr");
        assert(size <= 30 && !negatives[0]);
        // todo(p3): finish.

        // [from, tod-1] -> values
        // {-1-2-4} -> values
        std::vector<std::vector<std::unordered_set<int_t>>> processed(
            size, std::vector<std::unordered_set<int_t>>(size));

        for (std::int32_t index{}; index < size; ++index)
        {
            // todo(p3): unused var?!   const auto sig = static_cast<int_t>(negatives[index] ? -1 : 1);
            processed[index][index].insert(arr[index]);

            if (index < size - 1)
            {
                auto sum = static_cast<int_t>(arr[index + 1] * (negatives[index] ? -1 : 1) + arr[index]);

                processed[index][index + 1].insert(sum);
            }
        }

        Inner::cas_slow_rec<int_t>(negatives, 0, processed, size);

        auto result = static_cast<std::int32_t>(processed[0].back().size());
        assert(0 < result);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
