#pragma once
#include <cassert>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace
{
    template<class int_t, int_t mod>
    int_t adjacent_swaps(const std::string& s)
    {
        static_assert(mod > 1);

        const auto size = static_cast<int_t>(s.size());
        int64_t swaps = 0;
        for (int_t i = 0, ones = 0; i < size; ++i)
        {
            const auto& c = s[i];
            assert(!c || c == 1);
            if (c)
                ++ones;
            else
                swaps += ones;
        }
        return static_cast<int_t>(swaps % mod);
    }

    template<class int_t, int_t mod>
    int_t string_pattern_adjacent_swap_count_slow_rec(std::string& s, const int_t pos)
    {
        static_assert(mod > 1);
        if (pos < 0)
        {
            const auto count = adjacent_swaps<int_t, mod>(s);
            assert(count >= 0 && count < mod);
            return count;
        }

        auto& c = s[pos];
        if (c != '?' - '0')
            return string_pattern_adjacent_swap_count_slow_rec<int_t, mod>(s, pos - 1);

        c = 0;
        const auto a = string_pattern_adjacent_swap_count_slow_rec<int_t, mod>(s, pos - 1);

        c = 1;
        const auto b = string_pattern_adjacent_swap_count_slow_rec<int_t, mod>(s, pos - 1);
        c = '?' - '0';

        int_t count = a + b;
        if (count >= mod)
            count -= mod;

        assert(count >= 0 && count < mod);
        return count;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // A string of "01?", where '?' can be '0' or '1'.
            // Count min adjacent swaps to sort all strings modulo mod.
            // Example pattern "?0?" generates strings with swaps:
            //  "000" - 0, "001", "100" - 2, "101" - 1.
            //  Total swaps: 3.
            template<class int_t = int, int_t mod = 1000 * 1000 * 1000 + 7>
            int_t string_pattern_adjacent_swap_count(const std::string& pattern)
            {
                static_assert(mod > 1);

                const auto size = static_cast<int_t>(pattern.size());
                RequirePositive(size, "str.size");

                int64_t count = 0;
                for (int_t i = 0, ones = 0, questions = 0, power2_q_minus1 = 1;
                    i < size; ++i)
                {
                    const auto& c = pattern[i];
                    assert(c == '0' || c == '1' || c == '?');
                    if (c == '1')
                    {
                        if (++ones >= mod)
                            ones -= mod;
                        continue;
                    }

                    if (c == '?')
                    {
                        // Every inversion before must be repeated:
                        // - once for ending '0',
                        // - and for ending '1'.
                        count <<= 1;
                        if (count >= mod)
                            count -= mod;
                    }

                    if (questions)
                    {
                        // 1?1??11010 + 0
                        count =
                            // +ones for every string of 2**questions
                            (static_cast<int64_t>(ones << 1) * power2_q_minus1
                                // questions=3, there are 24 bits, half are 1s,
                                // total ones (or 0s): questions*2**(questions-1) = 12.
                                + static_cast<int64_t>(questions) * power2_q_minus1 + count) % mod;
                    }
                    else
                    {
                        // 111010 + 0
                        // 001111 + 0
                        // +ones
                        count += ones;
                        if (count >= mod)
                            count -= mod;
                    }

                    assert(count >= 0 && count < mod);
                    if (c == '?' && questions++)
                    {
                        power2_q_minus1 <<= 1;
                        if (power2_q_minus1 >= mod)
                            power2_q_minus1 -= mod;
                    }
                }

                assert(count >= 0 && count < mod);
                return static_cast<int_t>(count);
            }

            template<class int_t = int, int_t mod = 1000 * 1000 * 1000 + 7>
            int_t string_pattern_adjacent_swap_count_slow(const std::string& pattern)
            {
                static_assert(mod > 1);

                const auto size = static_cast<int_t>(pattern.size());
                RequirePositive(size, "str.size");

                auto s = pattern;
                for (int_t i = 0; i < size; ++i)
                {
                    auto& c = s[i];
                    c -= '0';
                    assert(!c || c == 1 || c == '?' - '0');
                }

                const auto count = string_pattern_adjacent_swap_count_slow_rec<int_t, mod>(s, size - 1);
                assert(count >= 0 && count < mod);
                return count;
            }
        }
    }
}