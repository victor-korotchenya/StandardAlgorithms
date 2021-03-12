#include"subsequence_of_minimum_positive_sum_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"subsequence_of_minimum_positive_sum.h"
#include<array>

namespace
{
    using input_t = std::vector<std::int32_t>;
    using out_vec_t = std::vector<std::size_t>;

    constexpr auto not_computed = -1;

    constexpr void check_sum_by_indexes(const input_t &data, const std::int32_t sum, const out_vec_t &chosen_indexes)
    {
        if (sum < 0)
        {
            return;
        }

        Standard::Algorithms::require_positive(sum, "sum");
        Standard::Algorithms::require_positive(chosen_indexes.size(), "chosen indexes size");
        Standard::Algorithms::require_unique(chosen_indexes, "chosen indexes");

        std::int64_t total{};

        for (const auto &index : chosen_indexes)
        {
            total += data.at(index);
        }

        ::Standard::Algorithms::ert::are_equal(sum, total, "total sum for chosen indexes");
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, input_t &&data, std::int32_t sum = not_computed,
            out_vec_t &&chosen_indexes = {}) noexcept
            : base_test_case(std::move(name))
            , Data(data)
            , Sum(sum)
            , Chosen_indexes(chosen_indexes)
        {
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const input_t &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto sum() const noexcept -> std::int32_t
        {
            return Sum;
        }

        [[nodiscard]] constexpr auto chosen_indexes() const &noexcept -> const out_vec_t &
        {
            return Chosen_indexes;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Data.size(), "Data size");

            check_sum_by_indexes(Data, Sum, Chosen_indexes);
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("data", Data, str);
            ::Standard::Algorithms::print_value("sum", str, Sum);
            ::Standard::Algorithms::print("chosen indexes", Chosen_indexes, str);
        }

private:
        input_t Data;
        std::int32_t Sum;
        out_vec_t Chosen_indexes;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("zero", input_t{ 0 });

        test_cases.emplace_back("no positive", // NOLINTNEXTLINE
            input_t{ 0, -100 });

        test_cases.emplace_back("one number", // NOLINTNEXTLINE
            input_t{ 600 }, 600, out_vec_t{ 0 });

        test_cases.emplace_back("several positive", // NOLINTNEXTLINE
            input_t{ 600, 700, 50, 1'000 }, 50, out_vec_t{ 2 });

        test_cases.emplace_back("several positive large negative", // NOLINTNEXTLINE
            input_t{ 600, 700, 50, -1'000 }, 50, out_vec_t{ 2 });

        test_cases.emplace_back("several positive small negative", // NOLINTNEXTLINE
            input_t{ -10, 600, 50, 57'005 }, 40, out_vec_t{ 0, 2 });

        test_cases.emplace_back("several positives plus several negatives", // NOLINTNEXTLINE
            input_t{ -10, 600, 50, 700, -1'270 }, // NOLINTNEXTLINE
            20, out_vec_t{ 0, 1, 3, 4 }); // 600 + 700 - 1270 - 10

        test_cases.emplace_back("all numbers", // NOLINTNEXTLINE
            input_t{ -10, 600, 50, 700, -1'270, -65 }, // NOLINTNEXTLINE
            5, out_vec_t{ 0, 1, 2, 3, 4, 5 });
    }

    void run_test_case(const test_case &test)
    {
        out_vec_t chosen_indexes{};

        const auto sum =
            Standard::Algorithms::Numbers::subsequence_of_minimum_positive_sum_slow(test.data(), chosen_indexes);

        if (!(test.sum() < 0))
        {
            ::Standard::Algorithms::ert::are_equal(test.sum(), sum, "sum");
            ::Standard::Algorithms::ert::are_equal(test.chosen_indexes(), chosen_indexes, "chosen indexes");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::subsequence_of_minimum_positive_sum_tests()
{
    // todo(p4): add random tests.
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
