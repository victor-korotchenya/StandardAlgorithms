#include"fibonacci_heap_tests.h"
#include"fibonacci_heap.h"
#include"heap_tests.h"

void Standard::Algorithms::Heaps::Tests::fibonacci_heap_tests()
{
    using key_t = std::int32_t;
    using node_t = fib_node<key_t>;

    heap_tests<true, key_t, node_t, fibonacci_heap<key_t, node_t>>();
}
