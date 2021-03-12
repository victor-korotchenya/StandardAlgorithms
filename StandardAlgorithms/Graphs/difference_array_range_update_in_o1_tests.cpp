#include"difference_array_range_update_in_o1_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"difference_array_range_update_in_o1.h"
#include<tuple>

namespace
{
    using numb_t = std::int16_t;
    using tup_t = std::tuple<std::int32_t, std::int32_t, numb_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<numb_t> &&source, std::vector<tup_t> &&ops,
            std::vector<numb_t> &&expected) noexcept
            : base_test_case(std::move(name))
            , Source(std::move(source))
            , Ops(std::move(ops))
            , Expected(std::move(expected))
        {
        }

        [[nodiscard]] constexpr auto source() const &noexcept -> const std::vector<numb_t> &
        {
            return Source;
        }

        [[nodiscard]] constexpr auto ops() const &noexcept -> const std::vector<tup_t> &
        {
            return Ops;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::vector<numb_t> &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Source.size(), "source size.");
            Standard::Algorithms::require_positive(Ops.size(), "ops size.");
            ::Standard::Algorithms::ert::are_equal(Source.size(), Expected.size(), "expected size.");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("source", Source, str);
            ::Standard::Algorithms::print("ops", Ops, str);
            ::Standard::Algorithms::print("expected", Expected, str);
        }

private:
        std::vector<numb_t> Source;
        std::vector<tup_t> Ops;
        std::vector<numb_t> Expected;
    };

    template<class da_t>
    constexpr void calc_actual(const std::vector<tup_t> &ops, da_t &dat, std::vector<numb_t> &actual)
    {
        actual.clear();

        for (const auto &tup : ops)
        {
            dat.update(get<0>(tup), get<2>(tup), get<1>(tup));
        }

        dat.restore(actual);
    }

    void generate_test_case_random(std::vector<test_case> &test_cases)
    {
        constexpr auto min_size = 1;

        // NOLINTNEXTLINE
        constexpr auto max_size = ::Standard::Algorithms::is_debug ? 6 : 20;

        constexpr auto cutter = static_cast<numb_t>(100);

        Standard::Algorithms::Utilities::random_t<std::uint32_t> rnd(0, cutter);
        const auto size = rnd.operator() (min_size, max_size);
        std::vector<numb_t> source(size);

        Standard::Algorithms::Utilities::fill_random(source, size, cutter);

        constexpr auto steps = 20U;
        std::vector<tup_t> ops;

        for (std::uint32_t index{}; index < steps; ++index)
        {
            const auto val = static_cast<numb_t>(rnd());
            std::uint32_t left{};
            std::uint32_t right{};

            do
            {
                left = rnd(0U, size - 1U);
                right = rnd(0U, size);
            } while (left == right);

            if (right < left)
            {
                std::swap(left, right);
            }

            ops.emplace_back(left, right, val);
        }

        std::vector<numb_t> actual;

        Standard::Algorithms::Trees::difference_array_range_update_in_o1<numb_t> temp(source);
        calc_actual(ops, temp, actual);

        test_cases.emplace_back("Random", std::move(source), std::move(ops), std::move(actual));
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        generate_test_case_random(test_cases);

        test_cases.push_back({ "simple",
            // NOLINTNEXTLINE
            { 10, 40, 7, 20 },
            // NOLINTNEXTLINE
            { { 0, 3, static_cast<numb_t>(100) },
                // NOLINTNEXTLINE
                { 1, 2, static_cast<numb_t>(-1) },
                // NOLINTNEXTLINE
                { 0, 4, static_cast<numb_t>(25) },
                // NOLINTNEXTLINE
                { 2, 4, static_cast<numb_t>(250) } },
            // NOLINTNEXTLINE
            { 10 + 100 + 25, 40 + 100 - 1 + 25,
                // NOLINTNEXTLINE
                7 + 100 + 25 + 250,
                // NOLINTNEXTLINE
                20 + 25 + 250 } });
    }

    constexpr void run_test_case(const test_case &test)
    {
        Standard::Algorithms::Trees::difference_array_range_update_in_o1<numb_t> dat(test.source());

        std::vector<numb_t> actual;
        calc_actual(test.ops(), dat, actual);
        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "difference_array_range_update_in_o1");

        Standard::Algorithms::Trees::difference_array_slow<numb_t> da_slow(test.source());

        actual.clear();
        calc_actual(test.ops(), da_slow, actual);
        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "difference_array_slow");
    }
} // namespace

void Standard::Algorithms::Trees::Tests::difference_array_range_update_in_o1_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
