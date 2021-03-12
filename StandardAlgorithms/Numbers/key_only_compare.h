#pragma once
#include<compare>
#include<concepts>
#include<ostream>

namespace Standard::Algorithms::Graphs
{
    // Unlike the pair, only the key participates in comparison, the value does not.
    template<class key_t1, class value_t1>
    requires(std::is_default_constructible_v<key_t1> && std::is_default_constructible_v<value_t1>)
    struct key_only_compare final
    {
        using key_t = key_t1;
        using value_t = value_t1;

        // NOLINTNEXTLINE
        key_t key{};

        // NOLINTNEXTLINE
        value_t value{};

        // Must be in.
        [[nodiscard]] constexpr auto operator<=> (const key_only_compare &other) const noexcept
        {
            return key <=> other.key;
        }

        // It is sad, but the equality operator must be copy-pasted as well. // todo(p3): check in C++26.
        [[nodiscard]] constexpr auto operator== (const key_only_compare &other) const noexcept -> bool
        {
            return key == other.key;
        }

        // Not sure that it is needed, but let it also be just in case.
        [[nodiscard]] constexpr auto operator<(const key_only_compare &other) const noexcept -> bool
        {
            return key < other.key;
        }
    };

    // Deduction guide.
    template<class key_t, class value_t>
    requires(std::is_default_constructible_v<key_t> && std::is_default_constructible_v<value_t>)
    key_only_compare(key_t, value_t) -> key_only_compare<key_t, value_t>;

    template<class key_t, class value_t>
    auto operator<< (std::ostream &str, const key_only_compare<key_t, value_t> &key_value) -> std::ostream &
    {
        str << "{" << key_value.key << ", " << key_value.value << "}";
        return str;
    }
} // namespace Standard::Algorithms::Graphs
