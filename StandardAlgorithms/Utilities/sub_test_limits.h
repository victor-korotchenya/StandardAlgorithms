#pragma once
#include <semaphore>

namespace Privet::Algorithms::Tests
{
    std::counting_semaphore<>& get_sub_test_semaphore();
}