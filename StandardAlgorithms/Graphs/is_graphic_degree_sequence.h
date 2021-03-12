#pragma once
#include"../Numbers/to_unsigned.h"
#include<algorithm>
#include<cassert>
#include<concepts>
#include<cstdint>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner
{
    template<std::integral int_t>
    requires(sizeof(int_t) <= sizeof(std::uint64_t))
    [[nodiscard]] constexpr auto check_input(const std::vector<int_t> &degree_sorted_desc)
        -> std::vector<std::vector<int_t>>
    {
        const auto size = static_cast<int_t>(degree_sorted_desc.size());

        if (degree_sorted_desc.empty() || static_cast<std::size_t>(size) != degree_sorted_desc.size())
        {
            return {};
        }

        constexpr int_t zero{};

        std::vector<std::vector<int_t>> degree_ar(size);

        std::uint64_t degree_sum{};
        int_t ide{};
        auto previous_degree = degree_sorted_desc.at(0);

        for (const auto &degr : degree_sorted_desc)
        {
            if (previous_degree < degr || !(degr < size) || degr < zero)
            {
                return {};
            }

            degree_sum += ::Standard::Algorithms::Numbers::to_unsigned(degr);
            previous_degree = degr;

            if (zero != degr)
            {
                degree_ar[degr].push_back(ide++);
            }
        }

        if (const auto bit = degree_sum & 1U; bit != 0U)
        {
            return {};
        }

        return degree_ar;
    }

    template<class int_t>
    constexpr auto match_vertices(std::vector<std::vector<int_t>> &graph, const int_t first_deg,
        std::vector<std::vector<int_t>> &degree_ar, std::vector<std::pair<int_t, int_t>> &buf, const int_t two) -> bool
    {
        buf.clear();

        constexpr int_t zero{};
        auto deg = first_deg;
        auto left = first_deg;

        while (zero < deg)
        {
            auto &arr = degree_ar[deg];

            while (!arr.empty())
            {
                const auto one = arr.back();
                arr.pop_back();
                assert(one != two);

                graph[two].push_back(one);
                graph[one].push_back(two);

                if (deg - 1 != zero)
                {
                    buf.emplace_back(one, deg - 1);
                }

                if (zero != --left)
                {
                    continue;
                }

                for (const auto &par : buf)
                {
                    degree_ar[par.second].push_back(par.first);
                }

                return true;
            }

            --deg;
        }

        assert(zero < left);

        return false;
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    // Havel?Hakimi algorithm.
    template<std::integral int_t>
    [[nodiscard]] constexpr auto is_graphic_degree_sequence(
        const std::vector<int_t> &degree_sorted_desc, std::vector<std::vector<int_t>> &graph) -> bool
    {
        graph.clear();

        if (degree_sorted_desc.empty())
        {
            return true;
        }

        const auto size = static_cast<int_t>(degree_sorted_desc.size());

        if (const auto is_overflow = static_cast<std::size_t>(size) != degree_sorted_desc.size(); is_overflow)
        {
            return {};
        }

        graph.resize(size);

        auto degree_ar = Inner::check_input(degree_sorted_desc);

        if (degree_ar.empty())
        {
            return {};
        }

        std::vector<std::pair<int_t, int_t>> buf;

        constexpr int_t zero{};

        for (int_t deg = size - 1; zero < deg; --deg)
        {
            auto &arr = degree_ar[deg];

            while (!arr.empty())
            {
                const auto two = arr.back();
                arr.pop_back();

                if (!Inner::match_vertices<int_t>(graph, deg, degree_ar, buf, two))
                {
                    return false;
                }
            }
        }

        return true;
    }
} // namespace Standard::Algorithms::Graphs
