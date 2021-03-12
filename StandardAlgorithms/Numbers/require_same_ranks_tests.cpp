#include"require_same_ranks_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"require_same_ranks.h"
#include<initializer_list>
#include<optional>

namespace // build_ranks tests
{
    using int_t = std::int32_t;
    using in_data_t = std::vector<int_t>;
    using ranks_t = std::vector<std::size_t>;

    struct unique_tag final
    {
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, in_data_t, std::optional<ranks_t>>;

    constexpr void simple_validate(const test_case &test)
    {
        const auto &input = test.input();
        Standard::Algorithms::require_positive(input.size(), "input size.");

        const auto &opt_output = test.output();
        if (!opt_output.has_value())
        {
            return;
        }

        const auto &ranks = opt_output.value();
        Standard::Algorithms::Numbers::validate_ranks(input.size(), ranks);
    }

    void generate_test_cases_random(std::vector<test_case> &test_cases)
    {
        constexpr auto min_value = std::numeric_limits<int_t>::min();
        constexpr auto max_value = std::numeric_limits<int_t>::max();
        static_assert(min_value < max_value);

        Standard::Algorithms::Utilities::random_t<int_t> rnd(min_value, max_value);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 20;

            auto input = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);

            test_cases.emplace_back("Random" + std::to_string(att), std::move(input), std::nullopt);
        }
    }

    constexpr void generate_test_cases_xpr(std::vector<test_case> &test_cases)
    {
        constexpr int_t some = 20;

        test_cases.emplace_back("Trivial", in_data_t{ some }, ranks_t{ 0ZU });

        test_cases.emplace_back("Decr 2", in_data_t{ some, some - 1 }, ranks_t{ 1ZU, 0ZU });
        test_cases.emplace_back("Decr 20", in_data_t{ some + some, some }, ranks_t{ 1ZU, 0ZU });
        test_cases.emplace_back("Same 2", in_data_t{ some, some }, ranks_t{ 0ZU, 0ZU });
        test_cases.emplace_back("Incr 2", in_data_t{ some, some + 1 }, ranks_t{ 0ZU, 1ZU });
        test_cases.emplace_back("Incr 20", in_data_t{ some, some + some }, ranks_t{ 0ZU, 1ZU });

        test_cases.emplace_back("111", in_data_t{ some, some, some }, ranks_t{ 0ZU, 0ZU, 0ZU });
        test_cases.emplace_back("112", in_data_t{ some, some, some + 1 }, ranks_t{ 0ZU, 0ZU, 1ZU });
        test_cases.emplace_back("121", in_data_t{ some, some + 1, some }, ranks_t{ 0ZU, 1ZU, 0ZU });
        test_cases.emplace_back("122", in_data_t{ some, some + 1, some + 1 }, ranks_t{ 0ZU, 1ZU, 1ZU });
        test_cases.emplace_back("211", in_data_t{ some, some - 1, some - 1 }, ranks_t{ 1ZU, 0ZU, 0ZU });
        test_cases.emplace_back("212", in_data_t{ some, some / 3, some }, ranks_t{ 1ZU, 0ZU, 1ZU });
        test_cases.emplace_back("221", in_data_t{ some, some, some - 1 }, ranks_t{ 1ZU, 1ZU, 0ZU });

        test_cases.emplace_back("123", in_data_t{ some, some + 1, some + 2 }, ranks_t{ 0ZU, 1ZU, 2ZU });
        test_cases.emplace_back("132", in_data_t{ some + 1, some + 3, some + 2 }, ranks_t{ 0ZU, 2ZU, 1ZU });
        test_cases.emplace_back("213", in_data_t{ some + 2, some + 1, some + 3 }, ranks_t{ 1ZU, 0ZU, 2ZU });
        test_cases.emplace_back("231", in_data_t{ some + 2, some + 3, some + 1 }, ranks_t{ 1ZU, 2ZU, 0ZU });
        test_cases.emplace_back("312", in_data_t{ some + 3, some + 1, some + 2 }, ranks_t{ 2ZU, 0ZU, 1ZU });
        test_cases.emplace_back("321", in_data_t{ some, some - 1, some - 2 }, ranks_t{ 2ZU, 1ZU, 0ZU });

        test_cases.emplace_back("31233563 to 20122342",
            in_data_t{ some + 3, some + 1, some + 2, // NOLINTNEXTLINE
                some + 3, some + 3, some + 5, some + 6, some + 3 }, // NOLINTNEXTLINE
            ranks_t{ 2ZU, 0ZU, 1ZU, 2ZU, 2ZU, 3ZU, 4ZU, 2ZU });
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &input = test.input();
        const auto slow = Standard::Algorithms::Numbers::build_ranks_slow(input);

        if (const auto &opt_output = test.output(); opt_output.has_value())
        {
            const auto &expected = opt_output.value();
            ::Standard::Algorithms::ert::are_equal(expected, slow, "build_ranks_slow");
        }
        else
        {
            Standard::Algorithms::Numbers::validate_ranks(input.size(), slow);
        }

        {
            const auto fast = Standard::Algorithms::Numbers::build_ranks(input);
            ::Standard::Algorithms::ert::are_equal(slow, fast, "build_ranks");
        }
    }
} // namespace

namespace // require_same_ranks tests
{
    using func_t = bool (*)();

    constexpr auto never_throw_on_error = false;

    [[nodiscard]] constexpr auto all_same() -> bool
    {
        constexpr int_t some = 30;

        const std::vector<int_t> source{ some, some, some };
        const auto &good_ment = source;

        const auto actual = Standard::Algorithms::Numbers::require_same_ranks(source, "all_same", good_ment);
        return actual;
    }

    [[nodiscard]] constexpr auto all_similar() -> bool
    {
        constexpr int_t some = 30;
        constexpr auto size = 3U;

        const std::string an_0 = "Sim";
        const std::string an_1 = "Sima";
        const std::string an_2 = "Simon";

        const std::array<int_t, size> source{ some, some + 1, some - 1 };
        const std::vector<std::string> a_target{ an_1, an_2, an_0 };

        const auto actual = Standard::Algorithms::Numbers::require_same_ranks(source, "all_similar", a_target);
        return actual;
    }

    [[nodiscard]] constexpr auto diff_types_diff_sizes() -> bool
    {
        const std::vector<int_t> odd{ 7, 1, 4 };
        const std::vector<std::string> even{ "A", "Bb" };

        const auto actual =
            Standard::Algorithms::Numbers::require_same_ranks(odd, "diff_sizes", even, never_throw_on_error);

        return !actual;
    }

    void throw_diff_types_sizes_test() // todo(p4): constexpr in C++26?
    {
        const std::vector<int_t> odd{ 7, 1, 4 };
        const std::vector<std::string> even{ "A", "Bb" };

        const std::string name = "throw diff types size";
        const auto expected_message = "Same rank sizes. " + name;

        ::Standard::Algorithms::ert::expect_exception<std::runtime_error>(
            expected_message,
            [&odd, &name, &even]() -> void
            {
                Standard::Algorithms::Numbers::require_same_ranks(odd, name, even);
            },
            "throw_diff_types_sizes");
    }

    [[nodiscard]] constexpr auto same_rank_test_impl() -> bool
    {
        const auto funcs = std::initializer_list<func_t>{ all_same, all_similar, diff_types_diff_sizes };

        auto good = std::all_of(funcs.begin(), funcs.end(),
            [](const auto &func) -> bool
            {
                auto eval = func();
                return eval;
            });

        return good;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::require_same_ranks_tests()
{
    static_assert(same_rank_test_impl());

#ifndef __clang__
    static_assert(
        Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases_xpr));
#else
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases_xpr);
#endif

    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases_random);

    throw_diff_types_sizes_test();
}
