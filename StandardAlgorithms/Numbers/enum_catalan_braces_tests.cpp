#include "enum_catalan_braces.h"
#include "enum_catalan_braces_tests.h"
#include "index_of_catalan_braces.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    int64_t catalan_naive(const int n)
    {
        assert(n > 0);

        int64_t r = 1;
        for (auto i = 1; i <= n; ++i)
            r = r * (i + n) / i;

        Assert::AreEqual(0, r % (n + 1), "Remainder");
        r /= n + 1;
        assert(r >= n);
        return r;
    }

    struct test_case final : base_test_case
    {
        const vector<string> expected;
        const int n;

        test_case(string&& name,
            vector<string>&& expected,
            const int n)
            : base_test_case(forward<string>(name)),
            expected(forward<vector<string>>(expected)),
            n(n)
        {
            Assert::Greater(n, 0, "n");
            const auto size = static_cast<int>(this->expected.size());
            const auto expected_size = catalan_naive(n);
            Assert::AreEqual(expected_size, size, "expected.size");

            for (auto i = 0; i < size; ++i)
            {
                const auto& s = this->expected[i];
                const auto s_len = static_cast<int>(s.size());
                const auto is = to_string(i);
                Assert::AreEqual(n << 1, s_len, "expected size at " + is);
                Assert::AreEqual(true, is_catalan_string(s), "is_catalan_string at " + is);
                if (i)
                    Assert::Greater(s, this->expected[i - 1], "Strings must increase at " + is);
            }
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            // n presents in name.
            ::Print(", expected=", expected, str);
        }
    };

    void generate_test_cases(vector<test_case>& tests)
    {
        {
            constexpr auto n = 1;
            tests.emplace_back(to_string(n), vector<string> {
                "[]",
            }, n);
        }
        {
            constexpr auto n = 2;
            tests.emplace_back(to_string(n), vector<string> {
                "[[]]",
                    "[][]",
            }, n);
        }
        {
            constexpr auto n = 3;
            tests.emplace_back(to_string(n), vector<string> {
                "[[[]]]",
                    "[[][]]",
                    "[[]][]",
                    "[][[]]",
                    "[][][]",
            }, n);
        }
    }

    void enum_catalan_braces_test(const test_case& test)
    {
        vector<string> actual;
        auto proc = [&actual](const string& s) -> void {
            actual.push_back(s);
        };

        enum_catalan_braces(proc, test.n);
        Assert::AreEqual(test.expected, actual, "enum_catalan_braces");
    }

    void next_catalan_string_test(const test_case& test)
    {
        string str(test.n, '[');
        str.resize(test.n << 1, ']');
        Assert::AreEqual(test.expected.front(), str, "Initial string");

        const auto size = static_cast<int>(test.expected.size());
        for (auto i = 0; i <= size; ++i)
        {
            const auto cur_ind = (i + 1) % size;
            const auto& cur = test.expected[cur_ind];

            const auto has = next_catalan_string(str);
            const auto cis = to_string(cur_ind);
            Assert::AreEqual(cur, str, "next_catalan_string at " + cis);

            const auto prev_ind = i % size;
            const auto has_expected = prev_ind != size - 1;
            Assert::AreEqual(has_expected, has, "next_catalan_string has at " + cis);
        }
    }

    void index_of_catalan_braces_test(const test_case& test)
    {
        const auto& expected = test.expected;
        assert(expected.size());

        const auto size = static_cast<int>(expected[0].size());
        const auto table = compute_catalan_brace_table(size);
        auto i = 0;
        string temp(size + 2, 0);
        for (const auto& str : expected)
        {
            const auto actual = index_of_catalan_braces(table, str);
            Assert::AreEqual(i, actual, "index_of_catalan_braces " + str);

            constexpr auto garbage = 'g';
            temp.assign(size + 2, garbage);
            brace_string_by_index_of_catalan(table, temp, i);
            Assert::AreEqual(str, temp, "brace_string_by_index_of_catalan " + to_string(i));

            ++i;
        }
    }

    void run_test_case(const test_case& test)
    {
        enum_catalan_braces_test(test);
        next_catalan_string_test(test);
        index_of_catalan_braces_test(test);
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void enum_catalan_braces_tests()
                {
                    assert(catalan_naive(1) == 1);
                    assert(catalan_naive(2) == 2);
                    assert(catalan_naive(3) == 5);
                    assert(catalan_naive(4) == 14);
                    assert(catalan_naive(5) == 42);
                    assert(catalan_naive(6) == 132);

                    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
                }
            }
        }
    }
}