#include "../Utilities/Random.h"
#include "string_pattern_adjacent_swap_count_tests.h"
#include "string_pattern_adjacent_swap_count.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = int;
    constexpr int_t mod = 1000 * 1000 * 1000 + 7;

    struct test_case final : base_test_case
    {
        const int_t expected;

        test_case(string&& name,
            int_t expected)
            : base_test_case(forward<string>(name)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequireNonNegative(expected, "expected");
            RequireGreater(mod, expected, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_random(vector<test_case>& test_cases)
    {
        const auto test_max = max_element(test_cases.begin(), test_cases.end(),
            [](const test_case& t0, const test_case& t1) -> bool {
                return t0.get_Name().size() < t1.get_Name().size();
            });
        const auto n_max = test_cases.size() ? static_cast<int>(test_max->get_Name().size()) : 0;

        IntRandom r;
        const auto n = r(n_max + 1, n_max + 3);
        string s(n, 0);
        const array<char, 3> ar{ '0', '1', '?', };
        for (auto i = 0; i < n; ++i)
        {
            const auto m = r(0, static_cast<int>(ar.size()) - 1);
            s[i] = ar[m];
        }
        test_cases.emplace_back(move(s), 0);
    }

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.emplace_back("?0?", 3);
        test_cases.emplace_back("?00?", 5);
        test_cases.emplace_back("??", 1);
        test_cases.emplace_back("???", 6);
        test_cases.emplace_back("?11?", 5);
        test_cases.emplace_back("?01?", 5);
        test_cases.emplace_back("0", 0);
        test_cases.emplace_back("1", 0);
        test_cases.emplace_back("?", 0);
        generate_random(test_cases);
    }

    void run_test_case(const test_case& test_case)
    {
        const auto& pattern = test_case.get_Name();
        const auto actual = string_pattern_adjacent_swap_count<int_t, mod>(pattern);
        if (test_case.expected)
            Assert::AreEqual(test_case.expected, actual, "string_pattern_adjacent_swap_count");

        const auto slow = string_pattern_adjacent_swap_count_slow<int_t, mod>(pattern);
        Assert::AreEqual(actual, slow, "string_pattern_adjacent_swap_count_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::string_pattern_adjacent_swap_count_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}