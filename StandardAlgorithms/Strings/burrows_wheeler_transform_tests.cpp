#include"burrows_wheeler_transform_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"burrows_wheeler_transform.h"
#include"string_to_vector.h"
#include<optional>

namespace
{
    using plain_text_t = std::string;
    using text_char_t = typename plain_text_t::value_type;

    using encoded_char_t = std::int16_t;
    using encoded_t = std::vector<encoded_char_t>;

    static_assert(
        sizeof(text_char_t) < sizeof(encoded_char_t), "Use more RAM to guarantee the smallest unique minimum.");

    constexpr auto unique_smallest = std::numeric_limits<encoded_char_t>::min();
    static_assert(unique_smallest < 0);

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, plain_text_t, std::optional<encoded_t>>;

    constexpr void simple_validate(const test_case &test)
    {
        const auto &input = test.input();
        Standard::Algorithms::throw_if_empty("input", input);

        const auto &output = test.output();
        if (output.has_value())
        {
            const auto &expected = output.value();

            ::Standard::Algorithms::ert::are_equal(input.size() + 1LLU, expected.size(), "expected output size");
        }
    }

    [[nodiscard]] constexpr auto to_vector(const plain_text_t &beginning, const plain_text_t &ending) -> encoded_t
    {
        auto vec = Standard::Algorithms::Strings::string_to_vector<plain_text_t, encoded_char_t>(beginning);
        vec.reserve(1ZU + ending.size());
        vec.push_back(unique_smallest);

        auto endi = Standard::Algorithms::Strings::string_to_vector<plain_text_t, encoded_char_t>(ending);
        vec.insert(vec.end(), endi.begin(), endi.end());

        return vec;
    }

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        {// "-1" is a single, minimum char.
            const auto *const source = "banana"; // len = 6.
            // "banana-1" 0, len = 7.
            // "-1banana" 1
            // "a-1banan" 2
            // "na-1bana" 3
            // "ana-1ban" 4
            // "nana-1ba" 5
            // "anana-1b" 6
            //
            // Id, Sorted rotation, Original id:
            // 0 "-1banana" 1
            // 1 "a-1banan" 2
            // 2 "ana-1ban" 4
            // 3 "anana-1b" 6
            // 4 "banana-1" 0
            // 5 "na-1bana" 3
            // 6 "nana-1ba" 5
            auto expected = to_vector("annb", "aa");
            // {"annb", -1, "aa"} ; computed BWT.
            // {"0123",  4, "56"} ; ordinals.
            // {"1564",  0, "23"} ; ranks. Note. -1 has minimal rank = 0.
            // 'a' ranks are 1, 2, 3 from the leftmost 'a'.
            // 'b' rank is 4.
            // 'n' ranks are 5, 6.
            //
            // Given BWT and ranks:
            // Set result = "", pos = 0.
            // Cycle until result.size() + 1 < BWT.size():
            // - result += BWT[pos], which is 'a', so result = "a"; set pos = ranks[pos] = ranks[0] = 1.
            // - result += BWT[pos], which is 'n', result = "an"; pos = ranks[pos] = ranks[1] = 5.
            // - result += BWT[pos], which is 'a', result = "ana"; pos = ranks[pos] = ranks[5] = 2.
            // - result += BWT[pos], which is 'n', result = "anan"; pos = ranks[pos] = ranks[2] = 6.
            // - result += BWT[pos], which is 'a', result = "anana"; pos = ranks[pos] = ranks[6] = 3.
            // - result += BWT[pos], which is 'b', result = "ananab"; pos = ranks[pos] = ranks[3] = 4.
            // - Stop as (6 + 1 < 7) does not hold.
            // Reverse the result  "ananab" to restore the original "banana".
            test_cases.emplace_back(source, source, std::move(expected));
        }

        using random_t = Standard::Algorithms::Utilities::random_t<std::int32_t>;

        constexpr auto min_size = 1;
        constexpr auto max_size = min_size + 18;
        static_assert(0 < min_size && min_size < max_size);

        constexpr auto min_char = '0';
        constexpr auto max_char = 'z';
        static_assert(min_char < max_char);

        random_t rnd(min_char, max_char);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            auto text =
                Standard::Algorithms::Utilities::random_string<random_t, min_char, max_char>(rnd, min_size, max_size);

            test_cases.emplace_back("Random" + std::to_string(att), std::move(text), std::nullopt);
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &input = test.input();
        const auto vec = Standard::Algorithms::Strings::string_to_vector<plain_text_t, encoded_char_t>(input);

        const auto encoded = Standard::Algorithms::Strings::burrows_wheeler_transform_slow(vec);

        if (const auto &output = test.output(); output.has_value())
        {
            const auto &expected = output.value();

            ::Standard::Algorithms::ert::are_equal(expected, encoded, "burrows_wheeler_transform_slow");
        }
        {
            const auto &fast = Standard::Algorithms::Strings::burrows_wheeler_transform<encoded_t>(input);

            ::Standard::Algorithms::ert::are_equal(encoded, fast, "burrows_wheeler_transform");
        }
        {
            const auto &rev = Standard::Algorithms::Strings::bwt_reverse<plain_text_t>(encoded);

            ::Standard::Algorithms::ert::are_equal(input, rev, "bwt_reverse");
        }
    }
} // namespace

void Standard::Algorithms::Strings::Tests::burrows_wheeler_transform_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
