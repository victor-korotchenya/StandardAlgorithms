#include"multiway_search_tree_tests.h"

[[nodiscard]] auto Standard::Algorithms::Trees::Tests::Inner::build_random_multi_dim_point(
    Standard::Algorithms::Utilities::random_t<std::int32_t> &rnd) noexcept(false) -> tuple_t
{
    static_assert(0 < dimensions);

    tuple_t point(dimensions); // might throw.

    for (std::int32_t dim{}; dim < dimensions; ++dim)
    {
        point[dim] = static_cast<key_t>(rnd());
    }

    return point;
}

void Standard::Algorithms::Trees::Tests::Inner::ensure_non_empty_range(tuple_t &low, tuple_t &high) noexcept
{
    static_assert(0 < dimensions);

    assert(!(low.size() < static_cast<std::size_t>(dimensions)));
    assert(!(high.size() < static_cast<std::size_t>(dimensions)));

    for (std::int32_t dim{}; dim < dimensions; ++dim)
    {
        auto &low_i = low[dim];
        auto &high_i = high[dim];

        if (high_i < low_i)
        {
            std::swap(high_i, low_i);
        }
    }
}
