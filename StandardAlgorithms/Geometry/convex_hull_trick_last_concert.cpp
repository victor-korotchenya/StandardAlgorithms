#include <algorithm>
#include <cassert>
#include <set>

#include "convex_hull_trick_last_concert.h"
#include "../Utilities/ExceptionUtilities.h"
using namespace std;

namespace
{
    using pt = pair<double, double>;

    pt find_best_line(
        const set<pt>& lines// {coef, added value}
        , const double price)
    {
        assert(0 < lines.size());
        auto result = *lines.begin();
        auto best_price = result.first * price + result.second;
        for (const auto& line : lines)
        {
            const auto cand = line.first * price + line.second;
            if (cand < best_price)
            {
                best_price = cand;
                result = line;
            }
        }

        return result;
    }

    void add_and_remove_useless_lines(
        set<pt>& lines// {coef, added value}
        , const pt& line)
    {
        assert(0 < lines.size());
        auto it = lines.lower_bound(line);
        if (lines.end() == it)
            --it;

        const auto& prev = *it;
        if (prev.first == line.first)
        {
            if (prev.second <= line.second)
                return;
        }

        //todo: p0. finish.
        lines.insert(line);
    }
}

double Privet::Algorithms::Geometry::convex_hull_trick_last_concert_slow(
    const vector<pt>& ticket_ratio)
{
    RequirePositive(ticket_ratio.size(), "ticket_ratio.size");

    const auto n = static_cast<int>(ticket_ratio.size());
    vector<double> opt(n);

    for (auto i = 0; i < n; ++i)
    {
        opt[i] = ticket_ratio[i].first;
        for (auto j = 0; j < i; ++j)
        {
            const auto value = ticket_ratio[j].second * ticket_ratio[i].first + opt[j];
            opt[i] = min(opt[i], value);
        }
    }

    const auto& result = opt.back();
    return result;
}

double Privet::Algorithms::Geometry::convex_hull_trick_last_concert(
    const vector<pt>& ticket_ratio)
{
    RequirePositive(ticket_ratio.size(), "ticket_ratio.size");

    const auto n = static_cast<int>(ticket_ratio.size());

    auto last_best_price = 0.0;
    // {coef, added value}
    set<pt> lines;
    lines.insert(make_pair(1.0, 0.0));

    for (auto i = 0; i < n; ++i)
    {
        const auto the_best_line = find_best_line(lines, ticket_ratio[i].first);
        last_best_price = the_best_line.first * ticket_ratio[i].first + the_best_line.second;
        add_and_remove_useless_lines(lines, make_pair(ticket_ratio[i].second, last_best_price));
    }

    return last_best_price;
}