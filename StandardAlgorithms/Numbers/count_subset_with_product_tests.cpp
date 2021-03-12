#include"count_subset_with_product_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"count_subset_with_product.h"
#include"default_modulus.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;

    constexpr int_t mod = Standard::Algorithms::Numbers::default_modulus;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, int_t product, std::vector<int_t> &&arr, int_t expected = 0) noexcept
            : base_test_case(std::move(name))
            , Arr(std::move(arr))
            , Product(product)
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto arr() const &noexcept -> const std::vector<int_t> &
        {
            return Arr;
        }

        [[nodiscard]] constexpr auto product() const &noexcept -> const int_t &
        {
            return Product;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const int_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Arr.size(), "arr size");
            Standard::Algorithms::require_all_positive(Arr, "arr");

            Standard::Algorithms::require_positive(Product, "product");

            Standard::Algorithms::require_non_negative(Expected, "expected");
            Standard::Algorithms::require_greater(mod, Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("arr", Arr, str);
            ::Standard::Algorithms::print_value("product", str, Product);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::vector<int_t> Arr;
        int_t Product;
        int_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "base0", 1,
            // NOLINTNEXTLINE
            { 2, 10, 7, 5 }, 0 });

        test_cases.push_back({ "base1", 1,
            // NOLINTNEXTLINE
            { 1, 10, 8 }, 1 });

        test_cases.push_back({ "base11", 1,
            // NOLINTNEXTLINE
            { 1, 10, 7, 1 }, 3 });

        test_cases.push_back({ "base2", 4,
            // NOLINTNEXTLINE
            { 1, 1, 2, 2, 3 }, 4 });

        test_cases.push_back({ "base3", 2,
            // NOLINTNEXTLINE
            { 1, 1, 2, 2, 3 }, 8 });

        constexpr auto min_value = 1;
        constexpr auto max_value = 36;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_value, max_value);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto max_size = 6;
            const auto size = rnd(1, max_size);

            constexpr auto max_product = 50;
            const auto product = rnd(1, max_product);

            std::vector<int_t> arr(size);
            std::generate(arr.begin(), arr.end(),
                [&rnd]
                {
                    return rnd();
                });

            test_cases.emplace_back("Random" + std::to_string(att), product, std::move(arr));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast =
            Standard::Algorithms::Numbers::count_subset_with_product<long_int_t, int_t, mod>(test.arr(), test.product());

        if (test.expected() != 0)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), fast, "count_subset_with_product");
        }

        const auto slow = Standard::Algorithms::Numbers::count_subset_with_product_slow<long_int_t, int_t, mod>(
            test.arr(), test.product());

        ::Standard::Algorithms::ert::are_equal(fast, slow, "count_subset_with_product_slow");

        if (test.product() == 1)
        {// Others are buggy.
            return;
        }

        const auto other = Standard::Algorithms::Numbers::count_subset_with_product_other<long_int_t, int_t, mod>(
            test.arr(), test.product());

        ::Standard::Algorithms::ert::are_equal(fast, other, "count_subset_with_product_other");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::count_subset_with_product_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
