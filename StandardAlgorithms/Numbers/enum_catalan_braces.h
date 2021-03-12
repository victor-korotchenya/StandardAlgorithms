#pragma once
#include"../Utilities/require_utilities.h"

namespace Standard::Algorithms::Numbers::Inner
{
    constexpr auto catalan_string_max_size = static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max() - 7);

    template<class proc_t, char open_c, char close_c, class check_t>
    requires(open_c != close_c)
    constexpr void enum_catalan_braces_rec(
        proc_t proc, const std::int32_t open1, std::string &str, const std::int32_t close1, check_t check)
    {
        assert(0 <= open1 && 0 < close1 && open1 <= close1);

        if (0 < open1)
        {
            str.push_back(open_c);

            enum_catalan_braces_rec<proc_t, open_c, close_c, check_t>(proc, open1 - 1, str, close1, check);

            str.pop_back();
        }
        else if (close1 == 1)
        {
            str.push_back(close_c);

            {
                [[maybe_unused]] const auto good = check(str);

                assert(good);
            }

            proc(str);

            str.pop_back();

            return;
        }

        if (open1 == close1)
        {
            return;
        }

        str.push_back(close_c);

        enum_catalan_braces_rec<proc_t, open_c, close_c, check_t>(proc, open1, str, close1 - 1, check);

        str.pop_back();
    }

    template<char open_c = '[', char close_c = ']'>
    requires(open_c != close_c)
    constexpr void build_minimal_catalan_string(std::string &str, const std::int32_t size)
    {
        if (size < 0 || catalan_string_max_size < static_cast<std::uint32_t>(size) ||
            0U != (static_cast<std::uint32_t>(size) & 1U)) [[unlikely]]
        {
            auto err = "Bad Catalan string size " + std::to_string(size);

            throw std::runtime_error(err);
        }

        str.resize(size);

        if (size == 0)
        {
            return;
        }

        const auto half = size / 2;
        auto index = half - 1;

        assert(2 * half == size);

        do
        {
            assert(0 <= index);

            str[index] = open_c;
            str[index + half] = close_c;
        } while (0 <= --index);
    }

    template<char open_c = '[', char close_c = ']'>
    requires(open_c != close_c)
    constexpr void build_catalan_end(
        std::int32_t index, std::string &str, const std::pair<std::int32_t, std::int32_t> &open_close) noexcept
    {
        assert(4U <= str.size() && 0U == (str.size() & 1U) && str.size() <= catalan_string_max_size);

        const auto size = static_cast<std::int32_t>(str.size());
        assert(0 < index && index < size);

        auto open1 = open_close.first;
        auto close1 = open_close.second;
        assert(0 <= open1 && open1 < close1 && close1 <= size / 2);

        str[index] = close_c;

        do
        {
            assert(0 < index && index < size - 1);

            str[++index] = open_c;
        } while (0 < --open1);

        --close1;

        do
        {
            assert(0 < index && index < size - 1);

            str[++index] = close_c;
        } while (0 < --close1);
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    template<char open_c = '[', char close_c = ']'>
    requires(open_c != close_c)
    [[nodiscard]] constexpr auto is_catalan_string(const std::string &str) noexcept -> bool
    {
        const auto size = str.size();

        if ((size & 1U) != 0U)
        {// The size must be even.
            return false;
        }

        std::size_t open1{};
        std::size_t close1{};

        for (const auto &cha : str)
        {
            if (cha == open_c)
            {
                ++open1;
            }
            else if (cha != close_c || open1 < ++close1)
            {
                return false;
            }
        }

        return open1 == close1;
    }

    // Enumerate all valid/balanced Catalan strings.
    template<class proc_t, char open_c = '[', char close_c = ']'>
    requires(open_c != close_c)
    constexpr void enum_catalan_braces(proc_t proc, const std::int32_t size)
    {
        {
            const auto *const name = "size";

            require_positive(size, name);

            constexpr auto greater1 = 31;

            require_greater(greater1, size, name);
        }

        std::string str;
        str.reserve(size * 2LL);

        Inner::enum_catalan_braces_rec<proc_t, open_c, close_c>(
            proc, size, str, size, &is_catalan_string<open_c, close_c>);
    }

    // Method 2: find the rightmost imbalanced [ e.g. "[ ]]] [][]",
    // replace [ with ],
    // print [[[
    // and ]]]].
    // to build "] [[[ ]]]]".
    template<char open_c = '[', char close_c = ']'>
    requires(open_c != close_c)
    [[nodiscard]] constexpr auto next_catalan_string(std::string &str) -> bool
    {
        if (str.size() < 4U || Inner::catalan_string_max_size < str.size() || !is_catalan_string<open_c, close_c>(str))
        {
            return false;
        }

        const auto size = static_cast<std::int32_t>(str.size());
        assert(4 <= size && 0U == (str.size() & 1U));

        for (std::int32_t open1{}, close1{}, index = size - 1;;)
        {
            assert(0 < index && index < size && open1 <= close1);

            const auto &cha = str[index];
            // ..  [ ]] [][]
            // ..  ] [[[ ]]]
            //
            // ..  [ ]]
            // ..  ] []
            if (cha == open_c)
            {
                assert(open1 < close1);

                if (++open1 < close1)
                {
                    Inner::build_catalan_end<open_c, close_c>(index, str, std::make_pair(open1, close1));

                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        const auto ist = is_catalan_string<open_c, close_c>(str);
                        assert(ist);
                    }

                    return true;
                }
            }
            else
            {
                ++close1;

                assert(cha == close_c && open1 < close1);
            }

            if (0 == --index)
            {
                Inner::build_minimal_catalan_string<open_c, close_c>(str, size);


                if constexpr (::Standard::Algorithms::is_debug)
                {
                    const auto ist = is_catalan_string<open_c, close_c>(str);
                    assert(ist);
                }

                return false;
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
