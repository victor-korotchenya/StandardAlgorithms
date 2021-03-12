#pragma once
#include <chrono>

namespace Privet::Algorithms
{
    // Measure the elapsed time in nanoseconds.
    struct elapsed_time_ns final
    {
        inline int64_t elapsed() const
        {
            const auto now = std::chrono::high_resolution_clock::now();
            const auto el = std::chrono::duration_cast<std::chrono::nanoseconds>(now - t0).count();
            return el;
        }

    private:
        const std::chrono::steady_clock::time_point t0 = std::chrono::high_resolution_clock::now();
    };
}