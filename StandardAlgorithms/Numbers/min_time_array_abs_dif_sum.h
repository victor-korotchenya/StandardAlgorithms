#pragma once
#include <algorithm>
#include <cassert>
#include <map>
#include <type_traits>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template <class long_int_t, class int_t>
    void proc_debt(std::map<long_int_t, long_int_t>& holes,
        long_int_t& result,
        const long_int_t inf, const int_t val, int_t count)
    {
        //todo: p2. use a heap?

        assert(count > 0);
        do
        {
            assert(holes.size());
            if (1 == holes.size())
            {
                //auto &h = holes[0];
                const auto h = *holes.begin();
                // 1,1,1,1  [2, +8)   val=1, count=4
                // -1*4 + 2+3+4+5
                // -1*4 + 2*4 + 0+1+2+3
                const auto ad = long_int_t(count) * (h.first - val) +
                    (count * (count - long_int_t(1)) >> 1);
                assert(h.first > val && ad > 0);
                result += ad;

                holes.clear();
                holes[h.first + count] = inf;
                // h.first += count;
                return;
            }

            // todo: faster.
            //auto it = std::lower_bound(holes.begin(), holes.end(), { val, 0 });
            //assert(it != holes.end());
            {
                auto it = holes.begin();
                assert(it->first < it->second&& val != it->first);

                if (val < it->first)
                {
                    const auto ad = it->first - val;
                    result += ad;
                    assert(ad > 0 && result > 0);

                    const auto k2 = it->first + long_int_t(1),
                        v2 = it->second;
                    holes.erase(it);
                    if (k2 < v2)
                        holes[k2] = v2;
                    continue;
                }
            }

            auto it2 = holes.lower_bound(val);
            assert(it2 != holes.end() && it2 != holes.begin());

            auto it1 = std::prev(it2);

            const long_int_t c1 = abs(it1->second - long_int_t(1) - long_int_t(val)),
                c2 = abs(it2->first - long_int_t(val));

            if (c1 < c2)
            {
                result += c1;
                assert(result > 0);

                --(it1->second);
                if (it1->first == it1->second)
                    holes.erase(it1);
            }
            else
            {
                result += c2;
                assert(result > 0);

                const auto k2 = it2->first + long_int_t(1),
                    v2 = it2->second;
                holes.erase(it2);
                if (k2 < v2)
                    holes[k2] = v2;
            }
        } while (--count > 0);
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Compute min total cost to remove n>0 positive integers,
            // where item remove cost = abs(ar[i] - time),
            // time = 1 and increases by 1 after each removal.
            //
            // Time O(n*max(n, ar)). Space O(max(n, ar)).
            template<class long_int_t, class int_t>
            long_int_t min_time_array_abs_dif_sum(const std::vector<int_t>& source)
            {
                static_assert(std::is_signed_v<long_int_t> && std::is_signed_v<int_t> && sizeof(long_int_t) >= sizeof(int_t));

                const auto size = static_cast<int_t>(source.size());
                RequirePositive(size, "source.size");
                if (size == 1)
                    return source[0] >= 1
                    ? long_int_t(0)
                    : long_int_t(1) - source[0];

                auto ar = source;
                std::sort(ar.begin(), ar.end());
                assert(ar[0] > 0);
                RequirePositive(ar[0], "ar[0]");

                constexpr auto inf = std::numeric_limits<long_int_t>::max() / 3;

                // min cost to remove items in time t, corrected by index i to save space.
                // See the test for the details.
                std::vector<long_int_t> cur, prev;

                auto has_unused = false;
                if (has_unused)
                {
                    cur.reserve(ar[0] + 1);
                    cur.push_back(0);
                }

                for (long_int_t time = 0;;)
                {
                    const auto dif = abs(++time - ar[0]);
                    assert(dif >= 0);
                    cur.push_back(dif);
                    if (!dif)
                        break;
                }

#if _DEBUG
                std::vector<std::vector<long_int_t>> all_debug;
                all_debug.reserve(size);
                all_debug.push_back(cur);
#endif
                for (int_t i = 1; i < size; ++i)
                {
                    std::swap(prev, cur);
                    if (has_unused)
                        cur.assign(i + 1, inf);
                    else
                        cur.clear();

                    assert(prev.size());

                    for (long_int_t time = i + 1, prev_min = inf;;)
                    {
                        const long_int_t extr = has_unused ? 0 : i;
                        const auto is_time = time <= static_cast<long_int_t>(prev.size()) + extr;
                        if (is_time)
                        {
                            const auto ind = time - 1 - extr;
                            const auto& pr = prev[ind];
                            if (prev_min > pr)
                                prev_min = pr;
                        }

                        const auto c = prev_min + abs(time - ar[i]);
                        assert(c >= 0);
                        cur.push_back(c);

                        ++time;
                        if (!is_time && time > ar[i])
                            break;
                    }

                    while (cur.size() >= 2 && cur[cur.size() - 2] + long_int_t(1) == cur.back())
                        // Remove useless data.
                        cur.pop_back();
#if _DEBUG
                    all_debug.push_back(cur);
#endif
                }

                assert(cur.size());
                const auto result = *std::min_element(cur.begin(), cur.end());
                assert(result >= 0 && result < inf);
                return result;
            }

            template<class long_int_t, class int_t>
            long_int_t min_time_array_abs_dif_sum_bad_draft(const std::vector<int_t>& source)
            {
                static_assert(std::is_signed_v<long_int_t> && std::is_signed_v<int_t> && sizeof(long_int_t) >= sizeof(int_t));

                const auto size = static_cast<int_t>(source.size());
                RequirePositive(size, "source.size");
                if (size == 1)
                    return source[0] >= 1
                    ? long_int_t(0)
                    : long_int_t(1) - source[0];

                auto ar = source;
                std::sort(ar.begin(), ar.end());
                assert(ar[0] > 0);

                // Idea: Find dense regions.

                //if (ar[0] > 0)
                //{
                //    int_t i = 0;
                //    while (++i < size &&ar[i - 1] < ar[i])
                //    {
                //    }
                //
                //    if (i == size) // All positive, and unique.
                //        return 0;
                //}

                long_int_t result = 0;
                //, time = 1;
                //int_t i = 0;
                //while (i < size &&ar[i] <= 0)
                //{
                //    result += time - ar[i];
                //    assert(result > 0);
                //    ++time, ++i;
                //}
                //if (i == size) // All <= 0.
                //    return result;

                std::map<long_int_t, long_int_t> holes;
                //std::vector<std::pair<long_int_t, long_int_t>> holes;
                std::vector<std::pair<int_t, int_t>> debts;
                //for (; i < size; ++i)
                //{
                //    const auto &a = ar[i];
                //    if (time < a)
                //    {
                //        beg_end.push_back({time, a});
                //        time = a - 1;
                //        continue;
                //    }
                //    if (time == a)
                //    {
                //        ++time;
                //        continue;
                //    }
                //    // time > a
                //    debts.push_back(a);
                //}

                if (ar[0] > 1)
                    //holes.push_back({ long_int_t(1), ar[0] });
                    holes[1] = ar[0];

                for (int_t j = 0; j < size - 1; ++j)
                {
                    const auto& prev = ar[j], & cur = ar[j + 1];
                    if (prev + long_int_t(1) < cur)
                        // 2,3 -> skip.
                        // 2,4 -> add [3,4)
                        holes[prev + long_int_t(1)] = cur;
                    //holes.push_back({ prev, cur });
                    else if (prev == cur)
                    {
                        if (debts.empty() || debts.back().first != cur)
                            debts.push_back({ cur, 1 });
                        else
                            ++(debts.back().second);
                    }
                }

                if (debts.empty())
                    return result; // All unique.

                constexpr auto inf = std::numeric_limits<long_int_t>::max() / 3;
                //holes.push_back({ ar.back() + long_int_t(1), inf });
                holes[ar.back() + long_int_t(1)] = inf;

                //std::reverse(holes.begin(), holes.end());
                std::reverse(debts.begin(), debts.end());

                // auto pos = static_cast<int_t>(holes.size() - 1);
                for (const auto& de : debts)
                {
                    proc_debt<long_int_t, int_t>(holes, result, inf, de.first, de.second);
                }

                assert(result >= 0);
                return result;
            }

            // They. Time/space O(n*max(n, ar)).
            template<class long_int_t, class int_t>
            long_int_t min_time_array_abs_dif_sum_they(const std::vector<int_t>& source)
            {
                static_assert(std::is_signed_v<long_int_t> && std::is_signed_v<int_t> && sizeof(long_int_t) >= sizeof(int_t));

                const auto size = static_cast<int_t>(source.size());
                RequirePositive(size, "source.size");

                auto ar = source;
                std::sort(ar.begin(), ar.end());

                constexpr auto inf = std::numeric_limits<long_int_t>::max() / 3;
                const auto remove_time_max = static_cast<long_int_t>(
                    // Fix for large numbers.
                    std::max(ar.back(), size)) << 1;

                // min cost to remove elements [0..i] using j time
                std::vector<std::vector<long_int_t>> cost(size + 1,
                    std::vector<long_int_t>(remove_time_max + 1, inf));

                cost[0][0] = 0;

                for (int_t i = 0; i < size; ++i)
                {
                    auto prev = cost[i][0];

                    for (long_int_t time = 1; time <= remove_time_max; ++time)
                    {
                        const auto c = prev + abs(time - ar[i]);
                        auto& val = cost[i + 1][time];
                        if (val > c)
                            val = c;

                        const auto& v2 = cost[i][time];
                        if (prev > v2)
                            prev = v2;
                    }
                }

                const auto mi = *std::min_element(cost[size].begin() + 1,
                    cost[size].begin() + remove_time_max + 1);

                assert(mi >= 0 && mi < inf);
                return mi;
            }
        }
    }
}