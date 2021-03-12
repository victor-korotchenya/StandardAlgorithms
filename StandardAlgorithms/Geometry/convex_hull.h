#pragma once
#include"../Utilities/require_utilities.h"
#include"point_utilities.h"
#include<algorithm>
#include<vector>

namespace Standard::Algorithms::Geometry
{
    template<class long_int_t, class point_t, class coll_t>
    requires(sizeof(typename point_t::distance_t) <= sizeof(long_int_t))
    constexpr void convex_hull_graham_scan(
        // The source points might be edited.
        std::pair<coll_t &, coll_t &> &source_result)
    {
        auto &source = source_result.first;
        require_positive(source.size(), "source size");

        const auto min_point = *std::min_element(source.cbegin(), source.cend());

        const auto comparer = [min_point](const point_t &one, const point_t &two) -> bool
        {
            const auto angle = calc_angle<long_int_t, point_t>(min_point, one, two);
            if (angle != 0)
            {
                return 0 < angle;
            }

            const auto bim = min_point - one;
            const auto cim = min_point - two;
            const auto dist_bim = dist2<long_int_t>(bim);
            const auto dist_cim = dist2<long_int_t>(cim);
            return dist_bim < dist_cim;
        };

        std::sort(source.begin(), source.end(), comparer);

        auto &result = source_result.second;
        result.clear();

        for (const auto &pnt : source)
        {
            if (!result.empty() && result.back() == pnt)
            {
                continue;
            }

            while (1U < result.size() &&
                calc_angle<long_int_t>(result[result.size() - 2U], result[result.size() - 1U], pnt) <= 0)
            {
                result.pop_back();
            }

            result.push_back(pnt);
        }
    }

    template<class long_int_t, class point_t>
    constexpr void convex_hull_low_upper(
        // The source points are editable!
        std::vector<point_t> &source, std::vector<point_t> &ret)
    {
        require_positive(source.size(), "source size");

        std::sort(source.begin(), source.end());
        source.resize(std::unique(source.begin(), source.end()) - source.begin());

        if (source.size() <= 2U)
        {
            ret = std::move(source);
            return;
        }

        ret.clear();

        for (const auto &pnt : source)
        {
            while (1U < ret.size() && cross_product3<long_int_t>(ret[ret.size() - 2U], ret[ret.size() - 1U], pnt) <= 0)
            {
                ret.pop_back();
            }

            ret.push_back(pnt);
        }

        const auto size = ret.size();
        auto ite = std::prev(source.cend());
        do
        {
            const auto &pnt = *--ite;

            while (
                size < ret.size() && cross_product3<long_int_t>(ret[ret.size() - 2U], ret[ret.size() - 1U], pnt) <= 0)
            {
                ret.pop_back();
            }

            ret.push_back(pnt);
        } while (source.cbegin() != ite);

        ret.pop_back();
    }
} // namespace Standard::Algorithms::Geometry
