#pragma once
#include <cassert>
#include <unordered_map>
#include <vector>

namespace
{
    template <class int_t2, class int_t>
    std::vector<int_t> restore_subsequence(const std::vector<int_t>& ar, const int len_max, int pos_max, const int_t2 dif_max)
    {
        static_assert(sizeof(int_t) <= sizeof(int_t));
        assert(len_max >= 2 && pos_max > 0);

        std::vector<int_t> result;
        result.reserve(len_max);
        result.push_back(ar[pos_max]);

        while (--pos_max >= 0)
            if (result.back() - dif_max == ar[pos_max])
                result.push_back(ar[pos_max]);

        assert(static_cast<int>(result.size()) == len_max);
        std::reverse(result.begin(), result.end());
        return result;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Longest subsequence of a sorted list which forms an arithmetic progression.
            // Time O(n**2), space O(n).

            // Slow time, space O(n**2).
            template <class int_t2, class int_t>
            std::vector<int_t> longest_arithmetic_progression_in_sorted_slow2(const std::vector<int_t>& ar)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t));

                const auto size = static_cast<int>(ar.size());
                if (size <= 2)
                    return ar;

                assert(ar[0] <= ar[1] && ar[1] <= ar.back());
                auto len_max = 2, pos_max = 1;
                int_t2 dif_max = static_cast<int_t2>(ar[1]) - ar[0];

                std::vector<std::unordered_map<int_t2, int>> buf(size);

                for (auto i = 1; i < size; ++i)
                {
                    assert(ar[i - 1] <= ar[i]);
                    auto& bcur = buf[i];

                    for (auto j = i - 1; j >= 0; --j)
                    {
                        const int_t2 dif = static_cast<int_t2>(ar[i]) - ar[j];
                        auto& cur = bcur[dif];

                        const auto& bp = buf[j];
                        const auto it = bp.find(dif);
                        if (it == bp.end())
                        {
                            if (!cur)
                                cur = 2;
                            continue;
                        }

                        assert(it->second >= 2);
                        const auto cand = it->second + 1;
                        if (cur >= cand)
                            continue;

                        cur = cand;
                        if (len_max >= cand)
                            continue;

                        len_max = cand, pos_max = i, dif_max = dif;
                    }
                }

                const auto result = restore_subsequence<int_t2, int_t>(ar, len_max, pos_max, dif_max);
                return result;
            }

            // O(n*n) space.
            // It is 30 per cent slower than longest_arithmetic_progression_in_sorted_slow2.
            template <class int_t2, class int_t>
            std::vector<int_t> longest_arithmetic_progression_in_sorted_they(const std::vector<int_t>& ar)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t));

                const auto size = static_cast<int>(ar.size());
                if (size <= 2)
                    return ar;

                assert(ar[0] <= ar[1] && ar[1] <= ar.back());

                int_t2 dif_max = static_cast<int_t2>(ar[1]) - ar[0];
                auto len_max = 2, pos_max = 1;

                // Max length of a subsequence, having a difference, starting at.
                std::unordered_map<int_t2, std::unordered_map<int, int>> bufs;

                for (auto prev = 0; prev < size - 1; ++prev)
                {
                    for (auto cur = prev + 1; cur < size; ++cur)
                    {
                        const auto dif = static_cast<int_t2>(ar[cur]) - ar[prev];
                        auto& buf = bufs[dif];
                        auto& length = buf[cur];

                        const auto it = buf.find(prev);
                        if (it == buf.end())
                        {
                            length = 2;
                            continue;
                        }

                        assert(it->second >= 2 && length <= it->second + 1);
                        length = it->second + 1;

                        if (len_max >= length)
                            continue;

                        len_max = length, dif_max = dif, pos_max = cur;
                    }
                }

                const auto result = restore_subsequence<int_t2, int_t>(ar, len_max, pos_max, dif_max);
                return result;
            }

            // Slow time O(n**3).
            template <class int_t2, class int_t>
            std::vector<int_t> longest_arithmetic_progression_in_sorted_slow(const std::vector<int_t>& ar)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t));

                const auto size = static_cast<int>(ar.size());
                if (size <= 2)
                    return ar;

                std::vector<int_t> result{ ar[0], ar[1] }, buf;
                assert(ar[0] <= ar[1] && ar[1] <= ar.back());

                for (auto p = 0; p < size - 1; ++p)
                {
                    for (auto q = p + 1; q < size; ++q)
                    {
                        const int_t2 dif = static_cast<int_t2>(ar[q]) - ar[p];
                        buf.resize(1);
                        buf[0] = ar[p];

                        for (auto j = p - 1; j >= 0; --j)
                            if (buf.back() - dif == ar[j])
                                buf.push_back(ar[j]);

                        std::reverse(buf.begin(), buf.end());
                        buf.push_back(ar[q]);

                        for (auto j = q + 1; j < size; ++j)
                            if (buf.back() + dif == ar[j])
                                buf.push_back(ar[j]);

                        if (buf.size() > result.size())
                            result = std::move(buf);
                    }
                }

                return result;
            }
        }
    }
}