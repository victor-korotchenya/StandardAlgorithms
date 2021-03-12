#include"adl_tests.h"
#include<cstdint>

namespace
{
    using int_t = std::int32_t;

    constexpr int_t num_one = 1;
    constexpr int_t num_three = 3;
    constexpr int_t num_twenty = 20;
} // namespace

namespace Mumbo
{
    struct strong final
    {
        inline explicit constexpr strong(int_t arg = {}) noexcept
            : Argument(arg)
        {
        }

        [[nodiscard]] inline constexpr auto argument() const &noexcept -> const int_t &
        {
            return Argument;
        };

        [[nodiscard]] inline constexpr auto argument() &noexcept -> int_t &
        {
            return Argument;
        };

        inline constexpr void ex_change(strong &other) noexcept
        {
            auto old = Argument;
            Argument = other.Argument;
            other.Argument = old;
        }

        inline constexpr void friend roosevelt_change_non_member(strong &left, strong &right) noexcept
        {// It is a non-member function.
            left.ex_change(right);
        }

private:
        int_t Argument;
    };

    inline constexpr void jumbo(strong &bystro, const int_t increment) noexcept
    {
        bystro.argument() += increment;
    }
} // namespace Mumbo

namespace ArgumentDependentLookupTest
{
    [[nodiscard]] consteval inline auto test() noexcept -> int_t
    {
        ::Mumbo::strong instance{};
        ::Mumbo::jumbo(instance, num_one);

        // Here is the actual test - no namespace specified, yet it functions.
        jumbo(instance, num_twenty);

        return instance.argument();
    }
} // namespace ArgumentDependentLookupTest

namespace FriendTest
{
    [[nodiscard]] consteval inline auto test() noexcept -> int_t
    {
        ::Mumbo::strong one{ num_one };
        ::Mumbo::strong two{ num_three };
        roosevelt_change_non_member(one, two);

        auto res = one.argument() * num_twenty + two.argument();
        return res;
    }
} // namespace FriendTest

void Standard::Algorithms::Utilities::Tests::adl_tests()
{
    static_assert(ArgumentDependentLookupTest::test() == num_twenty + num_one);

    static_assert(FriendTest::test() == num_three * num_twenty + num_one);
}
