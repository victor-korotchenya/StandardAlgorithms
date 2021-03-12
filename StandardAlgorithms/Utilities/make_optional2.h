#pragma once
#include<algorithm>
#include<cassert>
#include<concepts> // std::convertible_to
#include<optional>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Utilities
{
    template<class value_t>
    [[nodiscard]] [[gnu::pure]] constexpr auto make_optional2(const bool has_value, value_t &&value)
    {
        return has_value ? std::make_optional(std::forward<value_t>(value)) : std::nullopt;
    }

    template<class value_t>
    requires(requires(value_t value) {
                 {
                     value.empty()
                     } -> std::convertible_to<bool>;
             })
    [[nodiscard]] [[gnu::pure]] constexpr auto make_optional2(value_t &&value)
    {
        const auto has_value = !value.empty();

        return has_value ? std::make_optional(std::forward<value_t>(value)) : std::nullopt;
    }
} // namespace Standard::Algorithms::Utilities
