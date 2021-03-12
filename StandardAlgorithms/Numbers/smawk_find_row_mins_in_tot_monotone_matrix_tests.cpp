#include <array>
#include "smawk_find_row_mins_in_tot_monotone_matrix_tests.h"
#include "smawk_find_row_mins_in_tot_monotone_matrix.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = short;

    template<class t>
    size_t get_min_position(const vector<vector<t>>& ar,
        const size_t row, const size_t col, const size_t col2_excl)
    {
        assert(ar.size() && row < ar.size() && col < col2_excl&& col2_excl <= ar[0].size());

        const auto& a = ar[row];
        const auto start = a.begin() + col;
        const auto stop = a.begin() + col2_excl;

        const auto it = min_element(start, stop);
        const auto pos = it - start;
        return pos;
    }

    template<class t>
    bool is_totally_monotonic(const vector<vector<t>>& ar)
    {
        const auto size = ar.size();
        if (size <= 1)
            return true;

        const auto row0_size = ar[0].size();
        RequirePositive(row0_size, "ar[0].size() in require_totally_monotonic");
        if (row0_size <= 1)
            return true;

        assert(row0_size == ar.back().size());

        for (size_t r = 0; r < size - 1u; ++r)
        {
            assert(row0_size == ar[r].size());
            for (auto re = r + 1u; re < size; ++re)
            {
                for (size_t c = 0; c < row0_size - 1u; ++c)
                {
                    for (auto ce = c + 1u; ce < row0_size; ++ce)
                    {
                        const auto& v10 = ar[re][c], & v11 = ar[re][ce];
                        if (v11 < v10)
                            continue;

                        const auto& v00 = ar[r][c], & v01 = ar[r][ce];
                        if (v01 < v00)
                            return false;
                    }
                }
            }
        }

        return true;
    }

    struct test_case final : base_test_case
    {
        vector<vector<int_t>> ar;
        vector<size_t> expected;

        test_case(string&& name,
            vector<vector<int_t>>&& ar,
            vector<size_t>&& expected)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<vector<int_t>>>(ar)),
            expected(forward<vector<size_t>>(expected))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = ar.size();
            RequirePositive(size, "ar.size");

            const auto columns = ar[0].size();
            RequirePositive(columns, "ar[0].size");

            for (size_t i = 1; i < size; ++i)
                Assert::AreEqual(columns, ar[i].size(), "ar[i].size()");

            const auto ism = is_totally_monotonic(ar);
            Assert::AreEqual(true, ism, "is_totally_monotonic");

            if (expected.empty())
                return;

            Assert::AreEqual(size, expected.size(), "expected.size()");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("ar", ar, str);
            ::Print("expected", expected, str);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "larmore",{
            { 25, 21, 13,10,20,13,19,35,37,41,58,66,82,99,124,133,156,178 },
            //           10
            //           20
            { 42, 35, 26,20,29,21,25,37,36,39,56,64,76,91,116,125,146,164 },
            { 57, 48, 35,28,33,24,28,40,37,37,54,61,72,83,107,113,131,146 },
            //                 24
            //                 35
            { 78, 65, 51,42,44,35,38,48,42,42,55,61,70,80,100,106,120,135 },
            { 90, 76, 58,48,49,39,42,48,39,35,47,51,56,63, 80, 86, 97,110 },
            //                             35
            //                             33
            { 103, 85,67,56,55,44,44,49,39,33,41,44,49,56, 71, 75, 84, 96 },
            { 123,105,86,75,73,59,57,62,51,44,50,52,55,59, 72, 74, 80, 92 },
            //                             44
            //                             43
            {142,123,100,86,82,65,61,62,50,43,47,45,46,46, 58, 59, 65, 73 },
            //                                          20
            { 151,130,104,88,80,59,52,49,37,29,29,24,23,20, 28, 25, 31, 39 },
            //},{ 10, 20, 24, 35, 35, 33, 44, 43, 20 } });
            },{ 3,3,5,5,9,9,9,9,13 } });

        test_cases.push_back({ "base3",{ { 2, 4, 3 },{ 2, 4, 1 } },{ 0, 2 } });
        test_cases.push_back({ "base1",{ { 200 } },{ 0 } });
        test_cases.push_back({ "base2",{ { 200, 100 },{ 150, 101 } },{ 1, 1 } });
        test_cases.push_back({ "base22",{ { 20, 100 },{ 150, 101 } },{ 0, 1 } });
        test_cases.push_back({ "base24",{ { 200, 100, 99, 100 },{ 150, 130, 111, 102 } },{ 2, 3 } });
        test_cases.push_back({ "base42",{ { 99, 100 },{ 120, 130 },{ 111, 102 },{ -1, -2 } },{ 0,0,1,1 }
            });
        //todo: p3. do IntRandom r;
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = smawk_find_row_mins_in_tot_monotone_matrix(test_case.ar);
        if (test_case.expected.size())
            Assert::AreEqual(test_case.expected, actual, "smawk_find_row_mins_in_tot_monotone_matrix");

        const auto slow = find_row_mins_in_2d_matrix_slow(test_case.ar);
        Assert::AreEqual(actual, slow, "find_row_mins_in_2d_matrix_slow");
    }
}

void test_is_totally_monotonic()
{
    const vector<vector<int_t>> v{ { 2, 4, 1 },{ 2, 4, 3 } };
    const auto actual = is_totally_monotonic(v);
    Assert::AreEqual(false, actual, "is_totally_monotonic");
}

void Privet::Algorithms::Numbers::Tests::smawk_find_row_mins_in_tot_monotone_matrix_tests()
{
    test_is_totally_monotonic();

    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}