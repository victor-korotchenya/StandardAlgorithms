#include "../Utilities/Random.h"
#include "land2d_cut_vert_horiz_line_tests.h"
#include "land2d_cut_vert_horiz_line.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = int;
    constexpr auto not_computed = -1;

    void cut_tests()
    {
        {
            constexpr auto area = 18;
            const auto area_cuts = cut_2d_areas(area);
            const auto actual = can_cut_area_from_2d(area_cuts, area, 4, 5);
            Assert::AreEqual(false, actual, "4*5 cannot cut area 18");
        }
        {
            constexpr auto r = 3, c = 5;
            const vector<int> imposs{ 7,11,13,14 };
            const auto prefix = to_string(r) + " * " + to_string(c) + " cannot cut area ";

            for (auto area = 1; area <= r * c; ++area)
            {
                const auto area_cuts = cut_2d_areas(area);
                const auto actual = can_cut_area_from_2d(area_cuts, area, 4, 5);
                const auto expected = imposs.end() == find(imposs.begin(), imposs.end(), area);
                Assert::AreEqual(expected, actual, prefix + to_string(area));
            }
        }
    }

    struct test_case final : base_test_case
    {
        vector<int_t> pieces;
        int_t r, c, expected;

        test_case(string&& name,
            vector<int_t>&& pieces,
            int_t r, int_t c,
            int_t expected = not_computed)
            : base_test_case(forward<string>(name)),
            pieces(forward<vector<int_t>>(pieces)),
            r(r), c(c),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int>(pieces.size());
            RequirePositive(size, "pieces.size");
            RequireGreater(16, size, "pieces.size");
            RequireAllPositive(pieces, "pieces");
            RequirePositive(r, "r");
            RequirePositive(c, "c");
            RequireGreater(expected, not_computed - 1, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("pieces", pieces, str);
            PrintValue(str, "r", r);
            PrintValue(str, "c", c);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        // 9 rows, 8 cols
        // 2*5 4*2 4*1
        // 7*5         5*3
        test_cases.push_back({ "base 0", { 15, 10, 4, 35, 8, }, 9, 8, true });
        test_cases.push_back({ "base 02", { 35, 10, 8, 4, 15 }, 10, 11,
            //true });
            false });

        test_cases.push_back({ "base1", { 1 }, 1, 1, true });
        test_cases.push_back({ "base2", { 4, 4 }, 2, 4, true });
        test_cases.push_back({ "leftovers", { 1 }, 2, 3,
            //true });
            false });

        test_cases.push_back({ "too wide", { 5 }, 2, 3, false });
        test_cases.push_back({ "divisible", { 4 }, 2, 3,
            //true });
            false });

        test_cases.push_back({ "long 1", { 6, 4,2 }, 1, 12, true });
        test_cases.push_back({ "long 20", { 6, 4,2 }, 1, 20,
            //true });
            false });

        test_cases.push_back({ "long not", { 6, 4,2 }, 1, 11, false });
        test_cases.push_back({ "b12", { 6, 4,2 }, 4, 3, true });
        test_cases.push_back({ "b12a", { 6, 4,1 }, 4, 3,
            //true });
            false });

        test_cases.push_back({ "b12b", { 6, 4,1,1 }, 4, 3, true });
        test_cases.push_back({ "b12n", { 6, 4,1,2 }, 4, 3, false });
        test_cases.push_back({ "b12n2", { 7,1,2 }, 4, 3, false });

        IntRandom r;
        vector<int_t> pieces;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 6;
            const auto rows = r(1, size_max), cols = r(1, size_max), pc = r(1, size_max >> 1);

            pieces.resize(pc);
            for (auto i = 0; i < pc; ++i)
                pieces[i] = r(1, size_max);

            test_cases.emplace_back("random" + to_string(att), move(pieces), rows, cols);
        }

        for (auto& test : test_cases)
        {
            ShuffleArray(test.pieces);
        }
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = land2d_cut_vert_horiz_line<int_t>(test_case.pieces, test_case.r, test_case.c);
        if (test_case.expected != not_computed)
            Assert::AreEqual(test_case.expected > 0, actual.size() > 0, "land2d_cut_vert_horiz_line");

        const auto slow = land2d_cut_vert_horiz_line<int_t>(test_case.pieces, test_case.r, test_case.c);
        Assert::AreEqual(actual.size() > 0, slow.size() > 0, "land2d_cut_vert_horiz_line_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::land2d_cut_vert_horiz_line_tests()
{
    cut_tests();
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}