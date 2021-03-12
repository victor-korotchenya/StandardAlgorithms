#include"is_concept_tests.h"
#include"is_char_ptr.h"
#include"is_class_helper.h"
#include"is_integral_pure.h"
#include"test_utilities.h"
#include<exception>

namespace
{
    static_assert(std::is_integral_v<std::int32_t>);
    static_assert(!std::is_integral_v<std::int32_t &>);
    static_assert(Standard::Algorithms::is_integral_pure<std::int32_t &>);
    static_assert(Standard::Algorithms::is_integral_pure<std::int32_t>);

    struct safe_assign_oper final
    {
    };

    static_assert(Standard::Algorithms::is_assignment_operator_noexcept<safe_assign_oper>);

    static_assert(Standard::Algorithms::is_move_assignment_operator_noexcept<safe_assign_oper>);

    struct unsafe_assign_oper final
    {
        constexpr unsafe_assign_oper() noexcept = default;

        unsafe_assign_oper(const unsafe_assign_oper &) noexcept = delete;

        // It is a test for "noexcept(false)".
        // NOLINTNEXTLINE
        auto operator= (const unsafe_assign_oper &) &noexcept(false) -> unsafe_assign_oper &
        {
            throw std::exception{};
        }

        unsafe_assign_oper(unsafe_assign_oper &&) noexcept = delete;

        auto operator= (unsafe_assign_oper &&) &noexcept -> unsafe_assign_oper & = delete;

        constexpr ~unsafe_assign_oper() noexcept = default;
    };

    static_assert(!Standard::Algorithms::is_assignment_operator_noexcept<unsafe_assign_oper>);

    struct unsafe_move_assign_oper final
    {
        constexpr unsafe_move_assign_oper() noexcept = default;

        unsafe_move_assign_oper(const unsafe_move_assign_oper &) noexcept = delete;

        auto operator= (const unsafe_move_assign_oper &) &noexcept -> unsafe_move_assign_oper & = delete;

        unsafe_move_assign_oper(unsafe_move_assign_oper &&) noexcept = delete;

        // It is needed for a test to throw.
        // NOLINTNEXTLINE
        auto operator= ([[maybe_unused]] unsafe_move_assign_oper &&unused) &noexcept(false) -> unsafe_move_assign_oper &
        {
            throw std::exception{};
        }

        constexpr ~unsafe_move_assign_oper() noexcept = default;
    };

    static_assert(!Standard::Algorithms::is_move_assignment_operator_noexcept<unsafe_move_assign_oper>);

    template<class item_t>
    constexpr void integral_true()
    {
        constexpr auto is_integral_value = Standard::Algorithms::is_integral_pure<item_t>;

        static_assert(is_integral_value);
    }

    static_assert(Standard::Algorithms::is_char_ptr<const char *const>);
    static_assert(Standard::Algorithms::is_char_ptr<const char *>);
    static_assert(Standard::Algorithms::is_char_ptr<char *const>);
    static_assert(Standard::Algorithms::is_char_ptr<char *>);

    static_assert(!Standard::Algorithms::is_char_ptr<void *>);
    static_assert(!Standard::Algorithms::is_char_ptr<std::string *>);
    static_assert(!Standard::Algorithms::is_char_ptr<std::string>);
    static_assert(!Standard::Algorithms::is_char_ptr<std::int32_t &>);
    static_assert(!Standard::Algorithms::is_char_ptr<std::int32_t>);
} // namespace

void Standard::Algorithms::Utilities::Tests::is_concept_tests()
{
    integral_true<std::int32_t>();

    integral_true<const std::int32_t>();
    integral_true<volatile std::int32_t>();
    integral_true<std::int32_t &>();

    integral_true<const std::int32_t &>();
    integral_true<const volatile std::int32_t>();
    integral_true<volatile std::int32_t &>();

    integral_true<const volatile std::int32_t &>();

    // &&
    integral_true<std::int32_t &&>();

    integral_true<const std::int32_t &&>();
    integral_true<volatile std::int32_t &&>();

    integral_true<const volatile std::int32_t &&>();
}
