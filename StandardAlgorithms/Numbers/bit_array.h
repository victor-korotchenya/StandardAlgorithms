#pragma once
#include<cstddef>
#include<cstdint>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Might be not optimal for sizes, not divisible by 64 bits - up to 7 bytes space waste.
    // Also, keep in mind the second and further vector resizings.
    struct bit_array final
    {
        explicit bit_array(std::size_t initial_size_bits = 0);

        void resize(std::size_t new_size_bits);

        void set_bit(std::size_t position);

        [[nodiscard]] auto get_bit(std::size_t position) const -> std::uint8_t;

        [[nodiscard]] auto count_set_bits() const -> std::uint64_t;

private:
        std::vector<std::uint64_t> Data{};
    };
} // namespace Standard::Algorithms::Numbers
