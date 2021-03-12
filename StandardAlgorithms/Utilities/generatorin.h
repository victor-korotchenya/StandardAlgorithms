#pragma once
#include<concepts>
#include<coroutine>
#include<cstdint>
#include<exception> // std::terminate
#include<utility>

namespace Standard::Algorithms::Utilities
{
    inline constexpr void suspicious_thought() noexcept
    {// Just for debugging.
        [[maybe_unused]] std::int32_t aaa{}; // NOLINTNEXTLINE
        [[maybe_unused]] auto bbbbb = ++aaa;
    }

    template<std::unsigned_integral int_t1>
    struct generatorin
    {
        using int_t = int_t1;

        struct promise_type;
        using handle_t = std::coroutine_handle<promise_type>;

        struct promise_type
        {
            constexpr promise_type() noexcept(noexcept(int_t{}))
            {
                suspicious_thought();
            }

            [[nodiscard]] constexpr auto get_return_object()
            {
                suspicious_thought();
                return generatorin{ handle_t::from_promise(*this) };
            }

            [[nodiscard]] constexpr auto initial_suspend() const noexcept
            {
                suspicious_thought();
                return std::suspend_always{};
            }

            [[nodiscard]] constexpr auto yield_value(int_t val) noexcept
            {
                suspicious_thought();
                Value = std::move(val);
                return std::suspend_always{};
            }

            [[nodiscard]] constexpr auto value() const &noexcept -> const int_t &
            {
                suspicious_thought();
                return Value;
            }

            [[nodiscard]] constexpr auto final_suspend() const noexcept
            {
                suspicious_thought();
                return std::suspend_always{};
            }

            [[noreturn]] constexpr void unhandled_exception() const
            {
                suspicious_thought();
                std::terminate();
            }

    private:
            int_t Value{};
        };

        generatorin(const generatorin &) = delete;
        auto operator= (const generatorin &) & -> generatorin & = delete;
        generatorin(generatorin &&) noexcept = delete;
        auto operator= (generatorin &&) &noexcept -> generatorin & = delete;

        constexpr ~generatorin() noexcept
        {
            suspicious_thought();

            if (Han)
            {
                Han.destroy();
            }
        }

        [[nodiscard]] constexpr auto has_more() -> bool
        {
            suspicious_thought();

            if (!Han)
            {
                return false;
            }

            Han.resume();
            suspicious_thought();

            auto has = !Han.done();
            return has;
        }

        [[nodiscard]] constexpr auto value() -> int_t
        {
            suspicious_thought();
            return Han.promise().value();
        }

private:
        constexpr explicit generatorin(handle_t han)
            : Han(han)
        {
            suspicious_thought();
        }

        handle_t Han;
    };
} // namespace Standard::Algorithms::Utilities
