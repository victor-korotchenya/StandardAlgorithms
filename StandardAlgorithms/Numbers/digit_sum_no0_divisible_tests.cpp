#include"digit_sum_no0_divisible_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"digit_sum_no0_divisible.h"
#include"hash_utilities.h"
#include<map>
#include<unordered_map>

namespace
{
    using int_t = std::int64_t;
    using tuple_t = std::tuple<int_t, int_t, std::int32_t>;
    using random_t = Standard::Algorithms::Utilities::random_t<std::uint32_t>;

    constexpr auto sorted_slow = false;

    using map_t = std::conditional_t<sorted_slow,
        // slow:
        std::map<tuple_t, int_t>,
        // fast:
        std::unordered_map<tuple_t, int_t, Standard::Algorithms::Numbers::tuple_hash>>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, int_t numb, const std::pair<int_t, int_t> &sum_divisor, int_t expected) noexcept
            : base_test_case(std::move(name))
            , Numb(numb)
            , Sum(sum_divisor.first)
            , Divisor(sum_divisor.second)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto numb() const noexcept -> int_t
        {
            return Numb;
        }

        [[nodiscard]] constexpr auto sum() const noexcept -> int_t
        {
            return Sum;
        }

        [[nodiscard]] constexpr auto divisor() const noexcept -> int_t
        {
            return Divisor;
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> int_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Numb, "numb");
            Standard::Algorithms::require_positive(Sum, "sum");
            Standard::Algorithms::require_positive(Divisor, "divisor");

            Standard::Algorithms::require_non_negative(Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("numb", str, Numb);
            ::Standard::Algorithms::print_value("sum", str, Sum);
            ::Standard::Algorithms::print_value("divisor", str, Divisor);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        int_t Numb;
        int_t Sum;
        int_t Divisor;
        int_t Expected;
    };

    [[nodiscard]] constexpr auto gen1(int_t &numb, map_t &uniq, int_t &sum, random_t &rnd, int_t &divisor,
        std::vector<char> &digits, map_t &map1, int_t &expected) -> bool
    {
        constexpr auto max_attemps = 100;

        for (std::int32_t att{}; att < max_attemps; ++att)
        {
            if (numb <= 0)
            {
                constexpr auto mask = 0x3fFfU;

                numb = static_cast<int_t>(rnd() & mask);
                continue;
            }

            if (sum <= 0)
            {
                constexpr auto mask = 0xfU;

                sum = static_cast<int_t>(rnd() & mask);
                continue;
            }

            if (divisor <= 0)
            {
                constexpr auto mask = 0x3fU;

                divisor = static_cast<int_t>(rnd() & mask);
                continue;
            }

            expected = Standard::Algorithms::Numbers::digit_sum_no0_divisible<int_t>(numb, map1, sum, digits, divisor);

            const tuple_t key{ numb, sum, static_cast<std::int32_t>(divisor) };

            if (0 < expected && uniq.insert({ key, expected }).second)
            {
                return true;
            }

            numb = sum = divisor = 0;
        }

        return false;
    }

    void gen_random(std::vector<test_case> &test_cases)
    {
        map_t uniq{};
        random_t rnd{};
        std::vector<char> digits{};
        map_t map1{};

        constexpr auto max_attemps = 1;

        for (std::int32_t att{}; att < max_attemps; ++att)
        {
            int_t numb{};
            int_t sum{};
            int_t divisor{};
            int_t expected{};

            if (gen1(numb, uniq, sum, rnd, divisor, digits, map1, expected))
            {
                test_cases.emplace_back("Random" + std::to_string(att), numb, std::make_pair(sum, divisor), expected);
            }
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("base1",
            // NOLINTNEXTLINE
            100, std::make_pair(9, 4), 2); // 36, 72

        test_cases.emplace_back("base2",
            // NOLINTNEXTLINE
            100, std::make_pair(8, 2), 4); // 8, 26, 44, 62

        test_cases.emplace_back("base3",
            // NOLINTNEXTLINE
            200, std::make_pair(4, 11), 2); // 22, 121

        gen_random(test_cases);
    }

    void run_test_case(const test_case &test)
    {
        std::vector<char> digits;
        map_t map1;

        const auto fast =
            Standard::Algorithms::Numbers::digit_sum_no0_divisible(test.numb(), map1, test.sum(), digits, test.divisor());

        ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "digit_sum_no0_divisible");

        const auto slow =
            Standard::Algorithms::Numbers::digit_sum_no0_divisible_slow(test.numb(), test.sum(), test.divisor());

        ::Standard::Algorithms::ert::are_equal(fast, slow, "digit_sum_no0_divisible_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::digit_sum_no0_divisible_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests<all_cores>(run_test_case, generate_test_cases);
}
