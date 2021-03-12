#pragma once
#include"../Utilities/require_utilities.h"
#include"subset.h"

namespace Standard::Algorithms::Numbers
{
    // Generates all the subsets of a set of size n.
    template<std::integral int_t>
    struct subset_number final
    {
        subset_number() = delete;

        using sub_sets_t = typename subset<int_t>::sub_sets_t;
        using sub_sequence_t = typename subset<int_t>::sub_sequence_t;

        [[nodiscard]] static constexpr auto all_non_empty_sub_sets(
            // Because of the default value,
            // NOLINTNEXTLINE
            const int_t &setSize, // the warning "2 adjacent similar type parameters" is ignored.
            const int_t &minimum_value = {}) -> sub_sets_t
        {
            {
                constexpr int_t min_set_size{};
                constexpr int_t max_set_size = 63;

                require_between(min_set_size, setSize, max_set_size, "set size");
            }

            sub_sets_t result{};

            for (int_t size = 1; size <= setSize; ++size)
            {
                subset<int_t> sub_set{ sub_sequence_t{ minimum_value + size } };
                result.insert(std::move(sub_set));
            }

            return result;
        }
    };
} // namespace Standard::Algorithms::Numbers
