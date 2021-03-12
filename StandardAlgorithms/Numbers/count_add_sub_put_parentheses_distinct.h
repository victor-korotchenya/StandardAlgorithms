#pragma once
#include<unordered_set>
#include<type_traits>
#include<vector>
#include"../Utilities/ExceptionUtilities.h"

namespace
{
    template<class int_t>
    void count_add_sub_put_parentheses_distinct_slow_rec(
        const std::vector<bool>& negatives,
        std::vector<std::vector<std::unordered_set<int_t>>>& processed,
        const int from, const int to)
    {
        static_assert(std::is_signed_v<int_t>);
        assert(0 <= from && from + 1 < to && to <= static_cast<int>(processed.size()));

        auto& s = processed[from][to - 1];
        assert(s.empty());

        // No sense to calc (5-1+..) ...
        // as it is the same as 5-1+.. ...
        for (auto start = from + 1; start < to - 1; ++start)
        {
            for (auto stop = start + 1; stop <= to; ++stop)
            {
                const auto& s0 = processed[from][start - 1];
                if (s0.empty())
                    count_add_sub_put_parentheses_distinct_slow_rec<int_t>(negatives, processed, from, start);

                const auto& s1 = processed[start][stop - 1];
                if (s1.empty())
                    count_add_sub_put_parentheses_distinct_slow_rec<int_t>(negatives, processed, start, stop);

                const int_t si = negatives[start] ? -1 : 1;

                // [from, start) [start, stop) [stop to); last can miss
                if (stop != to)
                {// 3 parts
                    const auto& s2 = processed[stop][to - 1];
                    if (s2.empty())
                        count_add_sub_put_parentheses_distinct_slow_rec<int_t>(negatives, processed, stop, to);

                    continue;
                }

                // 2 parts: [from, start) [start, to);
                // 2 parts: [0, 2) [2, 4); negatives:0011
                // 0+1- (2-3)
                for (const auto& val0 : s0)
                {
                    for (const auto& val1 : s1)
                    {
                        const auto sum = static_cast<int_t>(val1 * si + val0);
                        s.insert(sum);
                    }
                }
            }
        }

        assert(s.size());
    }

    template<class int_t>
    struct cas_ppd final
    {
        const std::vector<int_t>& ar;
        const std::vector<bool>& negatives;

        const int size;
        const int_t total_sum;

        std::vector<bool> has;

        // index, # of '(', wheter sum at [sum] is reachable.
        // Note that [index, open] do NOT define the state - must use the sum.
        std::vector<std::vector<std::vector<bool>>> processed;

        cas_ppd(const std::vector<int_t>& ar, const std::vector<bool>& negatives, const int size)
            : ar(ar), negatives(negatives),
            size(RequirePositive(size, "size")),
            total_sum(RequirePositive(std::accumulate(ar.begin(), ar.begin() + size, int_t()), "total_sum")),
            has(total_sum << 1 | 1),
            processed(size + 1, std::vector<std::vector<bool>>(size + 1, has))
        {
            assert(size <= ar.size() && ar.size() == negatives.size() && !negatives[0]);
        }
    };

    template<class int_t>
    void count_asp_rec(
        cas_ppd<int_t>& context,
        const int_t sum, const int i = {},
        // At the end, there can be as many ')' as needed open_pares.
        const int open_pares = {})
    {
        static_assert(std::is_signed_v<int_t>);
        assert(0 <= i && i <= context.size && 0 <= open_pares && open_pares <= i && sum >= 0 && !context.processed[i][open_pares][sum]);

        context.processed[i][open_pares][sum] = true;
        if (i == context.size)
        {
            context.has[sum] = true;
            return;
        }

        const auto sum2 = static_cast<int_t>(context.ar[i] *
            (context.negatives[i] ? -1 : 1) * (open_pares & 1 ? -1 : 1) + sum);
        assert(0 <= sum2 && sum2 <= (context.total_sum << 1));

        if (!context.processed[i + 1][open_pares][sum2]) // Do not put '(' .
            count_asp_rec<int_t>(context, sum2, i + 1, open_pares);

        if (context.negatives[i] && // Place '(' only before '-'
            !context.processed[i + 1][open_pares + 1][sum2])
            count_asp_rec<int_t>(context, sum2, i + 1, open_pares + 1);

        if (open_pares && !context.processed[i + 1][open_pares - 1][sum2]) // Put ')'
            count_asp_rec<int_t>(context, sum2, i + 1, open_pares - 1);
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Count how many distinct numbers can be obtained after
            // adding any number of parentheses to an arithmetic expression of n <= 30 ints
            // a0 + a1 - a2 - a3 etc.
            // provided that the expression is valid, and 0 <= ai <= 100.
            // e.g. 1 - 10 - 50 = -59,
            // 1 - (10 - 50) = -39; thus there are 2 distinct numbers.
            // Time, space O(n*n).
            template<class int_t>
            int count_add_sub_put_parentheses_distinct(const std::vector<int_t>& ar, const std::vector<bool>& negatives)
            {
                static_assert(std::is_signed_v<int_t>);

                auto size = static_cast<int>(ar.size());
                assert(size <= 30);
                RequirePositive(size, "size");

                if (negatives.size() != ar.size())
                    throw std::invalid_argument("negatives.size " +
                        std::to_string(negatives.size()) + " must be " +
                        std::to_string(ar.size()));

                // Skip 0s at the end.
                while (size && !ar[size - 1])
                    --size;

                if (size <= 2)
                    return 1;
                {
                    const auto last = negatives.begin() + size;
                    const auto it = std::find(negatives.begin(), last, true);
                    if (it == last) // No negatives
                        return 1;
                }

                cas_ppd<int_t> context(ar, negatives, size);
                count_asp_rec<int_t>(context, context.total_sum); // Avoid negative values.

                const auto result = std::accumulate(context.has.begin(), context.has.end(), 0);
                assert(result > 0);
                return result;
            }

            template<class int_t>
            int count_add_sub_put_parentheses_distinct_slow(const std::vector<int_t>& ar, const std::vector<bool>& negatives)
            {
                static_assert(std::is_signed_v<int_t>);

                const auto size = static_cast<int>(ar.size());
                if (size <= 2)
                    return 1;

                if (negatives.size() != ar.size())
                    throw std::invalid_argument("negatives.size " +
                        std::to_string(negatives.size()) + " must be " +
                        std::to_string(ar.size()));
                RequireAllNonNegative(ar, "ar");
                assert(size <= 30 && !negatives[0]);
                //todo: 91. end.

                // [from,to-1] -> values
                // {-1-2-4} -> values
                std::vector<std::vector<std::unordered_set<int_t>>> processed(size, std::vector<std::unordered_set<int_t>>(size));

                for (auto i = 0; i < size; ++i)
                {
                    const auto si = static_cast<int_t>(negatives[i] ? -1 : 1);
                    processed[i][i].insert(ar[i]);
                    if (i < size - 1)
                    {
                        const auto sum = static_cast<int_t>(
                            ar[i + 1] * (negatives[i] ? -1 : 1) + ar[i]);
                        processed[i][i + 1].insert(sum);
                    }
                }

                count_add_sub_put_parentheses_distinct_slow_rec<int_t>(negatives, processed, 0, size);

                const auto result = static_cast<int>(processed[0].back().size());
                assert(result > 0);
                return result;
            }
        }
    }
}