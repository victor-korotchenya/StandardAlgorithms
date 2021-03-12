#pragma once
#include<stdexcept>
#include<string>

namespace Standard::Algorithms::Numbers
{
    struct arithmetic_exception final : std::runtime_error
    {
        inline explicit arithmetic_exception(const std::string &message)
            : std::runtime_error(message)
        {
        }

        inline explicit arithmetic_exception(const char *const message)
            : std::runtime_error(message)
        {
        }
    };
} // namespace Standard::Algorithms::Numbers
