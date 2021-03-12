#include "../Utilities/Random.h"
#include "min_swaps_between_permutations_tests.h"
#include "min_swaps_between_permutations.h"
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
        string_t source, target;
        int expected;

        test_case(string&& name,
            string_t&& source,
            string_t&& target,
            int expected = not_computed)
            : base_test_case(forward<string>(name)),
            source(forward<string_t>(source)),
            target(forward<string_t>(target)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int>(source.size());
            RequirePositive(size, "source.size");
            Assert::GreaterOrEqual(20, size, "source.size");
            Assert::AreEqual(size, static_cast<int>(target.size()), "target.size");

            RequireGreater(expected, not_computed - 1, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            PrintValue(str, "source", source);
            PrintValue(str, "target", target);
            PrintValue(str, "expected", expected);
        }
    };

    void add_random(vector<test_case>& test_cases)
    {
        IntRandom r;
        set<string_t> uniq;
        string_t source, target, sum;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 5;
            const auto n = r(2, size_max);

            source.resize(n);
            target.resize(n);
            for (auto i = 0; i < n; ++i)
                source[i] = target[i] = char(i + '1');

            ShuffleArray(source.data(), n);
            ShuffleArray(target.data(), n);

            sum.resize(n << 1);
            copy(source.begin(), source.end(), sum.begin());
            copy(target.begin(), target.end(), sum.begin() + n);

            if (uniq.insert(sum).second)
                test_cases.emplace_back("random" + to_string(att), move(source), move(target));
        }
    }

    void generate_test_cases(vector<test_case>& test_cases)
    {
        add_random(test_cases);

        test_cases.push_back({ "base 46", "4321", "1234", 6 });
        test_cases.push_back({ "base 2a", "2341", "2431", 1 });
        test_cases.push_back({ "base 2", "231", "312", 2 });
        test_cases.push_back({ "same 1", "1", "1", 0 });
        test_cases.push_back({ "same 2", "21", "21", 0 });
        test_cases.push_back({ "same 3", "312", "312", 0 });
        test_cases.push_back({ "base 1", "132", "312", 1 });
        test_cases.push_back({ "base11", "321", "312", 1 });
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = min_swaps_between_permutations<string_t>(test_case.source, test_case.target);
        if (test_case.expected != not_computed)
            Assert::AreEqual(test_case.expected, actual, "min_swaps_between_permutations");

        const auto still = min_swaps_between_permutations_slow_still<string_t>(test_case.source, test_case.target);
        Assert::AreEqual(actual, still.first, "min_swaps_between_permutations_slow_still");

        const auto slow = min_swaps_between_permutations_slow<string_t>(test_case.source, test_case.target);
        Assert::AreEqual(actual, slow.first, "min_swaps_between_permutations_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::min_swaps_between_permutations_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}