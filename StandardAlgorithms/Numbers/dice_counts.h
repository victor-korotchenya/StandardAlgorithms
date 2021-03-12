#pragma once
#include"arithmetic.h"
#include"xor_1.h"
#include<array>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // A dice is rolled size>0 times.
    // What are the counts of all possible sums?
    // E.g. after 1 throw, all sums 1..6 have count=1.
    template<class int_t>
    struct dice_counts final
    {
        dice_counts() = delete;

        // In the returned vector: the index is the sum, and the value is the count.
        [[nodiscard]] static constexpr auto calc_count_per_each_sum(const int_t &size) -> std::vector<int_t>
        {
            require_positive(size, "size");

            const auto max_size =
                add_unsigned(multiply_unsigned(static_cast<std::size_t>(sides), size), static_cast<std::size_t>(1));

            assert(static_cast<std::size_t>(sides) < max_size);

            using v_t = std::vector<int_t>;

            std::array<v_t, 2> buffer{ v_t(max_size), v_t(max_size) };

            for (std::int32_t index = 1; index <= sides; ++index)
            {
                buffer[0][index] = static_cast<int_t>(1);
            }

            std::int32_t curr{};

            for (int_t roll_index = 2; roll_index <= size; ++roll_index)
            {
                const auto futur = xor_1(curr);

                const auto sum_index_max = static_cast<int_t>(roll_index * static_cast<int_t>(sides));

                assert(int_t{} < sum_index_max);

                for (int_t sum_index{}; sum_index <= sum_index_max; ++sum_index)
                {
                    buffer[futur][sum_index] = 0;
                }

                for (auto sum_index = roll_index; sum_index <= sum_index_max; ++sum_index)
                {
                    const auto upper_index = std::min<int_t>(sides, sum_index);

                    for (int_t side_index = 1; side_index <= upper_index; ++side_index)
                    {
                        buffer[futur][sum_index] += buffer[curr][sum_index - side_index];
                    }
                }

                curr = futur;
            }

            return std::move(buffer[curr]);
        }

        static constexpr auto sides = 6;
    };
} // namespace Standard::Algorithms::Numbers
