#include"min_max_heap_tests.h"
#include"depq_heap_tests.h"
#include"min_max_heap.h"

void Standard::Algorithms::Heaps::Tests::min_max_heap_tests()
{
    using random_t = Standard::Algorithms::Utilities::random_t<std::int16_t>;
    using key_t = typename random_t::int_t;
    using heap_t = min_max_heap<key_t>;

    const auto max_atts = 1;

    for (std::int32_t att{}; att < max_atts; ++att)
    {
        depq_heap_tests<random_t, key_t, heap_t>();
    }
}
