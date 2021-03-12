#include"../Utilities/ert.h"
#include"key_only_compare.h"

namespace
{
    static_assert(std::is_same<Standard::Algorithms::Graphs::key_only_compare<std::string, std::int32_t>,
        decltype([]{
            // No const.
            // NOLINTNEXTLINE
            Standard::Algorithms::Graphs::key_only_compare koc { std::string{}, std::int32_t{} };

            {
                constexpr auto make_not_const = 1;
                koc.value += make_not_const;
            }
            return koc;
            }())>(),
        "Deduction guide test.");

    [[nodiscard]] constexpr auto key_only_compare_tests() -> bool
    {
        using key_t = std::int32_t;
        using value_t = std::string;
        using pair_t = Standard::Algorithms::Graphs::key_only_compare<key_t, value_t>;

        ::Standard::Algorithms::ert::are_equal(pair_t{}, pair_t{}, "same default ctor");

        constexpr key_t key_1 = 119;
        constexpr key_t key_2 = key_1 + 1;
        static_assert(key_1 != key_2 && key_1 < key_2);

        ::Standard::Algorithms::ert::not_equal(key_1, key_2, "not same keys");
        ::Standard::Algorithms::ert::greater(key_2, key_1, "key 2 greater");

        const value_t value_1 = "azre";
        const value_t value_2 = "bazr";

        ::Standard::Algorithms::ert::not_equal(value_1, value_2, "not same values");
        ::Standard::Algorithms::ert::greater(value_2, value_1, "value 2 greater again");

        ::Standard::Algorithms::ert::are_equal(pair_t{ key_1, value_1 }, pair_t{ key_1, value_1 }, "same all");

        ::Standard::Algorithms::ert::are_equal(pair_t{ key_2, value_2 }, pair_t{ key_2, value_2 }, "same all 2");

        ::Standard::Algorithms::ert::are_equal(pair_t{ key_1, value_1 }, pair_t{ key_1, value_2 }, "same keys");

        ::Standard::Algorithms::ert::are_equal(pair_t{ key_1, value_2 }, pair_t{ key_1, value_1 }, "same keys again");

        ::Standard::Algorithms::ert::not_equal(pair_t{ key_1, value_1 }, pair_t{ key_2, value_1 }, "not same keys");

        ::Standard::Algorithms::ert::not_equal(pair_t{ key_2, value_1 }, pair_t{ key_1, value_1 }, "not same keys 2");

        ::Standard::Algorithms::ert::not_equal(pair_t{ key_1, value_1 }, pair_t{ key_2, value_2 }, "not same all");

        ::Standard::Algorithms::ert::not_equal(pair_t{ key_2, value_2 }, pair_t{ key_1, value_1 }, "not same all 2");

        ::Standard::Algorithms::ert::greater(pair_t{ key_2, value_2 }, pair_t{ key_1, value_1 }, "Greater all");

        ::Standard::Algorithms::ert::greater(
            pair_t{ key_2, value_1 }, pair_t{ key_1, value_1 }, "Greater key 2 same values");

        ::Standard::Algorithms::ert::greater(
            pair_t{ key_2, value_2 }, pair_t{ key_1, value_2 }, "Greater key 2 same values 2");

        ::Standard::Algorithms::ert::greater(pair_t{ key_2, value_1 }, pair_t{ key_1, value_2 }, "Greater key 2 value 1");

        return true;
    }

    static_assert(key_only_compare_tests());
} // namespace
