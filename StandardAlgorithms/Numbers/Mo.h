#pragma once
#include <algorithm>
#include <cmath>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "Arithmetic.h"
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/StreamUtilities.h"

namespace
{
    template<typename Size>
    struct query_with_id final
    {
        Size Left, Right, Index;

        query_with_id(Size left = 0, Size right = 0, Size index = 0)
            : Left(left), Right(right), Index(index)
        {
        }
    };

    template<typename Size>
    class block_comparer final
    {
        Size _block_size;

    public:
        explicit block_comparer(Size blockSize = 0)
            : _block_size(RequirePositive(blockSize, "blockSize"))
        {
        }

        bool operator()(
            const query_with_id<Size>& x, const query_with_id<Size>& y) const
        {
            const auto block1 = x.Left / _block_size;
            const auto block2 = y.Left / _block_size;

            if (block1 != block2)
                //Sort by block.
                return block1 < block2;

            //Sort by the Right value.
            if (x.Right != y.Right)
                return x.Right < y.Right;

            return x.Left < y.Left;
        }
    };

    template <typename Size, typename pair_t>
    void check_query(const Size size_minus_one, const Size i,
        const pair_t& query)
    {
        if (!is_within<Size>(0, size_minus_one, query.first))
        {
            std::ostringstream ss;
            ss << "The range_queries[" << i << "].left ("
                << query.first
                << ") must be smaller than size (" << size_minus_one + 1 << ").";
            StreamUtilities::throw_exception(ss);
        }
        if (!is_within<Size>(0, size_minus_one, query.second))
        {
            std::ostringstream ss;
            ss << "The range_queries[" << i << "].right ("
                << query.second
                << ") must be smaller than size (" << size_minus_one + 1 << ").";
            StreamUtilities::throw_exception(ss);
        }
        if (query.second < query.first)
        {
            std::ostringstream ss;
            ss << "The range_queries[" << i << "].right ("
                << query.second
                << ") cannot be smaller than min (" << query.first << ").";
            StreamUtilities::throw_exception(ss);
        }
    }

    template<typename Size, typename pair_t>
    std::vector<query_with_id<Size>> check_prepare_queries(
        const Size data_size,
        const std::vector<pair_t>& queries,
        const Size block_size)
    {
        std::vector<query_with_id<Size>> result(queries.size());
        for (Size i = 0; i < queries.size(); ++i)
        {
            const auto& query = queries[i];
            check_query<Size>(data_size - 1, i, query);

            result[i].Left = query.first;
            result[i].Right = query.second;
            result[i].Index = i;
        }

        const block_comparer<Size> comparer(block_size);
        std::sort(result.begin(), result.end(), comparer);

        return result;
    }

    template <typename Number, typename Size,
        typename remove_count_t, typename add_count_t,
        typename frequencies_t// = std::vector<Size>
    >
        void run_query(
            const std::vector<Number>& data,
            const query_with_id<Size>& query,
            remove_count_t remove_count, add_count_t add_count,
            frequencies_t& frequencies,
            Size& left, Size& right_incl, Size& count)
    {
        //Make sure the "frequencies" are non-negative: first add, then subtract.
        while (query.Left < left)
            add_count(data[--left], frequencies, count);

        while (right_incl < query.Right)
            add_count(data[++right_incl], frequencies, count);

        while (query.Right < right_incl)
            remove_count(data[right_incl--], frequencies, count);

        while (left < query.Left)
            remove_count(data[left++], frequencies, count);
    }

    template<typename Number, typename Size,
        typename remove_count_t, typename add_count_t>
        std::vector<Size> process_queries(
            const std::vector<Number>& data,
            const std::vector<query_with_id<Size>>& id_queries,
            remove_count_t remove_count, add_count_t add_count)
    {
        const auto queries_size = id_queries.size();
        std::vector<Size> result(queries_size);

        const auto max_value = *std::max_element(data.begin(), data.end());
        Size left = 1, right_incl = 0, count = 0;

        std::vector<Size> frequencies(max_value
            //Count zero.
            + 1);

        for (Size i = 0; i < queries_size; ++i)
        {
            const auto& query = id_queries[i];

            run_query<Number, Size,
                add_count_t, remove_count_t,
                std::vector<Size>>(
                    data, query,
                    remove_count, add_count,
                    frequencies,
                    left, right_incl, count);

            result[query.Index] = count;
        }

        return result;
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // The query.right is inclusive.
            template<typename Number,
                typename remove_count_t, typename add_count_t,
                typename Size = size_t,
                typename pair_t = std::pair<Size, Size>
            >
                std::vector<Size> MoAlgorithm(
                    const std::vector<Number>& data,
                    const std::vector<pair_t>& range_queries,
                    remove_count_t remove_count, add_count_t add_count)
            {
                static_assert(std::is_unsigned<Size>::value, "The Size must be unsigned.");

                const auto size = static_cast<Size>(data.size());
                RequirePositive(size, "data.size in " + std::string(__FUNCTION__));

                const auto queries_size = range_queries.size();
                RequirePositive(queries_size, "range_queries.size in " + std::string(__FUNCTION__));

                const auto sq = sqrt(size);
                const auto block_size = static_cast<Size>(sq) + (size == sq * sq ? 0 : 1);

                const auto id_queries = check_prepare_queries<Size>(
                    size, range_queries, block_size);

                const auto result = process_queries<Number, Size,
                    add_count_t, remove_count_t>(data, id_queries,
                        remove_count, add_count);

                return result;
            }
        }
    }
}