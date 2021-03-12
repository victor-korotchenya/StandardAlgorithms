#pragma once
// "demand_char_unique.h"
#include<algorithm>
#include<cassert>
#include<cstddef>
#include<stdexcept>
#include<string>

namespace Standard::Algorithms::Strings
{
    constexpr void demand_item_unique(const std::string &error, const auto &container1, const auto &unique_item)
    {
        assert(!error.empty());

        const auto iter = std::find(container1.begin(), container1.end(), unique_item);

        if (container1.end() == iter) [[likely]]
        {
            return;
        }

        throw std::invalid_argument(error);
    }

    constexpr void demand_last_char_unique(const auto &str)
    {
        auto size = str.size();
        if (size-- < 2U)
        {
            return;
        }

        const auto &back = str.back();

        for (std::size_t index{}; index < size; ++index)
        {
            if (str[index] == back) [[unlikely]]
            {
                throw std::invalid_argument(
                    "The last char '" + std::to_string(back) + "' is duplicated at " + std::to_string(index));
            }
        }
    }
} // namespace Standard::Algorithms::Strings
