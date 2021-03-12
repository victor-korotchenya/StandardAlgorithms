#pragma once
#include"../Utilities/require_utilities.h"

namespace Standard::Algorithms::Numbers
{
    template<std::floating_point floating_t,
        // std::fabs
        class absolute_value_t1 = floating_t (*)(const floating_t &)>
    struct number_comparer final
    {
        using absolute_value_t = absolute_value_t1;

        constexpr number_comparer(const floating_t &epsilon, absolute_value_t absolute_value)
            : Epsilon(require_positive(epsilon, "epsilon"))
            , Absolute_value(absolute_value)
        {
            throw_if_null_skip_non_pointer("Absolute value function", absolute_value);
        }

        [[nodiscard]] constexpr auto are_equal(const floating_t &one, const floating_t &two) const -> bool
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                require_positive(Epsilon, "Epsilon");

                throw_if_null_skip_non_pointer("Absolute value function inside the comparison", Absolute_value);
            }

            if (one == two)
            {// Note that (0 == (infinity - infinity)) is false.
                return true;
            }

            [[maybe_unused]] constexpr floating_t zero{};

            const auto diff = two - one;
            assert(zero != diff);

            const auto abs_diff = Absolute_value(diff);
            auto result = abs_diff <= Epsilon;
            return result;

            // It does not work to compare 0 and a small number, but it should have been!
            //
            // const auto abs_one = Absolute_value(one);
            // const auto abs_two = Absolute_value(two);
            // const auto max_value = std::max(abs_one, abs_two);
            // auto prod = max_value * Epsilon;
            // auto result1 = !(prod < abs_diff);
            // return result1;

            // 1000000000 and 1000000001 are equal which is bad.
            // const auto sum = abs_one + abs_two;
            // (0 < infinity) is false.
            // assert(!(abs_one < zero) && !(abs_two < zero) && !(abs_diff < zero) && !(sum < zero));
            //
            // if (const auto div = abs_diff / sum; div <= Epsilon)
            // {
            //    return true;
            // }
            //
            // auto result = abs_one <= Epsilon && abs_two <= Epsilon;
            // return result;
        }

private:
        floating_t Epsilon;
        absolute_value_t Absolute_value;
    };
} // namespace Standard::Algorithms::Numbers
