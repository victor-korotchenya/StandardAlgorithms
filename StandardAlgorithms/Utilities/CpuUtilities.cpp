#include <thread>
#include <stdexcept>
#include <algorithm>
#include "CpuUtilities.h"

using namespace std;

size_t Privet::Algorithms::Utilities::get_core_count(const size_t inputSize, size_t cpuCount)
{
    size_t result = 1;
    if (inputSize >= 2)
    {
        if (!cpuCount)
        {
            cpuCount = thread::hardware_concurrency();
            if (!cpuCount)
                throw runtime_error("thread::hardware_concurrency returned 0.");
        }

        //Do not waste CPU.
        result = min(inputSize, cpuCount);
    }

    return result;
}