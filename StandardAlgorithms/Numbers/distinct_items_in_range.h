#pragma once
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Mo.h"
#include "../Graphs/BinaryIndexedTree.h"
#include "StandardOperations.h"

namespace
{
#ifdef _DEBUG
    template<typename Number, typename Size>
    inline void require_non_zero_frequency(
        const Number& item,
        const Size frequency,
        Size& count,
        const std::string& function_name)
    {
        if (0 == frequency)
        {
            std::ostringstream ss;
            ss << "Error: 0 == frequencies[" << item << "], count="
                << count << " in " << function_name << ".";
            StreamUtilities::throw_exception(ss);
        }
    }
#endif

    template<typename Number, typename Size>
    inline void add_count(
        const Number& item,
        std::vector<Size>& frequencies,
        Size& count)
    {
        if (1 == ++frequencies[item])
        {
            ++count;
        }

#ifdef _DEBUG
        require_non_zero_frequency(
            item, frequencies[item], count, std::string(__FUNCTION__));
#endif
    }

    template<typename Number, typename Size>
    inline void remove_count(
        const Number& item,
        std::vector<Size>& frequencies,
        Size& count)
    {
#ifdef _DEBUG
        require_non_zero_frequency(
            item, frequencies[item], count, std::string(__FUNCTION__));
#endif

        if (0 == --frequencies[item])
        {
            --count;
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //Given an array of items,
            // the queries ask for the number of the distinct items
            // in a range [a..b], where both a and b are inclusive.
            //Run-time is power(n, 3/2).
            template<typename Number, typename Size,
                typename pair_t = std::pair<Size, Size>
            >
                std::vector<Size> distinct_items_in_range_slow(
                    const std::vector<Number>& data,
                    const std::vector<pair_t>& range_queries)
            {
                const auto result = MoAlgorithm<Number,
                    decltype(remove_count<Number, Size>),
                    decltype(add_count<Number, Size>),
                    Size>(
                        data, range_queries,
                        &remove_count<Number, Size>, &add_count<Number, Size>);

                return result;
            }

            //Fast run-time is n*log(n).
            template<typename Number, typename Size,
                typename pair_t = std::pair<Size, Size>
            >
                std::vector<Size> distinct_items_in_range_fast(
                    const std::vector<Number>& data,
                    const std::vector<pair_t>& range_queries)
            {
                const auto data_size = data.size();
                const auto queries_size = static_cast<Size>(range_queries.size());

                std::vector<Size> result(queries_size);

                //Sort by the right!
                std::vector<std::vector<pair_t>> qs(data_size);

                for (Size i = 0; i < queries_size; ++i)
                {
                    const auto& left = range_queries[i].first;
                    const auto& right = range_queries[i].second;
#ifdef _DEBUG
                    if (left < 0 || right < left || data_size <= right)
                    {
                        throw std::runtime_error("bad range in distinct_items_in_range_fast.");
                    }
#endif
                    qs[right].push_back(std::make_pair(left, i));
                }

                std::unordered_map<Number, Size> last_position;

                using Operation_t = Size(*)(const Size&, const Size&);
                using tree_t = Privet::Algorithms::Trees::BinaryIndexedTree<Size, Operation_t>;
                constexpr auto init_index = tree_t::InitialIndex;
                tree_t tree(data_size, Sum_unchecked<Size>, Subtract_unchecked<Size>, 0);

                for (Size right = 0; right < data_size; ++right)
                {
                    const auto it = last_position.find(data[right]);
                    if (it != last_position.end())
                    {
                        tree.set(init_index + (it->second), 0 - static_cast<Size>(1));
                    }

                    tree.set(init_index + right, 1);

                    last_position[data[right]] = right;

                    for (const auto& q : qs[right])
                    {
                        result[q.second] = tree.get(init_index + q.first, init_index + right);
                    }
                }

                return result;
            }
        }
    }
}