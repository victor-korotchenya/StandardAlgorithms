#pragma once
#include <type_traits>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/StreamUtilities.h"
#include "prime_number_utility.h"

namespace Privet::Algorithms::Numbers
{
    template<typename Number = unsigned,
        typename LongNumber = unsigned long long>
        class Catalan_modulo final
    {
        static_assert(
            std::is_unsigned<Number>::value
            && std::is_unsigned<LongNumber>::value,
            "Number and LongNumber must be both unsigned.");

        static_assert(//Avoid an overflow.
            (sizeof(Number) << 1) <= sizeof(LongNumber),
            "Expect that (sizeof(Number) << 1) <= sizeof(LongNumber).");

        static_assert(//Again avoid an overflow.
            (sizeof(Number) << 1) <= sizeof(size_t),
            "Expect that (sizeof(Number) << 1) <= sizeof(size_t).");

        static_assert(std::is_integral<
            Number>::value, "The Number must be an integral type.");

        static_assert(std::is_integral<
            LongNumber>::value, "The LongNumber must be an integral type.");

        std::vector<Number> _Data;

    public:
        //The "maxValue" argument must be positive.
        //The "modulus" argument must be a prime number.
        explicit Catalan_modulo(const Number maxValue, const Number modulus)
            : _Data(calculate_size(maxValue, modulus))
        {
            compute(static_cast<LongNumber>(modulus));
        }

        Number get(const Number value) const
        {
            check_value(get_max_value(), value);

            const auto& result = _Data[value];
            return result;
        }

    private:
        static size_t calculate_size(const Number maxValue, const Number modulus)
        {
            RequirePositive<Number>(maxValue, "Catalan_modulo maxValue");

            if (modulus < 2 || !is_prime_simple<Number>(modulus))
            {
                std::ostringstream ss;
                ss << "The modulus (" << modulus
                    << ") in Catalan_modulo must be a prime number, at least 2.";
                StreamUtilities::throw_exception(ss);
            }

            const auto result = static_cast<size_t>(maxValue)
                + static_cast<size_t>(1);
            return result;
        }

        static inline void check_value(const Number maxValue, const Number value)
        {
            const auto isOk = 0 <= value
                && size_t(value) <= maxValue;
            if (!isOk)
            {
                std::ostringstream ss;
                ss << "The value (" << value
                    << ") in Catalan_modulo must be between 0 and "
                    << maxValue << ".";
                StreamUtilities::throw_exception(ss);
            }
        }

        Number inline get_max_value() const
        {
            const auto result = static_cast<Number>(_Data.size() - 1);
            return result;
        }

        void compute(const LongNumber modulus)
        {
            _Data[0] = 1;

            const auto maxValue = get_max_value();

            //Catalan(n) = C(2n, n) / (n+1)
            //
            // [1 2 .. n] * [(n+1) ..  2n]
            // [1 2 .. n] * [1 2 .. (n+1)]

            //Another formula:
            //Catalan(n + 1) = SUM (Catalan(i) * Catalan(n - i) , i = [0..n]).

            for (Number n = 0; n < maxValue; ++n)
            {
                LongNumber sum = 0;

                for (Number i = 0; i <= n; ++i)
                {
                    const LongNumber prod = static_cast<LongNumber>(_Data[i])
                        * static_cast<LongNumber>(_Data[n - i]);

                    sum = (sum + (prod % modulus)) % modulus;
                }

                _Data[n + 1ll] = static_cast<Number>(sum);
            }
        }
    };
}