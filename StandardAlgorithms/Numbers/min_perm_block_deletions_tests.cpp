#include"min_perm_block_deletions_tests.h"
#include"../Utilities/test_utilities.h"
#include"min_perm_block_deletions.h"

namespace
{
    using int_t = std::int32_t;
    using test_t = std::pair<std::vector<int_t>, std::size_t>;

    [[nodiscard]] constexpr auto build_tests() -> std::vector<test_t>
    {
        return {// NOLINTNEXTLINE
            { { 0, 4, 2, 1, 3, 5 }, 4 }, // {1}, {2,3}, {4,5}, {0}
            // NOLINTNEXTLINE
            { { 4, 3, 2, 1, 0 }, 5 }, // NOLINTNEXTLINE
            { { 3, 1, 0, 2 }, 3 }, // NOLINTNEXTLINE
            { { 0, 3, 1, 2 }, 2 }, // NOLINTNEXTLINE
            { { 0, 1, 2, 3 }, 1 }
        };
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::min_perm_block_deletions_tests()
{
    const auto tests = build_tests();
    std::size_t ide{};

    for (const auto &[input, expected] : tests)
    {
        const auto actual = min_perm_block_deletions<int_t>(input);

        ::Standard::Algorithms::ert::are_equal(expected, actual.first, "min_perm_block_deletions " + std::to_string(ide));

        // todo(p3): check the actual erase sequence.

        ++ide;
    }
}
