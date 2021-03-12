#include"three_strings_interleaving_tests.h"
#include"../Utilities/test_utilities.h"
#include"three_strings_interleaving.h"

namespace
{
    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::string &&one, std::string &&two, std::string &&both,
            bool expected_result, std::vector<bool> &&expected_positions = {}) noexcept
            : base_test_case(std::move(name))
            , One(std::move(one))
            , Two(std::move(two))
            , Both(std::move(both))
            , Expected_result(expected_result)
            , Expected_positions(std::move(expected_positions))
        {
        }

        [[nodiscard]] constexpr auto one() const &noexcept -> const std::string &
        {
            return One;
        }

        [[nodiscard]] constexpr auto two() const &noexcept -> const std::string &
        {
            return Two;
        }

        [[nodiscard]] constexpr auto both() const &noexcept -> const std::string &
        {
            return Both;
        }

        [[nodiscard]] constexpr auto expected_positions() const &noexcept -> const std::vector<bool> &
        {
            return Expected_positions;
        }

        [[nodiscard]] constexpr auto one() &noexcept -> std::string &
        {
            return One;
        }

        [[nodiscard]] constexpr auto two() &noexcept -> std::string &
        {
            return Two;
        }

        [[nodiscard]] constexpr auto expected_positions() &noexcept -> std::vector<bool> &
        {
            return Expected_positions;
        }

        [[nodiscard]] constexpr auto expected_result() const noexcept -> bool
        {
            return Expected_result;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            if (Expected_result)
            {
                ::Standard::Algorithms::ert::are_equal(One.size() + Two.size(), Both.size(), "Both.size()");

                ::Standard::Algorithms::ert::are_equal(Expected_positions.size(), Both.size(), "Expected positions.size");

                std::string actual{};
                actual.reserve(Expected_positions.size() + 1U);

                for (std::size_t index{}, pos1{}, pos2{}; index < Expected_positions.size(); ++index)
                {
                    if (Expected_positions[index])
                    {
                        ::Standard::Algorithms::ert::greater(One.size(), pos1, "pos1");
                        actual += One[pos1++];
                    }
                    else
                    {
                        ::Standard::Algorithms::ert::greater(Two.size(), pos2, "pos2");
                        actual += Two[pos2++];
                    }
                }

                ::Standard::Algorithms::ert::are_equal(Both, actual, "Restored Both.");
            }
        }

        void print(std::ostream &str) const override
        {
            str << ", One " << One << ", Two " << Two << ", Both " << Both << ", Expected result " << Expected_result;

            ::Standard::Algorithms::print(
                ", Expected positions", Expected_positions.cbegin(), Expected_positions.cend(), str);
        }

private:
        std::string One;
        std::string Two;
        std::string Both;
        bool Expected_result;
        std::vector<bool> Expected_positions;
    };

    constexpr void add_reverted(std::vector<test_case> &tests)
    {
        ::Standard::Algorithms::ert::greater(tests.size(), 0U, "There must be tests to add reverted.");

        auto last = tests.back();
        last.name(last.name() + " reverted");
        std::swap(last.one(), last.two());
        last.expected_positions().flip();

        tests.push_back(std::move(last));
    }

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        tests.emplace_back(
            "Simple", "ab", "cde", "cadbe", true, std::vector<bool>({ false, true, false, true, false }));
        add_reverted(tests);

        tests.emplace_back("Trivial", "", "", "", true);

        tests.emplace_back("One empty", "", "a", "a", true, std::vector<bool>({ false }));
        add_reverted(tests);

        tests.emplace_back("Two same strings", "a", "a", "aa", true, std::vector<bool>({ true, false }));
        // add_reverted(tests);

        tests.emplace_back("Inside", "ab", "cd", "acdb", true, std::vector<bool>({ true, false, false, true }));
        add_reverted(tests);

        tests.emplace_back("Chess", "ab", "cd", "acbd", true, std::vector<bool>({ true, false, true, false }));
        add_reverted(tests);

        // No.
        tests.emplace_back("Size mismatch", "b", "a", "aab", false);
        add_reverted(tests);

        tests.emplace_back("Char Mismatch", "b", "a", "ac", false);
        add_reverted(tests);
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<bool> chosen_positions;

        const auto actual =
            ::Standard::Algorithms::Strings::is_interleaving(test.one(), test.two(), test.both(), chosen_positions);

        ::Standard::Algorithms::ert::are_equal(test.expected_result(), actual, "Result");

        ::Standard::Algorithms::ert::are_equal(test.expected_positions(), chosen_positions, "Positions");
    }
} // namespace

void Standard::Algorithms::Strings::Tests::three_strings_interleaving_tests()
{
    // todo(p3): static_assert
    (Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases));
}
