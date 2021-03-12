#include "../Utilities/Random.h"
#include "sparse_table_min_tests.h"
#include "sparse_table_min.h"
#include "../test_utilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using value_t = int;
    constexpr auto n = 100, log_n = 7, value_max = 10 * n;
}

void Privet::Algorithms::Trees::Tests::sparse_table_min_tests()
{
    IntRandom r;
    sparse_table_min<value_t, log_n, n> table;

    auto& data = table.data;
    for (auto i = 0; i < n; i++)
        data[i] = r(-value_max, value_max);

    const auto size = n - !(data[0] & 1);
    assert(size > 0);
    table.init(size);

    int start = 0, stop = 1;
    try
    {
        for (auto i = 0; i < n; i++)
        {
            start = r(0, size - 1),
                stop = r(start + 1, size);

            const auto actual = table.range_minimum_query_index(start, stop);

            const auto slow = min_element(&data[start], &data[stop - 1ll] + 1) - &data[0];

            const auto& a = data[actual], & b = data[slow];
            Assert::AreEqual(a, b, "range_minimum_query_index");
        }
    }
    catch (const exception& e)
    {
        stringstream ss;
        ss << e.what();
        ss << ", start " << start
            << ", stop " << stop << " "
            << ", size " << size << ", data ";

        ss << data;
        throw runtime_error(ss.str());
    }
}