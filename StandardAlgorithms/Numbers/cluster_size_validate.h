#pragma once
#include"../Utilities/zu_string.h"
#include<cstddef>
#include<stdexcept>

namespace Standard::Algorithms::Numbers
{
    inline constexpr void cluster_size_validate(const std::size_t clusters, const std::size_t points_size)
    {
        if (clusters < 2U) [[unlikely]]
        {
            auto err =
                "The clusters (" + ::Standard::Algorithms::Utilities::zu_string(clusters) + ") must be at least 2.";

            throw std::invalid_argument(err);
        }

        if (points_size < clusters) [[unlikely]]
        {
            auto err = "The points size (" + ::Standard::Algorithms::Utilities::zu_string(points_size) +
                ") must be at least clusters " + ::Standard::Algorithms::Utilities::zu_string(clusters) + ".";

            throw std::invalid_argument(err);
        }
    }
} // namespace Standard::Algorithms::Numbers
