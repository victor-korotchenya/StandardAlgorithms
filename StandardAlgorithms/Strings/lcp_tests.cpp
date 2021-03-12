#include"lcp_tests.h"
#include"../Numbers/permutation.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"suffix_array.h"

namespace
{
    void run_lcp_test(std::vector<bool> &taken, const std::vector<std::int32_t> &raw_sa_expected,
        const std::string &str, const std::vector<std::int32_t> &raw_lcp_expected, const bool is_computed = true)
    {
        try
        {
            std::vector<std::int32_t> lcp;
            const auto sar = Standard::Algorithms::Strings::suffix_array_slow(str, &lcp);

            const auto &sa_expected = is_computed ? raw_sa_expected : sar;
            ::Standard::Algorithms::ert::are_equal(str.size(), sa_expected.size(), "sar expected size");

            const auto &lcp_expected = is_computed ? raw_lcp_expected : lcp;
            ::Standard::Algorithms::ert::are_equal(str.size(), lcp_expected.size(), "lcp expected size");
            {
                const auto oka = Standard::Algorithms::Numbers::is_permutation1(sa_expected, taken, true);

                ::Standard::Algorithms::ert::are_equal(true, oka, "expected suffix array is permutation");
            }
            {
                std::vector<std::int32_t> sa_fast;
                std::vector<std::int32_t> lcp_fast;
                Standard::Algorithms::Strings::suffix_array_other(str, sa_fast, &lcp_fast);
                ::Standard::Algorithms::ert::are_equal(sa_expected, sa_fast, "sar suffix_array_other");
                ::Standard::Algorithms::ert::are_equal(lcp_expected, lcp_fast, "lcp suffix_array_other");
            }
            {
                const auto sa_still = Standard::Algorithms::Strings::suffix_array_slow_still(str);

                ::Standard::Algorithms::ert::are_equal(sa_expected, sa_still, "sar suffix_array_slow_still");
            }

            if (is_computed)
            {
                ::Standard::Algorithms::ert::are_equal(sa_expected, sar, "sar suffix_array_slow");
                ::Standard::Algorithms::ert::are_equal(lcp_expected, lcp, "lcp suffix_array_slow");
            }
        }
        catch (const std::exception &exc)
        {
            throw std::runtime_error("Error testing the string '" + str + "': " + exc.what());
        }
    }

    void random_tests()
    {
        const std::vector<std::int32_t> empt;
        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};
        std::vector<bool> taken;

        constexpr auto min_size = 1;
        constexpr auto max_size = min_size + 20;
        static_assert(min_size < max_size);

        {
            const std::string str(max_size, 'a');
            const auto sar = ::Standard::Algorithms::Utilities::iota_vector_reversed<std::int32_t>(max_size);
            const auto lcp = ::Standard::Algorithms::Utilities::iota_vector<std::int32_t>(max_size);
            run_lcp_test(taken, sar, str, lcp);
        }

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto str = Standard::Algorithms::Utilities::random_string(rnd, min_size, max_size);

            run_lcp_test(taken, empt, str, empt, false);
        }
    }

    void preset_tests()
    {
        // Here the last character don't have to be unique.

        std::vector<bool> taken;
        {
            // "banana" 0, len = 6.
            // "anana" 1
            // "nana" 2
            // "ana" 3
            // "na" 4
            // "a" 5
            //
            // Id, Sorted rotation, Start position:
            // 0 "a" 5
            // 1 "ana" 3
            // 2 "anana" 1
            // 3 "banana" 0
            // 4 "na" 4
            // 5 "nana" 2
            run_lcp_test(taken, // sa. NOLINTNEXTLINE
                { 5, 3, 1, 0, 4, 2 },
                "banana", // lcp. NOLINTNEXTLINE
                { 0, 1, 3, 0, 0, 2 });

            run_lcp_test(taken, // NOLINTNEXTLINE
                { 6, 5, 3, 1, 0, 4, 2 }, "bananaZ", // NOLINTNEXTLINE
                { 0, 0, 1, 3, 0, 0, 2 });
        }

        run_lcp_test(taken, // NOLINTNEXTLINE
            { 4, 5, 13, 16, 7, 9, 17, 3, 6, 8, 12, 2, 0, 10, 15, 11, 14, 1 }, "393200212178308812", // NOLINTNEXTLINE
            { 0, 1, 1, 0, 2, 1, 0, 1, 1, 2, 0, 1, 1, 0, 0, 1, 1, 0 });

        run_lcp_test(taken, { 3, 2, 1, 0 }, "9300", { 0, 1, 0, 0 });
        run_lcp_test(taken, { 1, 0 }, "22", { 0, 1 });
        run_lcp_test(taken, { 2, 1, 0 }, "222", { 0, 1, 2 });
        run_lcp_test(taken, { 3, 2, 1, 0 }, "2222", { 0, 1, 2, 3 });
        run_lcp_test(taken, { 4, 3, 2, 1, 0 }, "22222", { 0, 1, 2, 3, 4 });
        run_lcp_test(taken, { 4, 3, 2, 1, 0 }, "32222", { 0, 1, 2, 3, 0 });
        run_lcp_test(taken, {}, {}, {});
        run_lcp_test(taken, { 0 }, "c", { 0 });
        run_lcp_test(taken, { 1, 0 }, "cc", { 0, 1 });
        run_lcp_test(taken, { 0, 1 }, "ab", { 0, 0 });
        run_lcp_test(taken, { 1, 0 }, "ba", { 0, 0 });
        run_lcp_test(taken, { 2, 1, 0 }, "cba", { 0, 0, 0 });
        run_lcp_test(taken, { 0, 1, 2 }, "abc", { 0, 0, 0 });
        run_lcp_test(taken, { 0, 1, 2 }, "aab", { 0, 1, 0 });
    }
} // namespace

void Standard::Algorithms::Strings::Tests::lcp_tests()
{
    // Here the last character can by any.
    // See also suffix_array_tests.
    preset_tests();
    random_tests();
}
