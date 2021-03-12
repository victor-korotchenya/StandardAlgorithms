#include"chain_hash_table_tests.h"
#include"chain_hash_table.h"
#include"hash_table_simple_tests.h"
#include"identity.h"

namespace
{
    using key_t = std::int64_t;
    constexpr auto is_noexcept = true;
    using hash_func_t = Standard::Algorithms::Numbers::identity<key_t, is_noexcept>;

    static_assert(Standard::Algorithms::Numbers::hash_function<hash_func_t, key_t>);

    static_assert(
        !Standard::Algorithms::Numbers::hash_function<Standard::Algorithms::Numbers::identity<key_t, !is_noexcept>, key_t>);
} // namespace

void Standard::Algorithms::Numbers::Tests::chain_hash_table_tests()
{
    using table_t = Standard::Algorithms::Numbers::chain_hash_table<key_t, hash_func_t>;

    Standard::Algorithms::Numbers::Tests::hash_table_simple_tests<key_t, table_t>("Linear probing hash table");
}
