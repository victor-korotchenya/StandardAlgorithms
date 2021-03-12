#include"binomial_heap_tests.h"
#include"binomial_heap.h"
#include"heap_tests.h"

void Standard::Algorithms::Heaps::Tests::binomial_heap_tests()
{
    using key_t = std::int32_t;

    constexpr auto max_attempts = 1;

    for (std::int32_t att{}; att < max_attempts; ++att)
    {
        heap_tests<false, key_t, binomial_node<key_t>, binomial_heap<key_t>>();
    }
}
