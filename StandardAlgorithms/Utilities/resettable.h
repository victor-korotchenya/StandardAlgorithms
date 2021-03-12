#pragma once
#include<concepts>

namespace Standard::Algorithms::Utilities
{// todo(p3): del file?
    template<class item_t>
    concept resettable = requires(item_t &item)
    // Avoid 70 chars long prefixes.
    // NOLINTNEXTLINE
    {
        {
            item.reset()
        } noexcept;
    };

    // Stack only storage.
    template<resettable item_t>
    struct resetter final
    {
        constexpr explicit resetter(item_t &item)
            : Item(item)
        {
        }

        resetter(const resetter &) = delete;
        auto operator= (const resetter &) & -> resetter & = delete;
        resetter(resetter &&) noexcept = delete;
        auto operator= (resetter &&) &noexcept -> resetter & = delete;

        constexpr ~resetter() noexcept
        {
            Item.reset();
        }

private:
        item_t &Item;
    };

    // Deduction guide.
    template<resettable item_t>
    resetter(item_t) -> resetter<item_t>;
} // namespace Standard::Algorithms::Utilities
