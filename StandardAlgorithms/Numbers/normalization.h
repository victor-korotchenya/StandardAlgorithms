#pragma once
// "normalization.h"
#include"../Utilities/require_utilities.h"
#include"power_root.h" // sqrt
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given numbers x, rescale xi = (xi - min) / (max - min).
    // E.g. given {1, 2, 4, 5, 6, 3} with min = 1, max = 6,
    // return {0, 0.2, 0.6, 0.8, 1, 0.4}.
    template<class int_t, std::floating_point floating_t>
    requires(std::is_arithmetic_v<int_t> && std::convertible_to<int_t, floating_t>)
    constexpr void min_max_normalization(const std::vector<int_t> &input, std::vector<floating_t> &output)
    {
        if constexpr (std::is_same_v<std::remove_cvref_t<int_t>, std::remove_cvref_t<floating_t>>)
        {
            static_assert(&input != &output);
        }

        constexpr int_t zero{};
        constexpr int_t one{ 1 };
        static_assert(zero < one);

        const auto size = input.size();
        output.resize(size, zero);

        if (0U == size)
        {
            return;
        }

        const auto [min_it, max_it] = std::minmax_element(input.begin(), input.cend());

        if (*min_it == *max_it)
        {
            const auto &valu = zero == *min_it ? zero : one;
            output.assign(size, valu);
            return;
        }

        constexpr floating_t f_zero{};
        constexpr floating_t f_one{ 1 };
        static_assert(f_zero < f_one);

        const auto f_min = static_cast<floating_t>(*min_it);
        const auto f_max = static_cast<floating_t>(*max_it);
        const auto divi = require_positive(static_cast<floating_t>(f_max - f_min), "max-min difference");

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &inp = input[index];
            auto &outp = output[index];

            outp = static_cast<floating_t>((static_cast<floating_t>(inp) - f_min) / divi);

            assert(f_zero <= outp && outp <= f_one);
        }
    }

    // 2 std::accumulate calls waste CPU cache - do both in 1.
    // todo(p4): also compute std::minmax_element.
    template<class sum_t>
    requires(std::is_arithmetic_v<sum_t>)
    [[nodiscard]] constexpr auto sum_and_squared_sum(const auto &input) -> std::pair<sum_t, sum_t>
    {
        using int_t = std::remove_cvref_t<decltype(*(input.begin()))>;

        static_assert(std::is_arithmetic_v<int_t> && std::convertible_to<int_t, sum_t>);

        sum_t sum{};
        sum_t squared_sum{};

        for (const auto &datum : input)
        {
            const auto &f_dat = static_cast<sum_t>(datum);
            sum += f_dat;
            squared_sum += static_cast<sum_t>(f_dat * f_dat);

            assert(sum_t{} <= squared_sum);
        }

        return { sum, squared_sum };
    }

    // Given numbers x, set xi = (xi - mean) / st.deviation,
    // where the mean is average, and the deviation is standard.
    template<class int_t, std::floating_point floating_t>
    requires(std::is_arithmetic_v<int_t> && std::convertible_to<int_t, floating_t>)
    constexpr void score_standardize(const std::vector<int_t> &input, std::vector<floating_t> &output)
    {
        if constexpr (std::is_same_v<std::remove_cvref_t<int_t>, std::remove_cvref_t<floating_t>>)
        {
            static_assert(&input != &output);
        }

        constexpr int_t zero{};
        constexpr int_t one{ 1 };
        static_assert(zero < one);

        const auto size = input.size();
        output.resize(size, zero);

        if (0U == size)
        {
            return;
        }

        const auto [min_it, max_it] = std::minmax_element(input.begin(), input.cend());

        if (*min_it == *max_it)
        {
            const auto &valu = zero == *min_it ? zero : one;
            output.assign(size, valu);
            return;
        }

        const auto f_size = require_positive(static_cast<floating_t>(size), "floating size");

        const auto [sum, squared_sum] = sum_and_squared_sum<floating_t>(input);
        const auto mean = static_cast<floating_t>(sum / f_size);

        const auto variance = require_positive(static_cast<floating_t>(squared_sum / f_size - mean * mean), "variance");
        const auto standard_deviation = require_positive(sqrt<floating_t>(variance), "standard deviation");

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &inp = input[index];
            auto &outp = output[index];

            outp = static_cast<floating_t>((static_cast<floating_t>(inp) - mean) / standard_deviation);
        }
    }
} // namespace Standard::Algorithms::Numbers
