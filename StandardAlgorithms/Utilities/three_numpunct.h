#pragma once
// "three_numpunct.h"
#include<locale>

namespace Standard::Algorithms
{
    // To print 1000 as 1,000.
    struct three_numpunct final : std::numpunct<char>
    {
        [[nodiscard]] constexpr auto do_thousands_sep() const -> char_type override
        {
            return ',';
        }

        [[nodiscard]] constexpr auto do_grouping() const -> string_type override
        {
            return "\3";
        }
    };
} // namespace Standard::Algorithms
