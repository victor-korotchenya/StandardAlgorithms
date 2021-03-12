#include"next_greater_element_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"next_greater_element.h"
#include<optional>

namespace
{
    using item_t = std::int16_t;
    using size_t1 = std::int32_t;

    constexpr size_t1 no_next = -static_cast<size_t1>(1);
    static_assert(no_next < size_t1{});

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<item_t> &&source, std::vector<size_t1> &&expected) noexcept
            : base_test_case(std::move(name))
            , Source(std::move(source))
            , Expected(std::make_optional(std::move(expected)))
        {
        }

        constexpr test_case(std::string &&name, std::vector<item_t> &&source) noexcept
            : base_test_case(std::move(name))
            , Source(std::move(source))
        {
        }

        [[nodiscard]] constexpr auto source() const &noexcept -> const std::vector<item_t> &
        {
            return Source;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::optional<std::vector<size_t1>> &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            if (!Expected.has_value())
            {
                return;
            }

            const auto &exp = Expected.value();

            if (exp.empty())
            {
                return;
            }

            ::Standard::Algorithms::ert::are_equal(Source.size(), exp.size(), "Source and expected sizes.");

            ::Standard::Algorithms::ert::are_equal(no_next, exp.back(), "last expected");

            {
                const auto [min_it, max_it] = std::minmax_element(exp.cbegin(), exp.cend());

                {
                    const auto &min1 = *min_it;
                    Standard::Algorithms::require_less_equal(no_next, min1, "min expected");
                }

                if (const auto &max1 = *max_it; size_t1{} < max1)
                {
                    const auto max_index = static_cast<std::size_t>(max1);
                    Standard::Algorithms::require_greater(Source.size(), max_index, "max expected");
                }
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("source", Source, str);

            if (Expected.has_value())
            {
                const auto &exp = Expected.value();
                ::Standard::Algorithms::print("expected", exp, str);
            }
        }

private:
        std::vector<item_t> Source;
        std::optional<std::vector<size_t1>> Expected{};
    };

    void generate_test_case_random(std::vector<test_case> &test_cases)
    {
        constexpr auto min_size = 1;
        constexpr auto max_size = ::Standard::Algorithms::is_debug ? 10 : 50;

        std::vector<item_t> source(Standard::Algorithms::Utilities::random_t<std::int32_t>(min_size, max_size)());

        Standard::Algorithms::Utilities::fill_random(source, source.size());

        test_cases.emplace_back("Random", std::move(source));
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        generate_test_case_random(test_cases);

        test_cases.push_back({ "simple", // NOLINTNEXTLINE
            { 10, 40, 7, 20 }, { 1, -1, 3, -1 } });

        test_cases.push_back({ "trivial", // NOLINTNEXTLINE
            { 5 }, { -1 } });

        test_cases.push_back({ "empty", {}, {} });

        test_cases.push_back({ "pair", // NOLINTNEXTLINE
            { 5, 8 }, { 1, -1 } });

        test_cases.push_back({ "pair reversed", // NOLINTNEXTLINE
            { 8, 5 }, { -1, -1 } });
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<size_t1> fast;
        Standard::Algorithms::Numbers::next_greater_element(test.source(), fast);

        if (const auto &exp = test.expected(); exp.has_value())
        {
            ::Standard::Algorithms::ert::are_equal(exp.value(), fast, "next_greater_element");
        }

        std::vector<size_t1> slow;
        Standard::Algorithms::Numbers::next_greater_element_slow(test.source(), slow);

        ::Standard::Algorithms::ert::are_equal(fast, slow, "next_greater_element_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::next_greater_element_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
