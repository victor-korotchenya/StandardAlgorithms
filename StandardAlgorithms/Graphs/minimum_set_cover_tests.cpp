#include"minimum_set_cover_tests.h"
#include"../Utilities/test_utilities.h"
#include"graph_helper.h"
#include"minimum_set_cover.h"
#include<bit> // std::popcount

namespace
{
    using int_t = std::uint32_t;
    // todo(p3): using subset_t = std::flat_set<int_t>;
    using subset_t = std::vector<int_t>;
    using vec_subset_t = std::vector<subset_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(const int_t &atom_count, vec_subset_t &&subsets, const int_t &expected_mask)
            : base_test_case(::Standard::Algorithms::Utilities::zu_string(atom_count) + " atoms " +
                  ::Standard::Algorithms::Utilities::zu_string(expected_mask) + " expected mask")
            , Atom_count(atom_count)
            , Subsets(std::move(subsets))
            , Expected_mask(expected_mask)
        {
        }

#if defined(__GNUG__) && !defined(__clang__) // todo(p3): del this garbage after G++ is fixed.
        // Avoid an error: 'virtual constexpr {anonymous}::test_case::~test_case()' used before its definition.
        // The "= default" will not compile because of the bug. // NOLINTNEXTLINE
        constexpr ~test_case() noexcept override {}
#endif // todo(p3): End of: del this garbage after G++ is fixed.

        [[nodiscard]] constexpr auto atom_count() const noexcept -> int_t
        {
            return Atom_count;
        }

        [[nodiscard]] constexpr auto subsets() const &noexcept -> const vec_subset_t &
        {
            return Subsets;
        }

        [[nodiscard]] constexpr auto expected_mask() const noexcept -> int_t
        {
            return Expected_mask;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();
            {
                const auto expected_size = static_cast<std::size_t>(std::popcount(Expected_mask));
                Standard::Algorithms::require_less_equal(expected_size, Atom_count, "expected bit count vs atom count");
            }
        }

        void print(std::ostream &str) const override
        {// The atoms, expected mask are in the name.
            ::Standard::Algorithms::print(", subsets ", Subsets, str);
        }

private:
        int_t Atom_count;
        vec_subset_t Subsets;
        int_t Expected_mask;
    };

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        {
            constexpr int_t atoms = 0;
            tests.push_back(test_case{ atoms, vec_subset_t{}, 0U });
        }
        {
            constexpr int_t atoms = 1;
            tests.emplace_back(atoms, vec_subset_t{ { 0 } }, 1U);
        }
        {
            constexpr int_t atoms = 2;
            tests.emplace_back(atoms, vec_subset_t{ { 0, 1 }, { 1 } }, 1U);
            tests.emplace_back(atoms, vec_subset_t{ { 0 }, { 0, 1 } }, 2U);
            tests.emplace_back(atoms, vec_subset_t{ { 0 }, { 1 } }, 3U);
        }
        {
            constexpr int_t atoms = 3;
            tests.emplace_back(atoms, vec_subset_t{ { 0, 1, 2 } }, 1U);
            tests.emplace_back(atoms, vec_subset_t{ { 2 }, { 0, 1, 2 } }, 2U);
            // NOLINTNEXTLINE
            tests.emplace_back(atoms, vec_subset_t{ { 2 }, { 1, 2 }, { 0, 1, 2 } }, 4U);

            // NOLINTNEXTLINE
            tests.emplace_back(atoms, vec_subset_t{ { 0, 1 }, { 1 }, { 2 } }, 5U);
            // NOLINTNEXTLINE
            tests.emplace_back(atoms, vec_subset_t{ { 1 }, { 0, 1 }, { 1, 2 } }, 6U);
            // NOLINTNEXTLINE
            tests.emplace_back(atoms, vec_subset_t{ { 0 }, { 1 }, { 2 } }, 7U);
            // NOLINTNEXTLINE
            tests.emplace_back(atoms, vec_subset_t{ { 0 }, { 1 }, { 0, 1 }, { 0, 2 }, { 0, 1, 2 }, { 1, 2 } }, 16U);

            tests.emplace_back( // NOLINTNEXTLINE
                atoms, vec_subset_t{ { 0 }, { 1 }, { 2 }, { 0, 1 }, { 0, 2 }, { 0, 1, 2 }, { 1, 2 } }, 32U);
        }
        {
            // Initially, no subset is a part of another.
            // But 3 belongs to only 1 subset.
            // After removing that subset, both {0,1} and {0,2} become equal to {0}.
            constexpr int_t atoms = 4;

            // NOLINTNEXTLINE
            tests.emplace_back(atoms, vec_subset_t{ { 0, 1 }, { 0, 2 }, { 1, 2, 3 } }, 5U);
        }
        {// Choosing a maximal subset may be not good.
            // 0 - 3 --- 4 - 1
            //        \   /
            //         5 - 2
            constexpr int_t atoms = 6;

            // NOLINTNEXTLINE
            tests.emplace_back(atoms, vec_subset_t{ { 0, 3 }, { 1, 4 }, { 2, 5 }, { 3, 4, 5 } }, 7U);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto slow = Standard::Algorithms::Graphs::minimum_set_cover_slow(test.atom_count(), test.subsets());

        ::Standard::Algorithms::ert::are_equal(test.expected_mask(), slow, "minimum exact unweighted set cover slow");

        const auto expected_size = static_cast<std::size_t>(std::popcount(test.expected_mask()));
        {
            const auto approx =
                Standard::Algorithms::Graphs::minimum_set_cover_approximation(test.atom_count(), test.subsets());

            const auto approx_size = static_cast<std::size_t>(std::popcount(approx));

            Standard::Algorithms::require_between(expected_size, approx_size, test.atom_count(),
                "minimum_set_cover_approximation bits " + ::Standard::Algorithms::Utilities::zu_string(approx));
        }
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::minimum_set_cover_tests()
{
    static_assert(Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases));
}
