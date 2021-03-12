#pragma once

#include <algorithm>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
#include "point_utilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Geometry
        {
            template<class long_int_t, class point_t>
            void convex_hull_graham_scan(
                // The source points might be edited!
                std::vector<point_t>& source,
                std::vector<point_t>& result)
            {
                static_assert(sizeof(long_int_t) >= sizeof(point_t::distance_t));

                RequirePositive(source.size(), "size");

                const point_t min_point = *std::min_element(source.begin(), source.end());

                const auto compare = [min_point](const point_t& b, const point_t& c) -> bool
                {
                    const auto angle = calc_angle<long_int_t, point_t>(min_point, b, c);
                    if (angle != 0)
                        return angle > 0;

                    const auto bm = b - min_point, cm = c - min_point;
                    const auto dist_bm = dist2<long_int_t>(bm), dist_cm = dist2<long_int_t>(cm);
                    return dist_bm < dist_cm;
                };

                std::sort(source.begin(), source.end(), compare);

                result.clear();
                for (const auto& p : source)
                {
                    while (result.size() >= 2 &&
                        calc_angle<long_int_t>(result[result.size() - 2], result[result.size() - 1], p) <= 0)
                        result.pop_back();

                    if (result.size() && result.back() == p)
                        result.pop_back();

                    result.push_back(p);
                }
            }

            template<class long_int_t, class point_t>
            void convex_hull_low_upper(
                // The source points are editable!
                std::vector<point_t>& source,
                std::vector<point_t>& ret)
            {
                RequirePositive(source.size(), "size");

                std::sort(source.begin(), source.end());
                source.resize(std::unique(source.begin(), source.end()) - source.begin());

                if (source.size() <= 2)
                {
                    ret = std::move(source);
                    return;
                }

                ret.clear();
                for (const auto& p : source)
                {
                    while (ret.size() >= 2 &&
                        cross_product3<long_int_t>(ret[ret.size() - 2], ret[ret.size() - 1], p) <= 0)
                        ret.pop_back();

                    ret.push_back(p);
                }

                const auto s = ret.size();
                auto it = std::prev(source.cend());
                do
                {
                    const auto& p = *--it;
                    while (ret.size() > s &&
                        cross_product3<long_int_t>(ret[ret.size() - 2], ret[ret.size() - 1], p) <= 0)
                        ret.pop_back();

                    ret.push_back(p);
                } while (source.cbegin() != it);

                ret.pop_back();
            }
        }
    }
}