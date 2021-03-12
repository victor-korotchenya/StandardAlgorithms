#pragma once
// "check_chosen_sum.h"
#include"require_utilities.h"

namespace Standard::Algorithms::Utilities
{
    template<std::integral long_int_t>
    constexpr void check_chosen_sum(const std::string &name, const auto &all_items, const long_int_t &expected_sum = {},
        const auto &chosen_indexes = {})
    {
        assert(!name.empty());

        std::vector<bool> is_already_chosen(all_items.size());
        long_int_t total_sum{};

        for (const auto &index : chosen_indexes)
        {
            {
                using raw_index_t = decltype(index);
                using index_t = std::remove_cvref_t<raw_index_t>;
                static_assert(std::is_unsigned_v<index_t>);
            }

            require_greater(is_already_chosen.size(), index, name + " index");

            if (is_already_chosen[index]) [[unlikely]]
            {
                auto err = name + " The index " + std::to_string(index) + " is already chosen.";

                throw std::runtime_error(err);
            }

            is_already_chosen[index] = true;

            const auto &item = all_items[index];
            total_sum += static_cast<long_int_t>(item); // todo(p3): overflow check.
        }

        require_equal(expected_sum, name + " computed total sum", total_sum);
    }
} // namespace Standard::Algorithms::Utilities
