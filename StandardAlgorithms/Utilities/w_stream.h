#pragma once
#include<sstream> // ostrstream, stringstream

namespace Standard::Algorithms::Utilities
{
    // auto str = ::Standard::Algorithms::Utilities::w_stream();
    [[nodiscard]] auto w_stream() -> std::ostringstream;

    void reset(std::ostringstream &str);
} // namespace Standard::Algorithms::Utilities
