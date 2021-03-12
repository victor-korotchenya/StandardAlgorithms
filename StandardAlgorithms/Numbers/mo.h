#pragma once
// "mo.h"
#include"../Utilities/check_size.h"
#include"arithmetic.h"
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::integral size_t1>
    struct query_with_index final
    {
        size_t1 left{};
        size_t1 right{};
        size_t1 index{};
    };

    template<std::integral size_t1>
    struct block_comparer final
    {
        constexpr explicit block_comparer(const size_t1 &block_size)
            : Block_size(require_positive(block_size, "block size"))
        {
        }

        [[nodiscard]] constexpr auto block_size() const &noexcept -> const size_t1 &
        {
            return Block_size;
        }

        [[nodiscard]] constexpr auto operator() (
            const query_with_index<size_t1> &one, const query_with_index<size_t1> &two) const noexcept -> bool
        {
            assert(size_t1{} < Block_size);

            const auto block1 = one.left / Block_size;
            const auto block2 = two.left / Block_size;

            if (block1 != block2)
            {
                return block1 < block2;
            }

            if (one.right != two.right)
            {
                return one.right < two.right;
            }

            return one.left < two.left;
        }

private:
        size_t1 Block_size;
    };

    template<std::unsigned_integral size_t1, class pair_t>
    constexpr void check_query(const size_t1 &index, const pair_t &query, const size_t1 &size_minus_one)
    {
        if (!Standard::Algorithms::Numbers::is_within_inclusive<size_t1>(query.first, { size_t1{}, size_minus_one }))
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The range queries[" << index << "].left (" << query.first << ") must be smaller than size ("
                << (size_minus_one + 1LLU) << ").";

            throw_exception(str);
        }

        if (!Standard::Algorithms::Numbers::is_within_inclusive<size_t1>(query.second, { size_t1{}, size_minus_one }))
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The range queries[" << index << "].right (" << query.second << ") must be smaller than size ("
                << (size_minus_one + 1LLU) << ").";

            throw_exception(str);
        }

        if (query.second < query.first)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The range queries[" << index << "].right (" << query.second << ") cannot be smaller than min ("
                << query.first << ").";

            throw_exception(str);
        }
    }

    template<std::unsigned_integral size_t1, class pair_t>
    [[nodiscard]] constexpr auto check_prepare_queries(const size_t1 &data_size, const std::vector<pair_t> &queries,
        const size_t1 &block_size) -> std::vector<query_with_index<size_t1>>
    {
        require_positive(block_size, "block size");
        require_less_equal(block_size, data_size, "block vs data size ");

        std::vector<query_with_index<size_t1>> result(queries.size());

        const auto size1 = Standard::Algorithms::Utilities::check_size<size_t1>("queries size", queries.size());

        assert(size1 == queries.size());

        const auto size_minus_one = data_size - 1U;

        for (size_t1 index{}; index < size1; ++index)
        {
            const auto &query = queries[index];
            check_query<size_t1>(index, query, size_minus_one);

            auto &res = result[index];
            res.left = query.first;
            res.right = query.second;
            res.index = index;
        }

        const block_comparer<size_t1> comparer(block_size);
        std::sort(result.begin(), result.end(), comparer);

        return result;
    }

    template<class int_t, std::unsigned_integral size_t1, class subtract_count_t, class add_count_t,
        class frequencies_t> // = std::vector<size_t1>
    constexpr void run_query(const std::vector<int_t> &data, const query_with_index<size_t1> &query, size_t1 &left,
        frequencies_t &frequencies, size_t1 &right_incl, subtract_count_t subtract_count, add_count_t add_count,
        size_t1 &count)
    {
        while (query.left < left)
        {
            add_count(data[--left], frequencies, count);
        }

        while (right_incl < query.right)
        {
            add_count(data[++right_incl], frequencies, count);
        }

        // Make sure the "frequencies" are non-negative: first add, then subtract.

        while (query.right < right_incl)
        {
            subtract_count(data[right_incl--], frequencies, count);
        }

        while (left < query.left)
        {
            subtract_count(data[left++], frequencies, count);
        }
    }

    template<std::integral int_t, std::unsigned_integral size_t1, class subtract_count_t, class add_count_t>
    [[nodiscard]] constexpr auto process_queries(const std::vector<int_t> &data,
        const std::vector<query_with_index<size_t1>> &index_queries, subtract_count_t subtract_count,
        add_count_t add_count) -> std::vector<size_t1>
    {
        throw_if_empty("data", data);

        const auto queries_size = index_queries.size();

        std::vector<size_t1> result(queries_size);

        const auto [min_it, max_it] = std::minmax_element(data.cbegin(), data.cend());
        const auto &max_value = *max_it;

        {
            const auto &min_value = *min_it;
            assert(!(max_value < min_value));

            using typ = std::remove_cvref_t<int_t>;

            if constexpr (std::is_signed_v<typ>)
            {
                constexpr int_t zero{};

                if (min_value < zero)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The minimum datum " << min_value << " cannot be negative.";

                    throw_exception(str);
                }
            }
        }

        using frequencies_t = std::vector<size_t1>;

        frequencies_t frequencies(max_value
            // Count the zero.
            + 1LL);

        size_t1 left = 1;
        size_t1 right_incl{};
        size_t1 count{};

        for (size_t1 index{}; index < queries_size; ++index)
        {
            const auto &query = index_queries[index];

            run_query<int_t, size_t1, subtract_count_t, add_count_t, frequencies_t>(
                data, query, left, frequencies, right_incl, subtract_count, add_count, count);

            result[query.index] = count;
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // The query.right is inclusive.
    template<std::integral int_t, class subtract_count_t, class add_count_t, std::unsigned_integral size_t1,
        class pair_t = std::pair<size_t1, size_t1>>
    [[nodiscard]] constexpr auto mo_algorithm(const std::vector<int_t> &data, const std::vector<pair_t> &range_queries,
        subtract_count_t subtract_count, add_count_t add_count) -> std::vector<size_t1>
    {
        const auto *const func_name = static_cast<const char *>(__FUNCTION__);

        const auto size_name = std::string("data size in ") + func_name;

        const auto size = Standard::Algorithms::Utilities::check_size<size_t1>(size_name, data.size());

        require_positive(size, size_name);

        require_positive(range_queries.size(), std::string("range queries size in ") + func_name);

        const auto draft_sqr = static_cast<size_t1>(std::sqrt(size)); // todo(p3): use a correct sqrt.

        const auto block_size = static_cast<size_t1>(draft_sqr + (size == draft_sqr * draft_sqr ? 0ZU : 1ZU));

        const auto index_queries = Inner::check_prepare_queries<size_t1>(size, range_queries, block_size);

        auto result = Inner::process_queries<int_t, size_t1, subtract_count_t, add_count_t>(
            data, index_queries, subtract_count, add_count);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
