#pragma once
#include<semaphore>

namespace Standard::Algorithms::Tests
{
    [[nodiscard]] auto get_sub_test_semaphore() -> std::counting_semaphore<> &;
} // namespace Standard::Algorithms::Tests
