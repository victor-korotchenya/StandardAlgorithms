#pragma once
#include<cstddef>
#include<cstdint>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    [[nodiscard]] auto size_bytes(std::size_t bits) noexcept -> std::size_t;

    struct bit_set final
    {
        explicit bit_set(std::size_t bits = 1);

        void set(std::size_t bit);

        [[nodiscard]] auto has(std::size_t bit) const -> bool;

        [[nodiscard]] auto max_bits() const noexcept -> std::size_t;

private:
        std::vector<std::uint8_t> Data;
    };
} // namespace Standard::Algorithms::Numbers
