#include "max_in_sliding_window.h"
#include "max_in_sliding_window_tests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace Privet::Algorithms::Numbers::Tests
{
    void max_in_sliding_window_tests()
    {
        using DataType = short;

        constexpr size_t width = 3;
        const vector<DataType> data{ 9, 5, 4, 7, 6, 9, 8, 4, 5, 7 },
            expected{ 9, 7, 7, 9, 9, 9, 8, 7 };
        Assert::AreEqual(expected.size(), data.size() - width + 1, "expectedSize");

        vector<DataType> actual;
        max_in_sliding_window(data, width, actual);
        Assert::AreEqual(expected, actual, "maximums");
    }
}