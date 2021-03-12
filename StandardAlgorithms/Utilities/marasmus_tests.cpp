#include"marasmus_tests.h"
#include<cstdint>

namespace
{
    using int_t = std::int32_t;

    enum class [[nodiscard]] return_result
    {
        non_template,

        generic_template,
        int_specialization,
        ptr_specialization,

        more_specific_ptr_argument,
    };

    constexpr auto marasmus([[maybe_unused]] int_t value = {}) noexcept -> return_result
    {
        return return_result::non_template;
    }

    template<class giant>
    constexpr auto marasmus([[maybe_unused]] giant value = {}) noexcept -> return_result
    {
        return return_result::generic_template;
    }

    template<>
    constexpr auto marasmus([[maybe_unused]] int_t value) noexcept -> return_result
    {
        return return_result::int_specialization;
    }

    template<>
    [[maybe_unused]] constexpr auto marasmus([[maybe_unused]] const int_t *const value) noexcept -> return_result
    {
        return return_result::ptr_specialization;
    }

    template<class giant>
    constexpr auto marasmus([[maybe_unused]] const giant *const value = {}) noexcept -> return_result
    {
        return return_result::more_specific_ptr_argument;
    }

    consteval auto obvious_test() noexcept -> bool
    {
        constexpr int_t zero{};

        constexpr auto non_templ = marasmus(zero);
        constexpr auto speci = marasmus<int_t>(zero);

        auto res = return_result::non_template == non_templ && return_result::int_specialization == speci;
        return res;
    }

    constexpr auto marasmus_tests_impl() noexcept -> bool
    {
        const int_t zero{};
        const auto *const ptr = &zero;
        const auto actual = marasmus(ptr);

        // ptr_specialization fits better,
        // but a more specific argument function wins over a generic template
        // because specializations are considered second.
        auto res = return_result::more_specific_ptr_argument == actual;
        return res;
    }
} // namespace

void Standard::Algorithms::Utilities::Tests::marasmus_tests()
{
    static_assert(obvious_test());
    static_assert(marasmus_tests_impl());
}
