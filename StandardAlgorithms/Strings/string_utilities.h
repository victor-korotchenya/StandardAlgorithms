#pragma once
// "string_utilities.h"
#include"../Utilities/ert.h"
#include<cctype> // tolower
#include<string_view>

namespace Standard::Algorithms::Strings
{
    template<class string_t, std::integral int_t>
    [[nodiscard]] constexpr auto is_palindrome(const string_t &str, const int_t &size) -> bool
    {
        constexpr int_t one{ 1 };

        const auto half_size = static_cast<int_t>(size / 2);
        assert(!(half_size < int_t{}));

        for (int_t index{}; index < half_size; ++index)
        {
            if (str[index] != str[size - one - index])
            {
                return false;
            }
        }

        return true;
    }

    template<class pair_t, std::integral int_t>
    constexpr void test_check_pair(const std::string &name, const pair_t &par, const int_t &size)
    {
        using inner_t = std::remove_cvref_t<decltype(par.first)>;
        {
            using inner_t_2 = std::remove_cvref_t<decltype(par.second)>;
            static_assert(std::is_same_v<inner_t, inner_t_2>);
        }

        constexpr inner_t zero{};

        if (zero < par.second)
        {
            ::Standard::Algorithms::ert::greater_or_equal(par.first, zero, name + ".first");
            ::Standard::Algorithms::ert::greater(par.second, par.first, name + ".second");

            ::Standard::Algorithms::ert::greater_or_equal(size, static_cast<int_t>(par.second), name + ".second");
        }
        else
        {
            ::Standard::Algorithms::ert::are_equal(zero, par.first, name + ".first");
        }
    }

    [[nodiscard]] inline constexpr auto find_count_max2(
        const std::string_view pattern, const std::string_view text) noexcept -> std::int32_t
    {
        std::size_t pos = std::string::npos;
        std::int32_t count{};

        for (;;)
        {
            pos = text.find(pattern, pos + 1U);

            if (pos == std::string::npos)
            {
                return count;
            }

            if (++count == 2)
            {
                return 2;
            }
        }
    }

    constexpr void to_lower(auto &str)
    {
        for (auto &cha : str)
        {
            cha = static_cast<char>(std::tolower(cha));
        }
    }
} // namespace Standard::Algorithms::Strings
