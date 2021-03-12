#pragma once
#include <vector>
#include "../Strings/string_utilities.h"

namespace
{
    template<class string_t, class int_t>
    int_t str_cut_min_pal_rec_slow(const string_t& s,
        std::vector<std::vector<int_t>>& buf,
        const int_t from, const int_t to_incl)
    {
        static_assert(std::is_signed_v<int_t>);
        assert(from >= 0 && from <= to_incl && to_incl < static_cast<int_t>(s.size()));

        auto& value = buf[from][to_incl];
        if (value >= 0)
            return value;

        const auto size = 1 + to_incl - from;
        assert(size > 0 && value == -1);
        if (Privet::Algorithms::Strings::is_palindrome(&s[from], size))
            return (value = 0);

        assert(size >= 2);
        constexpr auto inf = std::numeric_limits<int_t>::max();
        value = inf;

        for (auto mid = from; mid < to_incl; ++mid)
        {
            const auto
                left = str_cut_min_pal_rec_slow<string_t, int_t>(s, buf, from, mid),
                rig = str_cut_min_pal_rec_slow<string_t, int_t>(s, buf, mid + 1, to_incl),
                cand = left + rig;

            assert(0 <= cand && cand < inf && 0 <= left && left <= cand && 0 <= rig && rig <= cand);
            if (value > cand)
            {
                value = cand;
                if (!cand)
                    break;
            }
        }

        assert(0 <= value && value < inf);
        ++value;
        assert(0 < value);
        return value;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Compute the min cuts (partitions) of a string into palindromes.
            // Time/space O(n*n).
            template<class string_t, class int_t = int>
            int_t string_cut_into_min_palindromes(const string_t& s)
            {
                static_assert(std::is_signed_v<int_t>);

                const auto size = static_cast<int_t>(s.size());
                if (Privet::Algorithms::Strings::is_palindrome(&s[0], size))
                    return 0; // Entire string is a palindrome.

                assert(size >= 2);
                constexpr auto inf = std::numeric_limits<int_t>::max() / 3;

                // from, to_incl -> cuts
                std::vector<std::vector<int_t>> buf(size, std::vector<int_t>(size, inf));

                for (int_t mid = 0; mid < size; ++mid)
                {
                    buf[mid][mid] = 0;

                    {//Odd
                        const auto odd_length = std::min(mid, size - 1 - mid);
                        assert(odd_length >= 0);
                        for (int_t x = 1; x <= odd_length && s[mid - x] == s[mid + x]; ++x)
                            buf[mid - x][mid + x] = 0;
                    }
                    {//Even
                        const auto even_length = std::min(mid, size - 2 - mid);
                        assert(even_length >= -1);
                        for (int_t x = 0; x <= even_length && s[mid - x] == s[mid + x + 1]; ++x)
                            buf[mid - x][mid + x + 1] = 0;
                    }
                }

                for (int_t start = size - 2; start >= 0; --start)
                {
                    auto& val = buf[start].back();
                    if (!val)
                        continue;

                    assert(val == inf);
                    auto mid = start;
                    do
                    {
                        const int_t& left = buf[start][mid],
                            & right = buf[mid + 1].back(),
                            cand = left + right;
                        assert(cand >= 0 && left >= 0 && left <= inf && left <= cand && right >= 0 && right <= inf && right <= cand && cand <= inf * 2);
                        if (val > cand)
                        {
                            val = cand;
                            if (!cand)
                                break;
                        }
                    } while (++mid != size - 1);

                    assert(val >= 0 && val < inf);
                    ++val;
                }

                const auto& result = buf[0][size - 1];
                assert(0 < result && result < inf);
                return result;
            }

            // Slow time O(n*n*n), space O(n*n).
            template<class string_t, class int_t = int>
            int_t string_cut_into_min_palindromes_slow(const string_t& s)
            {
                static_assert(std::is_signed_v<int_t>);

                const auto size = static_cast<int_t>(s.size());
                if (Privet::Algorithms::Strings::is_palindrome(&s[0], size))
                    return 0; // Entire string is a palindrome.

                assert(size >= 2);
                // from, to_incl -> cuts
                std::vector<std::vector<int_t>> buf(size, std::vector<int_t>(size, -1));

                const auto result = str_cut_min_pal_rec_slow<string_t, int_t>(s, buf, 0, size - 1);
                assert(0 < result);
                return result;
            }
        }
    }
}