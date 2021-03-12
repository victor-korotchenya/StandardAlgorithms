#include "../Assert.h"
#include "../Utilities/Random.h"
#include "../Utilities/RandomGenerator.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"
#include "distinct_items_in_range.h"
#include "DistinctItemsInRangeTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    template<typename Number, typename Size>
    struct TestCase final : base_test_case
    {
        using range_t = pair<Size, Size>;

        vector<Number> Data;
        vector<range_t> Queries;
        vector<Size> Expected;

        TestCase(
            string&& name = "",
            vector<Number>&& data = {},
            vector<range_t>&& queries = {},
            vector<Size>&& expected = {})
            :
            base_test_case(forward<string>(name)),
            Data(forward<vector<Number>>(data)),
            Queries(forward<vector<range_t>>(queries)),
            Expected(forward<vector<Size>>(expected))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            const auto data_size = Data.size();
            RequirePositive(data_size, "Data size.");

            RequirePositive(Queries.size(), "Queries size.");
            Assert::AreEqual(Queries.size(), Expected.size(), "Expected size.");

            for (const auto& query : Queries)
            {
                Assert::GreaterOrEqual(query.first, static_cast<Size>(0),
                    "query.left");
                Assert::Greater(data_size, query.first, "query.left");

                Assert::GreaterOrEqual(query.second, static_cast<Size>(0),
                    "query.right");
                Assert::Greater(data_size, query.second, "query.right");
            }
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);

            ::Print(", Data=", Data, str);
            ::Print(", Queries=", Queries, str);
            ::Print(", Expected=", Expected, str);
        }
    };

    template<typename T>
    inline string append_type_name(const string& name)
    {
        auto type_name = typeid(T).name();
        auto result = name + "_" + string(type_name);
        return result;
    }

    template<typename Number, typename Size>
    inline string append_type_names(const string& name)
    {
        auto result = append_type_name<Size>(
            append_type_name<Number>(name));
        return result;
    }

    template<typename Number, typename Size>
    void add_random_test_case(vector<TestCase<Number, Size>>& testCases)
    {
        const Size minValue = 1;
#ifdef _DEBUG
        const Size maxValue = 10;
#else
        const Size maxValue = 20;
#endif

        UnsignedIntRandom sizeRandom;
        const Size size = sizeRandom(minValue, maxValue);

        const auto queries_size = size << 1;
        vector<typename TestCase<Number, Size>::range_t> range_queries(queries_size);
        {
            vector<Size> data1;
            FillRandom(data1, queries_size, size);

            vector<Size> data2;
            FillRandom(data2, queries_size, size);

            for (Size i = 0; i < queries_size; ++i)
            {
                if (data2[i] < data1[i])
                {
                    range_queries[i].first = data2[i];
                    range_queries[i].second = data1[i];
                }
                else
                {
                    range_queries[i].first = data1[i];
                    range_queries[i].second = data2[i];
                }
            }
        }

        vector<Number> data;
        //TODO: use hash_map instead of max_element.
        const Number max_value = 32000;
        FillRandom<Number, unsigned>(data, size, max_value);

        vector<Size> expected = distinct_items_in_range_slow<Number, Size>(
            data, range_queries);

        testCases.push_back({ append_type_names<Number, Size>("Random"),
          move(data), move(range_queries), move(expected) });
    }

    template<typename Number, typename Size>
    void GenerateTestCases(vector<TestCase<Number, Size>>& testCases)
    {
        testCases.push_back(
            {
              append_type_names<Number, Size>("Simple"),
              { 4,6,5,4,5, 4,6,5,4,3,5,1 },
              {
                { 0,0 },
                { 0,1 },
                { 2,4 },
                { 0,10 },
                { 0,11 },
              },
              { 1,2,2,4,5 }
            });

        add_random_test_case<Number, Size>(testCases);
    }

    template<typename Number, typename Size>
    void RunTestCase(const TestCase<Number, Size>& testCase)
    {
        {
            const auto actual = distinct_items_in_range_slow<Number, Size>(
                testCase.Data, testCase.Queries);
            Assert::AreEqual(testCase.Expected, actual, "result_slow");
        }
        {
            const auto actual = distinct_items_in_range_fast<Number, Size>(
                testCase.Data, testCase.Queries);
            Assert::AreEqual(testCase.Expected, actual, "result_fast");
        }
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
                void DistinctItemsInRangeTests()
                {
                    using test1 = TestCase<long long unsigned, unsigned>;
                    using test2 = TestCase<short, long long unsigned>;

                    test_utilities<test1>::run_tests(RunTestCase, GenerateTestCases);
                    test_utilities<test2>::run_tests(RunTestCase, GenerateTestCases);
                }
            }
        }
    }
}