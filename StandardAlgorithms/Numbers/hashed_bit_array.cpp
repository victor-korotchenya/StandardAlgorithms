#include"hashed_bit_array.h"
#include"../Utilities/require_utilities.h"
#include"hashed_bit_array_utility.h"
#include"prime_number_utility.h"

namespace
{
    constexpr std::size_t bit_array_default_size = 1;

    inline constexpr void check_invariants(const Standard::Algorithms::Numbers::hashed_bit_array &instance)
    {
        Standard::Algorithms::require_positive(instance.bit_array_size(), "Bit array size");

        Standard::Algorithms::require_positive(instance.number_of_hash_functions(), "Number of hash functions");
    }
} // namespace

Standard::Algorithms::Numbers::hashed_bit_array::hashed_bit_array(
    const std::size_t capacity, const std::size_t bit_array_size, const std::size_t number_of_hash_functions)
    : Bit_array(bit_array_default_size)
{
    init(capacity, bit_array_size, number_of_hash_functions);
}

Standard::Algorithms::Numbers::hashed_bit_array::hashed_bit_array(const std::size_t capacity)
    : Bit_array(bit_array_default_size)
{
    const auto error_probability = hashed_bit_array_utility::error_probability(capacity);

    const auto bit_array_size1 = hashed_bit_array_utility::bit_array_size(capacity, error_probability);

    const auto hash_functions2 = hashed_bit_array_utility::number_of_hash_functions(capacity, bit_array_size1);

    init(capacity, bit_array_size1, hash_functions2);
}

void Standard::Algorithms::Numbers::hashed_bit_array::init(
    const std::size_t capacity, const std::size_t bit_array_size, const std::size_t number_of_hash_functions)
{
    hashed_bit_array_utility::check_capacity(capacity);

    hashed_bit_array_utility::check_bit_array_size(bit_array_size);

    hashed_bit_array_utility::check_number_of_hash_functions(capacity, bit_array_size, number_of_hash_functions);

    if constexpr (::Standard::Algorithms::is_debug)
    {
        if (!is_prime_simple(bit_array_size))
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "_DEBUG. The bitArraySize (" << bit_array_size << ") must be a prime number.";

            throw_exception(str);
        }
    }

    Bit_array_size = bit_array_size;
    Hash_functions_size = number_of_hash_functions;
    Bit_array.resize(bit_array_size);

    check_invariants(*this);
}

void Standard::Algorithms::Numbers::hashed_bit_array::add(const std::size_t hash_code)
{
    check_invariants(*this);

    auto sum = hash_code;

    for (std::size_t index{}; index < Hash_functions_size; ++index)
    {
        {
            const auto bitte = sum % Bit_array_size;
            Bit_array.set_bit(bitte);
        }

        sum += hash_code;
    }
}

[[nodiscard]] auto Standard::Algorithms::Numbers::hashed_bit_array::has(const std::size_t hash_code) const -> bool
{
    check_invariants(*this);

    auto sum = hash_code;

    for (std::size_t index{}; index < Hash_functions_size; ++index)
    {
        const auto bitte = sum % Bit_array_size;

        if (const auto value = Bit_array.get_bit(bitte); std::uint8_t{} == value)
        {
            return false;
        }

        sum += hash_code;
    }

    constexpr auto maybe = true;
    return maybe;
}

[[nodiscard]] auto Standard::Algorithms::Numbers::hashed_bit_array::count_set_bits() const -> std::uint64_t
{
    auto result = Bit_array.count_set_bits();

    if constexpr (::Standard::Algorithms::is_debug)
    {
        require_less_equal(result, Bit_array_size, "count_set_bits");

        check_invariants(*this);
    }

    return result;
}
