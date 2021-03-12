#include"subset_tests.h"
#include"../Utilities/test_utilities.h"
#include"subset.h"
#include<vector>

namespace
{
    using int_t = std::uint16_t;
    using sub_set_t = Standard::Algorithms::Numbers::subset<int_t>;
    using sub_sets_t = sub_set_t::sub_sets_t;
    using sub_sequence_t = sub_set_t::sub_sequence_t;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        test_case(std::string &&name, sub_set_t &&sub_set) noexcept
            : base_test_case(std::move(name))
            , Input(std::move(sub_set))
        {
        }

        [[nodiscard]] constexpr auto input() const &noexcept -> const sub_set_t &
        {
            return Input;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            std::size_t count1{};
            auto is_ok = false;

            for (const auto &name = this->name(); const auto &cha : name)
            {
                if ('{' == cha)
                {
                    is_ok = true;
                    ++count1;
                }
                else if ('}' == cha)
                {
                    Standard::Algorithms::require_positive(count1, "Count closing parenthesis");
                    --count1;
                }
            }

            ::Standard::Algorithms::ert::are_equal(0U, count1, "final count");
            ::Standard::Algorithms::ert::are_equal(true, is_ok, "missing parenthesis");
        }

        void print(std::ostream &str) const override
        {
            str << ", Input " << Input;
        }

private:
        sub_set_t Input;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr int_t one = 4;
        constexpr int_t two = 30;
        constexpr int_t three = 218;
        constexpr int_t four{ 1579 };
        static_assert(one < two && two < three && three < four);

        const auto one_s = ::Standard::Algorithms::Utilities::zu_string(one);
        const auto two_s = ::Standard::Algorithms::Utilities::zu_string(two);
        const auto three_s = ::Standard::Algorithms::Utilities::zu_string(three);
        const auto four_s = ::Standard::Algorithms::Utilities::zu_string(four);

        // The empty set.
        test_cases.emplace_back("{}", sub_set_t{});

        // Singles.
        test_cases.emplace_back("{" + one_s + "}", sub_set_t{ sub_sequence_t{ one } });

        test_cases.emplace_back("{" + two_s + "}", sub_set_t{ sub_set_t{ sub_sequence_t{ two } } });

        test_cases.emplace_back("{" + three_s + "}", sub_set_t{ sub_set_t{ sub_set_t{ sub_sequence_t{ three } } } });

        test_cases.emplace_back(
            "{" + four_s + "}", sub_set_t{ sub_set_t{ sub_set_t{ sub_set_t{ sub_sequence_t{ four } } } } });

        // Doubles.
        test_cases.emplace_back("{" + one_s + ", " + two_s + "}",
            sub_set_t{ sub_sequence_t{// Notice that the order is reversed.
                two, one } });

        test_cases.emplace_back(
            "{" + one_s + ", " + four_s + "}", sub_set_t{ sub_set_t{ sub_sequence_t{ four, one } } });

        // todo(p3): printout is not sorted.
        test_cases.emplace_back("{{" + three_s + "}, {" + one_s + "}}",
            sub_set_t{ sub_sequence_t{ three }, { sub_set_t{ sub_sequence_t{ one } } } });

        test_cases.emplace_back("{{" + two_s + "}, {" + four_s + "}}",
            sub_set_t{ sub_sets_t{ sub_set_t{ sub_sequence_t{ four } }, sub_set_t{ sub_sequence_t{ two } } } });

        // Triplets.
        test_cases.emplace_back(
            "{" + one_s + ", " + two_s + ", " + three_s + "}", sub_set_t{ sub_sequence_t{ three, one, two } });

        test_cases.emplace_back("{{" + two_s + "}, {" + three_s + "}, {" + four_s + "}}",
            sub_set_t{ sub_sets_t{ sub_set_t{ sub_sequence_t{ three } }, sub_set_t{ sub_sequence_t{ four } },
                sub_set_t{ sub_sequence_t{ two } } } });

        test_cases.emplace_back("{{" + three_s + "}, {" + one_s + "}, {" + two_s + "}}",
            sub_set_t{ sub_sequence_t{ three },
                sub_sets_t{ sub_set_t{ sub_sequence_t{ two } }, sub_set_t{ sub_sequence_t{ one } } } });

        test_cases.emplace_back("{{" + four_s + "}, {" + one_s + ", " + three_s + "}}",
            sub_set_t{ sub_sequence_t{ four }, sub_sets_t{ sub_set_t{ sub_sequence_t{ three, one } } } });

        // 4-tuples
        test_cases.emplace_back("{{" + one_s + "}, {" + two_s + "}, {" + three_s + ", " + four_s + "}}",
            sub_set_t{ sub_sequence_t{ one },
                sub_sets_t{ sub_set_t{ sub_sequence_t{ four, three } }, sub_set_t{ sub_sequence_t{ two } } } });

        test_cases.emplace_back("{{" + four_s + "}, {{" + two_s + ", " + three_s + "}, {" + one_s + "}}}",
            sub_set_t{ sub_sequence_t{ four },
                sub_sets_t{
                    sub_set_t{ sub_sequence_t{ three, two }, sub_sets_t{ sub_set_t{ sub_sequence_t{ one } } } } } });

        // todo(p3): finish.
    }

    void run_test_case(const test_case &test)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << test.input();

        const auto &expected = test.name();
        const auto actual = str.str();
        ::Standard::Algorithms::ert::are_equal(expected, actual, "string representation");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::subset_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
