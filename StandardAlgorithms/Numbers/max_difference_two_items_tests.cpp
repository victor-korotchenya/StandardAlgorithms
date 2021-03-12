#include"max_difference_two_items_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"arithmetic.h"
#include"max_difference_two_items.h"

namespace
{
    using int_t = std::int16_t;

    using subtract_t = int_t (*)(const int_t &one, const int_t &two);

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&data, int_t expected_difference = {},
            std::pair<std::size_t, std::size_t> &&expected_indexes = {}) noexcept
            : base_test_case(std::move(name))
            , Data(std::move(data))
            , Expected_difference(expected_difference)
            , Expected_indexes(std::move(expected_indexes))
        {
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<int_t> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto expected_difference() const &noexcept -> const int_t &
        {
            return Expected_difference;
        }

        [[nodiscard]] constexpr auto expected_indexes() const &noexcept -> const std::pair<std::size_t, std::size_t> &
        {
            return Expected_indexes;
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Data", Data, str);

            str << " Expected difference " << Expected_difference << ", expected index min " << Expected_indexes.first
                << ", expected index max " << Expected_indexes.second;
        }

private:
        std::vector<int_t> Data;
        int_t Expected_difference;
        std::pair<std::size_t, std::size_t> Expected_indexes;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("Simple1", // NOLINTNEXTLINE
            std::vector<int_t>{ -1548, 920, -1685, 1376, 2148 }, // NOLINTNEXTLINE
            3833, std::make_pair(2, 4));

        test_cases.emplace_back("Simple2", // NOLINTNEXTLINE
            std::vector<int_t>{ 6865, 8522, 12780, 14155, 290, // NOLINTNEXTLINE
                11779, 13995, 102, 12761 }, // NOLINTNEXTLINE
            13705, std::make_pair(4, 6));

        test_cases.emplace_back("Simple3", // NOLINTNEXTLINE
            std::vector<int_t>{ 500, 500, 2, 3, 10, 10, 10, // NOLINTNEXTLINE
                7, 5, 9, 0 }, // NOLINTNEXTLINE
            10 - 2, std::make_pair(2, 4));

        test_cases.emplace_back("Many decreasing.", // NOLINTNEXTLINE
            std::vector<int_t>{ 12, 7, 10, 5, 6, 3, 2, 1 }, // NOLINTNEXTLINE
            10 - 7, std::make_pair(1, 2));

        {
            constexpr auto size = 5;

            std::vector<int_t> data;

            for (std::int32_t index{}; index < size; ++index)
            {
                data.push_back(static_cast<int_t>((size - index) * 2));
            }

            test_cases.emplace_back("All decreasing by 2.", std::move(data), // NOLINTNEXTLINE
                0 - 2, std::make_pair(0, 1));
        }

        test_cases.emplace_back("Two decrem", // NOLINTNEXTLINE
            std::vector<int_t>{ 1779, 10 }, // NOLINTNEXTLINE
            -1769, std::make_pair(0, 1));

        {
            constexpr int_t big = 100;

            std::vector<int_t> data(1, big);
            auto previous = big;

            // NOLINTNEXTLINE
            int_t delta = 7;

            do
            {
                previous = static_cast<int_t>(previous - delta);
                data.push_back(previous);
            } while (3 <= --delta);

            test_cases.emplace_back("Decreasing by 7, 6, 5, 4, 3.",
                std::move(data), // NOLINTNEXTLINE
                0 - 3, std::make_pair(4, 5));
        }

        constexpr auto max_attempts = 1;

        for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
        {
            constexpr auto size = 10;
            constexpr int_t cutter = std::numeric_limits<int_t>::max() / 2;

            std::vector<int_t> data;

            Standard::Algorithms::Utilities::fill_random(data, size + attempt, cutter);

            test_cases.emplace_back("random " + std::to_string(attempt), std::move(data));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        using return_t = std::pair<int_t, std::pair<std::size_t, std::size_t>>;

        using func_t = return_t (*)(subtract_t, const std::vector<int_t> &);

        const std::array sub_cases{
            std::make_pair("Slow",
                static_cast<func_t>(Standard::Algorithms::Numbers::max_difference_two_items_slow<int_t, subtract_t>)),
            std::make_pair(
                "Fast", static_cast<func_t>(Standard::Algorithms::Numbers::max_difference_two_items<int_t, subtract_t>))
        };

        subtract_t subtract = Standard::Algorithms::Numbers::subtract_signed<int_t>;

        // Random tests aren't computed - max index is zero.
        return_t expected{ test.expected_difference(), test.expected_indexes() };

        for (const auto &sub : sub_cases)
        {
            const auto name = std::string(sub.first);

            const auto [actual, min_max_indexes] = sub.second(subtract, test.data());

            const auto &[min_index, max_index] = min_max_indexes;

            ::Standard::Algorithms::ert::greater(test.data().size(), max_index, "max index " + name);

            ::Standard::Algorithms::ert::greater(max_index, min_index, "max min indexes " + name);

            {
                const auto checked = test.data().at(max_index) - test.data().at(min_index);

                ::Standard::Algorithms::ert::are_equal(checked, actual, "checked " + name);
            }

            if (const auto &exp_ind = expected.second.second; 0U == exp_ind)
            {
                expected = { actual, min_max_indexes };
                continue;
            }

            ::Standard::Algorithms::ert::are_equal(expected.first, actual, "difference " + name);

            ::Standard::Algorithms::ert::are_equal(expected.second.first, min_index, "min index " + name);

            ::Standard::Algorithms::ert::are_equal(expected.second.second, max_index, "max index " + name);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::max_difference_two_items_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
