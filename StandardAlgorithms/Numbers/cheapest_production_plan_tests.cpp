#include"cheapest_production_plan_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"arithmetic.h"
#include"cheapest_production_plan.h"

namespace
{
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;
    using extra_t = long_int_t (*)(long_int_t);

    constexpr long_int_t inf = 1'125'899'906'842'624; // 1 << 50

    [[nodiscard]] constexpr auto extra_two(long_int_t val) -> long_int_t
    {
        Standard::Algorithms::require_non_negative(val, "extra_two");

        constexpr long_int_t coef = 2;

        auto prod = Standard::Algorithms::Numbers::multiply_signed(val, coef);

        constexpr long_int_t limit = 100;

        auto xtra = limit < val ? Standard::Algorithms::Numbers::add_signed(prod, inf) : prod;

        return xtra;
    }

    [[nodiscard]] constexpr auto extra_seven(long_int_t val) -> long_int_t
    {
        Standard::Algorithms::require_non_negative(val, "extra_seven");

        constexpr long_int_t coef = 7;

        auto prod = Standard::Algorithms::Numbers::multiply_signed(val, coef);

        constexpr long_int_t limit = 50;

        auto xtra = limit < val ? Standard::Algorithms::Numbers::add_signed(prod, inf) : prod;

        return xtra;
    }

    [[nodiscard]] constexpr auto extra_squared(long_int_t val) -> long_int_t
    {
        Standard::Algorithms::require_non_negative(val, "extra_squared");

        auto prod = Standard::Algorithms::Numbers::multiply_signed(val, val);

        assert(!(val < long_int_t{}) && !(prod < val));

        return prod;
    }

    [[nodiscard]] constexpr auto compute_prod_cost(const std::vector<int_t> &sell_plan, extra_t extra_costs,
        const long_int_t &daily_max_wagons, extra_t store_costs, const std::vector<long_int_t> &produce_plan)
        -> long_int_t
    {
        const auto size = Standard::Algorithms::require_positive(sell_plan.size(), "sell plan size");
        ::Standard::Algorithms::ert::are_equal(size, produce_plan.size(), "produce plan size");
        Standard::Algorithms::require_positive(daily_max_wagons, "daily max wagons");
        Standard::Algorithms::throw_if_null("extra costs func", extra_costs);
        Standard::Algorithms::throw_if_null("store costs func", store_costs);

        long_int_t stored_wagons{};
        long_int_t computed_cost{};

        for (std::size_t day{}; day < size; ++day)
        {
            const auto suffix = " at day " + std::to_string(day);
            Standard::Algorithms::require_non_negative(stored_wagons, "stored wagons" + suffix);

            const auto &sales = sell_plan[day];
            Standard::Algorithms::require_non_negative(sales, "sell daily" + suffix);

            const auto &today_wagons = produce_plan[day];
            Standard::Algorithms::require_non_negative(today_wagons, "today produce plan" + suffix);

            if (daily_max_wagons < today_wagons)
            {
                const auto add = extra_costs(today_wagons);
                Standard::Algorithms::require_positive(add, "extra costs" + suffix);
                computed_cost = Standard::Algorithms::Numbers::add_signed(computed_cost, add);
            }

            stored_wagons = Standard::Algorithms::Numbers::add_signed(stored_wagons, today_wagons);

            Standard::Algorithms::require_less_equal(sales, stored_wagons, "sales vs stored wagons" + suffix);
            stored_wagons -= sales;

            if (long_int_t{} < stored_wagons)
            {
                const auto add = store_costs(stored_wagons);
                Standard::Algorithms::require_positive(add, "store cost" + suffix);
                computed_cost = Standard::Algorithms::Numbers::add_signed(computed_cost, add);
            }
        }

        ::Standard::Algorithms::ert::are_equal(long_int_t{}, stored_wagons, "final stored wagons");

        Standard::Algorithms::require_non_negative(computed_cost, "computed cost");
        return computed_cost;
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&sell_plan, extra_t extra_costs,
            long_int_t daily_max_wagons, extra_t store_costs, std::vector<long_int_t> &&produce_plan,
            long_int_t central_plan_min_cost)
            : base_test_case(std::move(name))
            , Sell_plan(std::move(sell_plan))
            , Extra_costs(extra_costs)
            , Daily_max_wagons(daily_max_wagons)
            , Store_costs(store_costs)
            , Produce_plan(std::move(produce_plan))
            , Central_plan_min_cost(central_plan_min_cost)
        {
        }

        [[nodiscard]] constexpr auto sell_plan() const &noexcept -> const std::vector<int_t> &
        {
            return Sell_plan;
        }

        [[nodiscard]] constexpr auto extra_costs() const noexcept -> extra_t
        {
            return Extra_costs;
        }

        [[nodiscard]] constexpr auto daily_max_wagons() const noexcept -> long_int_t
        {
            return Daily_max_wagons;
        }

        [[nodiscard]] constexpr auto store_costs() const noexcept -> extra_t
        {
            return Store_costs;
        }

        [[nodiscard]] constexpr auto produce_plan() const &noexcept -> const std::vector<long_int_t> &
        {
            return Produce_plan;
        }

        [[nodiscard]] constexpr auto central_plan_min_cost() const noexcept -> long_int_t
        {
            return Central_plan_min_cost;
        }

        void print(std::ostream &str) const override
        {
            str << ", daily max wagons " << Daily_max_wagons;
            ::Standard::Algorithms::print("sell plan", Sell_plan, str);

            ::Standard::Algorithms::print("expected central production plan", Produce_plan, str);
            ::Standard::Algorithms::print_value("expected central plan cost", str, Central_plan_min_cost);
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Sell_plan.size(), "sell plan size");
            ::Standard::Algorithms::ert::are_equal(Sell_plan.size(), Produce_plan.size(), "produce plan size");

            Standard::Algorithms::require_all_non_negative(Sell_plan, "sell plan");
            Standard::Algorithms::require_all_non_negative(Produce_plan, "produce plan");

            {
                const auto expected = std::accumulate(Sell_plan.cbegin(), Sell_plan.cbegin(), long_int_t{});
                const auto actual = std::accumulate(Produce_plan.cbegin(), Produce_plan.cbegin(), long_int_t{});

                ::Standard::Algorithms::ert::are_equal(expected, actual, "sell and produce plans");
            }

            Standard::Algorithms::require_positive(Daily_max_wagons, "daily max wagons");

            Standard::Algorithms::throw_if_null("extra costs func", Extra_costs);
            Standard::Algorithms::throw_if_null("store costs func", Store_costs);

            Standard::Algorithms::require_non_negative(Central_plan_min_cost, "central plan min cost");

            {
                const auto actual =
                    compute_prod_cost(Sell_plan, Extra_costs, Daily_max_wagons, Store_costs, Produce_plan);

                ::Standard::Algorithms::ert::are_equal(Central_plan_min_cost, actual, "actual central plan cost");
            }
        }

private:
        std::vector<int_t> Sell_plan;
        extra_t Extra_costs;
        long_int_t Daily_max_wagons;
        extra_t Store_costs;
        std::vector<long_int_t> Produce_plan;
        long_int_t Central_plan_min_cost;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr int_t zero{};
        constexpr int_t one = 1;
        constexpr int_t two = 2;
        constexpr int_t ten = 10;
        constexpr int_t eleven = 11;
        constexpr int_t twelve = 12;
        constexpr int_t twenty = 20;

        test_cases.emplace_back("store expensive",
            std::vector<int_t>{ twelve, twenty }, // sell
                                                  // 12 20 -> 2*32 = 64 ; min
                                                  // 21 11 -> 2*21 + 7*(21 - 12) = 105
            extra_two, eleven, extra_seven, // plans:
            std::vector<long_int_t>{ twelve, twenty }, extra_two(twelve + twenty));

        test_cases.emplace_back("store nothing 72",
            std::vector<int_t>{ ten, twenty }, // sell
                                               // 10 20 -> 7*20 = 140 ; min
                                               // 20 10 -> 7*20 + 2*(20 - 10) = 160
            extra_seven, ten, extra_two, // plans:
            std::vector<long_int_t>{ ten, twenty }, extra_seven(twenty));

        test_cases.emplace_back("store nothing 27",
            std::vector<int_t>{ ten, twenty }, // sell
                                               // 10 20 -> 2*20 = 40 ; min
                                               // 20 10 -> 2*20 + 7*(20 - 10) = 110
            extra_two, ten, extra_seven, // plans:
            std::vector<long_int_t>{ ten, twenty }, extra_two(twenty));

        test_cases.emplace_back("store something",
            std::vector<int_t>{ twelve, twenty }, // sell
                                                  // 12 20 -> 7*(12 + 20) = 224
                                                  // 22 10 -> 7*(22) + 2*(22 - 12) = 174 ; min
            extra_seven, ten, extra_two, // plans:
            std::vector<long_int_t>{ twenty + two, ten }, extra_seven(twenty + two) + extra_two(twenty + two - twelve));

        {
            // day0 | day1 || prod cost | store cost | total cost
            // 12 | 20 || 12*12 + 20*20 | 7*0 | 144 + 400 + 0 = 544
            // 13 | 19 || 13*13 + 19*19 | 7*1 | 169 + 361 + 7 = 537
            // 14 | 18 || 14*14 + 18*18 | 7*2 | 196 + 324 + 14 = 534 ; local pot
            // 15 | 17 || 15*15 + 17*17 | 7*3 | 225 + 289 + 21 = 535
            // 16 | 16 || 16*16 + 16*16 | 7*4 | 512 + 28 = 540
            // 17 | 15 || 17*17 + 15*15 | 7*5 | 289 + 225 + 35 = 549
            // 18 | 14 || 18*18 + 14*14 | 7*6 | 324 + 196 + 42 = 562
            // 19 | 13 || 19*19 + 13*13 | 7*7 | 361 + 169 + 49 = 579
            // 20 | 12 || 20*20 + 12*12 | 7*8 | 400 + 144 + 56 = 600
            // 21 | 11 || 21*21 + 00*00 | 7*9 | 441 + 0 + 63 = 504 ; min
            // 22 | 10 || 22*22 + 00*0 | 7*10 | 484 + 0 + 70 = 554
            // 23 | 09 || 23*23 + 00*0 | 7*11 | 529 + 0 + 77 = 606
            // 24 | 08 || 23*23 + 00*0 | 7*12 | 576 + 0 + 84 = 660
            // 25 | 07 || 23*23 + 00*0 | 7*13 | 625 + 0 + 91 = 716
            // 26 | 06 || 23*23 + 00*0 | 7*14 | 676 + 0 + 98 = 774
            // 27 | 05 || 23*23 + 00*0 | 7*15 | 729 + 0 + 105 = 834
            // 28 | 04 || 23*23 + 00*0 | 7*16 | 784 + 0 + 112 = 896
            // 29 | 03 || 23*23 + 00*0 | 7*17 | 841 + 0 + 119 = 960
            // 30 | 02 || 23*23 + 00*0 | 7*18 | 900 + 0 + 126 = 1026
            // 31 | 01 || 23*23 + 00*0 | 7*19 | 961 + 0 + 133 = 1094
            // 32 | 00 || 32*32 + 00*0 | 7*20 | 1024 + 140 = 1164
            constexpr long_int_t min_cost = 504;

            static_assert(
                min_cost == extra_squared(twenty + one) + extra_squared(0) + extra_seven(twenty + one - twelve));

            test_cases.emplace_back("quad store something", std::vector<int_t>{ twelve, twenty }, // sell
                extra_squared, eleven, extra_seven, // plans:
                std::vector<long_int_t>{ twenty + one, eleven }, min_cost);

            test_cases.emplace_back("zero after all", std::vector<int_t>{ twelve, twenty, zero }, // sell
                extra_squared, eleven, extra_seven, // plans:
                std::vector<long_int_t>{ twenty + one, eleven, zero }, min_cost);
        }

        test_cases.emplace_back("trivial below max", std::vector<int_t>{ ten }, // sell
            extra_two, twelve, extra_seven, // plans:
            std::vector<long_int_t>{ ten }, zero);

        test_cases.emplace_back("trivial", std::vector<int_t>{ twelve }, // sell
            extra_two, twelve, extra_seven, // plans:
            std::vector<long_int_t>{ twelve }, zero);

        test_cases.emplace_back("trivial above max", std::vector<int_t>{ twenty }, // sell
            extra_two, twelve, extra_seven, // plans:
            std::vector<long_int_t>{ twenty }, extra_two(twenty));

        test_cases.emplace_back("trivial above max 7", std::vector<int_t>{ twenty }, // sell
            extra_seven, twelve, extra_two, // plans:
            std::vector<long_int_t>{ twenty }, extra_seven(twenty));
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<long_int_t> produce_plan;

        {
            const auto actual_cost = Standard::Algorithms::Numbers::cheapest_production_plan<long_int_t, extra_t, int_t>(
                test.sell_plan(), test.extra_costs(), test.daily_max_wagons(), test.store_costs(), produce_plan);

            ::Standard::Algorithms::ert::are_equal(test.central_plan_min_cost(), actual_cost, "central plan cost");
        }

        ::Standard::Algorithms::ert::are_equal(test.produce_plan(), produce_plan, "central produce plan");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::cheapest_production_plan_tests()
{
    ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
