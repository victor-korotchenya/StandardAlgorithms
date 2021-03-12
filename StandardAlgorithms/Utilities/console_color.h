#pragma once

namespace Standard::Algorithms::Colors
{
    // todo(p3): remove copy-paste with special member functions whilst not using inheritance.
    struct error_color final
    {
        error_color();

        error_color(const error_color &) = delete;
        auto operator= (const error_color &) & -> error_color & = delete;
        error_color(error_color &&) noexcept = delete;
        auto operator= (error_color &&) &noexcept -> error_color & = delete;

        constexpr ~error_color() noexcept = default;
    };

    struct success_color final
    {
        success_color();

        success_color(const success_color &) = delete;
        auto operator= (const success_color &) & -> success_color & = delete;
        success_color(success_color &&) noexcept = delete;
        auto operator= (success_color &&) &noexcept -> success_color & = delete;

        constexpr ~success_color() noexcept = default;
    };

    struct warn_color final
    {
        warn_color();

        warn_color(const warn_color &) = delete;
        auto operator= (const warn_color &) & -> warn_color & = delete;
        warn_color(warn_color &&) noexcept = delete;
        auto operator= (warn_color &&) &noexcept -> warn_color & = delete;

        constexpr ~warn_color() noexcept = default;
    };
} // namespace Standard::Algorithms::Colors
