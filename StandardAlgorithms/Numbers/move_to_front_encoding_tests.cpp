#include"move_to_front_encoding_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"move_to_front_encoding.h"
#include<iostream>
#include<optional>

#define D_USE_ONE_BYTE_MFT (1) // todo(p4): check in Clang next.

namespace
{
    // constexpr auto use_one_byte_mft = true; // todo(p4): check in Clang next.
    constexpr bool use_one_byte_mft = (D_USE_ONE_BYTE_MFT) != 0; // todo(p4): check in Clang next.

    constexpr auto run_mft_perf = false;
    constexpr auto shall_print_mft = run_mft_perf;
    constexpr auto min_size = run_mft_perf ? 1'000'000 : 1;

    using key_t = std::conditional_t<use_one_byte_mft, std::uint8_t, std::uint16_t>;
    using encoded_t = std::vector<key_t>;

    using plain_text_t = std::conditional_t<use_one_byte_mft, std::string, encoded_t>;
    using cha_t = typename plain_text_t::value_type;

    using fast_coder_t = Standard::Algorithms::Numbers::move_to_front_encoder_still<key_t>;
    using still_slow_coder_t = Standard::Algorithms::Numbers::move_to_front_encoder<key_t>;
    using slow_coder_t = Standard::Algorithms::Numbers::move_to_front_encoder_slow<key_t>;

    using ns_elapsed_type = Standard::Algorithms::ns_elapsed_type;

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

            ::Standard::Algorithms::ert::are_equal(input.size(), expected.size(), "expected output size");
        }
    }

    [[maybe_unused]] constexpr void append_one_byte_tests(auto &test_cases)
    {
        if constexpr (!use_one_byte_mft)
        {
            return;
        }

        {// in hex 676F'6F64 20 6D6F'6F64
            const auto *const name = "good mood";

            test_cases.emplace_back(name, name, // NOLINTNEXTLINE
                encoded_t{
                    // NOLINTNEXTLINE
                    0x67, 0x6F, // While the keys are initially increasing, rank = key. NOLINTNEXTLINE
                    0x00, // "og". A repetition of the previous key, rank = 0. NOLINTNEXTLINE
                    0x66, // "og". 'd' == 100 == 0x64, plus 2 greater chars ahead. NOLINTNEXTLINE
                    0x23, // "dog". ' ' == 32 == 0x20, plus 3 greater chars ahead. NOLINTNEXTLINE
                    0x6E, // " dog". 'm' == 0x6D, plus 1 greater char ahead "o".
                    0x03, // "m dog". 'o' position is 3.
                    0x00, // "om dg". 'o' repeats at position 0.
                    0x03 // "om dg". 'd' is at 3.
                    // Final data: "dom g", and the remaining chars 0x0, 0x1, etc.
                });
        }
        {
            const auto *const name = "abba abaaaa";

            test_cases.emplace_back(name, name,
                encoded_t{
                    // NOLINTNEXTLINE
                    'a', 'b', 0, 1, // NOLINTNEXTLINE
                    0x22, // "ab". NOLINTNEXTLINE
                    1, // " ab". 'a' is at 1.
                    2, // "a b". 'b' is at 2.
                    1, // "ba ". 'a' is at 1.
                    0, 0, 0 // Repetition of the previous char.
                });
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        if constexpr (use_one_byte_mft && !run_mft_perf)
        {
#if D_USE_ONE_BYTE_MFT // todo(p4): check in Clang next.
            append_one_byte_tests(test_cases);
#endif

            Standard::Algorithms::throw_if_empty("append_one_byte_tests failure", test_cases);
        }

        using random_t = Standard::Algorithms::Utilities::random_t<std::int32_t>;

        constexpr cha_t min_char = '0';
        constexpr cha_t max_char = 'z';
        static_assert(min_char < max_char);

        random_t rnd(min_char, max_char);

        constexpr auto max_size = min_size * 2 + 20;
        static_assert(min_size < max_size);

        auto gena = [&rnd]()
        {
            if constexpr (use_one_byte_mft)
            {
                return Standard::Algorithms::Utilities::random_string<random_t, min_char, max_char>(
                    rnd, min_size, max_size);
            }
            else
            {
                return Standard::Algorithms::Utilities::random_vector<random_t>(
                    rnd, min_size, max_size, min_char, max_char);
            }
        };

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            auto text = gena();
            test_cases.emplace_back("Random" + std::to_string(att), std::move(text), std::nullopt);
        }
    }

    void run_test_case(const test_case &test)
    {
        const auto &input = test.input();

        const Standard::Algorithms::elapsed_time_ns tim_es;
        const auto encoded = Standard::Algorithms::Numbers::encode_mtf_slow<key_t, slow_coder_t>(input);
        [[maybe_unused]] const auto en_slow_elapsed = tim_es.elapsed();

        if (const auto &output = test.output(); output.has_value())
        {
            const auto &expected = output.value();

            ::Standard::Algorithms::ert::are_equal(expected, encoded, "encode_mtf_slow");
        }

        [[maybe_unused]] ns_elapsed_type de_slow_elapsed{};
        {
            const Standard::Algorithms::elapsed_time_ns tim;
            const auto actual =
                Standard::Algorithms::Numbers::decode_mtf_slow<cha_t, key_t, slow_coder_t, plain_text_t>(encoded);
            de_slow_elapsed = tim.elapsed();

            ::Standard::Algorithms::ert::are_equal(input, actual, "decode_mtf_slow");
        }

        // Still.
        [[maybe_unused]] ns_elapsed_type en_still_elapsed{};
        {
            const Standard::Algorithms::elapsed_time_ns tim;
            const auto actual = Standard::Algorithms::Numbers::encode_mtf_slow<key_t, still_slow_coder_t>(input);
            en_still_elapsed = tim.elapsed();

            ::Standard::Algorithms::ert::are_equal(encoded, actual, "encode_mtf still");
        }
        [[maybe_unused]] ns_elapsed_type de_still_elapsed{};
        {
            const Standard::Algorithms::elapsed_time_ns tim;
            const auto actual =
                Standard::Algorithms::Numbers::decode_mtf_slow<cha_t, key_t, still_slow_coder_t, plain_text_t>(encoded);
            de_still_elapsed = tim.elapsed();

            ::Standard::Algorithms::ert::are_equal(input, actual, "decode_mtf still");
        }

        // Fast.
        [[maybe_unused]] ns_elapsed_type en_fast_elapsed{};
        {
            const Standard::Algorithms::elapsed_time_ns tim;
            const auto actual = Standard::Algorithms::Numbers::encode_mtf_slow<key_t, fast_coder_t>(input);
            en_fast_elapsed = tim.elapsed();

            ::Standard::Algorithms::ert::are_equal(encoded, actual, "encode_mtf fast");
        }
        [[maybe_unused]] ns_elapsed_type de_fast_elapsed{};
        {
            const Standard::Algorithms::elapsed_time_ns tim;
            const auto actual =
                Standard::Algorithms::Numbers::decode_mtf_slow<cha_t, key_t, fast_coder_t, plain_text_t>(encoded);
            de_fast_elapsed = tim.elapsed();

            ::Standard::Algorithms::ert::are_equal(input, actual, "decode_mtf fast");
        }

        if constexpr (!shall_print_mft)
        {
            return;
        }

        // still
        const auto en_still_ratio = ::Standard::Algorithms::ratio_compute(en_fast_elapsed, en_still_elapsed);

        const auto de_still_ratio = ::Standard::Algorithms::ratio_compute(de_fast_elapsed, de_still_elapsed);

        // slow
        const auto en_slow_ratio = ::Standard::Algorithms::ratio_compute(en_fast_elapsed, en_slow_elapsed);

        const auto de_slow_ratio = ::Standard::Algorithms::ratio_compute(de_fast_elapsed, de_slow_elapsed);

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << " '" << test.name() << "', size " << input.size() <<
            // Time:
            ", en fast " << en_fast_elapsed << ", de fast " << de_fast_elapsed <<
            // Ratio slow:
            ", en slow rat " << en_slow_ratio << ", de slow rat " << de_slow_ratio <<
            // Ratio still:
            ", en still rat " << en_still_ratio << ", de still rat " << de_still_ratio << "\n";

        std::cout << str.str();
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::move_to_front_encoding_tests()
{
    // todo(p5): test on std::uint16_t.

    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
