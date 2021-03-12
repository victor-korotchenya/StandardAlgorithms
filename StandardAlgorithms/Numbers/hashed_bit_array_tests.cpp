#include"hashed_bit_array_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"hashed_bit_array.h"

namespace
{
    constexpr std::size_t capacity = 345;
    constexpr std::size_t capacity_plus_extra = capacity + 1;
    constexpr std::size_t expected_bit_array_size = 4211;
    constexpr std::size_t expected_number_of_hash_functions = 8;

    constexpr void properties_test(const Standard::Algorithms::Numbers::hashed_bit_array &hashed_bit_array)
    {
        {
            const auto actual_bit_array_size = hashed_bit_array.bit_array_size();

            ::Standard::Algorithms::ert::are_equal(expected_bit_array_size, actual_bit_array_size, "bit_array_size");
        }
        {
            const auto actual_number_of_hash_functions = hashed_bit_array.number_of_hash_functions();

            ::Standard::Algorithms::ert::are_equal(
                expected_number_of_hash_functions, actual_number_of_hash_functions, "Number of hash functions");
        }
    }

    constexpr void add_values(
        const std::vector<std::size_t> &hashes, Standard::Algorithms::Numbers::hashed_bit_array &hashed_bit_array)
    {
        const auto hashes_count = hashes.size();

        for (std::size_t index{}; index < hashes_count; ++index)
        {
            const auto value = hashes[index];
            hashed_bit_array.add(value);

            if (!hashed_bit_array.has(value))
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Right after adding the hash[" << index
                    << "], hashed_bit_array must have a value, but it does not.";

                Standard::Algorithms::throw_exception(str);
            }
        }
    }

    constexpr void check_values(
        const std::vector<std::size_t> &hashes, const Standard::Algorithms::Numbers::hashed_bit_array &hashed_bit_array)
    {
        const auto hashes_count = hashes.size();

        for (std::size_t index{}; index < hashes_count; ++index)
        {
            const auto value = hashes[index];
            if (!hashed_bit_array.has(value))
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "hashed_bit_array must have a value, but it does not at index " << index << ".";

                Standard::Algorithms::throw_exception(str);
            }
        }
    }

    void bit_count_test(const Standard::Algorithms::Numbers::hashed_bit_array &hashed_bit_array)
    {
        const auto bit_count = hashed_bit_array.count_set_bits();

        Standard::Algorithms::require_less_equal(bit_count, expected_bit_array_size, "bitCount");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::hashed_bit_array_tests()
{
    hashed_bit_array hashed_bit_array(capacity_plus_extra);

    properties_test(hashed_bit_array);

    std::vector<std::size_t> hashes;
    Standard::Algorithms::Utilities::fill_random(hashes, capacity);

    try
    {
        add_values(hashes, hashed_bit_array);
        check_values(hashes, hashed_bit_array);
        bit_count_test(hashed_bit_array);
    }
    catch (const std::exception &exc)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();

        ::Standard::Algorithms::print("Random hashes", hashes, str);

        str << "\nError: " << exc.what();

        throw_exception(str);
    }
}
