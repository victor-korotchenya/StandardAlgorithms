#include"max_sum_increasing_subsequence_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"../Utilities/require_utilities.h"
#include"default_modulus.h"
#include"max_sum_increasing_subsequence.h"
#include<tuple>

namespace
{
    using int_t = std::uint32_t;
    using long_int_t = std::uint64_t;
    using size_t1 = std::uint32_t;

    using alg_t = Standard::Algorithms::Numbers::max_sum_increasing_subsequence<int_t, long_int_t, size_t1>;

    // Input, sum, chosen indexes.
    using test_case = std::tuple<std::vector<int_t>, long_int_t, std::vector<size_t1>>;

    // todo(p3): use the base test class.

    [[nodiscard]] auto gen_random_test() -> test_case
    {
        constexpr auto min_size = 1;
        constexpr auto max_size = 10;

        const auto size = Standard::Algorithms::Utilities::random_t<std::uint32_t>(min_size, max_size)();

        std::vector<int_t> data;
        {
            constexpr auto cutter = static_cast<int_t>(Standard::Algorithms::Numbers::default_modulus);

            Standard::Algorithms::Utilities::fill_random<int_t>(data, size, cutter);
        }

        std::transform(data.cbegin(), data.cend(), data.begin(),
            // Make positive.
            [] [[nodiscard]] (const auto &val)
            {
                return val + 1;
            });

        auto [sum, chosen_indexes] = alg_t::compute_fast(data);

        return { std::move(data), sum, std::move(chosen_indexes) };
    }

    [[nodiscard]] auto gen_test_cases() -> std::vector<test_case>
    {
        return std::vector<test_case>{// NOLINTNEXTLINE
            test_case({ 738'037'162, 149'507'639, 345'537'762, 367'063'963, 608'683'613, // NOLINTNEXTLINE
                          261'236'996, 653'617'450, 617'585'322, 243'701'855, 621'195'149 }, // NOLINTNEXTLINE
                2'709'573'448, { 1, 2, 3, 4, 7, 9 }), // NOLINTNEXTLINE
            test_case({ 15, 7, 2, 15, 8 }, // NOLINTNEXTLINE
                22, { 1, 3 }), // NOLINTNEXTLINE
            test_case({ 2, 4, 6, 4, 13, 3, 3, 16 }, // NOLINTNEXTLINE
                41, { 0, 1, 2, 4, 7 }),
            gen_random_test(), // NOLINTNEXTLINE
            test_case({ 3, 2, 5 }, 8, { 0, 2 }), // NOLINTNEXTLINE
            test_case({ 17 }, 17, { 0 }), // NOLINTNEXTLINE
            test_case({ 1, 2 }, 3, { 0, 1 }), // NOLINTNEXTLINE
            test_case({ 1, 612, 70'005 }, // NOLINTNEXTLINE
                70'618, { 0, 1, 2 }), // NOLINTNEXTLINE
            test_case({ 1, 3, 10, 83 }, // NOLINTNEXTLINE
                97, { 0, 1, 2, 3 })
        };
    }

    constexpr void run_test(const test_case &test)
    {
        const auto &[data, expected_sum, expected_indexes] = test;

        ::Standard::Algorithms::ert::not_empty(data, "data");

        Standard::Algorithms::require_positive(expected_sum, "expected sum");

        ::Standard::Algorithms::ert::not_empty(expected_indexes, "expected chosen indexes");

        Standard::Algorithms::require_sorted(expected_indexes, "expected chosen indexes", true);

        const auto &last_chosen_index = expected_indexes.back();

        ::Standard::Algorithms::ert::greater(data.size(), last_chosen_index, "last expected chosen index");

        {
            long_int_t sum{};
            int_t prev_value{};

            for (const auto &chosen : expected_indexes)
            {
                const auto &item = data.at(chosen);

                ::Standard::Algorithms::ert::greater(item, prev_value, "previous value");
                prev_value = item;

                const auto old = sum;
                sum += item;
                Standard::Algorithms::require_greater(sum, old, "sum");
            }

            ::Standard::Algorithms::ert::are_equal(expected_sum, sum, "expected sum");
        }
        {
            const auto fast = alg_t::compute_fast(data);

            ::Standard::Algorithms::ert::are_equal(expected_sum, fast.first, "fast sum");

            ::Standard::Algorithms::ert::are_equal(expected_indexes, fast.second, "fast chosen indexes");
        }
        {
            const auto slow = alg_t::compute_slow(data);

            ::Standard::Algorithms::ert::are_equal(expected_sum, slow.first, "slow sum");

            ::Standard::Algorithms::ert::are_equal(expected_indexes, slow.second, "slow chosen indexes");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::max_sum_increasing_subsequence_tests()
{
    const auto test_cases = gen_test_cases();

    for (std::size_t index{}; index < test_cases.size(); ++index)
    {
        const auto &test = test_cases[index];

        try
        {
            run_test(test);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();

            const auto &data = std::get<0>(test);
            ::Standard::Algorithms::print("Input data", data, str);

            str << "\nTest " << index << " has an error: " << exc.what();

            throw_exception(str);
        }
    }
}
