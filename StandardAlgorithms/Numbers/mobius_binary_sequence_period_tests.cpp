#include "mobius_binary_sequence_period.h"
#include "mobius_binary_sequence_period_tests.h"
#include "../test_utilities.h"

using namespace std;

namespace Privet::Algorithms::Numbers::Tests
{
    void mobius_binary_sequence_period_tests()
    {
        {
            const auto r = bin_sequence_period(6);
            Assert::AreEqual(r, 9, "bin_sequence_period");
        }

        for (int64_t i = 1, max_n = 8; i <= max_n; ++i)
        {
            const auto a = bin_sequence_period(i);
            const auto b = bin_sequence_period_slow(i);
            Assert::AreEqual(a, b, "bin_sequence_period_slow_" + to_string(i));
        }
    }
}