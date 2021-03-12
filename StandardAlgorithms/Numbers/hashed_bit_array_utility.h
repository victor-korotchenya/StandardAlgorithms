#pragma once
#include"../Utilities/floating_point_type.h"
#include<cstddef>

namespace Standard::Algorithms::Numbers
{
    struct hashed_bit_array_utility final
    {
        using floating_t = Standard::Algorithms::floating_point_type;

        hashed_bit_array_utility() = delete;

        [[nodiscard]] static auto error_probability(std::size_t capacity) -> floating_t;

        [[nodiscard]] static auto bit_array_size(std::size_t capacity, const floating_t &error_probability)
            -> std::size_t;

        [[nodiscard]] static auto number_of_hash_functions(std::size_t capacity, std::size_t bit_array_size)
            -> std::size_t;

        static void check_capacity(std::size_t value);

        static void check_bit_array_size(std::size_t value);

        static void check_error_probability(const floating_t &value);

        static void check_number_of_hash_functions(std::size_t capacity, std::size_t bit_array_size, std::size_t value);
    };
} // namespace Standard::Algorithms::Numbers
