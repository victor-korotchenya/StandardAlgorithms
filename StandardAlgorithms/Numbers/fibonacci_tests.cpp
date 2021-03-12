#include"fibonacci_tests.h"
#include"../Utilities/test_utilities.h"
#include"fibonacci.h"
#include<initializer_list>
#include<vector>

namespace
{
    using int_t = std::int64_t;
    using number_cache_t = std::unordered_map<int_t, int_t>;

    constexpr auto use_long_int_liberty = false;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(number_cache_t &number_cache,
            // Suppress 2 adjacent similar type parameters.
            // NOLINTNEXTLINE
            const int_t &value_, const int_t &expected_)
            : base_test_case(::Standard::Algorithms::Utilities::zu_string(value_))
            , Number_cache(number_cache)
            , Value(value_)
            , Expected(expected_)
        {
        }

        [[nodiscard]] constexpr auto number_cache() const &noexcept -> number_cache_t &
        {
            return Number_cache;
        }

        [[nodiscard]] constexpr auto value() const &noexcept -> const int_t &
        {
            return Value;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const int_t &
        {
            return Expected;
        }

        void print(std::ostream &str) const override
        {
            str << ", Value " << Value << ", Expected " << Expected;
        }

private:
        number_cache_t &Number_cache;
        int_t Value;
        int_t Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases, number_cache_t &number_cache)
    {
        const std::initializer_list<int_t> some_values{// NOLINTNEXTLINE
            0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144
        };

        int_t index{};

        for (const auto &value : some_values)
        {
            test_cases.emplace_back(number_cache, index, value);

            ++index;
        }

        // NOLINTNEXTLINE
        test_cases.emplace_back(number_cache, 38, 39088169);

        if constexpr (use_long_int_liberty)
        {
            // todo(p3): use/write a long integer mult/div.

            // NOLINTNEXTLINE
            test_cases.emplace_back(number_cache, 100, int_t("354224848179261915075"));

            // NOLINTNEXTLINE
            test_cases.emplace_back(number_cache, 1024,
                int_t("450669963367781981310438323572888604936786059621860483080302314960003064570872139624879260914103"
                      "0396"
                      "244873266580345011219530209367425581019871067646094200262285202346655868899711089246778413354004"
                      "1036"
                      "31553925405243"));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        constexpr int_t none = -1;

        auto actual = none;

        Standard::Algorithms::Numbers::fibonacci<int_t>::fast<int_t>(test.value(), test.number_cache(), actual);

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "Fast");

        if (constexpr int_t small = 10; small < test.value())
        {
            return;
        }

        actual = none;

        Standard::Algorithms::Numbers::fibonacci<int_t>::slow(test.value(), actual);

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "Slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::fibonacci_tests()
{
    number_cache_t number_cache{};

    Standard::Algorithms::Tests::test_utilities<test_case, number_cache_t &>::run_tests(
        run_test_case, generate_test_cases, std::ref(number_cache));
}
