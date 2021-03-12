#include"fib3_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"default_modulus.h"
#include"fib3.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    constexpr auto not_computed = -1;
    constexpr auto mod = Standard::Algorithms::Numbers::default_modulus;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        // It makes little sense to use a pair here.
        // NOLINTNEXTLINE
        constexpr test_case(const long_int_t &size_, const int_t &expected_)
            : base_test_case(::Standard::Algorithms::Utilities::zu_string(size_))
            , Size(size_)
            , Expected(expected_)
        {
        }

        [[nodiscard]] constexpr auto size() const &noexcept -> const long_int_t &
        {
            return Size;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const int_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_non_negative(Size, "size");

            ::Standard::Algorithms::ert::greater_or_equal(Expected, not_computed, "expected");

            ::Standard::Algorithms::ert::greater(mod, Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("size", str, Size);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        long_int_t Size;
        int_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &tests)
    {
        // NOLINTNEXTLINE
        tests.emplace_back(1'000'000'000'000'000'000LL, 538'436'942);
        tests.emplace_back(0, 1);
        tests.emplace_back(1, 1);
        tests.emplace_back(2, 2);

        // NOLINTNEXTLINE
        tests.emplace_back(3, 4);
        // NOLINTNEXTLINE
        tests.emplace_back(4, 7);

        constexpr auto min_size = 5;
        constexpr auto max_size = 100;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            auto size = rnd();
            tests.emplace_back(size, not_computed);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::fib3<long_int_t, mod>(test.size());

        if (0 <= test.expected())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "fib3");
        }

        if (constexpr long_int_t large = 1'000; large <= test.size())
        {
            return;
        }

        const auto slow = Standard::Algorithms::Numbers::fib3_slow<long_int_t, mod>(test.size());

        ::Standard::Algorithms::ert::are_equal(fast, slow, "fib3_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::fib3_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
