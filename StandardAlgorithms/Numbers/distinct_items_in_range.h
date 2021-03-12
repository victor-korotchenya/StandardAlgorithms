#pragma once
#include"../Graphs/binary_indexed_tree.h"
#include"../Utilities/is_debug.h"
#include"mo.h"
#include"standard_operations.h"
#include<unordered_map>

namespace Standard::Algorithms::Numbers::Inner
{
    template<class int_t, std::unsigned_integral size_t1>
    constexpr void require_non_zero_frequency_debug([[maybe_unused]] const int_t &item,
        [[maybe_unused]] const size_t1 &frequency, [[maybe_unused]] const std::string &function_name,
        [[maybe_unused]] const size_t1 &count)
    {
        if constexpr (!::Standard::Algorithms::is_debug)
        {
            return;
        }

        if (size_t1{} < frequency)
        {
            return;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "Error: " << frequency << " == frequencies[" << item << "], count " << count << " in " << function_name
            << ".";

        throw_exception(str);
    }

    template<std::integral int_t, std::unsigned_integral size_t1>
    constexpr void add_count(const int_t &item, std::vector<size_t1> &frequencies, size_t1 &count)
    {
        assert(!(item < int_t{}) && static_cast<std::size_t>(item) < frequencies.size());

        auto &freq = frequencies[item];

        if (0U == freq++)
        {
            ++count;
        }

        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto *const func_name = static_cast<const char *>(__FUNCTION__);

            require_non_zero_frequency_debug(item, freq, func_name, count);
        }
    }

    template<std::integral int_t, std::unsigned_integral size_t1>
    constexpr void subtract_count(const int_t &item, std::vector<size_t1> &frequencies, size_t1 &count)
    {
        assert(!(item < int_t{}) && static_cast<std::size_t>(item) < frequencies.size());

        auto &freq = frequencies[item];

        if constexpr (::Standard::Algorithms::is_debug)
        {
            const auto *const func_name = static_cast<const char *>(__FUNCTION__);

            require_non_zero_frequency_debug(item, freq, func_name, count);
        }

        if (0U == --freq)
        {
            --count;
        }
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Given an array of items,
    // the queries ask for the number of the distinct items
    // in a range [a..b], where both a and b are inclusive, and a <= b.
    // Time O(n*log(n)).
    template<std::integral int_t, std::unsigned_integral size_t1, class pair_t = std::pair<size_t1, size_t1>>
    [[nodiscard]] constexpr auto distinct_items_in_range(
        const std::vector<int_t> &data, const std::vector<pair_t> &range_queries) -> std::vector<size_t1>
    {
        const auto data_size = require_positive(data.size(), "data size");
        const auto queries_size = require_positive(static_cast<size_t1>(range_queries.size()), "range queries size");

        std::vector<size_t1> result(queries_size);

        // Sort by the right!
        std::vector<std::vector<pair_t>> qus(data_size);

        for (size_t1 index{}; index < queries_size; ++index)
        {
            const auto &left = range_queries[index].first;
            const auto &right = range_queries[index].second;

            if (right < left || !(right < data_size)) [[unlikely]]
            {
                throw std::invalid_argument("bad range in distinct_items_in_range.");
            }

            qus[right].emplace_back(left, index);
        }

        std::unordered_map<int_t, size_t1> last_position;

        using operation_t = size_t1 (*)(const size_t1 &, const size_t1 &);
        using tree_t = Standard::Algorithms::Trees::binary_indexed_tree<size_t1, operation_t>;

        constexpr auto init_index = tree_t::initial_index;

        tree_t tree(data_size, sum_unchecked<size_t1>, subtract_unchecked<size_t1>, size_t1{});

        for (size_t1 right{}; right < data_size; ++right)
        {
            const auto &datum = data[right];

            if (const auto iter = last_position.find(datum); iter != last_position.end())
            {
                constexpr auto inf = 0U - static_cast<size_t1>(1);

                tree.set(init_index + (iter->second), inf);
            }

            // todo(p3): try to reuse the iter not to search twice.
            last_position[datum] = right;

            tree.set(init_index + right, 1U);

            for (const auto &queries = qus[right]; const auto &quer : queries)
            {
                result[quer.second] = tree.get({ init_index + quer.first, init_index + right });
            }
        }

        return result;
    }

    // Slow time is power(n, 3/2).
    template<std::integral int_t, std::unsigned_integral size_t1, class pair_t = std::pair<size_t1, size_t1>>
    [[nodiscard]] constexpr auto distinct_items_in_range_slow(
        const std::vector<int_t> &data, const std::vector<pair_t> &range_queries) -> std::vector<size_t1>
    {
        auto result = mo_algorithm<int_t, decltype(Inner::subtract_count<int_t, size_t1>),
            decltype(Inner::add_count<int_t, size_t1>), size_t1>(
            data, range_queries, &Inner::subtract_count<int_t, size_t1>, &Inner::add_count<int_t, size_t1>);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
