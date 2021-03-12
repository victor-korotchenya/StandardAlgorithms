#include <array>
#include <chrono>
#include "hash_utilities.h"
#include "../test_utilities.h"
#include "swap_knights_min_moves.h"
#include "swap_knights_min_moves_tests.h"
#include "../Utilities/Random.h"

using namespace std::chrono;
using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = short;
    using int_t2 = int;
    using pair_t = pair<int_t, int_t>;

    struct test_case final : base_test_case
    {
        int_t n;
        pair_t p0, p1;
        int expected;

        test_case(string&& name,
            int n,
            int x0, int y0,
            int x1, int y1,
            int expected)
            : base_test_case(forward<string>(name)),
            n(static_cast<int_t>(n)),
            p0(static_cast<int_t>(x0), static_cast<int_t>(y0)),
            p1(static_cast<int_t>(x1), static_cast<int_t>(y1)),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            Assert::GreaterOrEqual(n, 3, "n");

            check_point(n, p0.first, p0.second);
            check_point(n, p1.first, p1.second);
            Assert::NotEqual(p0, p1, "pairs");

            Assert::GreaterOrEqual(expected, 0, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            str << ", n " << n
                << ", p0 " << p0
                << ", p1 " << p1
                << ", expected " << expected;
        }
    };

    void generate_test_cases(vector<test_case>& tests)
    {
        tests.emplace_back(
            "base3",
            3,
            0, 0,
            0, 1,
            8);
        // Runs for 1.1 sec - make faster
        //tests.emplace_back(
        //    "base4",
        //    4,
        //    0,0,
        //    0,1,
        //    //0,3, will run for 138 sec in Debug build.
        //    6);

        // Runs for 20 sec - make faster
        //tests.emplace_back(
        //    "base5",
        //    5,
        //    0,0,
        //    0,1,
        //    6);

        ShortRandom r;
        for (auto att = 0, att_max = 0; att < att_max; ++att)
        {
            const auto n = r(3, 4);

            array<int_t, 4> ar;
            for (auto& a : ar)
                a = r(0, n - 1);

            const auto& x0 = ar[0], & y0 = ar[1], & x1 = ar[2], & y1 = ar[3];
            if (x0 == x1 && y0 == y1)
                continue;

            if (n == 3 && (x0 == y0 && x0 == 1 ||
                x1 == y1 && x1 == 1))
                continue;

            tests.emplace_back("random" + to_string(att), n,
                x0, y0, x1, y1, 0);
        }
        //
        //{
        //    auto att = 0ll;
        //    for (int_t x0 = 0; x0 < 4; ++x0)
        //    for (int_t y0 = 0; y0 < 4; ++y0)
        //    for (int_t x1 = x0; x1 < 4; ++x1)
        //    for (int_t y1 = y0; y1 < 4; ++y1)
        //    {
        //        const auto n = 4;
        //        if (x0 == x1 && y0 == y1)
        //            continue;
        //
        //        tests.emplace_back("_all_" + to_string(++att), n,
        //            x0, y0, x1, y1, 0);
        //    }
        //}
    }

    void run_test_case(const test_case& test)
    {
        calls_g = 0;
        const auto t0 = high_resolution_clock::now();

        const auto actual = swap_knights_min_moves<int_t2, int_t>(test.n, test.p0.first, test.p0.second, test.p1.first, test.p1.second);

        const auto t1 = high_resolution_clock::now();
        const auto sec = duration_cast<milliseconds>(t1 - t0).count();
        static int64_t time_max = 0;
        if (time_max < sec)
            time_max = sec;

        //cout << test << ", time ms " << sec
        //    << ", actual " << actual
        //    << ", calls " << calls_g << ", time max " << sec_max << '\n';
        if (test.expected)
            Assert::AreEqual(test.expected, actual, "swap_knights_min_moves");

        const auto t10 = high_resolution_clock::now();
        const auto slow = swap_knights_min_moves_slow<int_t2, int_t, pair_t, std::pair_hash>(test.n, test.p0, test.p1);
        const auto t11 = high_resolution_clock::now();
        const auto sec1 = duration_cast<milliseconds>(t11 - t10).count();
        if (sec1)
        {
        }

        Assert::AreEqual(actual, slow, "swap_knights_min_moves_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::swap_knights_min_moves_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}