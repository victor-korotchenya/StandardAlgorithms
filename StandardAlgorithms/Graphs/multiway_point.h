#pragma once
#include<cstdint>

namespace Standard::Algorithms::Trees
{
    template<std::int32_t dimensions, class point_t, class limits_t>
    requires(0 < dimensions)
    [[nodiscard]] constexpr auto is_between(const point_t &point, const limits_t &limits) -> bool
    {
        const auto &low = limits.first;
        const auto &high = limits.second;

        for (std::int32_t dim{}; dim < dimensions; ++dim)
        {
            const auto &coord = point[dim];

            if (coord < low[dim] || high[dim] < coord)
            {
                return false;
            }
        }

        return true;
    }
} // namespace Standard::Algorithms::Trees
