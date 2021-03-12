#include"array_concatenate_min_tests.h"
#include"../Utilities/ert.h"
#include"array_concatenate_min.h"
#include<tuple>

namespace
{
    constexpr void predicate_test()
    {
        using tupo_t = std::tuple<std::int32_t, std::int32_t, bool>;

        const std::vector<std::string> data{ "1", "12", "123", "0", "32" };
        const std::vector<tupo_t> tuples{ tupo_t(0, 1, true), tupo_t(0, 2, true), tupo_t(1, 2, true),
            // Zero
            tupo_t(0, 3, false), tupo_t(3, 0, true),
            // 32
            tupo_t(0, 4, true), tupo_t(4, 0, false), tupo_t(2, 4, true), tupo_t(4, 2, false),
            // self
            tupo_t(2, 2, false) };

        for (std::size_t index{}; const auto &[pos, pos_2, expected] : tuples)
        {
            const auto &one = data.at(pos);
            const auto &two = data.at(pos_2);

            const auto actual = Standard::Algorithms::Numbers::is_ab_less_ba(one, two);

            ::Standard::Algorithms::ert::are_equal(
                expected, actual, "is_ab_less_ba at index " + ::Standard::Algorithms::Utilities::zu_string(index));

            ++index;
        }
    }

    constexpr void main_test()
    {
        using int_t = std::uint64_t;

        const std::vector<int_t> data{// NOLINTNEXTLINE
            159, 16, 4, 3, 1, 15, 153
        };

        const auto actual = Standard::Algorithms::Numbers::array_concatenate_min(data);

        const std::string expected = "1151531591634";
        ::Standard::Algorithms::ert::are_equal(expected, actual, "array_concatenate_min");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::array_concatenate_min_tests()
{
    predicate_test();
    main_test();
}
