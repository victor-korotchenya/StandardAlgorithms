#pragma once
// "random_matrix.h"
#include"random_generator.h"
#include<algorithm>

namespace Standard::Algorithms::Utilities
{
    constexpr auto default_floating_max = 100;
    static_assert(0 < default_floating_max);

    template<class int_t>
    requires(std::is_arithmetic_v<int_t>)
    [[nodiscard]] constexpr auto random_matrix(const bool add_extra_column = false, const std::size_t min_size = 1,
        const std::size_t max_size = 10, const int_t &min_value = -default_floating_max,
        const int_t &max_value = default_floating_max) -> std::vector<std::vector<int_t>>
    {
        assert(min_size <= max_size && min_value <= max_value);

        const auto size = random_t<std::size_t>(min_size, max_size)();

        std::vector<std::vector<int_t>> matrix(size, //
            std::vector<int_t>(size + (add_extra_column ? 1ZU : 0ZU), int_t{}));

        random_t<int_t> rnd(min_value, max_value);

        const auto gen = [&rnd]
        {
            return rnd();
        };

        for (auto &linie : matrix)
        {
            std::generate(linie.begin(), linie.end(), gen);
        }

        return matrix;
    }
} // namespace Standard::Algorithms::Utilities
