#include <algorithm>
#include <array>
#include <limits>
#include "../Numbers/StandardOperations.h"
#include "../Utilities/PrintUtilities.h"
#include "../Utilities/Random.h"
#include "../Utilities/RandomGenerator.h"
#include "../test_utilities.h"
#include "SparseTable_RangeMinimumQuery_2D.h"
#include "SparseTable_RangeMinimumQuery_2DTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using Number = short;
    using data_t = vector<vector<Number>>;
    using query_t = vector<int>;

    constexpr int N = 10, logN = 4, M = 20, logM = 5;
    struct TestCase final : base_test_case
    {
        data_t Data;
        vector<query_t> Queries;

        TestCase(string&& name = "",
            data_t&& data = {},
            vector<query_t>&& queries = {})
            : base_test_case(forward<string>(name)),
            Data(forward<data_t>(data)),
            Queries(forward<vector<query_t>>(queries))
        {
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("Data", Data, str);
            ::Print("Queries", Queries, str);
        }
    };

    void GenerateTestCases(vector<TestCase>& testCases)
    {
        data_t data(N);
        for (int i = 0; i < N; i++)
        {
            FillRandom(data[i], M);
        }

        vector<query_t> queries;
        IntRandom random;
        const int cnt = 10;
        for (int i = 0; i < cnt; i++)
        {
            auto x1 = random.operator()(0, N - 1);
            auto x2 = random.operator()(0, N - 1);
            if (x2 < x1)
                swap(x1, x2);

            auto y1 = random.operator()(0, M - 1);
            auto y2 = random.operator()(0, M - 1);
            if (y2 < y1)
                swap(y1, y2);

            auto ans = numeric_limits<Number>::max();
            for (int row = x1; row <= x2; row++)
            {
                for (int col = y1; col <= y2; col++)
                {
                    ans = min(ans, data[row][col]);
                }
            }

            queries.push_back({ x1,y1,x2,y2, ans });
        }

        testCases.push_back({ "random test", move(data), move(queries) });
    }

    void RunTestCase(const TestCase& testCase)
    {
        using Operation_t = Number(*)(const Number&, const Number&);

        SparseTable_RangeMinimumQuery_2D<Number, N, logN, M, logM, Operation_t>
            rmq(testCase.Data, Min<Number>);

        for (size_t i = 0; i < testCase.Queries.size(); i++)
        {
            const auto& query = testCase.Queries[i];
            const auto i_as_string = to_string(i);
            Assert::GreaterOrEqual(5, query.size(),
                "Queries[" + i_as_string + "].size()");

            const auto actual = rmq.get(query[0], query[1], query[2], query[3]);
            Assert::AreEqual(query[4], actual, "Result for query " + i_as_string + ".");
        }
    }
}

void Privet::Algorithms::Trees::Tests::SparseTable_RangeMinimumQuery_2DTests()
{
    test_utilities<TestCase>::run_tests(RunTestCase, GenerateTestCases);
}