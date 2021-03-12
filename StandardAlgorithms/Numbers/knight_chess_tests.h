#pragma once
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"knight_chess.h"
#include<vector>

namespace Standard::Algorithms::Numbers::Tests
{
    using int_t = std::int16_t;
    using point_t = std::pair<int_t, int_t>;
    using long_int_t = std::int32_t;

    constexpr long_int_t not_computed = -static_cast<long_int_t>(1);
    constexpr long_int_t zero{};
    static_assert(not_computed < zero);

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        inline constexpr test_case(std::string &&name, int_t size, point_t from, point_t tod, long_int_t expected) noexcept
            : base_test_case(std::move(name))
            , Size(size)
            , From(std::move(from))
            , Tod(std::move(tod))
            , Expected(expected)
        {
        }

        [[nodiscard]] inline constexpr auto size() const noexcept -> int_t
        {
            return Size;
        }

        [[nodiscard]] inline constexpr auto from() const &noexcept -> const point_t &
        {
            return From;
        }

        [[nodiscard]] inline constexpr auto tod() const &noexcept -> const point_t &
        {
            return Tod;
        }

        [[nodiscard]] inline constexpr auto expected() const noexcept -> long_int_t
        {
            return Expected;
        }

        [[nodiscard]] inline constexpr auto is_computed() const noexcept -> bool
        {
            return zero <= Expected;
        }

        inline constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::Numbers::Inner::check_knights(From, Size, Tod);

            const auto squared = 2LL * static_cast<long_int_t>(Size) * Size;
            require_between(not_computed, Expected, squared, "expected");
        }

        inline void print(std::ostream &str) const override
        {
            str << ", size " << Size;
            ::Standard::Algorithms::print(", from ", From, str);
            ::Standard::Algorithms::print(", to ", Tod, str);
            str << ", expected " << Expected;
        }

private:
        int_t Size;
        point_t From;
        point_t Tod;
        long_int_t Expected;
    };

    inline void add_random_test_cases(std::vector<test_case> &tests)
    {
        constexpr int_t min_size = Standard::Algorithms::Numbers::Inner::knights_min_size;
        constexpr int_t max_size = 10;

        Standard::Algorithms::Utilities::random_t<int_t> rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            static_assert(int_t{} < min_size && min_size < max_size);

            const auto size = rnd();
            const auto coord_max = static_cast<int_t>(size - 1);

            // No initialization wanted.
            // NOLINTNEXTLINE
            std::array<point_t, 2> points;

            for (auto &point : points)
            {
                point.first = rnd(0, coord_max);
                point.second = rnd(0, coord_max);
            }

            auto &from = points[0];
            auto &tod = points[1];

            if (from == tod || Standard::Algorithms::Numbers::Inner::is_banned(from, size, tod))
            {
                continue;
            }

            tests.emplace_back("Random" + std::to_string(att), size, std::move(from), std::move(tod), not_computed);
        }
    }
} // namespace Standard::Algorithms::Numbers::Tests
