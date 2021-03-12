#include"suffix_array_tests.h"
#include"../Numbers/permutation.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"suffix_array.h"
#include"ukkonen_suffix_tree_utilities.h"

namespace
{
    using uc_t = std::uint8_t;

    void sa_test(std::vector<bool> &taken, const std::string &str, const std::vector<std::int32_t> &raw_expected,
        const bool is_computed = true)
    {
        try
        {
            std::vector<std::int32_t> lcp{};
            const auto sar = Standard::Algorithms::Strings::suffix_array_slow(str, &lcp);
            const auto &expected = is_computed ? raw_expected : sar;
            ::Standard::Algorithms::ert::are_equal(str.size(), expected.size(), "expected size");
            {
                const auto oki = Standard::Algorithms::Numbers::is_permutation1(expected, taken, true);

                ::Standard::Algorithms::ert::are_equal(true, oki, "expected suffix array is permutation");
            }
            {
                const auto actual = Standard::Algorithms::Strings::suffix_array_slow_still(str);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "suffix_array_slow_still");
            }
            {
                std::vector<std::int32_t> sa_fast{};
                std::vector<std::int32_t> lcp_fast{};
                Standard::Algorithms::Strings::suffix_array_other(str, sa_fast, &lcp_fast);
                ::Standard::Algorithms::ert::are_equal(expected, sa_fast, "sar suffix_array_other");
                ::Standard::Algorithms::ert::are_equal(lcp, lcp_fast, "lcp suffix_array_other");
            }
            {
                const Standard::Algorithms::Strings::ukkonen_suffix_tree<std::string, std::int32_t, uc_t> tree(str);
                const auto &nodes = tree.nodes();
                const auto actual = Standard::Algorithms::Strings::build_suffix_array<std::int32_t>(str, nodes);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "build_suffix_array");
            }

            if (is_computed)
            {
                ::Standard::Algorithms::ert::are_equal(expected, sar, "suffix_array_slow");
            }
        }
        catch (const std::exception &exc)
        {
            throw std::runtime_error("Error testing the string '" + str + "': " + exc.what());
        }
    }

    void sa_test_pair(std::vector<bool> &taken, const std::pair<std::string, std::vector<std::int32_t>> &str_expected)
    {
        sa_test(taken, str_expected.first, str_expected.second);
    }

    [[nodiscard]] constexpr auto sample1()
    {
        std::string stri("invis");
        stri.push_back(2);
        stri.push_back(0);
        {
            constexpr auto min = std::numeric_limits<char>::min();
            stri.push_back(min);
            stri.push_back(min);
        }
        stri.push_back('i');
        stri.push_back('n');
        stri.push_back('b');

        // NOLINTNEXTLINE
        std::vector<std::int32_t> sar{ 6, 5, 11, 9, 0, 3, 10, 1, 4, 2, 8, 7 };
        return std::make_pair(stri, sar);
    }

    [[nodiscard]] constexpr auto long_sample22()
    {
        constexpr auto size = 20;

        std::string stri(size, 'a');
        ++stri.back();

        auto sar = ::Standard::Algorithms::Utilities::iota_vector<std::int32_t>(size);
        return std::make_pair(stri, std::move(sar));
    }

    [[nodiscard]] constexpr auto long_sample333()
    {
        constexpr auto size = 30;

        std::string stri(size, 'b');
        --stri.back();

        auto rev = ::Standard::Algorithms::Utilities::iota_vector_reversed<std::int32_t>(size);
        return std::make_pair(stri, std::move(rev));
    }

    [[nodiscard]] constexpr auto some_chars()
    {
        constexpr auto size = 4;

        std::string stri{};
        std::vector<std::int32_t> sar{};

        for (auto index = -size; index <= size; ++index)
        {
            stri.push_back(static_cast<char>(index));

            const auto val = index <= 0 ? index + 2 * size : index - 1;
            sar.push_back(val);
        }

        return std::make_pair(stri, sar);
    }

    [[nodiscard]] constexpr auto all_chars()
    {
        constexpr std::int32_t mini = -128;
        constexpr std::int32_t maxi = 127;
        static_assert(mini < 0 && 0 < maxi);

        std::string stri{};
        std::vector<std::int32_t> sar{};

        for (auto index = mini; index <= maxi; ++index)
        {
            stri.push_back(static_cast<char>(index));

            const auto val = index < 0 ? index - 2 * mini : index;
            sar.push_back(val);
        }

        return std::make_pair(stri, sar);
    }

    [[nodiscard]] constexpr auto all_chars_signed()
    {
        std::string stri{};
        std::vector<std::int32_t> sar{};

        constexpr auto mini = std::numeric_limits<uc_t>::min();
        constexpr auto maxo = std::numeric_limits<uc_t>::max();

        for (std::int32_t index = mini; index <= maxo; ++index)
        {
            stri.push_back(static_cast<char>(index));
            sar.push_back(index);
        }

        return std::make_pair(stri, sar);
    }

    void random_sa_test()
    {
        const std::string last_unique = "!";

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};
        std::vector<bool> taken{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = min_size + 20;
            static_assert(min_size < max_size);

            const auto str = Standard::Algorithms::Utilities::random_string(rnd, min_size, max_size) + last_unique;

            sa_test(taken, str, {}, false);
        }
    }
} // namespace

void Standard::Algorithms::Strings::Tests::suffix_array_tests()
{
    // The last character must be unique - required by Ukkonen's alg.
    // See also lcp_tests, having no such restriction.

    std::vector<bool> taken{};

    // NOLINTNEXTLINE
    sa_test(taken, "mississippiZ", { 11, 10, 7, 4, 1, 0, 9, 8, 6, 3, 5, 2 });

    // NOLINTNEXTLINE
    sa_test(taken, "abracadabraZ", { 11, 10, 7, 0, 3, 5, 8, 1, 4, 6, 9, 2 });

    // NOLINTNEXTLINE
    sa_test(taken, "abcabx", { 0, 3, 1, 4, 2, 5 });

    // NOLINTNEXTLINE
    sa_test(taken, "abcdefgh", { 0, 1, 2, 3, 4, 5, 6, 7 });

    // NOLINTNEXTLINE
    sa_test(taken, "hgfedcba", { 7, 6, 5, 4, 3, 2, 1, 0 });

    // NOLINTNEXTLINE
    sa_test(taken, "ccccccca", { 7, 6, 5, 4, 3, 2, 1, 0 });

    // NOLINTNEXTLINE
    sa_test(taken, "ccca", { 3, 2, 1, 0 });
    sa_test(taken, "cca", { 2, 1, 0 });
    sa_test(taken, "ca", { 1, 0 });
    sa_test(taken, "a", { 0 });

    sa_test_pair(taken, sample1());
    sa_test_pair(taken, long_sample22());
    sa_test_pair(taken, long_sample333());
    sa_test_pair(taken, some_chars());
    sa_test_pair(taken, all_chars());
    sa_test_pair(taken, all_chars_signed());

    const std::string last_unique = "#";

    // NOLINTNEXTLINE
    sa_test(taken, "cccacc" + last_unique, { 6, 3, 5, 2, 4, 1, 0 });

    // NOLINTNEXTLINE
    sa_test(taken, "cccabcc" + last_unique, { 7, 3, 4, 6, 2, 5, 1, 0 });

    // NOLINTNEXTLINE
    sa_test(taken, "ABAACBAB" + last_unique, { 8, 2, 6, 0, 3, 7, 1, 5, 4 });

    // NOLINTNEXTLINE
    sa_test(taken, "banana" + last_unique, { 6, 5, 3, 1, 0, 4, 2 });

    random_sa_test();
}
