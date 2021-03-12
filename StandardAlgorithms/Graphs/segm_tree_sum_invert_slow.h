#pragma once
#include<cstddef>
#include<vector>

namespace Standard::Algorithms::Trees
{
    // It is slow - see "segm_tree_sum_invert".
    struct segm_tree_sum_invert_slow final
    {
        explicit segm_tree_sum_invert_slow(std::size_t size);

        void clear() noexcept(false);

        // Set the given bit.
        void set1(std::size_t pos, bool bit);

        [[nodiscard]] auto count_in_range(std::size_t left, std::size_t ri_exclusive) const -> std::size_t;

        void invert_in_range(std::size_t left, std::size_t ri_exclusive);

private:
        std::vector<bool> flags;
    };
} // namespace Standard::Algorithms::Trees
