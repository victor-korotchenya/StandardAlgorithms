#pragma once
#include<cstdint>

namespace Standard::Algorithms::Utilities
{
    enum class [[nodiscard]] run_result_kind : std::uint8_t
    {
        okay,
        // The error has been processed, and re-thrown as launch_exception.
        error_in_functor_caught,
        // The error has been thrown by the function.
        error_in_functor_thrown,
        // The error has been thrown by the function, but
        //  could not be caught or there was not enough memory at the time.
        error_cannot_catch,
    };
} // namespace Standard::Algorithms::Utilities
