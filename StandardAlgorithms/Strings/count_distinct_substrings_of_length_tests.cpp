#include"count_distinct_substrings_of_length_tests.h"
#include"../Utilities/check_size.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"count_distinct_substrings_of_length.h"
#include<set>

namespace
{
    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::string &&str, std::vector<std::int32_t> &&lengths,
            std::vector<std::int32_t> &&expected) noexcept
            : base_test_case(std::move(name))
            , Data(std::move(str))
            , Lengths(std::move(lengths))
            , Expected(std::move(expected))
        {
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::string &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto lengths() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Lengths;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Data.size(), "string size.");
            Standard::Algorithms::require_positive(Lengths.size(), "lengths size.");
            ::Standard::Algorithms::ert::are_equal(Expected.size(), Lengths.size(), "lengths and expected sizes.");

            const auto size = Standard::Algorithms::Utilities::check_size<std::int32_t>("lengths size", Lengths.size());

            for (std::int32_t index{}; index < size; ++index)
            {
                Standard::Algorithms::require_between(1, Lengths[index], size, "lengths");
                Standard::Algorithms::require_between(1, Expected[index], size, "expecteds");
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value(", data", str, Data);
            ::Standard::Algorithms::print(", lengths", Lengths, str);
            ::Standard::Algorithms::print("expected", Expected, str);
        }

private:
        std::string Data;
        std::vector<std::int32_t> Lengths;
        std::vector<std::int32_t> Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        {
            std::string val = "abbab";
            const auto len = static_cast<std::int32_t>(val.size());

            test_cases.emplace_back("simple", std::move(val), std::vector<std::int32_t>({ 1, 2, 3, 4, len }),
                std::vector<std::int32_t>({ 2, 3, 3, 2, 1 }));
        }

        constexpr auto min_cha = 'A';
        constexpr auto max_cha = 'z';
        static_assert(min_cha < max_cha);

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_cha, max_cha);

        constexpr auto min_size = 1;
        constexpr auto max_size = 10; // Might not work for large strings.
        static_assert(min_size < max_size);

        const auto size = rnd(min_size, max_size);
        assert(0 < size);

        std::string str(size, 1);
        std::vector<std::int32_t> lengths(size);

        for (std::int32_t index{}; index < size; ++index)
        {
            str[index] = static_cast<char>(rnd());
            lengths[index] = index + 1;
        }

        auto expected = Standard::Algorithms::Strings::count_distinct_substrings_of_length_slow(str, lengths);

        test_cases.emplace_back("Random", std::move(str), std::move(lengths), std::move(expected));
    }

    void run_test_case(const test_case &test)
    {
        {
            const auto actual =
                Standard::Algorithms::Strings::count_distinct_substrings_of_length_slow(test.data(), test.lengths());

            ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "count_distinct_substrings_of_length_slow");
        }
        {
            const auto actual =
                Standard::Algorithms::Strings::count_distinct_substrings_of_length(test.data(), test.lengths());

            ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "count_distinct_substrings_of_length");
        }
    }
} // namespace

void Standard::Algorithms::Strings::Tests::count_distinct_substrings_of_length_tests()
{
    ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
