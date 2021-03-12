#pragma once
#include<cstddef>
#include<ostream>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    struct precinct final
    {
        std::size_t a_voters{};
        std::size_t b_voters{};
    };

    [[nodiscard]] auto sum(const precinct &preci) noexcept(false) -> std::size_t;

    auto operator<< (std::ostream &str, const precinct &preci) -> std::ostream &;

    // Whether the first party wins by manipulating voters. Actually, the people aren't stupid, but who would ask for
    // their opinion? There are 2 parties A and B, and a positive even number of precincts. Each precinct has the same
    // positive total number of voters. The aim is to divide the precincts into 2 equal halves (districts) so that the
    // first party wins in both. Pseudo-polynomial time, similar to subset sum problem. Time O(size^4 * (a+b)^2).
    [[nodiscard]] auto gerry_mandering_partition(
        const std::vector<precinct> &precincts, std::vector<std::size_t> &chosen_indexes) -> bool;

    // Slow time O((2*n choose n) * n).
    [[nodiscard]] auto gerry_mandering_partition_slow(
        const std::vector<precinct> &precincts, std::vector<std::size_t> &chosen_indexes) -> bool;
} // namespace Standard::Algorithms::Numbers
