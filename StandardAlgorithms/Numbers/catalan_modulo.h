#pragma once
#include"../Utilities/require_utilities.h"
#include"prime_number_utility.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    template<std::unsigned_integral int_t = std::uint32_t, std::unsigned_integral long_int_t = std::uint64_t>
    requires((sizeof(int_t) << 1U) <= std::min(sizeof(long_int_t), sizeof(std::size_t))) // Avoid an overflow.
    struct catalan_modulo final
    {
        // The "maxValue" argument must be positive.
        // The "modulus" argument must be a prime number.
        constexpr catalan_modulo(const int_t &max_value, const int_t &modulus)
            : Data(calculate_size(max_value, modulus))
        {
            compute(static_cast<long_int_t>(modulus));
        }

        [[nodiscard]] constexpr auto get(const int_t &value) const & -> const int_t &
        {
            check_value(max_value(), value);

            const auto &result = Data.at(value);
            return result;
        }

private:
        template<class annoying_clan>
        [[nodiscard]] static constexpr auto calculate_size(const int_t &max_value, const annoying_clan &modulus)
            -> std::size_t
        {
            require_positive(max_value, "Catalan modulo maxValue");

            if (modulus < 2U || !is_prime_simple<int_t>(modulus))
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The modulus (" << modulus << ") in Catalan modulo must be a prime number.";

                throw_exception(str);
            }

            auto result = require_positive(static_cast<std::size_t>(max_value) + 1U, "Catalan modulo calculated size");

            return result;
        }

        static constexpr void check_value(const int_t &max_value, const int_t &value)
        {
            if (max_value < static_cast<std::size_t>(value))
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The value " << value << " in Catalan modulo must be between 0 and " << max_value
                    << " inclusive.";

                throw_exception(str);
            }
        }

        [[nodiscard]] constexpr auto max_value() const noexcept -> int_t
        {
            auto result = static_cast<int_t>(Data.size() - 1U);
            return result;
        }

        constexpr void compute(const long_int_t &modulus)
        {
            Data.at(0) = 1;

            const auto max_value1 = max_value();

            // Catalan(n) = C(2n, n) / (n+1)
            //
            // [1 2 .. n] * [(n+1) ..  2n]
            // [1 2 .. n] * [1 2 .. (n+1)]
            //
            // Another formula:
            // Catalan(n + 1) = SUM (Catalan(i) * Catalan(n - i) , i = [0..n]).

            for (int_t nnn{}; nnn < max_value1; ++nnn)
            {
                long_int_t sum{};

                for (int_t ind_2{}; ind_2 <= nnn; ++ind_2)
                {
                    const auto prod =
                        static_cast<long_int_t>(Data[ind_2]) * static_cast<long_int_t>(Data[nnn - ind_2]) % modulus;

                    sum += static_cast<long_int_t>(prod);

                    if (modulus <= sum)
                    {
                        sum -= modulus;
                    }

                    assert(sum < modulus);
                }

                Data[nnn + 1LLU] = static_cast<int_t>(sum);
            }
        }

        std::vector<int_t> Data;
    };
} // namespace Standard::Algorithms::Numbers
