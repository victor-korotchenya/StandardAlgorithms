#pragma once
// "tests.h"
#include"test_function.h"

namespace Standard::Algorithms
{
    [[nodiscard]] auto gather_all_tests() -> std::vector<test_function>;

    [[nodiscard]] auto run_all_tests(bool is_debugger_attached) -> std::int32_t;
} // namespace Standard::Algorithms
