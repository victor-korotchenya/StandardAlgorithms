#pragma once
#include"max_double_precision.h"
#include<iomanip> // std::setprecision
#include<locale>
#include<ostream>

namespace Standard::Algorithms::Utilities
{
    [[nodiscard]] auto fetch_locale() noexcept -> const std::locale &;

    inline void localize_stream(std::ostream &stream)
    {
        stream << std::setprecision(max_double_precision);
        stream.imbue(fetch_locale());
    };
} // namespace Standard::Algorithms::Utilities
