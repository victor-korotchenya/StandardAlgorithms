#include"is_noexcept.h"
#include<cstdint>
#include<string>

namespace
{
    template<class item_t>
    [[nodiscard]] consteval auto che() noexcept -> bool
    {
        return Standard::Algorithms::is_noexcept_add<item_t>;
    }

    static_assert(che<std::int8_t>());
    static_assert(che<std::int16_t>());
    static_assert(che<std::int32_t>());
    static_assert(che<std::uint32_t>());
    static_assert(che<std::uint64_t>());
    static_assert(che<std::size_t>());

    static_assert(!che<std::string>(), "Throwing");

    struct safe_add final
    {
        std::uint32_t all{};
    };

    struct throwing_add final
    {
        std::int32_t all{};
    };

#ifdef __clang__
#pragma clang diagnostic push // It is needed to compile.
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#endif

    [[nodiscard]] constexpr auto operator+ (const safe_add &once, const safe_add &and_for) noexcept -> safe_add
    {
        return { once.all + and_for.all };
    };

    [[nodiscard]] consteval auto operator+ (const throwing_add &once, const throwing_add &and_for) -> throwing_add
    {// It won't throw, but the omitted noexcept declaration says it might in theory.
        return { once.all + and_for.all };
    };

#ifdef __clang__
#pragma clang diagnostic pop
#endif

    static_assert(che<safe_add>());
    static_assert(!che<throwing_add>(), "Unsafe");
} // namespace
