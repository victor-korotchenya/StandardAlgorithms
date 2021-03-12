#pragma once

#if defined(__clang__)

#include<algorithm>
#include<thread>
#include<type_traits>
#include<utility>

// NOLINTNEXTLINE
namespace std
{
#if !defined(jthread)
    // NOLINTNEXTLINE
    struct jthread final // todo(p2): remove when Clang has C++20 std::jthread class.
    {
        using id = thread::id;

        jthread() noexcept = default;

        template<class callable_t, class... args_t,
            class = enable_if_t<!is_same_v<remove_cvref_t<callable_t>, jthread>>>
        explicit jthread(callable_t &&callable, args_t &&...args)
            : Thread(::std::forward<callable_t>(callable), ::std::forward<args_t>(args)...)
        {
        }

        jthread(const jthread &) = delete;
        auto operator= (const jthread &) & -> jthread & = delete;

        jthread(jthread &&) noexcept = default;

        auto operator= (jthread &&other) &noexcept -> jthread &
        {
            ::std::jthread(::std::move(other)).swap(*this);
            return *this;
        }

        ~jthread()
        {
            if (joinable())
            {
                join();
            }
        }

        void swap(jthread &other) noexcept
        {
            ::std::swap(Thread, other.Thread);
        }

        [[nodiscard]] auto joinable() const noexcept -> bool
        {
            return Thread.joinable();
        }

        void join()
        {
            Thread.join();
        }

        void detach()
        {
            Thread.detach();
        }

        [[nodiscard]] auto get_id() const noexcept -> id
        {
            return Thread.get_id();
        }

        // NOLINTNEXTLINE
        friend void swap(jthread &lhs, jthread &rhs) noexcept
        {
            lhs.swap(rhs);
        }

private:
        thread Thread{};
    };
#endif
} // namespace std
#endif
