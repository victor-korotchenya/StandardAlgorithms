#pragma once
#include"../Utilities/throw_exception.h"
#include<set>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // todo(p3): test.
    template<class item1, class item2, class result_item = std::pair<item1, item2>>
    [[nodiscard]] constexpr auto join_to_set(const std::vector<item1> &values1, const std::vector<item2> &values2)
        -> std::set<result_item>
    {
        const auto size = values1.size();

        if (const auto size2 = values2.size(); size2 < size)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The values1.size (" << size << ") is greater than values2.size (" << size2 << ").";

            throw_exception<std::out_of_range>(str);
        }

        // Remove duplicates, sort.
        std::set<result_item> result;

        for (std::size_t index{}; index < size; ++index)
        {
            result.emplace(values1[index], values2[index]);
        }

        return result;
    }
} // namespace Standard::Algorithms::Numbers
