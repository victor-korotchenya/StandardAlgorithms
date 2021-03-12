#include"cross_point_two_incr_decr_lines_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"cross_point_two_incr_decr_lines.h"
#include<vector>

namespace
{
    using number_x = std::int32_t;
    using number_y = std::int64_t;

    void one_run_cross_point_tests()
    {
        constexpr auto maxi = ::Standard::Algorithms::is_debug ? 64 : 641;
        constexpr auto mini = -maxi;
        constexpr auto cutter = ::Standard::Algorithms::is_debug ? 64 : 0;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(mini, maxi);

        const auto one = rnd();
        const auto two = rnd();

        const auto beginning = std::min(one, two);
        const auto ending = std::max(one, two);
        const auto size = ending - beginning + 1;

        std::vector<number_y> inc_v(size);

        Standard::Algorithms::Utilities::fill_random<number_y, number_x>(inc_v, size, cutter);
        std::sort(inc_v.begin(), inc_v.end());

        std::vector<number_y> dec_v(size);
        Standard::Algorithms::Utilities::fill_random<number_y, number_x>(dec_v, size, cutter);

        std::sort(dec_v.begin(), dec_v.end(),
            [] [[nodiscard]] (const number_y &val1, const number_y &val2)
            {// reverse
                return val2 < val1;
            });

        auto incr_line = [&inc_v, &beginning](const number_x &val)
        {
            return inc_v.at(val - beginning);
        };

        auto decr_line = [&dec_v, &beginning](const number_x &val)
        {
            return dec_v.at(val - beginning);
        };

        try
        {
            const auto slow = Standard::Algorithms::Numbers::cross_point_two_incr_decr_lines_slow<number_y, number_x,
                decltype(incr_line), decltype(decr_line)>(beginning, incr_line, decr_line, ending);

            ::Standard::Algorithms::ert::greater_or_equal(slow, beginning, "slow beginning");
            ::Standard::Algorithms::ert::greater_or_equal(ending, slow, "slow ending");

            const auto fast = Standard::Algorithms::Numbers::cross_point_two_incr_decr_lines<number_y, number_x,
                decltype(incr_line), decltype(decr_line)>(beginning, incr_line, decr_line, ending);

            if (slow == fast)
            {
                return;
            }

            const auto inc1 = incr_line(slow);
            const auto dec1 = decr_line(slow);
            const auto dif1 = Standard::Algorithms::Numbers::abs_diff(dec1, inc1);

            const auto inc2 = incr_line(fast);
            const auto dec2 = decr_line(fast);
            const auto diff2 = Standard::Algorithms::Numbers::abs_diff(dec2, inc2);

            const auto message = "diff, slow " + std::to_string(slow) + ", fast " + std::to_string(fast);

            ::Standard::Algorithms::ert::are_equal(dif1, diff2, message);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Error: " << exc.what() << "\nbeginning " << beginning << ", ending " << ending << '\n';

            ::Standard::Algorithms::print("inc_v", inc_v, str);
            ::Standard::Algorithms::print("dec_v", dec_v, str);

            Standard::Algorithms::throw_exception(str);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::cross_point_two_incr_decr_lines_tests()
{
    constexpr auto max_attempts = 1;

    for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
    {
        one_run_cross_point_tests();
    }
}
