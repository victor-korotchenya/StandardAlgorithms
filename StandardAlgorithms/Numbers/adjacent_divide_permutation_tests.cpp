#include"adjacent_divide_permutation_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"adjacent_divide_permutation.h"

namespace
{
    constexpr auto n_min = 2;
    constexpr auto n_max = 7;
    constexpr auto mod = 101;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<char> &&allowed, std::int32_t size,
            std::vector<std::int32_t> &&expected = {}) noexcept
            : base_test_case(std::move(name))
            , Allowed(std::move(allowed))
            , Size(size)
            , Expected(std::move(expected))
        {
        }

        [[nodiscard]] constexpr auto allowed() const &noexcept -> const std::vector<char> &
        {
            return Allowed;
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::int32_t
        {
            return Size;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            ::Standard::Algorithms::ert::greater_or_equal(Size, n_min, "size");
            ::Standard::Algorithms::ert::greater_or_equal(n_max, Size, "size");

            ::Standard::Algorithms::ert::greater(Allowed.size(), static_cast<std::size_t>(Size), "allowed size");

            if (Expected.empty())
            {
                return;
            }

            ::Standard::Algorithms::ert::are_equal(Expected.size(), static_cast<std::size_t>(Size), "expected size");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("allowed", Allowed, str);
            str << ", size " << Size;
            ::Standard::Algorithms::print("expected", Expected, str);
        }

private:
        std::vector<char> Allowed;
        std::int32_t Size;
        std::vector<std::int32_t> Expected;
    };

    void generate_test_cases(std::vector<test_case> &tests)
    {
        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(n_min, n_max);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto size = rnd();

            std::vector<char> allowed(size + 1LL);

            for (auto index = 2; index <= size; ++index)
            {
                allowed[index] = static_cast<char>(rnd(0, 1));
            }

            tests.emplace_back("Random" + std::to_string(att), std::move(allowed), size);
        }
    }

    void run_test_case(const test_case &test)
    {
        const auto fast = Standard::Algorithms::Numbers::adjacent_divide_permutation(test.size(), test.allowed(), mod);

        if (!test.expected().empty())
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "adjacent_divide_permutation");
        }

        const auto actual =
            Standard::Algorithms::Numbers::adjacent_divide_permutation_slow(test.size(), test.allowed(), mod);

        ::Standard::Algorithms::ert::are_equal(fast, actual, "adjacent_divide_permutation_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::adjacent_divide_permutation_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
