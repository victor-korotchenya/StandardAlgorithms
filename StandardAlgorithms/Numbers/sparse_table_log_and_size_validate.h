#pragma once
#include"logarithm.h"

namespace Standard::Algorithms::Numbers
{
    namespace Inner
    {
        // log_size_max must be rounded up log(size_max, 2).
        [[nodiscard]] inline constexpr auto draft_sparse_table_log_and_size_check(
            const std::size_t log, const std::size_t size) noexcept -> bool
        {
            constexpr auto one = 1LLU;
            constexpr auto max_log = 29U; // todo(p3): test on 800GB RAM PC.

            auto valid = one < log && log <= max_log && one < size && size <= (one << log);

            return valid;
        }
    } // namespace Inner

    [[nodiscard]] inline constexpr auto sparse_table_log_and_size_validate(
        const std::size_t log, const std::size_t size) noexcept -> bool
    {
        auto valid = Inner::draft_sparse_table_log_and_size_check(log, size) &&
            !Inner::draft_sparse_table_log_and_size_check(log - 1U, size);

        return valid;
    }
} // namespace Standard::Algorithms::Numbers
