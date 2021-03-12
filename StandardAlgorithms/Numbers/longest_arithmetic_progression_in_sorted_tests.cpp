//#define print_subseq 1
#if print_subseq
#include <iostream>
#include <chrono>
#endif

#include "longest_arithmetic_progression_in_sorted.h"
#include "longest_arithmetic_progression_in_sorted_tests.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"
#include "../Utilities/Random.h"

#if print_subseq
using namespace std::chrono;
#endif

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    template<class t>
    void demand_subsequence(const vector<t>& ar, const vector<t>& subseq, const string& name)
    {
        assert(name.size());
        if (subseq.empty())
            return;

        Assert::Greater(ar.size(), 0, "array size " + name);

        for (size_t i = 0, j = 0;;)
        {
            if (ar[i] == subseq[j] && ++j == subseq.size())
                return;

            if (++i == ar.size())
                throw runtime_error("expected " + to_string(subseq[j]) +
                    " must have a match in ar. " + name);
        }
    }

    template<class int_t2, class int_t>
    void compare_increasing_subsequences(const vector<int_t>& expected, const vector<int_t>& actual, const vector<int_t>& ar, const string& name)
    {
        static_assert(sizeof(int_t) <= sizeof(int_t));

        if (expected == actual)
            return;

        if (expected.size() != actual.size())
            Assert::AreEqual(expected, actual, name);

        demand_subsequence(ar, expected, "compare_increasing_subsequences expected " + name);
        demand_subsequence(ar, actual, "compare_increasing_subsequences actual " + name);
        if (actual.size() <= 2)
            return;

        const auto diff = static_cast<int_t2>(actual[0]) - actual[1];
        const auto size = static_cast<int>(actual.size());

        for (auto i = 1; i < size - 1; ++i)
        {
            const auto d = static_cast<int_t2>(actual[i]) - actual[i + 1];
            Assert::AreEqual(diff, d, to_string(i) + " " + name);
        }
    }

    using int_t = int;
    using int_t2 = int64_t;

    struct test_case final : base_test_case
    {
        vector<int_t> ar, expected;

        test_case(
            string&& name,
            vector<int_t>&& ar,
            vector<int_t>&& expected = {})
            : base_test_case(forward<string>(name)),
            ar(forward<vector<int_t>>(ar)), expected(forward<vector<int_t>>(expected))
        {
            sort(this->ar.begin(), this->ar.end());
            sort(this->expected.begin(), this->expected.end());
        }

        void Validate() const override
        {
            Assert::GreaterOrEqual(ar.size(), expected.size(), "ar vs expected");
            demand_subsequence(ar, expected, "test validate");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            AppendSeparator(str);
            ::Print("ar", ar, str);
            ::Print("expected", expected, str);
        }
    };

    void generate_test_cases(
        vector<test_case>& test_cases)
    {
        test_cases.push_back({ "s2",{ 3, 4, 5, 7, 9 }, { 3,5,7,9 } });
        test_cases.push_back({ "sa",{ 0, 3, 4, 5, 7, 9 }, { 3,5,7,9 } });
        test_cases.push_back({ "s0",{ 3,5,7,9 }, { 3,5,7,9 } });
        test_cases.push_back({ "s1",{ 0, 3, 5, 7, 9 }, { 3,5,7,9 } });
        test_cases.push_back({ "base1",{ 5, 15, 21, 25 }, { 5, 15, 25 } });
        test_cases.push_back({ "base2",{ 5, 15, 20, 25 }, { 5, 15, 25 } });
        test_cases.push_back({ "power2",{ 1, 2, 4, 8, 16, 32, 64 }, { 1, 2 } });
        test_cases.push_back({ "mono",{ 10, 10, 10, 10 }, { 10, 10, 10, 10 } });
        test_cases.push_back({ "1",{ -64 },{ -64 } });
        test_cases.push_back({ "2",{ -64, 57 },{ -64, 57 } });
        test_cases.push_back({ "2a",{ -64, 0, 57 },{ -64, 0 } });

        vector<int_t> ar;
        IntRandom r;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto size = r(1, 10);
            FillRandom(ar, size, size);

            // Make positive.
            sort(ar.begin(), ar.end());
            for (auto i = size - 1; i >= 0; --i)
                ar[i] -= ar[0];

            test_cases.emplace_back("random" + to_string(att), move(ar));
        }
    }

    void run_test_case(const test_case& test)
    {
#if print_subseq
        const auto t0 = high_resolution_clock::now();
#endif
        const auto actual = longest_arithmetic_progression_in_sorted_slow2<int_t2>(test.ar);

#if print_subseq
        const auto t1 = high_resolution_clock::now();
#endif
        if (test.expected.size())
            compare_increasing_subsequences<int_t2>(test.expected, actual, test.ar, "longest_arithmetic_progression_in_sorted");

#if print_subseq
        const auto t10 = high_resolution_clock::now();
#endif
        const auto they = longest_arithmetic_progression_in_sorted_they<int_t2>(test.ar);

#if print_subseq
        const auto t11 = high_resolution_clock::now();

        const auto elapsed_me = duration_cast<nanoseconds>(t1 - t0).count();
        const auto elapsed_they = duration_cast<nanoseconds>(t11 - t10).count();

        const auto ratio = elapsed_me ? double(elapsed_they) / elapsed_me : 0.0;
        cout << " me " << elapsed_me << ", they " << elapsed_they <<
            ", they/me " << ratio << '\n';
#endif

        compare_increasing_subsequences<int_t2>(actual, they, test.ar, "longest_arithmetic_progression_in_sorted_they");

        //const auto slow2 = longest_arithmetic_progression_in_sorted_slow2<int_t2>(test.ar);
        //compare_increasing_subsequences<int_t2>(actual, slow2, test.ar, "longest_arithmetic_progression_in_sorted_slow2");

        const auto slow = longest_arithmetic_progression_in_sorted_slow<int_t2>(test.ar);
        compare_increasing_subsequences<int_t2>(actual, slow, test.ar, "longest_arithmetic_progression_in_sorted_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::longest_arithmetic_progression_in_sorted_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}