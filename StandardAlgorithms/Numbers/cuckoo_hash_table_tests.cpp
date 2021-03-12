#include"cuckoo_hash_table_tests.h"
#include"../Numbers/prime_number_utility.h"
#include"../Utilities/random.h"
#include"cuckoo_hash_table.h"
#include"hash_table_simple_tests.h"
#include"hash_vector.h"

void Standard::Algorithms::Numbers::Tests::cuckoo_hash_table_tests()
{
    using key_t = std::int32_t;
    using random_t = Standard::Algorithms::Utilities::random_t<key_t>;

    constexpr auto func_size = 2;
    using hash_func_t = Standard::Algorithms::Numbers::hash_vector<random_t, func_size>;
    using table_t = Standard::Algorithms::Numbers::cuckoo_hash_table<key_t, random_t, hash_func_t>;

    Standard::Algorithms::Numbers::Tests::hash_table_simple_tests<key_t, table_t>("Cuckoo hash table");
}
