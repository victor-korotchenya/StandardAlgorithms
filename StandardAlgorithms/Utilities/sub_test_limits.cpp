#include "sub_test_limits.h"
#include "CpuUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Utilities;
using namespace Privet::Algorithms;

namespace
{
    std::counting_semaphore<> sem(1 + static_cast<int>(get_core_count(numeric_limits<size_t>::max())));
}

std::counting_semaphore<>& Privet::Algorithms::Tests::get_sub_test_semaphore()
{
    return sem;
}