#pragma once
#include"../Utilities/is_debug.h"
#include<cassert>
#include<cstddef>
#include<cstdint>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // todo(p4): enum class?
    constexpr std::int32_t white{};
    constexpr std::int32_t grey = -1;
    constexpr std::int32_t black = 1;

    // Let's save (8 - 2) = 6 bits of RAM per node.
    // todo(p3): 1 of 4 positions is not used - optimize more.
    struct color_2bit final
    {
        // Serves as a ctor (constructor).
        inline constexpr void white(std::size_t size)
        {
            size *= 2U;
            colors.assign(size, false);

            verify_internals();
        }

        [[nodiscard]] inline constexpr auto color(std::size_t node) const noexcept(!::Standard::Algorithms::is_debug)
            -> std::int32_t
        {
            verify_internals();

            node *= 2U;
            assert(node < colors.size() && node + 1LLU < colors.size());

            const auto low = colors[node];
            const auto high = colors[node + 1U];

            auto col = (high ? -2 : 0) + (low ? 1 : 0);

            assert(-1 <= col && col <= 1);

            return col;
        }

        inline constexpr void specific(const std::pair<std::size_t, std::int32_t> &node_color) noexcept(
            !::Standard::Algorithms::is_debug)
        {
            verify_internals();

            auto [node, col] = node_color;
            node *= 2U;

            assert(node < colors.size() && node + 1LLU < colors.size());
            assert(-1 <= col && col <= 1);

            const auto low = 0 != col; // 1
            const auto high = col < 0; // -2

            colors[node] = low;
            colors[node + 1U] = high;

            verify_internals();
        }

private:
        inline constexpr void verify_internals() const noexcept
        {
            if constexpr (!::Standard::Algorithms::is_debug)
            {
                return;
            }

            assert((colors.size() & 1U) == 0U); // The size must be even.
        }

        std::vector<bool> colors{};
    };
} // namespace Standard::Algorithms::Numbers
