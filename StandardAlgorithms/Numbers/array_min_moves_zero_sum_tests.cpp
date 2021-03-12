#include"array_min_moves_zero_sum_tests.h"
#include"../Utilities/test_utilities.h"
#include"array_min_moves_zero_sum.h"
#include<numeric>

namespace
{
    using int_t = std::int64_t;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&ring, const std::size_t expected_moves) noexcept
            : base_test_case(std::move(name))
            , Ring(std::move(ring))
            , Expected_moves(expected_moves)
        {
        }

        [[nodiscard]] constexpr auto ring() const &noexcept -> const std::vector<int_t> &
        {
            return Ring;
        }

        [[nodiscard]] constexpr auto expected_moves() const noexcept -> std::size_t
        {
            return Expected_moves;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto sum = std::accumulate(Ring.cbegin(), Ring.cend(), int_t{});
            ::Standard::Algorithms::ert::are_equal(int_t{}, sum, "Total sum");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Ring", Ring, str);
            str << " Expected moves" << Expected_moves;
        }

private:
        std::vector<int_t> Ring;
        std::size_t Expected_moves;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "Empty", {}, 0 });
        test_cases.push_back({ "One element", { 0 }, 0 });

        test_cases.push_back({ "Two elements",
            // NOLINTNEXTLINE
            { -34536, 34536 }, 1 });

        test_cases.push_back({ "Two zeros", { 0, 0 }, 0 });
        test_cases.push_back({ "With zero in the middle", { 1, 0, -1 }, 1 });

        test_cases.push_back({ "Simple",
            // NOLINTNEXTLINE
            { 10, 1, -4, 2, 4, -3, -6, -4 }, 6 });

        /*{
          // todo(p3): random test.

          constexpr auto sizeMin = 2;
          constexpr auto sizeMax = 50;

          Standard::Algorithms::Utilities::random_t<std::uint32_t> rnd(sizeMin, sizeMax);
          const auto size = rnd();

          std::vector<int_t> data;
          Standard::Algorithms::Utilities::fill_random(data, size);

          const auto index = rnd(0, size - 1);
          const auto s = std::accumulate(data.cbegin(), data.cend(), static_cast<int_t>(0));
          data[index] -= s;

          auto result_fast = Standard::Algorithms::Numbers::array_min_moves_zero_sum(data);
          test_cases.push_back({ "Random", std::move(data), result_fast });
        }*/
    }

    constexpr void run_test_case(const test_case &test)
    {
        using alg = std::size_t (*)(const std::vector<int_t> &ring);

        const std::vector<std::pair<std::string, alg>> sub_tests = {
            { "Fast", &Standard::Algorithms::Numbers::array_min_moves_zero_sum<int_t> },
            //{ "Slow", &array_min_moves_zero_sum_slow<int_t> } // todo(p3): slow
        };

        for (const auto &sub : sub_tests)
        {
            const auto actual = sub.second(test.ring());
            ::Standard::Algorithms::ert::are_equal(test.expected_moves(), actual, sub.first);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::array_min_moves_zero_sum_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
