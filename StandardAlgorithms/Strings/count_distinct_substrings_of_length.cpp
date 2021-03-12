#include <cassert>
#include <set>
#include "../Utilities/ExceptionUtilities.h"
#include "../Assert.h"
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"
#include "count_distinct_substrings_of_length.h"

using namespace std;
using namespace Privet::Algorithms::Strings::Tests;
using namespace Privet::Algorithms::Strings;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    void check_length(const int size, const int length, const char* const name)
    {
        if (length <= 0 || size < length)
        {
            ostringstream ss;
            ss << "The " << name << " (" << length << ") is out of bounds.";
            StreamUtilities::throw_exception(ss);
        }
    }

    struct TestCase final : base_test_case
    {
        const string data;
        const vector<int> lengths;
        const vector<int> expected;

        TestCase(string&& name = {},
            string&& str1 = {},
            vector<int>&& lengths1 = {},
            vector<int>&& expected1 = {})
            : base_test_case(forward<string>(name)),
            data(forward<string>(str1)),
            lengths(forward<vector<int>>(lengths1)),
            expected(forward<vector<int>>(expected1))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(data.size(), "string size.");
            RequirePositive(lengths.size(), "lengths size.");
            Assert::AreEqual(expected.size(), lengths.size(), "lengths and expected sizes.");
            const auto size = static_cast<int>(lengths.size());
            for (auto i = 0; i < size; i++)
            {
                check_length(size, lengths[i], "lengths");
                check_length(size, expected[i], "expected");
            }
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            ::PrintValue(str, ", data", data);
            Privet::Algorithms::Print(", lengths", lengths, str);
            Privet::Algorithms::Print("expected", expected, str);
        }
    };

    template <long long base, long long mod>
    vector<int> calc_powers(const int size)
    {
        assert(0 < size);
        vector<int> result(size + 1);
        result[0] = 1;
        for (auto i = 0; i < size; ++i)
            result[i + 1] = static_cast<int>(result[i] * base % mod);

        return result;
    }

    template <long long base, long long mod>
    vector<int> calc_hashes(const string& str)
    {
        const auto size = static_cast<int>(str.size());
        vector<int> result(size + 1);
        result[0] = 0;
        for (auto i = 0; i < size; ++i)
            result[i + 1] = static_cast<int>((result[i] * base + str[i]) % mod);

        return result;
    }

    template <long long mod>
    int calc_hash(const vector<int>& hashes, const vector<int>& powers, const int from_inc, const int to_exc)
    {
        assert(0 <= from_inc);
        assert(from_inc < to_exc);
        assert(to_exc < static_cast<int>(hashes.size()));

        const auto mul = static_cast<long long>(powers[to_exc - from_inc]) * hashes[from_inc] % mod, dif = (hashes[to_exc] + mod - mul) % mod,
            result = dif;
        if (0 == from_inc)
            assert(hashes[to_exc] == result);

        return static_cast<int>(result);
    }

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        testCases.push_back({
          "simple", "abbab",
          {1,2,3,4,5},
          {2,3,3,2,1} });

        IntRandom random;
        //Might not work for large long strings.
        const auto size = random(1, 10);
        assert(0 < size);
        string str(size, 1);
        vector<int> lengths(size);
        for (auto i = 0; i < size; ++i)
        {
            str[i] = static_cast<char>(random('A', 'z'));
            lengths[i] = i + 1;
        }

        auto expected = count_distinct_substrings_of_length_slow(str, lengths);

        testCases.push_back({ "random", move(str), move(lengths), move(expected) });
    }

    void RunTestCase(const TestCase& testCase)
    {
        {
            const auto actual = count_distinct_substrings_of_length_slow(testCase.data, testCase.lengths);
            Assert::AreEqual(testCase.expected, actual, "slow");
        }
        {
            const auto actual = count_distinct_substrings_of_length(testCase.data, testCase.lengths);
            Assert::AreEqual(testCase.expected, actual, "fast");
        }
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            vector<int> count_distinct_substrings_of_length_slow(
                const string& str, const vector<int>& lengths)
            {
                RequirePositive(str.size(), "str.size()");
                RequirePositive(lengths.size(), "lengths.size()");

                const auto strSize = static_cast<int>(str.size()),
                    resultSize = static_cast<int>(lengths.size());

                vector<int> result(resultSize);
                set<string> se;
                for (auto i = 0; i < resultSize; ++i)
                {
                    const auto& len = lengths[i];
                    const auto max_start = strSize - len;
                    for (auto j = 0; j <= max_start; ++j)
                        se.insert(str.substr(j, len));

                    result[i] = static_cast<int>(se.size());
                    se.clear();
                }

                return result;
            }

            vector<int> count_distinct_substrings_of_length(
                const string& str, const vector<int>& lengths)
            {
                RequirePositive(str.size(), "str.size()");
                RequirePositive(lengths.size(), "lengths.size()");

                const auto strSize = static_cast<int>(str.size()),
                    resultSize = static_cast<int>(lengths.size());

                vector<int> result(resultSize);
                constexpr long long base = 999999937, mod = static_cast<int>(1e9) + 7;
                const auto powers = calc_powers<base, mod>(strSize);
                const auto hashes = calc_hashes<base, mod>(str);
                set<int> se;
                for (auto i = 0; i < resultSize; ++i)
                {
                    const auto& len = lengths[i];
                    const auto max_start = strSize - len;
                    for (auto j = 0; j <= max_start; ++j)
                    {
                        const auto h = calc_hash<mod>(hashes, powers, j, j + len);
                        se.insert(h);
                    }

                    result[i] = static_cast<int>(se.size());
                    se.clear();
                }
                return result;
            }

            void Tests::count_distinct_substrings_of_length_tests()
            {
                test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
            }
        }
    }
}