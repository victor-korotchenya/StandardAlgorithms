#pragma once
#include"../Utilities/is_debug.h"
#include"../Utilities/require_utilities.h"
#include<algorithm>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Motivation: accessing a traditional matrix by an anti-diagonal is not cache-friendly.
    // A 3-by-8 matrix has 10 anti-diagonals:
    // 0  1` 2 3 4 5 6  7
    // 1` 2  3 4 5 6 7  8
    // 2  3  4 5 6 7 8` 9
    // {Row 2, col 6} is stored in cell [8, 1], where 8 is the anti-diagonal number, 1 - position.
    // {0, 1} in [1, 0].
    // {1, 0} in [1, 1].
    //
    // Another example, a 8-by-3 matrix has also 10 anti-diagonals:
    // 0 1 2
    // 1 2 3
    // 2 3 4
    // 3 4 5
    // 4 5 6
    // 5 6 7
    // 6 7 8'
    // 7 8 9
    // {6, 2} in [8, 0], where 8 is the anti-diagonal, 0 - position.
    template<class item_t, std::unsigned_integral length_t1 = std::size_t>
    requires(sizeof(length_t1) <= sizeof(std::size_t))
    struct matrix_antidiagonal final
    {
        using length_t = length_t1;

        constexpr matrix_antidiagonal(const length_t &rows, const length_t &columns)
            : Rows(Standard::Algorithms::require_positive(rows, "rows"))
            , Columns(Standard::Algorithms::require_positive(columns, "columns"))
            , Antidiagonals(rows - 1ZU + columns)
        {
            check_rock();
        }

        [[nodiscard]] inline constexpr auto to_antidiagonal_pos(
            const length_t &row, const length_t &column) const noexcept -> std::pair<length_t, length_t>
        {
            check_rock(row, column);

            auto antidiagonal = static_cast<length_t>(row + column);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(antidiagonal < Antidiagonals.size());
                assert(row <= antidiagonal && column <= antidiagonal);
            }

            auto position = std::min(row, static_cast<length_t>(Columns - 1ZU - column));

            return { antidiagonal, position };
        }

        [[nodiscard]] constexpr auto compute_antidiagonal_size(const length_t &antidiagonal) const noexcept -> length_t
        {
            check_rock();

            auto ant_size =
                std::min<std::size_t>({ antidiagonal + 1ZU, Antidiagonals.size() - antidiagonal, Rows, Columns });

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(
                    antidiagonal < Antidiagonals.size() && length_t{} < ant_size && ant_size <= Antidiagonals.size());
            }

            return static_cast<length_t>(ant_size);
        }

        constexpr void resize_antidiagonal(const length_t &antidiagonal)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                require_greater(Antidiagonals.size(), antidiagonal, "anti-diagonal resize");
            }

            const auto ant_size = compute_antidiagonal_size(antidiagonal);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                auto &anti_diagonal = Antidiagonals.at(antidiagonal);
                anti_diagonal.resize(ant_size);
            }
            else
            {
                auto &anti_diagonal = Antidiagonals[antidiagonal];
                anti_diagonal.resize(ant_size);
            }
        }

        [[nodiscard]] inline constexpr auto operator[] (
            const length_t &row, const length_t &column) const & -> const item_t &
        {
            const auto [antidiagonal, pos] = to_antidiagonal_pos(row, column);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                const auto &anti_diagonal = Antidiagonals.at(antidiagonal);
                assert(pos < anti_diagonal.size());

                const auto &cell = anti_diagonal.at(pos);
                return cell;
            }
            else
            {
                const auto &anti_diagonal = Antidiagonals[antidiagonal];
                const auto &cell = anti_diagonal[pos];
                return cell;
            }
        }

        [[nodiscard]] inline constexpr auto operator[] (const length_t &row, const length_t &column) & -> item_t &
        {
            const auto [antidiagonal, pos] = to_antidiagonal_pos(row, column);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                auto &anti_diagonal = Antidiagonals.at(antidiagonal);

                assert(pos < anti_diagonal.size());

                auto &cell = anti_diagonal.at(pos);
                return cell;
            }
            else
            {
                auto &anti_diagonal = Antidiagonals[antidiagonal];
                auto &cell = anti_diagonal[pos];
                return cell;
            }
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<std::vector<item_t>> &
        {
            check_rock();
            return Antidiagonals;
        }

        [[nodiscard]] constexpr auto data() &noexcept -> std::vector<std::vector<item_t>> &
        {
            check_rock();
            return Antidiagonals;
        }

        constexpr void zero_out()
        {
            check_rock();

            constexpr item_t zero{};

            for (auto &anti_diagonal : Antidiagonals)
            {
                std::fill(anti_diagonal.begin(), anti_diagonal.end(), zero);
            }
        }

private:
        inline constexpr void check_rock(
            [[maybe_unused]] const length_t &row = {}, [[maybe_unused]] const length_t &column = {}) const noexcept
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(
                    row < Rows && column < Columns && Rows <= Antidiagonals.size() && Columns <= Antidiagonals.size());

                assert(Rows - 1ZU + Columns == Antidiagonals.size() && !Antidiagonals.empty());
            }
        }

        length_t Rows;
        length_t Columns;
        std::vector<std::vector<item_t>> Antidiagonals;
    };
} // namespace Standard::Algorithms::Numbers
