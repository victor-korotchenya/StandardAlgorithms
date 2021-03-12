#include"deap_tests.h"
#include"deap.h"
#include"depq_heap_tests.h"

void Standard::Algorithms::Heaps::Tests::deap_tests()
{
    using random_t = Standard::Algorithms::Utilities::random_t<std::int16_t>;
    using key_t = typename random_t::int_t;
    using heap_t = deap<key_t>;

    constexpr auto max_attempts = 1;

    for (std::int32_t att{}; att < max_attempts; ++att)
    {
        depq_heap_tests<random_t, key_t, heap_t>();
    }
}
