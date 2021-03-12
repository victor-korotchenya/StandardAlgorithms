#include"enumerate_all_valid_brace_strings_tests.h"
#include"../Utilities/test_utilities.h"
#include"enumerate_all_valid_brace_strings.h"
#include<vector>

namespace
{
    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(
            std::string &&name, std::int32_t length, std::vector<std::string> &&expected_strings) noexcept
            : base_test_case(std::move(name))
            , Length(length)
            , Expected_strings(std::move(expected_strings))
        {
            std::sort(Expected_strings.begin(), Expected_strings.end());
        }

        [[nodiscard]] constexpr auto length() const noexcept -> std::int32_t
        {
            return Length;
        }

        [[nodiscard]] constexpr auto expected_strings() const &noexcept -> const std::vector<std::string> &
        {
            return Expected_strings;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            if (0 == Length)
            {
                if (1U == Expected_strings.size() && (*Expected_strings.cbegin()).empty()) [[likely]]
                {
                    return;
                }

                throw std::runtime_error("0 length must generate only an empty string.");
            }

            ::Standard::Algorithms::ert::greater(Expected_strings.size(), 0U, "ExpectedStrings.size()");

            for (const auto &str : Expected_strings)
            {
                ::Standard::Algorithms::ert::greater_or_equal(Length, static_cast<std::int32_t>(str.size()), str);
            }
        }

        void print(std::ostream &str) const override
        {
            str << ", Length " << Length;

            ::Standard::Algorithms::print<>("Expected strings", Expected_strings.cbegin(), Expected_strings.cend(), str);
        }

private:
        std::int32_t Length;
        std::vector<std::string> Expected_strings;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "Zero", 0, { "" } });
        test_cases.push_back({ "2", 2, { "", "()" } });
        test_cases.push_back({ "4", 4, { "", "()", "()()", "(())" } });

        const auto six = 6;
        test_cases.push_back(
            { "6", six, { "", "()", "()()", "(())", "()()()", "()(())", "(()())", "(())()", "((()))" } });
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<std::string> actual;

        auto callback = [&](const std::string &val) -> void
        {
            actual.push_back(val);
        };
        Standard::Algorithms::Strings::enumerate_all_valid_brace_strings(test.length(), callback);

        std::sort(actual.begin(), actual.end());
        ::Standard::Algorithms::ert::are_equal(test.expected_strings(), actual, "Generated strings");
    }
} // namespace

void Standard::Algorithms::Strings::Tests::enumerate_all_valid_brace_strings_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
