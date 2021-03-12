#pragma once
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../Utilities/StreamUtilities.h"

//todo: p2. test.

template<typename Item1, typename Item2,
    typename ResultItem = std::pair<Item1, Item2 >
>
std::set<ResultItem> join_to_set(
    const std::vector<Item1>& values1,
    const std::vector<Item2>& values2)
{
    const auto size = values1.size();
    if (values2.size() < size)
    {
        std::ostringstream ss;
        ss << "The values1.size (" << size
            << ") is greater than values2.size (" << values2.size()
            << ").";
        StreamUtilities::throw_exception<std::out_of_range>(ss);
    }

    //Remove duplicates, sort.
    std::set<ResultItem> result;
    for (size_t i = 0; i < size; ++i)
    {
        result.insert(ResultItem(values1[i], values2[i]));
    }

    return result;
}