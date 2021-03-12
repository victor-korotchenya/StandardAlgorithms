#include "../Utilities/Random.h"
#include "min_time_copy_books_tests.h"
#include "min_time_copy_books.h"
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
    using int_t2 = int64_t;

    struct test_case final : base_test_case
    {
        vector<int_t> books;
        int_t copiers, expected;

        test_case(string&& name,
            vector<int_t>&& books,
            int_t copiers,
            int_t expected = {})
            : base_test_case(forward<string>(name)),
            books(forward<vector<int_t>>(books)),
            copiers(copiers),
            expected(expected)
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto size = static_cast<int_t>(books.size());
            RequirePositive(size, "books.size");
            RequireAllPositive(books, "books");
            RequireNonNegative(expected, "expected");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("books", books, str);
            PrintValue(str, "copiers", copiers);
            PrintValue(str, "expected", expected);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "base1", { 10 }, 1, 10 });
        test_cases.push_back({ "base12", { 10 }, 2, 10 });
        test_cases.push_back({ "base2", { 10, 20 }, 2, 20 });
        test_cases.push_back({ "base3", { 10, 20, 5 }, 2, 25 });
        test_cases.push_back({ "base31", { 10, 20, 5 }, 1, 35 });
        test_cases.push_back({ "base33", { 10, 20, 5 }, 3, 20 });
        test_cases.push_back({ "base34", { 10, 20, 5 }, 4, 20 });
        test_cases.push_back({ "base4", { 4, 4, 4, 4 }, 2, 8 });
        test_cases.push_back({ "base43", { 4, 4, 4, 4 }, 3, 8 });

        IntRandom r;
        vector<int_t> books;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto size_max = 8, value_max = 100;
            const auto n = r(1, size_max), copiers = r(1, size_max);

            books.resize(n);
            for (auto& book : books)
                book = r(1, value_max);

            test_cases.emplace_back("random" + to_string(att), move(books), copiers);
        }
    }

    void run_test_case(const test_case& test)
    {
        const auto actual = min_time_copy_books_using_binary_search<int_t2, int_t>(test.books, test.copiers);
        if (test.expected)
            Assert::AreEqual(test.expected, actual, "min_time_copy_books_using_binary_search");

        const auto bs = min_time_copy_books_slow<int_t2, int_t>(test.books, test.copiers);
        Assert::AreEqual(actual, bs, "min_time_copy_books_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::min_time_copy_books_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}