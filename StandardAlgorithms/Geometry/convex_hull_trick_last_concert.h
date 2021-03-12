#pragma once
#include <utility>
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            //The last concert must be included - what is the min price to pay?
            //Prices are >= 0.
            //Ratios are in (0, 1).
            double convex_hull_trick_last_concert(
                // {price, ratio}
                const std::vector<std::pair<double, double>>& ticket_ratio);

            double convex_hull_trick_last_concert_slow(const std::vector<std::pair<double, double>>& ticket_ratio);

            namespace Tests
            {
                void convex_hull_trick_last_concert_tests();
            }
        }
    }
}
