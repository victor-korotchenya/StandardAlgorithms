#include"digit_and_sum_of_ab_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"default_modulus.h"
#include"digit_and_sum_of_ab.h"
#include"factorials_cached.h"
#include<unordered_set>

namespace
{
    using int_t = std::uint32_t;
    using long_int_t = std::uint64_t;

    using factorials_t = Standard::Algorithms::Numbers::factorials_cached<int_t, long_int_t>;

    constexpr auto mod = Standard::Algorithms::Numbers::default_modulus;

    constexpr auto digit_max = Standard::Algorithms::Numbers::digit_max;
    constexpr int_t n_large = 3;
    constexpr int_t n_max = ::Standard::Algorithms::is_debug ? 3 : 6;

    constexpr auto random_tests = 1;
    constexpr auto max_attempts = 100;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, const factorials_t &factorials, int_t numb,
            const std::pair<std::int32_t, std::int32_t> &digits, int_t expected, bool has_result = true)
            : base_test_case(std::move(name))
            , Factorials(factorials)
            , Numb(numb)
            , Expected(expected)
            , Digit1(static_cast<char>(digits.first))
            , Digit2(static_cast<char>(digits.second))
            , Has_result(has_result)
        {
        }

        [[nodiscard]] constexpr auto factorials() const &noexcept -> const factorials_t &
        {
            return Factorials;
        }

        [[nodiscard]] constexpr auto numb() const &noexcept -> const int_t &
        {
            return Numb;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const int_t &
        {
            return Expected;
        }

        [[nodiscard]] constexpr auto digit1() const noexcept -> char
        {
            return Digit1;
        }

        [[nodiscard]] constexpr auto digit2() const noexcept -> char
        {
            return Digit2;
        }

        [[nodiscard]] constexpr auto has_result() const noexcept -> bool
        {
            return Has_result;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Numb, "numb");
            Standard::Algorithms::require_positive(Digit1, "digit1");
            Standard::Algorithms::require_positive(Digit2, "digit2");

            ::Standard::Algorithms::ert::greater_or_equal(n_max, Numb, "numb");

            ::Standard::Algorithms::ert::greater_or_equal(digit_max, Digit1, "digit1");

            ::Standard::Algorithms::ert::greater_or_equal(digit_max, Digit2, "digit2");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("numb", str, Numb);
            ::Standard::Algorithms::print_value("digit1", str, static_cast<std::int32_t>(Digit1));
            ::Standard::Algorithms::print_value("digit2", str, static_cast<std::int32_t>(Digit2));
            ::Standard::Algorithms::print_value("has result", str, Has_result);

            if (Has_result)
            {
                ::Standard::Algorithms::print_value("expected", str, Expected);
            }
        }

private:
        const factorials_t &Factorials;

        int_t Numb;
        int_t Expected;
        char Digit1;
        char Digit2;
        bool Has_result;
    };

    void gen_random(std::vector<test_case> &test_cases, const factorials_t &factorials)
    {
        std::unordered_set<int_t> uniq{};
        Standard::Algorithms::Utilities::random_t<int_t> rnd{};

        for (std::int32_t index{}, attempt{}; index < random_tests && attempt < max_attempts; ++attempt)
        {
            const auto base = rnd();

            constexpr int_t zero{};
            constexpr auto number_mask = 0x7U;

            const auto numb = static_cast<int_t>(base & number_mask);

            if (numb == zero || n_max < numb)
            {
                continue;
            }

            constexpr auto digit_mask = 0x15U;
            constexpr auto shift1 = 3U;
            constexpr auto shift2 = 7U;

            auto digit1 = static_cast<char>((base >> shift1) & digit_mask);
            auto digit2 = static_cast<char>((base >> shift2) & digit_mask);

            if (digit2 < digit1)
            {
                std::swap(digit2, digit1);
            }

            if (digit1 <= 0 || digit_max < digit2)
            {
                continue;
            }

            if (const auto key = static_cast<int_t>(
                    numb ^ (static_cast<int_t>(digit1) << shift1) ^ (static_cast<int_t>(digit2) << shift2));
                !uniq.insert(key).second)
            {
                continue;
            }

            test_cases.emplace_back(
                "Random" + std::to_string(index), factorials, numb, std::make_pair(digit1, digit2), 0, false);

            ++index;
            attempt = 0;
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases, const factorials_t &factorials)
    {
        if constexpr (!::Standard::Algorithms::is_debug)
        {
            test_cases.emplace_back("base0", factorials,
                // NOLINTNEXTLINE
                6, std::make_pair(6, 3), 7); // ds(366_666)=33 +6, ds(666_666)=36 +1.
        }

        test_cases.emplace_back("base1", factorials, 1, std::make_pair(1, 1), 1); // 1
        test_cases.emplace_back("base9", factorials, 1, std::make_pair(digit_max, digit_max), 1); // digit_max
        test_cases.emplace_back("base49", factorials, 1, std::make_pair(4, digit_max), 2); // 4, digit_max

        test_cases.emplace_back("base2", factorials, 3, std::make_pair(1, 1), 0);
        test_cases.emplace_back("base3", factorials, 3, std::make_pair(1, 3), 1); // 111
        test_cases.emplace_back("base4", factorials, 2, std::make_pair(1, 1), 0);

        // test_cases.emplace_back("base10", factorials, 10, std::make_pair(2, 3}, 165); // 45 seconds is too slow.
        // test_cases.emplace_back("base11", factorials, 11, std::make_pair(1, 1), 1); // 11111111111
        // todo(p4): make it run faster.

        gen_random(test_cases, factorials);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &factorials = test.factorials();

        const auto fast = Standard::Algorithms::Numbers::digit_and_sum_of_ab<long_int_t, int_t, factorials_t, mod>(
            factorials, test.digit1(), test.numb(), test.digit2());

        if (test.has_result())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "digit_and_sum_of_ab");
        }
        {
            const auto actual =
                Standard::Algorithms::Numbers::digit_and_sum_of_ab_slow_still<long_int_t, int_t, factorials_t, mod>(
                    factorials, test.digit1(), test.numb(), test.digit2());

            ::Standard::Algorithms::ert::are_equal(fast, actual, "digit_and_sum_of_ab_slow_still");
        }

        if (n_large < test.numb())
        {
            return;
        }

        const auto slow = Standard::Algorithms::Numbers::digit_and_sum_of_ab_slow<long_int_t, mod>(
            test.digit1(), test.numb(), test.digit2());

        ::Standard::Algorithms::ert::are_equal(fast, slow, "digit_and_sum_of_ab_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::digit_and_sum_of_ab_tests()
{
    const auto p10 = Standard::Algorithms::Numbers::Inner::power10_slow(n_max);

    const auto uniq_facts = std::make_unique<factorials_t>(p10);

    const auto &factorials = *throw_if_null("factorials", uniq_facts.get());

    Standard::Algorithms::Tests::test_utilities<test_case, const factorials_t &>::run_tests<all_cores>(
        run_test_case, generate_test_cases, factorials);
}
