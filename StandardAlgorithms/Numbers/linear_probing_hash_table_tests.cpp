#include"linear_probing_hash_table_tests.h"
#include"hash_table_simple_tests.h"
#include"identity.h"
#include"linear_probing_hash_table.h"

void Standard::Algorithms::Numbers::Tests::linear_probing_hash_table_tests()
{
    using key_t = std::int64_t;
    constexpr auto is_noexcept = true;
    using hash_func_t = Standard::Algorithms::Numbers::identity<key_t, is_noexcept>;
    using table_t = Standard::Algorithms::Numbers::linear_probing_hash_table<key_t, hash_func_t>;

    Standard::Algorithms::Numbers::Tests::hash_table_simple_tests<key_t, table_t>("Linear probing hash table");
}
