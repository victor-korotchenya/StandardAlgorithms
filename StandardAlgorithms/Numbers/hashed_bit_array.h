#pragma once
#include"bit_array.h"

namespace Standard::Algorithms::Numbers
{
    // The query "Has(hashCode)" returns either "possibly in set" or "definitely not in set" aka Bloom's filter.
    struct hashed_bit_array final
    {
        hashed_bit_array(std::size_t capacity, std::size_t bit_array_size, std::size_t number_of_hash_functions);

        explicit hashed_bit_array(std::size_t capacity);

        void add(std::size_t hash_code);

        [[nodiscard]] auto has(std::size_t hash_code) const -> bool;

        [[nodiscard]] inline constexpr auto bit_array_size() const noexcept -> std::size_t
        {
            return Bit_array_size;
        }

        [[nodiscard]] inline constexpr auto number_of_hash_functions() const noexcept -> std::size_t
        {
            return Hash_functions_size;
        }

        [[nodiscard]] auto count_set_bits() const -> std::uint64_t;

private:
        void init(std::size_t capacity, std::size_t bit_array_size, std::size_t number_of_hash_functions);

        bit_array Bit_array;
        std::size_t Bit_array_size{};
        std::size_t Hash_functions_size{};
    };
} // namespace Standard::Algorithms::Numbers
