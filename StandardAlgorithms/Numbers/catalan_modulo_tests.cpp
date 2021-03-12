#include"catalan_modulo_tests.h"
#include"../Utilities/test_utilities.h"
#include"catalan_modulo.h"

namespace
{
    using int_t = std::uint8_t;
    using long_int_t = std::uint16_t;
    using input_output_t = std::pair<int_t, long_int_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, const int_t &max_number, std::vector<input_output_t> &&sub_cases,
            const int_t &modulus) noexcept
            : base_test_case(std::move(name))
            , Max_number(max_number)
            , Modulus(modulus)
            , Sub_cases(std::move(sub_cases))
        {
        }

        [[nodiscard]] constexpr auto max_number() const &noexcept -> const int_t &
        {
            return Max_number;
        }

        [[nodiscard]] constexpr auto modulus() const &noexcept -> const int_t &
        {
            return Modulus;
        }

        [[nodiscard]] constexpr auto sub_cases() const &noexcept -> const std::vector<input_output_t> &
        {
            return Sub_cases;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();
            Standard::Algorithms::require_positive(Max_number, "Max number");
            Standard::Algorithms::require_positive(Modulus, "Modulus");
            Standard::Algorithms::require_positive(Sub_cases.size(), "Sub cases size");

            constexpr auto max_possible_modulus = static_cast<int_t>(0U - static_cast<int_t>(1));

            ::Standard::Algorithms::ert::greater_or_equal(max_possible_modulus, Modulus, "maxPossibleModulus");

            for (const auto &sub_case : Sub_cases)
            {
                ::Standard::Algorithms::ert::greater_or_equal(Max_number, sub_case.first, "Max number");

                ::Standard::Algorithms::ert::greater(Modulus, sub_case.second, "Modulus");
            }
        }

        void print(std::ostream &str) const override
        {
            str << ", Max number " << static_cast<std::uint64_t>(Max_number) << ", Modulus "
                << static_cast<std::uint64_t>(Modulus);

            ::Standard::Algorithms::print("Sub cases", Sub_cases, str);
        }

private:
        int_t Max_number;
        int_t Modulus;
        std::vector<input_output_t> Sub_cases;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        const std::vector<std::uint64_t> first_few_numbers{// NOLINTNEXTLINE
            1, 1, 2, 5, 14,
            // 5
            // NOLINTNEXTLINE
            42, 132, 429, 1430,
            // 9
            // NOLINTNEXTLINE
            4862, 16796, 58786, 208012,
            // 13
            // NOLINTNEXTLINE
            742900, 2674440, 9694845, 35357670,
            // 17
            // NOLINTNEXTLINE
            129644790, 477638700, 1767263190, 6564120420,
            // 21
            // NOLINTNEXTLINE
            24466267020, 91482563640, 343059613650, 1289904147324,
            // 25
            // NOLINTNEXTLINE
            4861946401452
        };

        const auto size = first_few_numbers.size();

        constexpr int_t zero{};
        constexpr auto one = static_cast<int_t>(1);
        constexpr auto two = static_cast<int_t>(2);
        constexpr auto three = static_cast<int_t>(3);

        test_cases.push_back({ "Simplest", one, { { zero, one }, { one, one } }, two });

        test_cases.push_back({ "Simplest2", two, { { zero, one }, { one, one }, { two, zero } }, two });

        test_cases.push_back(
            { "Simplest3", three, { { zero, one }, { one, one }, { two, two }, { three, two } }, three });

        {
            constexpr int_t some_prime = 251;

            const auto some_index = static_cast<int_t>(size - 1);

            const auto some_value = static_cast<int_t>(first_few_numbers.at(some_index) % some_prime);

            constexpr auto before_prime_index = static_cast<int_t>(some_prime - 1);
            constexpr auto before_prime_value = before_prime_index;

            constexpr auto last_index = std::numeric_limits<int_t>::max();
            constexpr int_t last_value = 28;

            test_cases.push_back({ "Greatest", last_index,
                { { zero, one }, { one, one }, { two, two },
                    // NOLINTNEXTLINE
                    { three, static_cast<int_t>(5) }, { some_index, some_value },
                    { before_prime_index, before_prime_value }, { last_index, last_value } },
                some_prime });
        }
        {
            constexpr int_t some_prime = 251;
            static_assert(Standard::Algorithms::Numbers::is_prime_simple(some_prime));

            const auto max_number = static_cast<int_t>(size - 1);

            std::vector<input_output_t> subs(size);

            for (int_t index{}; index <= max_number; ++index)
            {
                const auto expected = static_cast<int_t>(first_few_numbers.at(index) % some_prime);

                subs[index] = { index, expected };
            }

            test_cases.emplace_back("Large", max_number, std::move(subs), some_prime);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const Standard::Algorithms::Numbers::catalan_modulo<int_t, long_int_t> catalan(test.max_number(), test.modulus());

        for (const auto &sub_case : test.sub_cases())
        {
            const auto actual = catalan.get(sub_case.first);
            const auto name = std::to_string(sub_case.first);

            ::Standard::Algorithms::ert::are_equal(sub_case.second, actual, name);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::catalan_modulo_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests<all_cores>(run_test_case, generate_test_cases);
}
