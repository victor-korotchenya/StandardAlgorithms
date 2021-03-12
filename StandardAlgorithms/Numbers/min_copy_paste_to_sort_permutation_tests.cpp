#include "../Utilities/Random.h"
#include "min_copy_paste_to_sort_permutation_tests.h"
#include "min_copy_paste_to_sort_permutation.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using string_t = string;
    constexpr auto not_computed = -1;

    struct test_case final : base_test_case
    {
        string_t source;
        int expected;

        test_case(string&& name,
            string_t&& source,
            int expected = not_computed)
            : base_test_case(forward<string>(name)),
            source(forward<string_t>(source)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int>(source.size());
            RequirePositive(size, "source.size");
            Assert::GreaterOrEqual(20, size, "source.size");

            RequireGreater(expected, not_computed - 1, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            PrintValue(str, "source", source);
            PrintValue(str, "expected", expected);
        }
    };

    void add_random(vector<test_case>& test_cases)
    {
        IntRandom r;
        string_t source;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 5;
            const auto n = r(2, size_max);

            source.resize(n);
            for (auto i = 0; i < n; ++i)
                source[i] = char(i + '1');

            ShuffleArray(source.data(), n);

            test_cases.emplace_back("random" + to_string(att), move(source));
        }
    }

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "base 53", "54321", 3 });
        add_random(test_cases);
        test_cases.push_back({ "base 43", "4321", 3 });
        test_cases.push_back({ "base 42", "4132", 2 });
        test_cases.push_back({ "base 41", "2341", 1 });
        test_cases.push_back({ "base 32", "321", 2 });
        test_cases.push_back({ "base 31", "132", 1 });
        test_cases.push_back({ "base 3a", "231", 1 });

        test_cases.push_back({ "same 1", "1", 0 });
        test_cases.push_back({ "same 5", "12345", 0 });
        test_cases.push_back({ "same 2", "12", 0 });
        test_cases.push_back({ "same 3", "123", 0 });
        test_cases.push_back({ "same 4", "1234", 0 });
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = min_copy_paste_to_sort_permutation<string_t>(test_case.source);
        if (test_case.expected != not_computed)
            Assert::AreEqual(test_case.expected, actual.first, "min_copy_paste_to_sort_permutation");

        const auto slow = min_copy_paste_to_sort_permutation_slow<string_t>(test_case.source);
        Assert::AreEqual(actual.first, slow.first, "min_copy_paste_to_sort_permutation_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::min_copy_paste_to_sort_permutation_tests()
{
    test_utilities<test_case>::run_tests<0>(run_test_case, generate_test_cases);
}