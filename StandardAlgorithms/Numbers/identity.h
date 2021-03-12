#pragma once

namespace Standard::Algorithms::Numbers
{
    // Be sure to specify a reference "&" when using a vector or a similar thick type.
    template<class typ, bool is_noexcept = false>
    struct identity final
    {
        [[nodiscard]] constexpr auto operator() (typ value) const noexcept(is_noexcept) -> typ
        {
            return value;
        }
    };
} // namespace Standard::Algorithms::Numbers
