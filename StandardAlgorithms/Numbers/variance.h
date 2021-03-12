#pragma once
// "variance.h"
#include"../Utilities/require_utilities.h"
#include"power_root.h" // sqrt
#include<numeric>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    inline constexpr auto correlation_coefficient_max = 1.0;

    // Covariance, correlation coefficient.
    // It is slow and not stable.
    template<std::floating_point floating_t, class int_t, class int_t_2>
    requires(std::is_arithmetic_v<int_t> && std::convertible_to<int_t, floating_t> && //
        std::is_arithmetic_v<int_t_2> && std::convertible_to<int_t_2, floating_t>)
    [[nodiscard]] constexpr auto naive_covariance_correlation(
        const std::vector<int_t> &input, const std::vector<int_t_2> &input_2) -> std::pair<floating_t, floating_t>
    {
        const auto size = require_positive(static_cast<std::size_t>(input.size()), "size");
        {
            const auto size_2 = static_cast<std::size_t>(input_2.size());
            require_equal(size, "second input size", size_2);
        }

        if (1U == size)
        {
            return { 0, 1 };
        }

        const auto size_f = require_positive(static_cast<floating_t>(size), "floating size");

        const floating_t mean = std::accumulate(input.begin(), input.end(), floating_t{}) / size_f;
        const floating_t mean_2 = std::accumulate(input_2.begin(), input_2.end(), floating_t{}) / size_f;

        floating_t vari{};
        floating_t vari_2{};
        floating_t covariance{};

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &val = input[index];
            {
                const auto dif = static_cast<floating_t>(val) - mean;
                const auto squar = dif * dif;
                vari += squar;
            }

            const auto &val_2 = input_2[index];
            {
                const auto dif = static_cast<floating_t>(val_2) - mean_2;
                const auto squar = dif * dif;
                vari_2 += squar;
            }
            {
                const auto co_delta = (static_cast<floating_t>(val) - mean) * (static_cast<floating_t>(val_2) - mean_2);

                covariance += co_delta;
            }
        }

        vari /= size_f;
        vari_2 /= size_f;
        covariance /= size_f;

        const auto standard_deviation = sqrt<floating_t>(vari);
        const auto standard_deviation_2 = sqrt<floating_t>(vari_2);

        if (const auto is_zero = floating_t{} == standard_deviation, is_zero_2 = floating_t{} == standard_deviation_2;
            is_zero || is_zero_2)
        {
            return { 0, is_zero && is_zero_2 ? 1 : 0 };
        }

        floating_t correlation_coeff = covariance / standard_deviation / standard_deviation_2;

        {
            constexpr floating_t minus_one{ -1 };
            constexpr floating_t one{ 1 };

            if (correlation_coeff < minus_one)
            {
                correlation_coeff = minus_one;
            }
            else if (one < correlation_coeff)
            {
                correlation_coeff = one;
            }
        }

        return { covariance, correlation_coeff };
    }
} // namespace Standard::Algorithms::Numbers
