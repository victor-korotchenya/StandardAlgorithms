#include"concept_requires_tests.h"
#include<concepts>

namespace
{
    struct ruble final
    {
    };

    template<class money>
    [[nodiscard]] constexpr auto test_value()
    {
        return 1;
    }

    template<class money>
    requires(-1 == test_value<money>()) // It must match exactly.
    [[nodiscard]] constexpr auto where_is()
    {
        return 2;
    }

    template<class money>
    requires(requires(money hare) {
                 -2 == test_value<money>(); // Only the syntax must be valid - the expressions is not evaluated here.
             })
    [[nodiscard]] constexpr auto here_is()
    {
        return 3;
    }

    template<class money>
    requires(requires(money hare) {
                 requires - 3 == test_value<money>(); // It is evaluated to false.
             })
    [[nodiscard]] constexpr auto nowhere_tripple()
    {
        return 4;
    }

    template<class money>
    requires(requires(money hare) {
                 requires 1 == test_value<money>(); // But here evaluates to true.
             })
    [[nodiscard]] constexpr auto somewhere_tripple()
    {
        return -4;
    }
} // namespace

void Standard::Algorithms::Utilities::Tests::concept_requires_tests()
{
    // static_assert(where_is<ruble>(), "It fails to compile.");
    // static_assert(4 == nowhere_tripple<ruble>(), "And this also fails to compile.");
    static_assert(3 == here_is<ruble>(), "But this one compiles successfully.");
    static_assert(-4 == somewhere_tripple<ruble>(), "And this succeeds.");
}
