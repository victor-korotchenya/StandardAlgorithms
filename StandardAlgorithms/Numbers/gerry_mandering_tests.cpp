#include"gerry_mandering_tests.h"
#include"../Utilities/test_utilities.h"
#include"arithmetic.h"
#include"gerry_mandering.h"
#include<unordered_set>

namespace
{
    using precinct = Standard::Algorithms::Numbers::precinct;
    using vec_precinct_t = std::vector<precinct>;
    using indexes_t = std::vector<std::size_t>;

    constexpr auto add_long_running = true;

    constexpr void test_indexes(
        const std::string &name, const vec_precinct_t &precincts, const indexes_t &chosen_indexes)
    {
        assert(!name.empty());
        assert(!precincts.empty());

        ::Standard::Algorithms::ert::are_equal(true, std::is_sorted(chosen_indexes.cbegin(), chosen_indexes.cend()),
            name + "chosen indexes must be sorted");

        std::pair<std::size_t, std::size_t> distr1{};
        std::pair<std::size_t, std::size_t> distr2{};

        const auto size = precincts.size();

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &precinct = precincts[index];

            const auto has = std::binary_search(chosen_indexes.cbegin(), chosen_indexes.cend(), index);

            auto &par = has ? distr1 : distr2;

            par.first = Standard::Algorithms::Numbers::add_unsigned(par.first, precinct.a_voters);

            par.second = Standard::Algorithms::Numbers::add_unsigned(par.second, precinct.b_voters);
        }

        Standard::Algorithms::require_greater(distr1.first, distr1.second, name + " A voters sum in district 1");

        Standard::Algorithms::require_greater(distr2.first, distr2.second, name + " A voters sum in district 2");
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, vec_precinct_t &&precincts, indexes_t &&expected_indexes = {},
            bool expected = {}) noexcept
            : base_test_case(std::move(name))
            , Precincts(std::move(precincts))
            , Expected_indexes(std::move(expected_indexes))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto precincts() const &noexcept -> const vec_precinct_t &
        {
            return Precincts;
        }

        [[nodiscard]] constexpr auto expected_indexes() const &noexcept -> const indexes_t &
        {
            return Expected_indexes;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> bool
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(Precincts.size(), "Precincts size");

            ::Standard::Algorithms::ert::are_equal(0U, size & 1U, "Precincts size evenness");

            // The same voter precinct sum check is done in the sol.

            const auto indexes_size = Expected_indexes.size();
            {
                const auto valid = Expected == (0U < indexes_size);

                ::Standard::Algorithms::ert::are_equal(true, valid, "valid test");
            }

            if (indexes_size == 0U)
            {
                return;
            }

            ::Standard::Algorithms::ert::are_equal(size >> 1U, indexes_size, "Expected indexes size");

            {
                const auto &maxi = *std::max_element(Expected_indexes.cbegin(), Expected_indexes.cend());

                Standard::Algorithms::require_greater(size, maxi, "max index");
            }

            const std::unordered_set<std::size_t> chosen_indexes(Expected_indexes.cbegin(), Expected_indexes.cend());

            ::Standard::Algorithms::ert::are_equal(indexes_size, chosen_indexes.size(), "Expected indexes uniqueness");

            test_indexes("validate", Precincts, Expected_indexes);
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Precincts", Precincts, str);
            ::Standard::Algorithms::print("Expected indexes", Expected_indexes, str);
            str << " Expected flag " << Expected;
        }

private:
        vec_precinct_t Precincts;
        indexes_t Expected_indexes;
        bool Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        if (add_long_running)
        {
            test_cases.emplace_back("Simple 1",
                vec_precinct_t({// NOLINTNEXTLINE
                    { 7, 4 }, { 3, 8 }, { 9, 2 }, { 5, 6 } }),
                //  {7, 4} + {5, 6}, {3, 8} + {9, 2}
                //  {12, 10}, {12, 10} // A wins in both districts.
                indexes_t({ 0, 3 }), true);

            // votersMinInclusive = 12 = (7+4)*4/4 + 1 = 11 + 1 = 12.
            //
            // row | initial_precincts | first_distr_sel_precincts | first_distr_a_voters | second_distr_a_voters |
            // case1 | case2 | from_row
            //
            // r0 | 0 | 0 | 0 | 0 // {7, 4}, {3, 8}, {9, 2}, {5, 6}
            // r1 | 1 | 1 | 7 | 0 | yes | no | r0 // 7
            // It is allowed because row
            //"r0 | 0 | 0 | 0 | 0" has been allowed.
            //
            // r2 | 2 | 1 | 7 | 3 | no | yes | r1 // 3
            // It is allowed because row
            //"r1 | 1 | 1 | 7 | 0" has been allowed.
            //
            // r3 | 2 | 2 | 10 | 0 | yes | no | r1 // 3
            //
            // r4 | 3 | 1 | 7 | 12 | no | yes | r2 // 9
            // r5 | 3 | 2 | 10 | 9 | no | yes | r3 // 9
            // r6 | 3 | 2 | 16 | 3 | yes | no | r2 // 9
            //
            // r7 | 4 | 1 | 7 | 17 | no | yes | r4 // 5
            //
            // Size | half size | see below.
            //
            // r8 | 4 | 2 | 10 | 14 | no | yes | r5 // 5
            // r9 | 4 | 2 | 12 | 12 | yes | no | r4 // The one and only winning position with last two >=
            // votersMinInclusive
            // === 12. r10 | 4 | 2 | 16 | 8 | no | yes | r6 // 5
            //
            //
            // Back tracing towards row 0:
            // r9 | 4 | 2 | 12 | 12 | yes | no | r4 // The winning position
            // r4 | 3 | 1 | 7 | 12 | no | yes | r2 // 9
            // r2 | 2 | 1 | 7 | 3 | no | yes | r1 // 3
            // r1 | 1 | 1 | 7 | 0 | yes | no | r0 // 7
            //
            //
            // Where case1 is true
            // r9 | 4 | 2 | 12 | 12 | yes | no | r4 // The winning position
            // r1 | 1 | 1 | 7 | 0 | yes | no | r0 // 7
            //
            //
            // So, the chosen indexes in the sorted order are: {1-1, 4-1} == {0, 3}.


            test_cases.emplace_back("Simple 2",
                vec_precinct_t({// NOLINTNEXTLINE
                    { 8, 4 }, { 4, 8 }, { 6, 6 }, { 10, 2 } }),
                // {8, 4} + {6, 6}, {4, 8} + {10, 2}
                // {14, 10}, {14, 10} // A wins.
                indexes_t({ 0, 2 }), true);
        }

        test_cases.emplace_back("Trivial win",
            vec_precinct_t({// NOLINTNEXTLINE
                { 4, 3 }, { 5, 2 } }), // win everywhere
            indexes_t({ 0 }), true);

        test_cases.emplace_back("Trivial win 1",
            vec_precinct_t({// NOLINTNEXTLINE
                { 1, 0 }, { 1, 0 } }),
            indexes_t({ 0 }), true);

        test_cases.emplace_back("Trivial loss 1",
            vec_precinct_t({// NOLINTNEXTLINE
                { 0, 1 }, { 0, 1 } }));

        test_cases.emplace_back("Trivial loss 2",
            vec_precinct_t({// NOLINTNEXTLINE
                { 1, 1 }, { 0, 2 } }));

        {
            constexpr auto min_loose = 1U;
            constexpr auto min_win = min_loose + 1U;

            test_cases.emplace_back("Trivial win 2",
                vec_precinct_t({// NOLINTNEXTLINE
                    { min_win, min_loose }, { min_win, min_loose } }), // win everywhere
                indexes_t({ 0 }), true);
        }
        {
            constexpr auto min_loose = 1U;
            constexpr auto min_win = 3U;

            test_cases.emplace_back("Trivial win 3",
                vec_precinct_t({// NOLINTNEXTLINE
                    { min_win, min_loose }, { min_win, min_loose } }), // win everywhere
                indexes_t({ 0 }), true);
        }

        test_cases.emplace_back("No half",
            vec_precinct_t({// NOLINTNEXTLINE
                { 6, 4 }, { 5, 5 } })); // Looses in the second district.

        test_cases.emplace_back("No half 2",
            vec_precinct_t({// NOLINTNEXTLINE
                { 3, 1 }, { 2, 2 } }));

        test_cases.emplace_back("No less",
            vec_precinct_t({// NOLINTNEXTLINE
                { 5, 3 }, { 2, 6 } }));

        test_cases.emplace_back("Win 2 out of 6",
            vec_precinct_t({// NOLINTNEXTLINE
                { 3, 2 }, { 3, 2 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 } }));

        test_cases.emplace_back("Win 3 out of 6",
            vec_precinct_t({// NOLINTNEXTLINE
                { 3, 2 }, { 3, 2 }, { 3, 2 }, { 2, 3 }, { 2, 3 }, { 2, 3 } }));

        test_cases.emplace_back("Win 4 out of 6",
            vec_precinct_t({// NOLINTNEXTLINE
                { 3, 2 }, { 3, 2 }, { 3, 2 }, { 3, 2 }, { 2, 3 }, { 2, 3 } }),
            indexes_t({ 0, 1, 4 }), true);

        test_cases.emplace_back("All same",
            vec_precinct_t({// NOLINTNEXTLINE
                { 2, 2 }, { 2, 2 } }));
    }

    void run_test_case(const test_case &test)
    {
        indexes_t chosen_indexes;

        const auto fast = Standard::Algorithms::Numbers::gerry_mandering_partition(test.precincts(), chosen_indexes);

        ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "gerry_mandering_partition flag");

        if (fast && test.expected_indexes() != chosen_indexes)
        {
            test_indexes("Fast", test.precincts(), chosen_indexes);
        }

        chosen_indexes.clear();

        const auto slow = Standard::Algorithms::Numbers::gerry_mandering_partition_slow(test.precincts(), chosen_indexes);

        ::Standard::Algorithms::ert::are_equal(fast, slow, "gerry_mandering_partition_slow flag");

        if (fast && test.expected_indexes() != chosen_indexes)
        {
            test_indexes("Slow", test.precincts(), chosen_indexes);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::gerry_mandering_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests<all_cores>(run_test_case, generate_test_cases);
}
