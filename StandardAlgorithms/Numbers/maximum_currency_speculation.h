#pragma once
#include"../Utilities/require_utilities.h"
#include"hash_utilities.h"
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    namespace Inner
    {
        template<std::floating_point floating_t>
        constexpr void check_max_cur_spec(
            const std::vector<std::vector<floating_t>> &exchange_rates, const floating_t &inf)
        {
            require_positive(inf, "inf");

            constexpr floating_t zero{};
            constexpr floating_t improfitable = 1;

            const auto size = exchange_rates.size();
            {
                constexpr auto max_size = 1'000'000U;

                require_less_equal(size, max_size, "size");
            }

            const auto bamf_finder = [&zero, &inf](const auto &val)
            {
                return val < zero || !(val < inf);
            };

            for (std::size_t index{}; index < size; ++index)
            {
                const auto &rate_row = exchange_rates[index];

                if (const auto cols = rate_row.size(); cols != size) [[unlikely]]
                {
                    auto err = "The column number (" + std::to_string(cols) + ") for the row " + std::to_string(index) +
                        " must be size " + std::to_string(size);

                    throw std::invalid_argument(err);
                }

                if (const auto &rate = rate_row[index]; rate != improfitable) [[unlikely]]
                {
                    throw std::invalid_argument("The self- exchange rates[" + std::to_string(index) + "][" +
                        std::to_string(index) + "] must be 1, not " + std::to_string(rate) + ".");
                }

                if (const auto bamf = std::find_if(rate_row.cbegin(), rate_row.cend(), bamf_finder);
                    bamf != rate_row.cend()) [[unlikely]]
                {
                    const auto ind_2 = bamf - rate_row.cbegin();

                    auto err = "The exchange rates[" + std::to_string(index) + "][" + std::to_string(ind_2) +
                        "] must be non-negative and less than infinity, but is " + std::to_string(*bamf) + ".";

                    throw std::invalid_argument(err);
                }
            }
        }

        template<std::floating_point floating_t, class set_t>
        constexpr void compute_max_cur_spec(std::vector<std::vector<floating_t>> &rates, const floating_t &inf,
            const std::size_t mid, set_t &prev, std::size_t &start_pos, floating_t &best_rate, std::size_t &mid_pos)
        {
            const auto size = rates.size();

            for (std::size_t from{}; from < size; ++from)
            {
                if (from == mid)
                {
                    continue;
                }

                const auto &from_mid_rate = rates[from][mid];
                assert(floating_t{} <= from_mid_rate && from_mid_rate < inf);

                for (std::size_t tod{}; tod < size; ++tod)
                {
                    if (mid == tod)
                    {
                        continue;
                    }

                    const auto &mid_tod_rate = rates[mid][tod];
                    assert(floating_t{} <= mid_tod_rate && mid_tod_rate < inf);

                    const auto cand = static_cast<floating_t>(from_mid_rate * mid_tod_rate);

                    if (cand < floating_t{} || !(cand < inf)) [[unlikely]]
                    {
                        auto err = "The computed exchange rate (" + std::to_string(cand) + ") is out of bounds";

                        throw std::out_of_range(err);
                    }

                    auto &from_tod_rate = rates[from][tod];
                    if (!(from_tod_rate < cand))
                    {
                        continue;
                    }

                    from_tod_rate = cand;
                    prev.emplace(from, tod);

                    if (from == tod && best_rate < cand)
                    {
                        best_rate = cand, start_pos = from, mid_pos = mid;
                    }
                }
            }
        }

#ifdef _DEBUG
        template<std::floating_point floating_t, class set_t>
#else
        template<class set_t>
#endif
        requires(sizeof(std::size_t) <= sizeof(std::uint64_t))
        constexpr void restore_cycle_max_cur_spec(
#ifdef _DEBUG
            const std::vector<std::vector<floating_t>> &exchange_rates,
#endif
            const std::size_t start_pos, const std::vector<set_t> &prevs, const std::size_t mid_pos,
            std::vector<std::size_t> &cycle)
        {
            [[maybe_unused]] constexpr auto npos = 0U - static_cast<std::size_t>(1);

            const auto size = prevs.size();

            assert(start_pos != mid_pos && std::max(start_pos, mid_pos) < size && size < npos);

            cycle.push_back(start_pos);

            const auto size2 = static_cast<std::uint64_t>(size) * size;

#if _DEBUG
            assert(exchange_rates.size() == size);

            floating_t check_rate = 1;
#endif

            using edge_t = std::pair<std::size_t, std::size_t>;

            std::vector<std::pair<edge_t, std::size_t>> sta(1, { { start_pos, start_pos }, mid_pos });

            do
            {
                const auto edge = sta.back().first;
                auto mid = sta.back().second;

                assert(std::max({ edge.first, edge.second, mid }) < size);

                sta.pop_back();

                for (;;)
                {
                    if (const auto &pre = prevs[mid]; pre.contains(edge))
                    {
                        assert(mid != edge.first && mid != edge.second && mid < size);

                        sta.push_back({ { mid, edge.second }, mid });
                        sta.push_back({ { edge.first, mid }, mid });

                        break;
                    }

                    if (0U < mid--)
                    {// Now the mid can only decrease because it could have increased in the computation.
                        continue;
                    }

                    assert(edge.first != edge.second && cycle.back() == edge.first);

#if _DEBUG
                    check_rate *= exchange_rates.at(edge.first).at(edge.second);
#endif

                    cycle.push_back(edge.second);

                    if (size2 < cycle.size()) [[unlikely]]
                    {
                        auto err = "The cycle length exceeds the size*size " + std::to_string(size2);

                        throw std::runtime_error(err);
                    }

                    break;
                }
            } while (!sta.empty());

#if _DEBUG
            require_greater(check_rate, 1, "check rate");
#endif
        }
    } // namespace Inner

    // Given n<=1e6 currencies and n*n non-negative finite exchange rates,
    // find a cycle of maximum profit > 1.0 if one exists.
    // Using 2 nodes (beginning and ending) plus the mask of the nodes in-between
    // gives O(n*n * 2**n) NP-complete algorithm which must be too slow.
    // Since the nodes in a true cycle cannot repeat,
    // let's cheat a little bit: the nodes (currencies) in a cycle may repeat,
    // yet the cycle length cannot exceed n*n.
    // Currency arbitrage time O(n**3), similar to that of Floyd-Warshall.
    template<std::floating_point floating_t>
    constexpr auto maximum_currency_speculation_simple(const std::vector<std::vector<floating_t>> &exchange_rates,
        const floating_t &inf, std::vector<std::size_t> &cycle) -> floating_t
    {
        // todo(p4): if all rates <= 1, immediately return 0.
        Inner::check_max_cur_spec<floating_t>(exchange_rates, inf);

        cycle.clear();

        const auto size = exchange_rates.size();

        if (size < 2U)
        {
            return {};
        }

        // todo(p4): remove the unneeded copy?
        auto rates_copy = exchange_rates;

        // Should keep the whole history O(n**3) space
        // because an edge might have a different cost for a different middle.
        using edge_t = std::pair<std::size_t, std::size_t>;

        using set_t = std::unordered_set<edge_t, Standard::Algorithms::Numbers::pair_hash>;

        constexpr floating_t improfitable = 1;
        constexpr auto npos = 0U - static_cast<std::size_t>(1);

        std::vector<set_t> prevs(size);
        auto best_rate = improfitable;
        auto start_pos = npos;
        auto mid_pos = npos;

        for (std::size_t mid{}; mid < size; ++mid)
        {
            Inner::compute_max_cur_spec<floating_t, set_t>(
                rates_copy, inf, mid, prevs[mid], start_pos, best_rate, mid_pos);
        }

        if (best_rate == improfitable)
        {
            assert(start_pos == npos && mid_pos == npos);

            return improfitable;
        }

        assert(improfitable < best_rate && std::max(start_pos, mid_pos) < size);

        Inner::restore_cycle_max_cur_spec<
#ifdef _DEBUG
            floating_t,
#endif
            set_t>(
#ifdef _DEBUG
            exchange_rates,
#endif
            start_pos, prevs, mid_pos, cycle);

        assert(2U <= cycle.size() && cycle[0] == cycle.back());

        return best_rate;
    }
} // namespace Standard::Algorithms::Numbers
