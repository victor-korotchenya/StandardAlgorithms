#pragma once
#include "../test_function.h"

namespace Privet::Algorithms::Numbers::Tests
{
    void add_tests(std::vector<std::pair<std::string, Privet::Algorithms::test_function>>& tests, const std::string& longTimeRunningPrefix);
}