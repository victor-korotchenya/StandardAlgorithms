#pragma once
#include<stdexcept>
#include<string>

namespace Standard::Algorithms::Utilities
{
    // It is thrown to indicate that an error in a functor has been processed.
    struct launch_exception final : std::runtime_error
    {
        explicit launch_exception(const std::string &message);

        explicit launch_exception(const char *message);

        explicit launch_exception(const std::exception &other);
    };
} // namespace Standard::Algorithms::Utilities
