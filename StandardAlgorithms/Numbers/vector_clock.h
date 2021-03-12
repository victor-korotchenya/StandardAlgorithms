#pragma once
#include"../Utilities/require_utilities.h"
#include<functional>
#include<mutex>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // An instance is thread-safe.
    template<std::signed_integral int_t>
    struct vector_clock final
    {
        constexpr explicit vector_clock(const std::pair<int_t, int_t> &size_ide)
            : Times(require_positive(size_ide.first, "size"))
            , Ide(size_ide.second)
        {
            check_id("ide", Ide);
        }

        // Not copyable, not moveable because of the mutex.
        vector_clock(vector_clock &source) = delete;
        vector_clock(vector_clock &&source) = delete;
        auto operator= (vector_clock &source) & -> vector_clock & = delete;
        auto operator= (vector_clock &&source) & -> vector_clock & = delete;

        constexpr ~vector_clock() noexcept = default;

        [[nodiscard]] constexpr auto times() const &noexcept -> const std::vector<int_t> &
        {
            return Times;
        }

        [[nodiscard]] constexpr auto ide() const noexcept -> int_t
        {
            return Ide;
        }

        [[nodiscard]] constexpr auto send(const int_t &to_id) -> std::vector<int_t>
        {
            assert(Ide != to_id);

            check_id("to ide", to_id);

            std::vector<int_t> result;

            event_inner(
                [&result, this]() -> void
                {
                    result = Times;
                });

            return result;
        }

        constexpr void receive(const std::vector<int_t> &received_times, const int_t &from_id)
        {
            assert(Ide != from_id);

            check_id("from ide", from_id);

            event_inner(
                [&received_times, this]() -> void
                {
                    assert(received_times.size() == Times.size());

                    const auto size_1 = size();

                    for (int_t index{}; index < size_1; ++index)
                    {
                        const auto &got = received_times[index];
                        auto &tim = Times[index];
                        tim = std::max(tim, got);
                    }
                });
        }

        constexpr void event()
        {
            event_inner();
        }

private:
        [[nodiscard]] constexpr auto size() const noexcept -> int_t
        {
            return static_cast<int_t>(Times.size());
        }

        constexpr void check_id(const char *const name, const int_t &identif) const
        {
            assert(name != nullptr);

            constexpr int_t zero{};
            constexpr int_t one = 1;

            const auto max_value = size() - one;
            require_between(zero, identif, max_value, name);
        }

        constexpr void event_inner(const std::function<void()> &func = nullptr)
        {
            assert(Ide < size());

            const std::lock_guard lock(Mutex);

            ++Times[Ide];

            if (func != nullptr)
            {
                func();
            }
        }

        std::vector<int_t> Times;
        std::mutex Mutex{};
        const int_t Ide;
    };
} // namespace Standard::Algorithms::Numbers
