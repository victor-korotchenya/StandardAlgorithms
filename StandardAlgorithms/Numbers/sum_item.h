#pragma once
#include"../Utilities/require_utilities.h"

namespace Standard::Algorithms::Numbers
{
    // todo(p3): use or del?
    template<std::integral int_t, std::integral int_t_2 = int_t>
    struct sum_item final
    {
        int_t count{};
        int_t_2 value{};
    };

    // Sort by value.
    template<std::integral int_t, std::integral int_t_2>
    [[nodiscard]] constexpr auto operator<(
        const sum_item<int_t, int_t_2> &one, const sum_item<int_t, int_t_2> &two) noexcept -> bool
    {
        return one.value < two.value;
    }

    template<std::integral int_t, std::integral int_t_2>
    constexpr void validate(const sum_item<int_t, int_t_2> &item)
    {
        require_positive(item.count, "count");
        require_positive(item.value, "value");
    }
} // namespace Standard::Algorithms::Numbers
