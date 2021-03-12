#include"segm_tree_sum_invert_slow.h"
#include"../Utilities/require_utilities.h"

namespace
{
    constexpr void check_range(const std::pair<std::size_t, std::size_t> &range, const std::size_t size)
    {
        const auto &left = range.first;
        const auto &ri_exclusive = range.second;

        const auto *const name = "slow right exclusive";

        Standard::Algorithms::require_greater(ri_exclusive, left, name);

        Standard::Algorithms::require_less_equal(ri_exclusive, size, name);
    }
} // namespace

Standard::Algorithms::Trees::segm_tree_sum_invert_slow::segm_tree_sum_invert_slow(const std::size_t size)
    : flags(require_positive(size, "size"))
{
}

void Standard::Algorithms::Trees::segm_tree_sum_invert_slow::clear() noexcept(false)
{
    // todo(p4): noexcept
    flags.assign(flags.size(), false);
}

void Standard::Algorithms::Trees::segm_tree_sum_invert_slow::set1(const std::size_t pos, const bool bit)
{
    check_range({ pos, pos + 1U }, flags.size());

    flags.at(pos) = bit;
}

void Standard::Algorithms::Trees::segm_tree_sum_invert_slow::invert_in_range(std::size_t left, std::size_t ri_exclusive)
{
    check_range({ left, ri_exclusive }, flags.size());

    while (left < ri_exclusive)
    {
        flags.at(left) = !flags.at(left);
        ++left;
    }
}

[[nodiscard]] auto Standard::Algorithms::Trees::segm_tree_sum_invert_slow::count_in_range(
    std::size_t left, std::size_t ri_exclusive) const -> std::size_t
{
    check_range({ left, ri_exclusive }, flags.size());

    std::size_t res{};

    while (left < ri_exclusive)
    {
        res += flags.at(left) ? 1U : 0U;
        ++left;
    }

    return res;
}
