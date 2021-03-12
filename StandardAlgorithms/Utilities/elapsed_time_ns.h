#pragma once
// "elapsed_time_ns.h"
#include"floating_point_type.h"
#include<cassert>
#include<chrono>
#include<concepts>
#include<cstdint>

namespace Standard::Algorithms
{
    using ns_elapsed_type = std::int64_t;

    // Measure the elapsed time in nanoseconds.
    struct elapsed_time_ns final
    {
        [[nodiscard]] inline auto elapsed() const &noexcept -> ns_elapsed_type
        {
            const auto now = std::chrono::high_resolution_clock::now();

            auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now - Start).count();

            return nanoseconds;
        }

private:
        std::chrono::time_point<std::chrono::high_resolution_clock> Start = std::chrono::high_resolution_clock::now();
    };

    // Compute the ratio.
    template<std::integral int_t, std::floating_point floating_t = floating_point_type>
    [[nodiscard]] constexpr auto ratio_compute(const int_t &one, const int_t &two) noexcept -> floating_t
    {
        assert(&one != &two);

        constexpr int_t zero{};

        auto ratio = zero < one ? static_cast<floating_t>(two) / static_cast<floating_t>(one) : floating_t{};

        return ratio;
    }
} // namespace Standard::Algorithms
