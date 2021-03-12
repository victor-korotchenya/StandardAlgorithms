#include"suffix_array_search_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"suffix_array.h"
#include<string_view>

namespace
{
    [[nodiscard]] constexpr auto is_submatch(
        const std::string &text, const std::int32_t text_index, const std::string &pattern) -> bool
    {
        const auto text_size = static_cast<std::int32_t>(text.size());
        const auto pat_size = static_cast<std::int64_t>(pattern.size());

        auto result = !(text_index < 0) && 0 < pat_size && text_index < text_size && text_index + pat_size <= text_size;

        if (result)
        {
            const std::string_view vie(text);
            const auto sub = vie.substr(text_index, pat_size);
            result = sub == pattern;
        }

        return result;
    }

    // There might be several matching positions e.g. "w" in "www".
    constexpr void verify_search_result(const std::string &text, const std::int32_t expected,
        const std::string &pattern, const std::int32_t actual, const std::string &test_name)
    {
        if (expected == actual)
        {
            return;
        }

        const auto is_expected = is_submatch(text, expected, pattern);
        const auto is_actual = is_submatch(text, actual, pattern);
        if (is_expected && is_actual)
        {
            return;
        }

        ::Standard::Algorithms::ert::are_equal(expected, actual, test_name);
    }

    void ssa_test(const std::string &text, const std::string &pattern, const std::int32_t raw_expected,
        const bool is_computed = true)
    {
        try
        {
            const auto suffix_array_of_text = Standard::Algorithms::Strings::suffix_array_slow_still(text);
            const auto slow =
                Standard::Algorithms::Strings::suffix_array_search_slow(text, suffix_array_of_text, pattern);

            const auto &expected = is_computed ? raw_expected : slow;
            ::Standard::Algorithms::ert::greater_or_equal(static_cast<std::int32_t>(text.size()), expected, "expected");
            {
                const auto fin = text.find(pattern);
                const auto find_position =
                    fin < text.size() ? static_cast<std::int32_t>(fin) : (pattern.empty() ? 0 : -1);

                verify_search_result(text, expected, pattern, find_position, "std find position");
            }

            if (is_computed)
            {
                verify_search_result(text, expected, pattern, slow, "suffix_array_search_slow");
            }
        }
        catch (const std::exception &exc)
        {
            throw std::runtime_error(std::string("Error testing the text '")
                                         .append(text)
                                         .append("', and the pattern '")
                                         .append(pattern)
                                         .append("': ")
                                         .append(exc.what()));
        }
    }
} // namespace

void Standard::Algorithms::Strings::Tests::suffix_array_search_tests()
{
    ssa_test("abcdefgh subtotal", "def", 3);
    ssa_test("abcdefgh", "def", 3);
    ssa_test("abc def gh", "def", 4);
    ssa_test("abc def", "def", 4);
    ssa_test("qty gh", "qty", 0);
    ssa_test("defdefdefdefdef", "def", 0);
    ssa_test("defdefdefdef", "def", 0);
    ssa_test("defdefdef", "def", 0);
    ssa_test("defdef", "def", 0);
    ssa_test("def", "def", 0);
    ssa_test(" def gh", "def", 1);
    ssa_test(" def gh", "de", 1);
    ssa_test(" def gh", "d", 1);
    ssa_test("def", "abcdefgh", -1);
    ssa_test("abcdefgh def", "def", 3);
    ssa_test("a", "a", 0);
    ssa_test("a", "", 0);
    ssa_test("", "", 0);
    ssa_test("", "a", -1);
    ssa_test("ba", "a", 1);
    ssa_test("ba", "A", -1);
    ssa_test("ccccccc", "ccc", 0);
    ssa_test("ccccccc", "cCc", -1);
    ssa_test("cccccCc", "cCc", 4);
    ssa_test("ccccccc", "ccca", -1);
    ssa_test("cca", "ca", 1);
    ssa_test("CACA", "CA", 0);
    ssa_test("CBAACA", "CA", 4);
    ssa_test("CBaaCa", "Ca", 4);

    Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

    constexpr auto max_attempts = 1;

    for (std::int32_t att{}; att < max_attempts; ++att)
    {
        constexpr auto min_size = 0;
        constexpr auto max_size = min_size + 40;

        const auto text = Standard::Algorithms::Utilities::random_string(rnd, min_size, max_size);

        const auto pattern = Standard::Algorithms::Utilities::random_string(rnd, min_size, max_size);

        ssa_test(text, pattern, 0, false);
    }
}
