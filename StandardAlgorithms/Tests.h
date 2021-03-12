#pragma once
#include "test_function.h"

namespace Privet::Algorithms
{
    std::vector<std::pair<std::string, Privet::Algorithms::test_function>> GetTests();

    int RunTests(const bool isDebuggerAttached = false, const size_t cpuCount = 0);
}