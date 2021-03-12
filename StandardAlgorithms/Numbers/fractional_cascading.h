#pragma once
#include"../Utilities/is_debug.h"
#include<algorithm>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<tuple>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given n sorted vectors each of size O(s), and a key,
    // find the lower bound in each vector
    // in O(n + log(s)) total time.
    template<class item_t, std::unsigned_integral pos_t = std::size_t>
    struct lower_bound_fractional_cascading final
    {
        // Current line position, next line position, value.
        using cur_next_val_t = std::tuple<pos_t, pos_t, item_t>;

        constexpr explicit lower_bound_fractional_cascading(const std::vector<std::vector<item_t>> &data)
            : cascades(data.size())
            , data_sizes(data.size())
        {
            const auto size = data.size();

            if (size == 0U)
            {
                return;
            }

            fill_line_last(data.back());

            if (size == 1U)
            {
                return;
            }

            for (auto pos = static_cast<pos_t>(size - 2U);;)
            {
                const auto &line = data[pos];
                fill_line(line, pos);

                if (pos-- == 0U)
                {
                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        validate_debug(data);
                    }

                    return;
                }
            }
        }

        constexpr void lower_bound_many(const item_t &key, std::vector<pos_t> &positions) const
        {
            const auto size = cascades.size();
            positions.resize(size);

            if (size == 0U)
            {
                return;
            }

            pos_t cur_index{};
            {
                const auto par = initial_lower_bound(key);
                positions[0] = par.first;
                cur_index = par.second;

                assert(positions[0] <= data_sizes[0]);
            }

            for (pos_t pos = 1; pos < size; ++pos)
            {
                cur_index = further_lower_bound(pos, positions, key, cur_index);
            }
        }

private:
        constexpr void fill_line_last(const std::vector<item_t> &line)
        {
            const auto size = static_cast<pos_t>(line.size());
            assert(size == line.size());

            data_sizes.back() = size;

            auto &cascade = cascades.back();
            cascade.resize(size);

            for (pos_t index{}; index < size; ++index)
            {
                cascade[index] = std::make_tuple(index, pos_t{}, line[index]);
            }
        }

        // 10, 30, 40     // When adding 40, size1=3 with value 60,
        // 20, 30, 50, 60 // but size1=2 with value 50 is lower.
        [[nodiscard]] static constexpr auto lowest_next_pos(
            const pos_t &size1, const std::vector<cur_next_val_t> &nex, const item_t &cur_val) -> pos_t
        {
            assert(0U < size1);

            const auto &tup = nex[size1 - 1U];
            const auto &nex_val = value(tup);

            auto posit = nex_val < cur_val ? size1 : static_cast<pos_t>(size1 - 1U);

            return posit;
        }

        constexpr void fill_line(const std::vector<item_t> &line, const pos_t &pos)
        {
            assert(pos < cascades.size() && pos + 1U < cascades.size());

            data_sizes[pos] = static_cast<pos_t>(line.size());
            assert(data_sizes[pos] == line.size()); // no overflow

            const auto &nex = cascades[pos + 1U];
            const auto nex_size = static_cast<pos_t>(line.size() + (nex.size() >> 1U));

            auto &cur = cascades[pos];
            cur.resize(nex_size);

            pos_t cur_index{};
            pos_t nex_index = 1;
            pos_t joined{};

            while (cur_index < line.size() && nex_index < nex.size())
            {
                assert(joined < nex_size);

                const auto &tup = nex[nex_index];
                const auto &cur_val = line[cur_index];
                const auto &nex_val = value(tup);

                if (cur_val < nex_val)
                {
                    const auto next_1 = lowest_next_pos(nex_index, nex, cur_val);

                    cur[joined] = std::make_tuple(cur_index, next_1, cur_val);
                    ++cur_index;
                }
                else
                {
                    cur[joined] = std::make_tuple(cur_index, nex_index, nex_val);
                    nex_index += 2;
                }

                ++joined;
            }

            while (nex_index < nex.size())
            {
                assert(joined < nex_size);

                const auto &tup = nex[nex_index];
                cur[joined] = std::make_tuple(cur_index, nex_index, value(tup));

                nex_index += 2;
                ++joined;
            }

            nex_index = static_cast<pos_t>(nex.size());

            while (cur_index < line.size())
            {
                assert(joined < nex_size);

                const auto &cur_val = line[cur_index];
                const auto next_1 = nex_index ? lowest_next_pos(nex_index, nex, cur_val) : nex_index;

                cur[joined] = std::make_tuple(cur_index, next_1, cur_val);

                ++cur_index;
                ++joined;
            }
        }

        // Time O(log(s)).
        [[nodiscard]] constexpr auto initial_lower_bound(const item_t &key) const -> std::pair<pos_t, pos_t>
        {
            assert(!cascades.empty() && !data_sizes.empty());

            const auto &cascade = cascades.at(0);

            if (cascade.empty())
            {
                return {};
            }

            {
                const auto &last_tup = cascade.back();
                const auto &max_val = value(last_tup);

                if (max_val < key)
                {// Greater than anything in the last line.
                    return std::make_pair(data_sizes[0], next_pos(last_tup));
                }

                if (const auto equal1 = !(key < max_val); equal1)
                {
                    assert(max_val == key);

                    return std::make_pair(cur_pos(last_tup), next_pos(last_tup));
                }

                assert(key < max_val);
            }

            const auto iter = std::lower_bound(
                cascade.cbegin(), cascade.cend() - 1, std::make_tuple(pos_t{}, pos_t{}, key), is_lesser_value);

            const auto &tup = *iter;

            return std::make_pair(cur_pos(tup), next_pos(tup));
        }

        [[nodiscard]] constexpr auto further_lower_bound(
            const pos_t &pos, std::vector<pos_t> &positions, const item_t &key, pos_t cur_index) const -> pos_t
        {
            assert(pos < cascades.size());

            const auto &cascade = cascades[pos];
            const auto size = static_cast<pos_t>(cascade.size());

            assert(cur_index <= size);

            auto &result_pos = positions[pos];
            if (size == 0U)
            {
                return (result_pos = 0U);
            }

            const auto &max_size = data_sizes[pos];

            if (cur_index != 0U)
            {
                const auto &pre = cascade[cur_index - 1U];

                if (const auto go_left = !(value(pre) < key); go_left)
                {
                    assert(cur_index == 1U || !(key < value(cascade.at(cur_index - 2U))));

                    result_pos = cur_pos(pre);

                    assert(result_pos <= max_size);

                    return next_pos(pre);
                }
            }

            if (cur_index + 1U < size)
            {
                const auto &next = cascade[cur_index + 1U];
                const auto &item = value(next);

                if (item < key)
                {
                    // 10, 70     -> 10, 40, 70, // key = 60
                    // 30         -> 30, 40      //
                    // 20, 40, 50 -> 20, 40, 50  // Jump twice, from 40 to +infinity.
                    ++cur_index;
                }
                else
                {
                    const auto &cur = cascade[cur_index];
                    const auto &tup = value(cur) < key ? next : cur;
                    result_pos = cur_pos(tup);

                    assert(result_pos <= max_size);

                    return next_pos(tup);
                }
            }

            if (size - 1U <= cur_index)
            {
                cur_index = static_cast<pos_t>(size - 1U);

                const auto &cas = cascade[cur_index];
                const auto &item = value(cas);

                if (item < key)
                {
                    result_pos = max_size;

                    return next_pos(cas);
                }
            }

            const auto &cu2 = cascade[cur_index];
            result_pos = cur_pos(cu2);

            assert(key <= value(cu2) && result_pos <= max_size);

            return next_pos(cu2);
        }

        [[nodiscard]] static constexpr auto is_lesser_value(const cur_next_val_t &one, const cur_next_val_t &two)
            -> bool
        {
            auto less = value(one) < value(two);
            return less;
        }

        [[nodiscard]] static constexpr auto cur_pos(const cur_next_val_t &tup) -> const pos_t &
        {
            return std::get<0>(tup);
        }

        [[nodiscard]] static constexpr auto next_pos(const cur_next_val_t &tup) -> const pos_t &
        {
            return std::get<1>(tup);
        }

        [[nodiscard]] static constexpr auto value(const cur_next_val_t &tup) -> const item_t &
        {
            return std::get<2>(tup);
        }

        template<class tup_t2 = cur_next_val_t>
        [[maybe_unused]] static constexpr void validate_compare_with_prev_debug(
            const cur_next_val_t &tup_prev, const tup_t2 &tup)
        {
            {
                const auto &curr_prev = cur_pos(tup_prev);
                const auto &nex_prev = next_pos(tup_prev);

                const auto &curr = cur_pos(tup);
                const auto &nex = next_pos(tup);

                assert(curr_prev <= curr && nex_prev <= nex);
                assert(curr_prev < curr || nex_prev < nex);
            }
            {
                const auto &item_prev = value(tup_prev);
                const auto &item = value(tup);

                assert(item_prev <= item);
            }
        }

        [[maybe_unused]] static constexpr void validate_next_pos_debug(
            const pos_t &mid, const std::vector<cur_next_val_t> &nex, const item_t &item)
        {
            const auto par =
                std::equal_range(nex.cbegin(), nex.cend(), std::make_tuple(pos_t{}, pos_t{}, item), is_lesser_value);

            const auto low = static_cast<pos_t>(par.first - nex.cbegin());
            const auto high = static_cast<pos_t>(par.second - nex.cbegin());

            assert(low <= mid && mid <= high);
        }

        [[maybe_unused]] constexpr void validate_debug(const std::vector<std::vector<item_t>> &data) const
        {
            assert(2U <= data.size() && data.size() == cascades.size() && data.size() == data_sizes.size());

            for (pos_t pos = 1; pos < data.size(); ++pos)
            {
                const auto &cur = cascades[pos - 1U];
                const auto &nex = cascades[pos];

                const auto max_cur = static_cast<pos_t>(data[pos - 1U].size());
                const auto max_nex = static_cast<pos_t>(nex.size());

                for (pos_t index{}; index < cur.size(); ++index)
                {
                    const auto &tup = cur[index];
                    {
                        const auto &curp = cur_pos(tup);

                        assert(curp <= max_cur);
                    }

                    const auto &nep = next_pos(tup);

                    assert(nep <= max_nex);

                    const auto &item = value(tup);

                    if (0U != max_nex)
                    {
                        validate_next_pos_debug(nep, nex, item);
                    }

                    if (index == 0U)
                    {
                        continue;
                    }

                    const auto &tup_prev = cur[index - 1U];

                    validate_compare_with_prev_debug(tup_prev, tup);
                }
            }
        }

        // Current line position, next line position, value.
        std::vector<std::vector<cur_next_val_t>> cascades;
        std::vector<pos_t> data_sizes;
    };

    // Slow time O(n*log(s)).
    template<class item_t, std::unsigned_integral pos_t>
    constexpr void lower_bound_many_slow(
        const std::vector<std::vector<item_t>> &data, const item_t &key, std::vector<pos_t> &positions)
    {
        const auto size = data.size();
        positions.resize(size);

        for (pos_t index{}; index < size; ++index)
        {
            const auto &datum = data[index];
            const auto iter = std::lower_bound(datum.cbegin(), datum.cend(), key);
            positions[index] = static_cast<pos_t>(iter - datum.cbegin());
        }
    }
} // namespace Standard::Algorithms::Numbers
