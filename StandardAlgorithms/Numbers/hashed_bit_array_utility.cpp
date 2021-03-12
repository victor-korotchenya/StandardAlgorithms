#include"hashed_bit_array_utility.h"
#include"../Utilities/require_utilities.h"
#include"prime_number_utility.h"

namespace
{
    using floating_t = Standard::Algorithms::Numbers::hashed_bit_array_utility::floating_t;
} // namespace

[[nodiscard]] auto Standard::Algorithms::Numbers::hashed_bit_array_utility::error_probability(const std::size_t capacity)
    -> floating_t
{
    check_capacity(capacity);

    const auto capacity_cast = static_cast<floating_t>(capacity);
    auto result = floating_t{ 1.0 } / capacity_cast;

    if constexpr (::Standard::Algorithms::is_debug)
    {
        check_error_probability(result);
    }

    return result;
}

[[nodiscard]] auto Standard::Algorithms::Numbers::hashed_bit_array_utility::bit_array_size(
    const std::size_t capacity, const floating_t &error_probability) -> std::size_t
{
    check_capacity(capacity);
    check_error_probability(error_probability);

    constexpr floating_t coef = -2.082;

    const auto ratio = coef * std::log(error_probability);

    const auto capacity_bits = static_cast<floating_t>(capacity) * ratio;

    const auto capacity_bits_cast = static_cast<std::size_t>(capacity_bits);

    require_less_equal(
        capacity, capacity_bits_cast, "capacity for error probability " + std::to_string(error_probability));

    auto result = calc_prime(capacity_bits_cast);

    require_less_equal(capacity_bits_cast, result,
        "calc_prime error, error probability " + std::to_string(error_probability) + ", capacity " +
            std::to_string(capacity));

    check_bit_array_size(result);

    return result;
}

[[nodiscard]] auto Standard::Algorithms::Numbers::hashed_bit_array_utility::number_of_hash_functions(
    const std::size_t capacity, const std::size_t bit_array_size) -> std::size_t
{
    check_capacity(capacity);
    check_bit_array_size(bit_array_size);

    static const auto log_2 = std::log(2.0); // todo(p3): constexpr

    const auto result_raw = static_cast<floating_t>(bit_array_size) * log_2 / static_cast<floating_t>(capacity);

    auto result = static_cast<std::size_t>(result_raw);

    check_number_of_hash_functions(capacity, bit_array_size, result);

    return result;
}

void Standard::Algorithms::Numbers::hashed_bit_array_utility::check_capacity(const std::size_t value)
{
    constexpr auto edge_value = 3U;

    require_greater(value, edge_value, "capacity");
}

void Standard::Algorithms::Numbers::hashed_bit_array_utility::check_bit_array_size(const std::size_t value)
{
    constexpr auto edge_value = 3U;

    require_greater(value, edge_value, "Bit array size");
}

void Standard::Algorithms::Numbers::hashed_bit_array_utility::check_error_probability(const floating_t &value)
{
    constexpr auto epsi = 1E-7;
    constexpr auto maxim = 1 - epsi;

    if (epsi < value && value < maxim)
    {
        return;
    }

    auto str = ::Standard::Algorithms::Utilities::w_stream();

    str << "The error probability (" << value << ") must belong to the range (" << epsi << ", " << maxim
        << ") exclusively.";

    throw_exception(str);
}

void Standard::Algorithms::Numbers::hashed_bit_array_utility::check_number_of_hash_functions(
    const std::size_t capacity, const std::size_t bit_array_size, const std::size_t value)
{
    if (constexpr auto min_value = 3U; value < min_value)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The number of hash functions (" << value << ") must be at least " << min_value << ", capacity "
            << capacity << ", bitArraySize " << bit_array_size << ".";

        throw_exception(str);
    }

    if (capacity <= value)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The number of hash functions (" << value << ") must be less than capacity (" << capacity << ")"
            << ", bitArraySize " << bit_array_size << ".";

        throw_exception(str);
    }
}
