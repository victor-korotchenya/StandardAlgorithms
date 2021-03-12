#include"z_alg_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"z_alg.h"

namespace // z+array tests.
{
    constexpr auto len5 = 5;
    constexpr auto len10 = 10;

    constexpr void z_test(
        const std::string &str, const std::vector<std::size_t> &raw_expected, const bool is_computed = true)
    {
        try
        {
            const auto fast = Standard::Algorithms::Strings::z_array(str);
            const auto &expected = is_computed ? raw_expected : fast;
            ::Standard::Algorithms::ert::are_equal(str.size(), expected.size(), "size");
            {
                const auto slow = Standard::Algorithms::Strings::z_array_slow(str);
                ::Standard::Algorithms::ert::are_equal(expected, slow, "z_array_slow");
            }

            if (is_computed)
            {
                ::Standard::Algorithms::ert::are_equal(expected, fast, "z_array");
            }
        }
        catch (const std::exception &exc)
        {
            throw std::runtime_error("z test '" + str + "': " + exc.what());
        }
    }

    [[nodiscard]] constexpr auto zar_tests() -> bool
    {
        z_test("AbAcAbAcAbAe", { len10 + 2, 0, 1, 0, len5 + 2, 0, 1, 0, 3, 0, 1, 0 });
        z_test("abacabacaba", { len10 + 1, 0, 1, 0, len5 + 2, 0, 1, 0, 3, 0, 1 });
        z_test("aab#baabaa", { len10, 1, 0, 0, 0, 3, 1, 0, 2, 1 });
        z_test("abcabda", { len5 + 2, 0, 0, 2, 0, 0, 1 });
        z_test("aaaaaacaaa", { len10, len5, 4, 3, 2, 1, 0, 3, 2, 1 });
        z_test("babababa", { len5 + 3, 0, len5 + 1, 0, 4, 0, 2, 0 });
        z_test("abababa", { len5 + 2, 0, len5, 0, 3, 0, 1 });
        z_test("bbbb", { 4, 3, 2, 1 });
        z_test("bbb", { 3, 2, 1 });
        z_test("bb", { 2, 1 });
        z_test("b", { 1 });
        z_test("", {});

        return true;
    }

    void random_zara_tests()
    {
        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto str = Standard::Algorithms::Utilities::random_string(rnd);

            z_test(str, {}, false);
        }
    }
} // namespace

namespace // string border tests.
{
    constexpr void border_test(
        const std::string &str, const std::vector<std::size_t> &raw_expected, const bool is_computed = true)
    {
        try
        {
            const auto fast = Standard::Algorithms::Strings::string_borders(str);
            const auto &expected = is_computed ? raw_expected : fast;
            {
                const auto slow = Standard::Algorithms::Strings::string_borders_slow(str);
                ::Standard::Algorithms::ert::are_equal(expected, slow, "string_borders_slow");
            }

            if (is_computed)
            {
                ::Standard::Algorithms::ert::are_equal(expected, fast, "string_borders");
            }
        }
        catch (const std::exception &exc)
        {
            throw std::runtime_error("border test '" + str + "': " + exc.what());
        }
    }

    [[nodiscard]] constexpr auto bordeaux_tests() -> bool
    {
        border_test("AbAcAbAcAbAe", {});
        border_test("abacabacaba", { 1, 3, len5 + 2 });
        border_test("aab#baabaa", { 1, 2 });
        border_test("abcabda", { 1 });
        border_test("aaaaaacaaa", { 1, 2, 3 });
        border_test("babababa", { 2, 4, len5 + 1 });
        border_test("abababa", { 1, 3, len5 });
        border_test("bbbb", { 1, 2, 3 });
        border_test("bbb", { 1, 2 });
        border_test("bb", { 1 });
        border_test("b", {});
        border_test("", {});

        return true;
    }

    void random_bordeaux_tests()
    {
        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto str = Standard::Algorithms::Utilities::random_string(rnd);
            border_test(str, {}, false);
        }
    }
} // namespace

void Standard::Algorithms::Strings::Tests::z_array_tests()
{
    static_assert(zar_tests());

    random_zara_tests();
}

void Standard::Algorithms::Strings::Tests::string_border_tests()
{
    static_assert(bordeaux_tests());

    random_bordeaux_tests();
}
