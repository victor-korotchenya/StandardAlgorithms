#include"find_sorted_2d_matrix_tests.h"
#include"../Utilities/test_utilities.h"
#include"find_sorted_2d_matrix.h"
#include<array>
#include<optional>

namespace
{
    using item_t = std::int16_t;
    using result_t = std::pair<std::size_t, std::size_t>;
    using data_t = std::vector<std::vector<item_t>>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, data_t &&data, const item_t &key, result_t &&expected) noexcept
            : base_test_case(std::move(name))
            , Data(std::move(data))
            , Key(key)
            , Expected(std::move(expected))
        {
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const data_t &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto key() const &noexcept -> const item_t &
        {
            return Key;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const result_t &
        {
            return Expected;
        }

        [[nodiscard]] constexpr auto not_found() const noexcept -> bool
        {
            auto result = std::numeric_limits<std::size_t>::max() == Expected.first;

            return result;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            if (const auto first_failure = check_matrix_sorted(); first_failure.has_value())
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                ::Standard::Algorithms::print("The matrix of must be sorted", first_failure.value(), str);

                Standard::Algorithms::throw_exception(str);
            }

            if (not_found())
            {
                return;
            }

            const auto size = Data.size();
            Standard::Algorithms::require_greater(size, Expected.first, "expected row");

            const auto row_size = Data[0].size();
            Standard::Algorithms::require_greater(row_size, Expected.second, "expected column");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Data", Data, str);
            ::Standard::Algorithms::print_value("Key", str, Key);
            ::Standard::Algorithms::print("Expected", Expected, str);
        }

        [[nodiscard]] constexpr auto check_matrix_sorted() const noexcept -> std::optional<result_t>
        {
            const auto size = Data.size();

            if (0U == size)
            {
                return std::nullopt;
            }

            const auto row_size = Data[0].size();

            for (std::size_t row{}; row < size; ++row)
            {
                const auto &data_row = Data[row];

                if (const auto row_size_2 = data_row.size(); row_size != row_size_2)
                {
                    return std::make_optional(result_t{ row_size, 0U });
                }

                const auto &prev_data_row = 0U < row ? Data[row - 1U] : data_row;

                for (std::size_t column{}; column < row_size; ++column)
                {
                    const auto &cur = data_row[column];

                    const auto is_error =
                        (0U < column && cur < data_row[column - 1U]) || (0U < row && cur < prev_data_row[column]);

                    if (is_error)
                    {
                        return std::make_optional(result_t{ row, column });
                    }
                }
            }

            return std::nullopt;
        }

private:
        data_t Data;
        item_t Key;
        result_t Expected;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr auto none = std::numeric_limits<std::size_t>::max();

        constexpr item_t key9 = 9;

        const data_t source( // NOLINTNEXTLINE
            { { 1, 3, 5, 7, 8 }, { 2, 3, 5, 8, 10 }, { 3, 5, 8, 8, 10 },
                // NOLINTNEXTLINE
                { 8, key9, 10, 123, 345 } });
        {
            auto dest = source;

            test_cases.emplace_back("Simple", std::move(dest), key9, result_t{ 3, 1 });
        }
        {
            auto dest = source;

            test_cases.emplace_back("Min value", std::move(dest), source[0][0], result_t{});
        }
        {
            auto dest = source;

            test_cases.emplace_back("Max value", std::move(dest), source.back().back(),
                result_t{ source.size() - 1U, source[0].size() - 1U });
        }
        {
            constexpr auto key = std::numeric_limits<item_t>::max();

            auto dest = source;

            test_cases.emplace_back("Not existing", std::move(dest), key, result_t{ none, none });
        }
        {
            auto dest = source;
            dest.erase(dest.begin() + 1, dest.end());

            result_t expected{ 0, dest.at(0).size() >> 1U };

            auto key = dest[expected.first][expected.second];

            test_cases.emplace_back("One row", std::move(dest), key, std::move(expected));
        }
        {
            // NOLINTNEXTLINE
            constexpr item_t key = 15;

            data_t dest({ { key } });

            test_cases.emplace_back("One value", std::move(dest), key, result_t{});
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const Standard::Algorithms::Numbers::find_sorted_2d_matrix<item_t> algol;

        const auto actual = algol.search(test.data(), test.key());

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "find_sorted_2d_matrix search");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::find_sorted_2d_matrix_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
