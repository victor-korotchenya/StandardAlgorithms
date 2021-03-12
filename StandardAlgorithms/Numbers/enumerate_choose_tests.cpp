#include"enumerate_choose_tests.h"
#include"../Utilities/test_utilities.h"
#include"enumerate_choose.h"

namespace
{
    using int_t = std::int16_t;
    using current_t = std::vector<int_t>;

    [[nodiscard]] constexpr auto enum_size_subsets(const int_t &mmm, const std::size_t expected_size, const int_t &nnn)
        -> std::vector<current_t>
    {
        Standard::Algorithms::require_positive(mmm, "mmm");
        Standard::Algorithms::require_positive(expected_size, "expected size");
        Standard::Algorithms::require_less_equal(mmm, nnn, "m vs. n");

        std::vector<current_t> actual{};

        Standard::Algorithms::Numbers::enumerate_choose<int_t> choo(mmm, nnn);

        do
        {
            Standard::Algorithms::require_greater(expected_size, actual.size(), "expected vs actual sizes.");

            actual.push_back(choo.current());
        } while (choo.next());

        return actual;
    }

    [[nodiscard]] constexpr auto enum_tests() -> bool
    {
        constexpr int_t mmm = 2;
        constexpr int_t nnn = 5;
        static_assert(int_t{} < mmm && mmm < nnn);

        const std::vector<current_t> expected{ { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 }, // NOLINTNEXTLINE
            { 1, 2 }, { 1, 3 }, { 1, 4 }, { 2, 3 }, { 2, 4 }, { 3, 4 } };

        const auto actual = enum_size_subsets(mmm, expected.size(), nnn);

        ::Standard::Algorithms::ert::are_equal(expected, actual, "enumerate_choose");

        return true;
    }

    [[nodiscard]] constexpr auto single_enum_test() -> bool
    {
        constexpr int_t mmm = 5;
        constexpr auto nnn = mmm;

        const std::vector<current_t> expected{ { 0, 1, 2, 3, 4 } };

        const auto actual = enum_size_subsets(mmm, expected.size(), nnn);

        ::Standard::Algorithms::ert::are_equal(expected, actual, "single enumerate_choose");

        return true;
    }

    [[nodiscard]] constexpr auto seq_enum_test() -> bool
    {
        constexpr int_t mmm = 1;
        constexpr auto nnn = 5;

        const std::vector<current_t> expected{ { 0 }, { 1 }, { 2 }, { 3 }, { 4 } };

        const auto actual = enum_size_subsets(mmm, expected.size(), nnn);

        ::Standard::Algorithms::ert::are_equal(expected, actual, "seq enumerate_choose");

        return true;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::enumerate_choose_tests()
{
    static_assert(enum_tests());
    static_assert(single_enum_test());
    static_assert(seq_enum_test());
}
