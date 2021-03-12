#pragma once
#include <exception>

namespace Privet::Algorithms::Utilities
{
    //It is thrown to indicate an error in a functor has been processed.
    //E.g. it is convenient to print the results as soon as they have become available.
    struct LaunchException final : std::exception
    {
        explicit LaunchException(const char* const& message = nullptr)
            : std::exception(message)
        {
        }

        explicit LaunchException(std::exception const& _Other)
            : std::exception(_Other)
        {
        }
    };
}