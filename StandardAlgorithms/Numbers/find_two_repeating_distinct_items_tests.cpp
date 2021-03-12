#include"find_two_repeating_distinct_items_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"find_two_repeating_distinct_items.h"

namespace
{
    using int_t = std::uint16_t;

    template<int_t size>
    requires(0U < size)
    constexpr void subtest(
        const std::string &name, std::array<int_t, size> &data, const std::pair<int_t, int_t> &expected)
    {
        assert(!name.empty());

        Standard::Algorithms::require_positive(expected.first, name);

        Standard::Algorithms::require_greater(expected.second, expected.first, name);

        {
            const auto edge_number = static_cast<int_t>(size - 1U);

            Standard::Algorithms::require_greater(edge_number, expected.second, name);
        }

        Standard::Algorithms::Utilities::shuffle_data(data);

        int_t one{};
        int_t two{};

        Standard::Algorithms::Numbers::find_two_repeating_distinct_items_slow(data.data(), size, one, two);

        if (two < one)
        {
            std::swap(two, one);
        }

        ::Standard::Algorithms::ert::are_equal(expected, std::make_pair(one, two), name);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::find_two_repeating_distinct_items_tests()
{
    {
        constexpr int_t size = 4U;

        std::array<int_t, size> data{ 1, 1, 2, 2 };

        subtest<size>("Case1", data, std::make_pair(1, 2));
    }
    {
        constexpr int_t size = 7U;

        std::array<int_t, size> data{// NOLINTNEXTLINE
            1, 2, 2, 3, 4, 4, 5
        };

        subtest<size>("Case2", data, std::make_pair(2, 4));
    }
    {
        constexpr int_t size = 10U;

        std::array<int_t, size> data{// NOLINTNEXTLINE
            1, 2, 3, 3, 4, 5, 6, 7, 8, 8
        };

        subtest<size>("Case3", data,
            std::make_pair(
                // NOLINTNEXTLINE
                3, 8));
    }
}
