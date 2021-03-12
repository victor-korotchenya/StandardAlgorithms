#pragma once
#include"../Utilities/is_noexcept.h"
#include"../Utilities/require_utilities.h"
#include<array>
#include<cstring>

namespace Standard::Algorithms::Numbers
{
    template<class int_t, std::uint32_t rows, std::uint32_t columns>
    requires(0U < rows && 0U < columns)
    struct matrix final
    {
        using length_t = std::uint32_t;

        static constexpr bool is_plain = std::is_arithmetic_v<int_t>;

        [[nodiscard]] constexpr matrix() = default;

        [[nodiscard]] constexpr explicit matrix(const std::array<std::array<int_t, columns>, rows> &source)
        {
            std::copy(source.cbegin(), source.cend(), Data.begin());
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::array<std::array<int_t, columns>, rows> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto data() &noexcept -> std::array<std::array<int_t, columns>, rows> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto operator[] (const length_t &row, const length_t &col) const & -> const int_t &
        {
            return Data.at(row).at(col);
        }

        [[nodiscard]] constexpr auto operator[] (const length_t &row, const length_t &col) & -> int_t &
        {
            return Data.at(row).at(col);
        }

        [[nodiscard]] constexpr auto operator[] (
            const std::pair<length_t, length_t> &row_column) const & -> const int_t &
        {
            return Data.at(row_column.first).at(row_column.second);
        }

        [[nodiscard]] constexpr auto operator[] (const std::pair<length_t, length_t> &row_column) & -> int_t &
        {
            return Data.at(row_column.first).at(row_column.second);
        }

        [[nodiscard]] constexpr auto operator[] (const length_t &row) const & -> const std::array<int_t, columns> &
        {
            return Data.at(row);
        }

        [[nodiscard]] constexpr auto operator[] (const length_t &row) & -> std::array<int_t, columns> &
        {
            return Data.at(row);
        }

        constexpr void zero_out()
        {
            if constexpr (is_plain)
            {
                std::memset(Data, 0, sizeof(Data));
            }
            else
            {
                constexpr int_t zero{};

                for (length_t row{}; row < rows; ++row)
                {
                    for (length_t col{}; col < columns; ++col)
                    {
                        Data[row][col] = zero;
                    }
                }
            }
        }

        constexpr void identity()
        {
            static_assert(rows == columns, "The matrix must be squared.");

            zero_out();

            for (length_t row{}; row < rows; ++row)
            {
                constexpr int_t one{ 1 };

                Data[row][row] = one;
            }
        }

        [[nodiscard]] constexpr auto operator== (const matrix &other) const noexcept -> bool
        {
            for (length_t row{}; row < rows; ++row)
            {
                for (length_t col{}; col < columns; ++col)
                {
                    if (!(Data[row][col] == other.Data[row][col]))
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        constexpr auto operator+= (const matrix &other) &noexcept(is_noexcept_add<int_t>) -> matrix &
        {// todo(p4): strong exception guarantee.
            for (length_t row{}; row < rows; ++row)
            {
                // Let's hope the compiler will hoist the common subexpressions e.g. Data[row], other.Data[row].
                for (length_t col{}; col < columns; ++col)
                {
                    Data[row][col] += other.Data[row][col];
                }
            }

            return *this;
        }

        [[nodiscard]] constexpr auto operator+ (const matrix &other) &&noexcept(is_noexcept_add<int_t>) -> matrix
        {
            for (length_t row{}; row < rows; ++row)
            {
                for (length_t col{}; col < columns; ++col)
                {
                    Data[row][col] += other.Data[row][col];
                }
            }

            return std::move(*this);
        }

        // Please use '+=' whenever possible as '+' is slower for L-values.
        [[nodiscard]] constexpr auto operator+ (const matrix &other) & -> matrix
        {
            auto result = *this;

            for (length_t row{}; row < rows; ++row)
            {
                for (length_t col{}; col < columns; ++col)
                {
                    result.Data[row][col] += other.Data[row][col];
                }
            }

            return result;
        }

        // todo(p3): Volker Strassen O(n**log(7)), blocks 32 by 32, parallel?
        // todo(p2): Karatsuba (div & con) in O(n**log2(3)) = O(n**1.6)
        //   A[n] = a1[n/2] + a0[n/2]
        //   B[n] = b1[n/2] + b0[n/2]
        //   A*B = a1*b1 + (a1*b0 + b1*a0) + a0*b0
        //    where (a1*b0 + b1*a0) = (a1 + a0)*(b1 + b0) - a1*b1 - a0*b0
        //    so 3, not 4, mults can be used.
        template<length_t columns2>
        [[nodiscard]] constexpr auto operator* (const matrix<int_t, columns, columns2> &other) -> matrix
        {
            matrix<int_t, rows, columns2> result;

            for (length_t row{}; row < rows; ++row)
            {
                const auto &src = Data[row];
                auto &dest = result.Data[row];

                for (length_t mid{}; mid < columns; ++mid)
                {
                    const auto &src_coef = src[mid];
                    const auto &ot_vec = other.Data[mid];

                    for (length_t col{}; col < columns2; ++col)
                    {
                        // CPU L3 cache misses are lower this way.
                        dest[col] += src_coef * ot_vec[col];
                    }
                }
            }

            return result;
        }

private:
        std::array<std::array<int_t, columns>, rows> Data{};
    };

    // Raise the matrix to a non-negative power
    // using product, exponentiation.
    template<class int_t, std::uint32_t size, std::integral power_t>
    requires(0U < size)
    constexpr void matrix_power(
        // The source matrix is edited.
        matrix<int_t, size, size> &source, power_t power, matrix<int_t, size, size> &result)
    {
        result.identity();

        constexpr power_t zero_power{};

        if (power == zero_power)
        {
            return;
        }

        if constexpr (std::is_signed_v<power_t>)
        {
            require_positive(power, "power");
        }

        constexpr power_t one_power = 1;

        do
        {
            if ((power & one_power) != zero_power)
            {
                result = result * source;
                --power;
            }
            else
            {
                source = source * source;
                power >>= 1U;
            }
        } while (power != zero_power);
    }
} // namespace Standard::Algorithms::Numbers
