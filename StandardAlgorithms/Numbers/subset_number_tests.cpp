#include"subset_number_tests.h"
#include"../Utilities/test_utilities.h"
#include"subset_number.h"

namespace
{
    using int_t = std::uint32_t;
    using long_int_t = std::uint64_t;
    using subset_t = Standard::Algorithms::Numbers::subset<int_t>;
    using sub_sets_t = subset_t::sub_sets_t;
    using sub_sequence_t = subset_t::sub_sequence_t;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        test_case(const int_t &set_size, sub_sets_t &&expected) noexcept(false)
            : base_test_case(::Standard::Algorithms::Utilities::zu_string(set_size))
            , Set_size(set_size)
            , Expected(std::move(expected))
        {
        }

        [[nodiscard]] constexpr auto set_size() const &noexcept -> const int_t &
        {
            return Set_size;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const sub_sets_t &
        {
            return Expected;
        }

        void validate() const override
        {
            base_test_case::validate();

            {
                constexpr int_t max_size = 20;

                Standard::Algorithms::require_less_equal(Set_size, max_size, "Set size");
            }

            constexpr long_int_t one = 1;

            const auto size = Expected.size();
            const auto size_lower_bound = (one << Set_size) - one;

            ::Standard::Algorithms::ert::greater_or_equal(size, size_lower_bound, "Sub sets lower bound");
        }

        void print(std::ostream &str) const override
        {
            const subset_t expected_sub_set{ Expected };

            str << ", Set size " << Set_size << ", Expected " << expected_sub_set;
        }

private:
        int_t Set_size;
        sub_sets_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        // test_cases.emplace_back(3, sub_sets_t{
        // Sub-sequences:
        // { 1 }, { 2 }, { 3 },
        // { 1, 2 }, { 1, 3 }, { 2, 3 },
        // { 1, 2, 3 },
        // Sub-sets:
        //  size 2:
        // { {1}, {2} }, { {1}, {3} }, { {2}, {3} },
        // { {1}, {2,3} }, { {2}, {1,3} }, { {3}, {1,2} },
        //  size 3:
        // { {1}, {2}, {3} } });

        test_cases.emplace_back(2,
            sub_sets_t{ subset_t{ sub_sequence_t{ 1 } }, //
                subset_t{ sub_sequence_t{ 2 } }, //
                subset_t{ sub_sequence_t{ 1, 2 } }, //
                subset_t{ sub_sets_t{ subset_t{ sub_sequence_t{ 1 } }, subset_t{ sub_sequence_t{ 2 } } } } });

        test_cases.emplace_back(1, sub_sets_t{ subset_t{ sub_sequence_t{ 1 } } });

        test_cases.emplace_back(0, sub_sets_t{});
    }

    void run_test_case(const test_case &test)
    {
        const subset_t expected_sub_set{ test.expected() };

        const auto actuals = Standard::Algorithms::Numbers::subset_number<int_t>::all_non_empty_sub_sets(test.set_size());
        const subset_t actual{ actuals };

        ::Standard::Algorithms::ert::are_equal(expected_sub_set, actual, "all_non_empty_sub_sets");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::subset_number_tests()
{
    // todo(p3): finish.
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
