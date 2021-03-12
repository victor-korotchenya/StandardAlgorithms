#pragma once
#include<cstddef>
#include<cstdint>

namespace Standard::Algorithms::Strings
{
    // For "001245670" of size 9,
    // the key_positions will have 8 entries e.g. ['0'] = {0, 1, 8}.
    template<class string_t, class map_t>
    constexpr void map_letter_to_positions(const string_t &str, map_t &key_positions)
    {
        key_positions.clear();

        for (std::size_t index{}; const auto &cha : str)
        {
            key_positions[cha].push_back(index++);
        }
    }
} // namespace Standard::Algorithms::Strings
