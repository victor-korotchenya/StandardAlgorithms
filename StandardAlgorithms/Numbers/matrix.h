#pragma once
#include <array>
#include <cstring>
#include <utility>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Numbers
{
    template<typename number_t, unsigned rows, unsigned columns>
    class matrix final
    {
        static_assert(0 < rows && 0 < columns);

        number_t _data[rows][columns];

    public:
        using length_t = unsigned;

        matrix() = default;

        explicit matrix(const std::array<std::array<number_t, columns>, rows>& data)
        {
            std::copy(data.begin(), data.end(), _data);
        }

        //reference to array.
        explicit matrix(const number_t(&data)[rows][columns])
        {
            std::memcpy(_data, data, sizeof _data);
        }

        auto& get_data()
        {
            return _data;
        }

        const number_t& operator[](const std::pair<length_t, length_t>& index) const
        {
            return _data[index.first][index.second];
        }

        number_t& operator[](const std::pair<length_t, length_t>& index)
        {
            return _data[index.first][index.second];
        }

        auto& operator[](const length_t index)
        {
            return _data[index];
        }

        void zero_out()
        {
            std::memset(_data, 0, sizeof _data);
        }

        void identity()
        {
            static_assert(rows == columns, "The matrix must be squared.");

            zero_out();

            for (auto i = 0u; i < rows; ++i)
                _data[i][i] = 1;
        }

        bool operator== (const matrix& b) const
        {
            for (auto row = 0u; row < rows; ++row)
            {
                for (auto col = 0u; col < columns; ++col)
                {
                    if (!(_data[row][col] == b._data[row][col]))
                        return false;
                }
            }

            return true;
        }

        matrix& operator+= (const matrix& b)
        {
            for (auto row = 0u; row < rows; ++row)
            {
                for (auto col = 0u; col < columns; ++col)
                    _data[row][col] += b._data[row][col];
            }

            return *this;
        }

        //Use the '+=' whenever possible as '+' is slower.
        matrix operator+(const matrix& b)
        {
            auto result = *this;
            for (auto row = 0u; row < rows; ++row)
            {
                for (auto col = 0u; col < columns; ++col)
                    result._data[row][col] += b._data[row][col];
            }

            return result;
        }

        // todo: p2. Strassen, blocks 32 by 32, parallel?
        template<length_t columns2>
        matrix operator*(const matrix<number_t, columns, columns2>& b)
        {
            matrix<number_t, rows, columns2> result;
            result.zero_out();

            for (auto row = 0u; row < rows; row++)
            {
                const auto& d = _data[row];
                auto& r = result._data[row];

                for (auto k = 0u; k < columns; k++)
                {
                    const auto& dk = d[k];
                    const auto& bdk = b._data[k];

                    for (auto col = 0u; col < columns2; col++)
                    {
                        // CPU L3 cache misses are lower this way.
                        r[col] += dk * bdk[col];
                    }
                }
            }

            return result;
        }
    };

    // Raise the matrix to a positive power. Product, exponentiation.
    // Note that the source matrice is edited!
    template<typename number_t, unsigned size, typename power_t>
    void matrix_power(matrix<number_t, size, size>& source,
        power_t power,
        matrix<number_t, size, size>& result)
    {
        static_assert(0 < size);
        RequirePositive(power, "power");

        result.identity();
        do
        {
            if (power & 1)
            {
                result = result * source;
                --power;
            }
            else
            {
                source = source * source;
                power >>= 1;
            }
        } while (power);
    }
}