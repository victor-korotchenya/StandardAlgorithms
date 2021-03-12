#pragma once
#include <unordered_map>
#include <queue>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class string_t>
    void check_permutation_strings(const string_t& source, const string_t& target)
    {
        const auto size = static_cast<int>(source.size());
        RequirePositive(size, "source.size");

        {
            const auto size2 = static_cast<int>(target.size());
            if (size != size2)
                throw std::runtime_error("Source and target permutations must have the same size.");
        }

        auto s = source, t = target;
        std::sort(s.begin(), s.end());
        std::sort(t.begin(), t.end());
        if (s != t)
            throw std::runtime_error("Source and target permutations must consist of the same items.");
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Count min swaps of two consecutive elements to turn a permutation into another one using 2 waves meeting, or meet in the middle, or bidirectional search.
            // Time O(n*n), space O(n).
            template<class string_t>
            int min_swaps_between_permutations(const string_t& source, const string_t& target)
            {
                if (source == target)
                    return {};

                // It can be omitted or made linear for a small ABC.
                check_permutation_strings<string_t>(source, target);

                auto size = static_cast<int>(source.size());
                assert(size >= 2);

                while (source[size - 1] == target[size - 1])
                {
                    assert(size >= 2);
                    --size;
                }

                auto start = 0;
                while (source[start] == target[start])
                {
                    assert(start < size - 1);
                    ++start;
                }

                auto count = 0;
                string_t cop = source;
                do
                {
                    const auto& ct = target[start];
                    if (ct == cop[start])
                        continue;

                    auto i = start + 1;
                    while (ct != cop[i])
                    {
                        assert(i < size - 1);
                        ++i;
                    }

                    count += i - start;

                    do std::swap(cop[i], cop[i - 1]);
                    while (--i > start);
                } while (++start < size);

                assert(cop == target && count > 0);
                return count;
            }

            template<class string_t>
            std::pair<int, size_t> min_swaps_between_permutations_slow_still(const string_t& source, const string_t& target)
            {
                if (source == target)
                    return {};

                check_permutation_strings<string_t>(source, target);

                const auto size = static_cast<int>(source.size());
                assert(size >= 2 && size <= 20);
                {
                    auto size1 = size;
                    while (size1 && source[size1 - 1] == target[size1 - 1])
                        --size1;

                    auto start = 0;
                    while (start < size1 && source[start] == target[start])
                        ++start;

                    if (start || size1 != size)
                    {
                        const string_t s = source.substr(start, size1 - start),
                            t = target.substr(start, size1 - start);

                        const auto res = min_swaps_between_permutations_slow_still<string_t>(s, t);
                        assert(res.first > 0 && res.second > 0);
                        return res;
                    }
                }

                // Use (distance + 1) to differentiate from 0.
                using dist1_istarget_t = std::pair<int, bool>;

                std::unordered_map<string_t, dist1_istarget_t> m;
                m[source] = { 1, false };
                m[target] = { 1, true };

                std::queue<std::pair<string_t, dist1_istarget_t>> q;
                q.push({ source, { 1, false } });
                q.push({ target,{ 1, true } });

                do
                {
                    auto str = q.front().first;
                    const auto dist1 = q.front().second.first + 1;
                    const auto istarget = q.front().second.second;
                    assert(dist1 >= 2);
                    q.pop();

                    for (auto i = 0; i < size - 1; ++i)
                    {
                        std::swap(str[i], str[i + 1]);
                        auto& info = m[str];
                        auto& d = info.first;
                        auto& istarget2 = info.second;
                        if (d)
                        {
                            if (istarget != istarget2)
                                return { dist1 + d -
                                // The initial two 1.
                                2, m.size() };
                        }
                        else
                        {
                            d = dist1;
                            istarget2 = istarget;
                            q.push({ str, { dist1, istarget } });
                        }

                        std::swap(str[i], str[i + 1]);
                    }
                } while (q.size());

                assert(0);
                throw std::runtime_error("min_swaps_between_permutations inner error.");
            }

            // Slow time O(n!*n*n), space O(n!).
            template<class string_t>
            std::pair<int, size_t> min_swaps_between_permutations_slow(const string_t& source, const string_t& target)
            {
                if (source == target)
                    return {};

                check_permutation_strings<string_t>(source, target);

                const auto size = static_cast<int>(source.size());
                assert(size >= 2 && size <= 20);

                std::unordered_map<string_t, int> str_distance;
                str_distance[source] = 0;

                std::queue<std::pair<string_t, int>> q;
                q.push({ source, 0 });

                do
                {
                    auto str = q.front().first;
                    const auto dist1 = q.front().second + 1;
                    assert(dist1 > 0);
                    q.pop();

                    for (auto i = 0; i < size - 1; ++i)
                    {
                        std::swap(str[i], str[i + 1]);
                        auto& d = str_distance[str];
                        if (!d)
                        {
                            if (target == str)
                                return { dist1, str_distance.size() };

                            d = dist1;
                            q.push({ str, dist1 });
                        }

                        std::swap(str[i], str[i + 1]);
                    }
                } while (q.size());

                assert(0);
                throw std::runtime_error("min_swaps_between_permutations_slow inner error.");
            }
        }
    }
}