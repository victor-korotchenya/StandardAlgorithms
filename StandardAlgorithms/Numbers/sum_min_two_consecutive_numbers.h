#pragma once
#include"arithmetic.h"
#include"power_root.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Return the consecutive integers sum
    // from "from" to "tod" inclusively.
    // Sum 1..x is x*(x+1)/2.
    template<std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto sum_to_n(const int_t &from, const int_t &tod) -> int_t
    {
        if (from == tod)
        {
            return from;
        }

        if (tod < from) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Error in sum_to_n: from(" << from << ") cannot exceed to(" << tod << ").";
            throw_exception(str);
        }

        constexpr int_t zero{};
        constexpr int_t one = 1;

        int_t sum = (zero < (tod & one)) // Avoid an overflow.
            ? multiply_unsigned(add_unsigned(tod, one) >> one, tod)
            : multiply_unsigned(add_unsigned(tod, one), tod >> one);

        if (from <= one)
        {
            return sum;
        }

        const auto pre_sum = sum_to_n(one, from - one);
        assert(zero < pre_sum && pre_sum < sum);

        sum -= pre_sum;
        assert(zero < sum);

        return sum;
    }

    // Given a positive number n >= 3,
    // compute all pairs such that for each pair {b, e}, where (b < e),
    // the sum of the consecutive numbers [b, b+1, .., e] equals n.
    // E.g. n=15 has 3 such pairs: {1,5}, {4,6}, {7,8}.
    //  15 = 1+2+3+4+5 = 4+5+6 = 7+8.
    template<std::integral int_t>
    struct sum_min_two_consecutive_numbers final
    {
        sum_min_two_consecutive_numbers() = delete;

        using pair_t = std::pair<int_t, int_t>;
        using vec_pair_t = std::vector<pair_t>;

        [[nodiscard]] static constexpr auto slow(const int_t &numb) -> vec_pair_t
        {
            if (check_is_power_of_two(numb))
            {
                return {};
            }

            constexpr int_t one = 1;
            constexpr int_t two = 2;

            const int_t low_max = (numb + one) >> one;

            vec_pair_t result{};
            auto low = one;
            auto high = estimate_number(numb);
            assert(!(high < low));

            auto sum = sum_to_n(low, high);

            do
            {
                if (sum == numb)
                {
                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        require_greater(high, low, "Slow high vs low");
                    }

                    result.emplace_back(low, high);

                    {// The set size is decreased by at least 1.
                        const auto subtr = (low << one) + one;

                        assert(!(sum < subtr));

                        sum -= subtr;
                    }

                    low += two;
                    sum += ++high;
                }
                else if (sum < numb)
                {
                    sum += ++high;
                }
                else
                {// numb < sum
                    do
                    {
                        assert(!(sum < low));

                        sum -= low;
                        ++low;
                    } while (numb < sum);
                }
            } while (low < low_max);

            return result;
        }

        // For numbers up to 1E9, it is actually slower.
        [[nodiscard]] static constexpr auto over_optimized(const int_t &numb) -> vec_pair_t
        {
            if (check_is_power_of_two(numb))
            {
                return {};
            }

            constexpr int_t zero{};
            constexpr int_t one = 1;
            constexpr int_t two = 2;

            const int_t low_max = (numb + one) >> one;

            vec_pair_t result{};
            auto low = one;
            auto high = estimate_number(numb);

            // Eventually, the low starts increasing by 1 or 2.
            auto shall_go_fast = true;
            auto sum = sum_to_n(low, high);

            do
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    const auto sum_temp = sum_to_n(low, high);

                    require_equal(sum_temp, "sum [" + std::to_string(low) + ".." + std::to_string(high) + "]", sum);
                }

                if (sum == numb)
                {
                    result.emplace_back(low, high);

                    {// The set size is decreased by at least 1.
                        const auto subtr = (low << one) + one;

                        assert(!(sum < subtr));

                        sum -= subtr;
                    }

                    low += two;
                    sum += ++high;
                }
                else if (sum < numb)
                {// The upper limit increases by one.
                    sum += ++high;
                }
                else
                {
                    if (shall_go_fast && low < (sum - numb))
                    {
                        // Let n = 4095 = Sum[1..90].
                        // Having subtracted(low + low + 1) and added new high,
                        //  the sum becomes 4095 - 1 - 2 + 91 = 4183; low = 3, high = 91.
                        // So, the sum must go down by delta=88 by increasing the low from 3 to 14.
                        //  Sum(1..2) = 3; Sum(1..3) = 6; Sum(1..4) = 10; Sum(1..13) = 91.
                        // Desired: delta = sum(1..x) - sum(1..low-1),
                        // sum(1..x) = delta + sum(1..low-1).
                        const auto sum_low_minus_one = sum_to_n(zero, low - one);
                        const auto sum_estimated = (sum - numb) + sum_low_minus_one;
                        const auto xxx = estimate_number(sum_estimated) + one;

                        constexpr int_t delta_limit = 100;

                        shall_go_fast = delta_limit + low < xxx;

                        if (low < xxx)
                        {
                            const auto sum_x = sum_to_n(one, xxx);

                            if constexpr (::Standard::Algorithms::is_debug)
                            {
                                require_greater(sum_x, sum_low_minus_one + xxx, "sumX");
                            }

                            {
                                const auto subtr = sum_x - sum_low_minus_one - xxx;

                                assert(!(sum < subtr));

                                sum -= subtr;
                            }

                            low = xxx;

                            continue;
                        }
                    } // shallGoFast..

                    do
                    {
                        assert(!(sum < low));

                        sum -= low;
                        ++low;
                    } while (numb < sum);
                }
            } while (low < low_max);

            return result;
        }

private:
        [[nodiscard]] static constexpr auto check_is_power_of_two(const int_t &numb) -> bool
        {
            {
                constexpr int_t min_value = 3;

                require_less_equal(min_value, numb, "number");
            }

            // Sum from m to n inclusively, where m < n.
            //  Let h = m - 1, so n = h + k, where 1 < k; 0 <= h.
            //
            // Sum= n(n+1)/2 - h(h+1)/2 = 1/2*[(h + k)* (h + k + 1) - h(h + 1)] =
            //  = 1/2*[(h*h + h*k + h) + (k*h + k*k + k) - h*h - h] =
            //  = 1/2*[(h*k) + (k*h + k*k + k)] =
            //  = [2*h + k + 1]*k/2.
            // If Sum = 2**z, then
            //   k must be k = 2**x, 0 < x;
            //   and (2*h + k + 1) = 2**y.
            // It is impossible as (2*h + k + 1) is an odd number.
            auto result = is_power_of_two(numb);
            return result;
        }

        [[nodiscard]] static constexpr auto estimate_number(const int_t &sum) -> int_t
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                require_positive(sum, "estimate number by sum");
            }

            // sum 1..x is x*(x+1)/2, then 0 = x*x + x - 2*sum.
            //  d = sqrt(1 + 8*sum). x = (-1 + d)/2
            constexpr int_t one = 1;
            constexpr int_t eight = 8;

            const auto discriminant = add_unsigned<int_t>(multiply_unsigned<int_t>(eight, sum), one);

            // At least 3 when sum=1.
            // At least 5 when sum=3.
            const auto crimi_root = int_sqrt<int_t>(discriminant);

            auto result = static_cast<int_t>((crimi_root - one) >> one);

            assert(3 <= crimi_root && int_t{} < result);

            return result;
        }
    };
} // namespace Standard::Algorithms::Numbers
