#pragma once
#include"../Utilities/require_utilities.h"
#include<array>

namespace Standard::Algorithms::Numbers::Inner
{
    constexpr auto knights_min_size = 3;
    constexpr auto knights_max_moves = 8U;

    template<std::signed_integral int_t>
    constexpr void check_point(const int_t &size, const std::pair<int_t, int_t> &point, const std::string &name)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;

        assert(zero < size && !name.empty());

        require_between(zero, point.first, size - one, "first coordinate " + name);

        require_between(zero, point.second, size - one, "second coordinate " + name);
    }

    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto is_banned(
        const std::pair<int_t, int_t> &from, const int_t &size, const std::pair<int_t, int_t> &tod) noexcept -> bool
    {
        if (size != knights_min_size || from == tod)
        {
            return false;
        }

        constexpr int_t one = 1;
        constexpr std::pair<int_t, int_t> banned{ one, one };

        auto bad = from == banned || tod == banned;
        return bad;
    }

    template<std::signed_integral int_t>
    constexpr void check_knights(
        const std::pair<int_t, int_t> &from, const int_t &size, const std::pair<int_t, int_t> &tod)
    {
        require_less_equal(knights_min_size, size, "size");

        check_point(size, from, "from");
        check_point(size, tod, "tod");

        if (from == tod) [[unlikely]]
        {
            throw std::invalid_argument("Points must be distinct.");
        }

        if (is_banned(from, size, tod)) [[unlikely]]
        {
            throw std::invalid_argument("Cannot move from or to (1, 1) when size is 3.");
        }
    }

    template<std::signed_integral int_t>
    [[nodiscard]] constexpr auto knight_moves() noexcept -> std::array<std::pair<int_t, int_t>, knights_max_moves>
    {
        return {
            std::make_pair(-2, -1),
            std::make_pair(-2, 1),
            std::make_pair(-1, -2),
            std::make_pair(-1, 2),
            std::make_pair(1, -2),
            std::make_pair(1, 2),
            std::make_pair(2, -1),
            std::make_pair(2, 1),
        };
    }
} // namespace Standard::Algorithms::Numbers::Inner
