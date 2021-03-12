#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "text_justification.h"
#include "text_justificationTests.h"

using namespace std;
using namespace Privet::Algorithms::Strings::Tests;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using pair_t = pair<size_t, size_t>;

    struct test_case final : base_test_case
    {
        vector<size_t> WordLengths;
        size_t LineLength;
        size_t Expected_result;
        vector<pair_t> Expected_chosen_indexes;

        test_case(string&& name, vector<size_t>&& wordLengths, size_t lineLength,
            size_t expected_result, vector<pair_t>&& expected_chosen_indexes)
            : base_test_case(forward<string>(name)),
            WordLengths(forward<vector<size_t>>(wordLengths)), LineLength(lineLength),
            Expected_result(expected_result),
            Expected_chosen_indexes(forward<vector<pair_t>>(expected_chosen_indexes))
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            str << ", LineLength=" << LineLength
                << ", Expected_result=" << Expected_result;
            ::Print(", WordLengths=", WordLengths, str);
            ::Print(", Expected_chosen_indexes=", Expected_chosen_indexes, str);
        }

        void Validate() const override
        {
            base_test_case::Validate();
            Assert::Greater(WordLengths.size(), 0, "There must be at least one word.");
            Assert::Greater(Expected_chosen_indexes.size(), 0, "There must be at least one chosen index.");

            Assert::GreaterOrEqual(WordLengths.size(), Expected_chosen_indexes.size(),
                "Expected_chosen_indexes.size.");

            Assert::AreEqual(Expected_chosen_indexes[0].first, 0, "The first index must be 0.");

            Assert::AreEqual(WordLengths.size() - 1, Expected_chosen_indexes.back().second,
                "The last chosen index must be equal to (size-1).");

            for (size_t i = 1; i < Expected_chosen_indexes.size(); ++i)
            {
                Assert::Greater(Expected_chosen_indexes[i].first,
                    Expected_chosen_indexes[i - 1].second, "chosen_indexes at " + to_string(i));
            }
        }
    };

    void generate_test_cases(vector<test_case>& testCases)
    {
        testCases.push_back({ "two lines",{ 9,
            1, 2, 1, 1//8
        },
            10, 1 + 4,
            { pair_t(0, 0), pair_t(1, 4) } });

        testCases.push_back({ "one line",{ 1, 2, 1, 1//8
        },
            10, 4,
            { pair_t(0, 3) } });

        testCases.push_back({ "simple",{ 6,
          3, 5,
          2, 4 },
          10, 4 * 4 + 1 + 9,
          { pair_t(0, 0), pair_t(1, 2), pair_t(3, 4) } });

        testCases.push_back({ "three lines",{ 1, 2, 3, //8
          5, 3,//9
          1,1,2,1//8
        },
          10, 4 + 1 + 4,
          { pair_t(0, 2), pair_t(3, 4), pair_t(5, 8) } });
    }

    void run_test_case(const test_case& test)
    {
        {
            vector<pair_t> chosen_indexes;
            const auto actual = text_justification_line_word_breaking_wrapping<pair_t>(test.WordLengths,
                test.LineLength, chosen_indexes);

            Assert::AreEqual(test.Expected_result, actual, "result");
            Assert::AreEqual(test.Expected_chosen_indexes, chosen_indexes, "chosen_indexes");
        }
        {
            vector<pair_t> chosen_indexes;
            const auto slow = text_justification_line_word_breaking_wrapping_slow<pair_t>(test.WordLengths, test.LineLength, chosen_indexes);

            Assert::AreEqual(test.Expected_result, slow, "result_slow");
            Assert::AreEqual(test.Expected_chosen_indexes, chosen_indexes, "chosen_indexes_slow");
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            namespace Tests
            {
                void text_justificationTests()
                {
                    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
                }
            }
        }
    }
}