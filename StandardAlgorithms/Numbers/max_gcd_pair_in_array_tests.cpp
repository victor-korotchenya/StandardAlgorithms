#include"max_gcd_pair_in_array_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"max_gcd_pair_in_array.h"

namespace
{
    using int_t = std::uint32_t;
    using long_int_t = std::uint64_t;

    constexpr auto min_size = 2U;
    constexpr auto max_size = 20U;

    constexpr auto min_value = 0U;
    constexpr auto max_value = 100U;
} // namespace

void Standard::Algorithms::Numbers::Tests::max_gcd_pair_in_array_tests()
{
    Standard::Algorithms::Utilities::random_t<std::uint32_t> rnd(min_value, max_value);
    std::vector<int_t> values;

    try
    {
        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            // todo(p4): hard-coded tests.
            values = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);

            const auto fast = max_gcd_pair_in_array<long_int_t, int_t>(values);

            const auto slow = max_gcd_pair_in_array_slow(values);

            ::Standard::Algorithms::ert::are_equal(fast, slow, "max_gcd_pair_in_array.");
        }
    }
    catch (const std::exception &exc)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << exc.what();
        ::Standard::Algorithms::print("values", values, str);

        throw std::runtime_error(str.str());
    }
}
