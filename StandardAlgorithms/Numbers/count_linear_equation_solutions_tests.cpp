#include "count_linear_equation_solutions.h"
#include "count_linear_equation_solutions_tests.h"
#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = int64_t;
    constexpr auto not_computed = -1, mod = 1000 * 1000 * 1000 + 7;

    struct test_case final : base_test_case
    {
        vector<int_t> data;
        int expected;

        test_case(string&& name,
            vector<int_t>&& data,
            const int expected = not_computed)
            : base_test_case(forward<string>(name)),
            data(forward<vector<int_t>>(data)),
            expected(expected)
        {
            Assert::GreaterOrEqual(this->data.size(), 2, "n");
            Assert::GreaterOrEqual(expected, not_computed, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print(", data=", data, str);
            str << "expected=" << expected;
        }
    };

    void generate_test_cases(vector<test_case>& tests)
    {
        // 3x + 4y + 12z + 5a = 20
        tests.emplace_back("base4", vector<int_t>{3, 4, 12, 5, 20},
            // 2: 12+5+3, 12+4*2
            // 4: 5*4, 5*2+4+3*2, 5+4*3+3, 5+3*5
            // 2: 4*5, 4*2+3*4
            8);
        tests.emplace_back("base3", vector<int_t>{2, 4, 6, 10},
            // 6+4, 6+2+2, 4+4+2, 4+2+2+2, 2*5
            5);
        tests.emplace_back("all 0s", vector<int_t>{2, 4, 6, 0}, 1);
        tests.emplace_back("no solution3", vector<int_t>{2, 4, 6, 1}, 0);
        tests.emplace_back("no solution2", vector<int_t>{2, 4, 1}, 0);
        tests.emplace_back("no solution1", vector<int_t>{2, 1}, 0);

        IntRandom r;
        for (auto att = 0, max_att = 1; att < max_att; ++att)
        {
            constexpr auto min_size = 2, max_size = 10,
                min_value = 1, max_value = 10;

            auto data = random_vector<int_t>(r, min_size, max_size, min_value, max_value);
            data.back() += r(0, 5);

            tests.emplace_back("random" + to_string(att), move(data));
        }
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = count_linear_equation_solutions<int_t>(test.data, mod);
        if (test.expected >= 0)
            Assert::AreEqual(test.expected, actual, "count_linear_equation_solutions");

        const auto slow = count_linear_equation_solutions_slow<int_t>(test.data, mod);
        Assert::AreEqual(actual, slow, "count_linear_equation_solutions_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::count_linear_equation_solutions_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}