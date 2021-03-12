#include"digit_product_count_tests.h"
#include"../Utilities/is_debug.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"digit_product_count.h"
#include"hash_utilities.h"
#include<map>
#include<unordered_map>

namespace
{
    using int_t = std::int64_t;
    using tuple_t = std::tuple<int_t, std::int32_t, char>;
    using random_t = Standard::Algorithms::Utilities::random_t<std::uint32_t>;

    constexpr auto sorted_slow = true;
    constexpr auto shall_run_large = !Standard::Algorithms::is_debug;

    using map_t = std::conditional_t<sorted_slow,
        // slow:
        std::map<tuple_t, int_t>,
        // fast:
        std::unordered_map<tuple_t, int_t, Standard::Algorithms::Numbers::tuple_hash>>;

    constexpr auto digit_max = Standard::Algorithms::Numbers::digit_max;
    constexpr auto digit_ten = Standard::Algorithms::Numbers::digit_ten;

    [[maybe_unused]] constexpr auto run1(std::vector<char> &digits, map_t &temp, const int_t &numb,
        const std::string &n_str, const int_t &prod, const bool use_slow = true) -> int_t
    {
        if (int_t{} == prod) // todo(p3): Solve for 0 product.
        {
            return {};
        }

        const auto prod_str = std::to_string(prod);

        const auto fast = Standard::Algorithms::Numbers::digit_product_count<int_t>(numb, digits, prod, temp);

        if (use_slow)
        {
            const auto slow = Standard::Algorithms::Numbers::digit_product_count_slow(numb, prod);

            ::Standard::Algorithms::ert::are_equal(fast, slow, "digit_product_count_slow " + n_str + ", " + prod_str);
        }

        return fast;
    }

    [[nodiscard]] constexpr auto eval_product(int_t numb, random_t &rnd, const int_t &high) -> int_t
    {
        assert(int_t{} <= numb && int_t{} < high);

        if (numb <= digit_max)
        {
            return numb * high;
        }

        constexpr int_t one = 1;

        auto prod = one;

        do
        {
            if (numb <= digit_max)
            {
                prod *= one < numb ? numb - one : one;
                break;
            }

            const auto temp = rnd() & 3U;
            if (temp == 0U)
            {
                break;
            }

            prod *= 1U < temp ? digit_max : (numb % digit_max + one);

            numb /= digit_ten;
        } while (int_t{} < numb);

        return prod * high;
    }

    constexpr void run_specific_subtests(
        const std::vector<std::pair<std::pair<int_t, int_t>, int_t>> &n_results, std::vector<char> &digits, map_t &temp)
    {
        for (const auto &n_r : n_results)
        {
            const auto &numb = n_r.first.first;
            const auto &prod = n_r.first.second;
            const auto &expected = n_r.second;
            const auto n_str = std::to_string(numb);

            constexpr int_t max_small = 10'000;

            const auto actual = run1(digits, temp, numb, n_str, prod, numb < max_small);

            ::Standard::Algorithms::ert::are_equal(expected, actual, n_str);
        }
    }

    constexpr void test_specific(std::vector<char> &digits, map_t &temp)
    {
        const std::vector<std::pair<std::pair<int_t, int_t>, int_t>> n_results{
            // NOLINTNEXTLINE
            { { 100, 5 }, 3 }, // 5, 15, 51
            // NOLINTNEXTLINE
            { { 9'876, 10 }, 20 }, // 25,52,125,152,215,251,512,521,
            // 1125,1152,1215,1251,1512,1521,
            // 2115,2151,2511,
            // 5112,5121,5211.
        };

        run_specific_subtests(n_results, digits, temp);
    }

    [[maybe_unused]] constexpr void run_many(std::vector<char> &digits, map_t &temp)
    {
        constexpr auto maxi = 12;

        for (int_t numb{}; numb <= maxi; ++numb)
        {
            const auto n_str = ::Standard::Algorithms::Utilities::zu_string(numb);

            for (int_t prod{}; prod <= digit_ten; ++prod)
            {
                run1(digits, temp, numb, n_str, prod);
            }
        }
    }

    void run_random(std::vector<char> &digits, map_t &temp)
    {
        random_t rnd{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto mask = 0xFfFfU;
            constexpr auto mask_2 = 7U;
            constexpr auto shift = 28U;

            const auto some1 = rnd();
            const auto numb = static_cast<int_t>(some1 & mask);
            const auto n_str = std::to_string(numb);

            const auto high = static_cast<int_t>(((some1 >> shift) & mask_2) | 1U);
            const auto prod = eval_product(numb, rnd, high);

            run1(digits, temp, numb, n_str, prod);
        }
    }

    [[maybe_unused]] constexpr void run_large(std::vector<char> &digits, map_t &temp)
    {
        constexpr int_t prod0 = 1'000;
        constexpr int_t prod2 = 840'000;

        constexpr int_t kkk = 1'000;
        constexpr int_t k18 = kkk * kkk * kkk * kkk * kkk * kkk;
        constexpr int_t som = k18 / digit_ten - 1'234'567;

        const std::vector<std::pair<std::pair<int_t, int_t>, int_t>> n_results{// NOLINTNEXTLINE
            { { k18, prod0 }, 1'596'912 },
            // NOLINTNEXTLINE
            { { som, prod0 }, 1'042'032 },
            // NOLINTNEXTLINE
            { { k18, prod2 }, 14'867'793'360 }
        };

        run_specific_subtests(n_results, digits, temp);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::digit_product_count_tests()
{
    std::vector<char> digits;
    map_t temp;

    test_specific(digits, temp);
    run_random(digits, temp);

    if constexpr (shall_run_large)
    {
        run_many(digits, temp);
        run_large(digits, temp);
    }
}
