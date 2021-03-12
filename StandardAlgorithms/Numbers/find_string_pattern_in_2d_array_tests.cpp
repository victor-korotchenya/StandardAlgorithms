#include"find_string_pattern_in_2d_array_tests.h"
#include"../Utilities/test_utilities.h"
#include"find_string_pattern_in_2d_array.h"

namespace
{
    using distance_t = std::size_t;
    using point_t = ::Standard::Algorithms::Geometry::point2d<distance_t>;
    using data_t = std::vector<std::string>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, data_t &&data, std::string &&pattern, bool expected_result = {},
            std::vector<point_t> &&expected_points = {}) noexcept
            : base_test_case(std::move(name))
            , Data(std::move(data))
            , Pattern(std::move(pattern))
            , Expected_result(expected_result)
            , Expected_points(std::move(expected_points))
        {
            std::sort(Expected_points.begin(), Expected_points.end());
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const data_t &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto pattern() const &noexcept -> const std::string &
        {
            return Pattern;
        }

        [[nodiscard]] constexpr auto expected_result() const noexcept -> bool
        {
            return Expected_result;
        }

        [[nodiscard]] constexpr auto expected_points() const &noexcept -> const std::vector<point_t> &
        {
            return Expected_points;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            ::Standard::Algorithms::ert::are_equal(
                Expected_result ? Pattern.size() : 0U, Expected_points.size(), "Pattern and points sizes.");
        }

        void print(std::ostream &str) const override
        {
            str << ", Pattern '" << Pattern << "'";
            ::Standard::Algorithms::print("Data", Data, str);
            str << " Expected result " << Expected_result;
            ::Standard::Algorithms::print("Expected points", Expected_points, str);
        }

private:
        data_t Data;
        std::string Pattern;

        bool Expected_result;
        std::vector<point_t> Expected_points;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("Simple1",
            data_t({ "nbed", // e
                "aacm", // m
                "ebad", // a
                "macn" }), // n
            "name", true,
            std::vector<point_t>({// NOLINTNEXTLINE
                point_t{ 0, 2 }, point_t{ 1, 3 }, // NOLINTNEXTLINE
                point_t{ 2, 2 }, point_t{ 3, 3 } }));

        //{todo(p4): "Random test."
        //    constexpr auto size = 5U;
        //    constexpr auto cutter = 50U;
        //    fill_random(test.Data, size, cutter);
        //}
    }

    void run_test_case(const test_case &test)
    {
        std::vector<point_t> actual_points;

        const auto actual = Standard::Algorithms::Numbers::find_string_pattern_in_2d_array_slow(
            test.data(), test.pattern(), actual_points);

        std::sort(actual_points.begin(), actual_points.end());

        ::Standard::Algorithms::ert::are_equal(test.expected_result(), actual, "Slow result");

        ::Standard::Algorithms::ert::are_equal(test.expected_points(), actual_points, "Slow points");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::find_string_pattern_in_2d_array_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
