#pragma once
#include"../Graphs/avl_tree.h"
#include"throw_exception.h"
#include<vector>

namespace Standard::Algorithms::Utilities
{
    // Return true only if there is a repetition.
    template<class item_t, class iterator_t = typename std::vector<item_t>::const_iterator,
        class get_value_t = const item_t &(*)(const item_t &), class hasher_t = std::hash<item_t>>
    [[nodiscard]] constexpr auto find_first_repetition(
        const iterator_t begin_inclusive, std::size_t &first_repeat_index, const iterator_t end_exclusive,
        const std::string &extra_error_message = "", const bool throw_on_exception = false,
        get_value_t get_value = [] [[nodiscard]] (const item_t &val) -> const item_t &
        {
            return val;
        }) -> bool
    {
        Standard::Algorithms::Trees::avl_tree<item_t> unique_values{};

        for (auto iter = begin_inclusive; end_exclusive != iter; ++iter)
        {
            const auto &item = get_value(*iter);

            const auto inserted = unique_values.insert(item);
            if (inserted.second) [[likely]]
            {
                continue;
            }

            first_repeat_index = iter - begin_inclusive;
            if (throw_on_exception)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The item '" << item << "' at index " << first_repeat_index << " is repeating.";

                throw_exception(str, extra_error_message);
            }

            return true;
        }

        return false;
    }
} // namespace Standard::Algorithms::Utilities
