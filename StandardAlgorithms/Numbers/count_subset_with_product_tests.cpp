#include "../Utilities/Random.h"
#include "count_subset_with_product_tests.h"
#include "count_subset_with_product.h"
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
    using long_int_t = int64_t;
    constexpr int_t mod = 1000 * 1000 * 1000 + 7;

    struct test_case final : base_test_case
    {
        const vector<int_t> ar;
        const int_t product, expected;

        test_case(string&& name,
            vector<int_t>&& ar,
            int_t product,
            int_t expected)
            : base_test_case(forward<string>(name)),
            ar(forward<vector<int_t>>(ar)),
            product(product),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(ar.size(), "ar.size");
            for (const auto& a : ar)
                RequirePositive(a, "a");

            RequirePositive(product, "product");
            RequireNonNegative(expected, "expected");
            RequireGreater(mod, expected, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("ar", ar, str);
            PrintValue(str, "product", product);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "base0", { 2, 10, 7, 5 }, 1, 0 });
        test_cases.push_back({ "base1", { 1, 10, 8 }, 1, 1 });
        test_cases.push_back({ "base11", { 1, 10, 7, 1 }, 1, 3 });
        test_cases.push_back({ "base2", { 1, 1, 2, 2, 3 }, 4, 4 });
        test_cases.push_back({ "base3", { 1, 1, 2, 2, 3 }, 2, 8 });

        IntRandom r;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            const auto n = r(1, 6), product = r(1, 50);
            vector<int_t> ar(n);
            for (auto i = 0; i < n; ++i)
                ar[i] = r(1, 36);

            test_cases.push_back({ "random" + to_string(att), move(ar), product, 0 });
        }
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = count_subset_with_product<int_t, long_int_t, mod>(test_case.ar, test_case.product);
        if (test_case.expected)
            Assert::AreEqual(test_case.expected, actual, "count_subset_with_product");

        const auto slow = count_subset_with_product_slow<int_t, long_int_t, mod>(test_case.ar, test_case.product);
        Assert::AreEqual(actual, slow, "count_subset_with_product_slow");

        if (test_case.product == 1)
            // They are buggy.
            return;

        const auto they = count_subset_with_product_they<int_t, long_int_t, mod>(test_case.ar, test_case.product);
        Assert::AreEqual(actual, they, "count_subset_with_product_they");
    }
}

void Privet::Algorithms::Numbers::Tests::count_subset_with_product_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}