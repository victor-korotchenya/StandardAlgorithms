#pragma once
#include"../Strings/string_utilities.h" // is_palindrome
#include"../Utilities/check_size.h"
#include<vector>

// todo(p4): to strings.

namespace Standard::Algorithms::Numbers::Inner
{
    template<class string_t, std::signed_integral int_t>
    [[nodiscard]] constexpr auto str_cut_min_pal_rec_slow(
        const string_t &str, std::vector<std::vector<int_t>> &buf, const int_t from, const int_t to_incl) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        assert(!(from < zero) && from <= to_incl && to_incl < static_cast<int_t>(str.size()));

        auto &value = buf[from][to_incl];

        if (const auto is_computed = !(value < zero); is_computed)
        {
            return value;
        }

        const auto size = static_cast<int_t>(to_incl - from + one);
        assert(zero < size && value == -one);

        if (Standard::Algorithms::Strings::is_palindrome(&str[from], size))
        {
            return (value = zero);
        }

        assert(one < size);

        value = std::numeric_limits<int_t>::max();

        for (auto mid = from; mid < to_incl; ++mid)
        {
            const auto left = str_cut_min_pal_rec_slow<string_t, int_t>(str, buf, from, mid);
            assert(zero <= left && left <= mid - from);

            const auto rig =
                str_cut_min_pal_rec_slow<string_t, int_t>(str, buf, static_cast<int_t>(mid + one), to_incl);
            assert(zero <= rig && rig <= to_incl - (mid + one));

            const auto cand = static_cast<int_t>(left + rig);
            assert(left <= cand && rig <= cand && cand < size);

            if (cand < value)
            {
                value = cand;

                if (zero == cand)
                {
                    break;
                }
            }
        }

        assert(zero <= value && value < size - one);
        ++value;
        assert(zero < value && value < size);

        return value;
    }

    template<class string_t, std::signed_integral int_t>
    constexpr void separ_by_mid_str_cuts_pals(
        const string_t &str, const int_t &size, std::vector<std::vector<int_t>> &buf)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        assert(one < size && static_cast<std::size_t>(size) == buf.size() && str.size() == buf.size());

        for (int_t mid{}; mid < size; ++mid)
        {
            buf[mid][mid] = zero;

            {// Odd
                const auto odd_length = std::min(mid, static_cast<int_t>(size - one - mid));
                assert(!(odd_length < zero));

                for (auto step = one; step <= odd_length && str[mid - step] == str[mid + step]; ++step)
                {
                    buf[mid - step][mid + step] = zero;
                }
            }
            {// Even
                const auto even_length = std::min(mid, static_cast<int_t>(size - 2 - mid));
                assert(-one <= even_length);

                for (int_t step{}; step <= even_length && str[mid - step] == str[mid + step + one]; ++step)
                {
                    buf[mid - step][mid + step + one] = zero;
                }
            }
        }
    }

    template<class string_t, std::signed_integral int_t, int_t inf>
    requires(int_t{} < inf)
    constexpr void comb_by_mid_str_cuts_pals(
        const string_t &str, const int_t &size, std::vector<std::vector<int_t>> &buf)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        assert(
            one < size && size - one < inf && static_cast<std::size_t>(size) == buf.size() && str.size() == buf.size());

        for (int_t start = size - 2; zero <= start; --start)
        {
            auto &val = buf[start].back();

            if (const auto already_optimal = zero == val; already_optimal)
            {
                continue;
            }

            assert(val == inf);

            auto mid = start;

            do
            {
                const auto &left = buf[start][mid];
                assert(!(left < zero) && left <= inf);

                const auto &right = buf[mid + one].back();
                assert(!(right < zero) && right <= inf);

                const auto cand = static_cast<int_t>(left + right);
                assert(left <= cand && right <= cand && cand <= inf * 2LL);

                if (!(cand < val))
                {
                    continue;
                }

                val = cand;

                if (zero == cand)
                {
                    break;
                }
            } while (++mid != size - one);

            assert(!(val < zero) && val < size);

            ++val;
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Compute the min number of cuts (partitions) of a string into palindromes.
    // For example, "abb" can be cut into palindromes by 1 cut: "a", "bb".
    // Time, space O(n*n).
    template<class string_t, std::signed_integral int_t = std::int32_t>
    [[nodiscard]] constexpr auto string_cut_into_min_palindromes(const string_t &str) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;
        constexpr int_t inf = std::numeric_limits<int_t>::max() / 3;
        static_assert(one < inf);

        const auto size = Standard::Algorithms::Utilities::check_size<int_t, inf>("string size", str.size());

        if (size <= one || Standard::Algorithms::Strings::is_palindrome(&str[0], size))
        {
            return {};
        }

        // from, to_incl -> cuts
        std::vector<std::vector<int_t>> buf(size, std::vector<int_t>(size, inf));

        Inner::separ_by_mid_str_cuts_pals<string_t, int_t>(str, size, buf);

        Inner::comb_by_mid_str_cuts_pals<string_t, int_t, inf>(str, size, buf);

        const auto &result = buf.front().back();

        assert(zero < result && result < size);

        return result;
    }

    // Slow time O(n*n*n), space O(n*n).
    template<class string_t, std::signed_integral int_t = std::int32_t>
    [[nodiscard]] constexpr auto string_cut_into_min_palindromes_slow(const string_t &str) -> int_t
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        const auto size = Standard::Algorithms::Utilities::check_size<int_t>("string size", str.size());

        if (size <= one || Standard::Algorithms::Strings::is_palindrome(&str[0], size))
        {
            return zero;
        }

        // from, to_incl -> cuts
        std::vector<std::vector<int_t>> buf(size, std::vector<int_t>(size, -one));

        auto result = Inner::str_cut_min_pal_rec_slow<string_t, int_t>(str, buf, zero, size - one);

        assert(zero < result);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
