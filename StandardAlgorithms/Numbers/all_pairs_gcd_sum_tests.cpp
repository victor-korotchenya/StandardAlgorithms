#include"all_pairs_gcd_sum_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"all_pairs_gcd_sum.h"
#include"gcd.h"

void Standard::Algorithms::Numbers::Tests::all_pairs_gcd_sum_tests()
{
    using int_t = std::uint32_t;
    using long_int_t = std::uint64_t;

    constexpr auto size = 10;
    constexpr auto cutter = static_cast<int_t>(size * 2);

    std::vector<int_t> arr(size);
    Standard::Algorithms::Utilities::fill_random(arr, size, cutter);

    std::sort(arr.begin(), arr.end());

    const auto slow = Standard::Algorithms::Numbers::all_pairs_gcd_sum_slow<long_int_t>(
        arr, &Standard::Algorithms::Numbers::gcd_unsigned<int_t>);

    std::vector<long_int_t> temp;

    const auto expected = Standard::Algorithms::Numbers::all_pairs_gcd_sum<long_int_t>(
        arr, &Standard::Algorithms::Numbers::gcd_unsigned<int_t>, temp);

    if (expected != slow)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();

        for (const auto &value : arr)
        {
            constexpr auto separ = ',';
            str << value << separ;
        }

        const auto name = str.str();
        ::Standard::Algorithms::ert::are_equal(expected, slow, name);
    }
}
