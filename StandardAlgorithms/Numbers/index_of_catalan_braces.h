#pragma once
#include"../Utilities/is_debug.h"
#include"enum_catalan_braces.h" // is_catalan_string
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Prepare once.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto compute_catalan_brace_table(const int_t &size) -> std::vector<std::vector<int_t>>
    {
        require_positive(size, "size");

        constexpr int_t one = 1;
        constexpr int_t two = 2;
        constexpr int_t max_diff = 3;

        {
            constexpr auto greater1 = static_cast<int_t>(std::numeric_limits<int_t>::max() - max_diff - max_diff);

            static_assert(max_diff < greater1);

            require_greater(greater1, size, "size");
        }

        std::vector<std::vector<int_t>> table(size + one);
        table.at(0) = { 0, 1, 0, 0 }; // empty string.

        for (int_t len = 1; len <= size; ++len)
        {
            const auto &prev = table[len - one];
            auto &cur = table[len];

            // Too large balance does not matter.
            const auto bal_max = std::min<int_t>(len, size + one - len);
            cur.resize(bal_max + max_diff + one);

            for (auto bal = static_cast<int_t>(len & 1); bal <= bal_max; bal += two)
            {
                const auto add = prev[bal] + prev[bal + two];
                // balance:  0 1 2 3
                // length
                //  0   1
                //  1     1
                //  2   1   1
                //  3     2   1
                //  4   2   3
                //  5     5
                //  6   5
                cur[bal + one] += add;
            }
        }

        return table;
    }

    // Count how many valid Catalan strings stand before.
    // E.g. "[[][]]" has index 1 in the sorted array:
    // "[[[]]]",
    // "[[][]]",
    // "[[]][]",
    // "[][[]]",
    // "[][][]".
    template<std::signed_integral int_t, char open_c = '[', char close_c = ']'>
    requires(open_c != close_c)
    [[nodiscard]] constexpr auto index_of_catalan_braces(
        const std::vector<std::vector<int_t>> &table, const std::string &str) -> int_t
    {
        assert(str.size() + 1LLU == table.size());

        if constexpr (::Standard::Algorithms::is_debug)
        {
            [[maybe_unused]] const auto is_cat = is_catalan_string<open_c, close_c>(str);

            assert(is_cat);
        }

        constexpr int_t zero{};
        constexpr int_t one = 1;

        const auto size = static_cast<int_t>(str.size());
        assert(static_cast<std::size_t>(size) == str.size());

        if (zero == size)
        {
            return zero;
        }

        require_positive(size, "size");

        assert(str.front() == open_c && str.back() == close_c);

        int_t count{};

        for (int_t pos{}, bal{}; pos < size; ++pos)
        {
            const auto &cha = str[pos];

            if (cha == open_c)
            {
                ++bal;
                continue;
            }

            assert(cha == close_c && zero < bal);

            const auto hindi = static_cast<int_t>(size - one - pos);
            const auto &add = table.at(hindi).at(bal + one + one);
            assert(zero <= add);

            count += add;
            assert(zero <= count);
            --bal;
        }

        return count;
    }

    // Given the index, build the valid Catalan string with that index. The string length is table.size - 1.
    // E.g. index 1 of length 6 has the string "[[][]]".
    template<std::signed_integral int_t, char open_c = '[', char close_c = ']'>
    requires(open_c != close_c)
    constexpr void brace_string_by_index_of_catalan(
        const std::vector<std::vector<int_t>> &table, std::string &str, const int_t &index)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        const auto size = static_cast<int_t>(table.size() - 1U);

        assert(!table.empty() && zero <= index && zero <= size && zero == (size & one));

        str.resize(size, open_c);

        if (size == zero)
        {
            assert(index == zero);

            return;
        }

        int_t bal{};
        auto left = index;

        for (int_t pos{}; pos < size; ++pos)
        {
            assert(zero <= bal);

            const auto hindi = static_cast<int_t>(size - one - pos);
            const auto &gre = table.at(hindi).at(bal + one + one);

            auto &cha = str[pos];

            if (left < gre)
            {
                ++bal;
                cha = open_c;
                continue;
            }

            assert(zero < bal && zero <= gre && gre <= left);

            --bal;
            cha = close_c;
            left -= gre;
        }

        assert(zero == bal && zero == left && str.front() == open_c && str.back() == close_c);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            [[maybe_unused]] const auto is_cat = is_catalan_string<open_c, close_c>(str);

            assert(is_cat);
        }
    }
} // namespace Standard::Algorithms::Numbers
