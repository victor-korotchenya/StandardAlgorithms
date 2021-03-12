#pragma once
#include <algorithm>
#include <cassert>
#include <mutex>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Numbers
{
    // An instance is thread-safe.
    template<class value_t = int>
    class vector_clock sealed
    {
        std::vector<value_t> times;
        std::mutex mutex;
        const int id;

        int size() const noexcept
        {
            return static_cast<int>(times.size());
        }

        void check_id(const char* const name, int src_id) const
        {
            RequireNonNegative(src_id, name);

            const auto s = size();
            RequireNotGreater(src_id, s - 1, name);
        }

        void event_inner(std::function<void()> func = nullptr)
        {
            std::lock_guard<decltype(mutex)> lock(mutex);

            ++times[id];

            if (!func)
                return;

            func();
        }

    public:
        explicit vector_clock(int size, int id) : times(RequirePositive(size, "size")),
            mutex(), id(id)
        {
            check_id("id", id);
        }

        // Not copyable, not moveable because of the mutex.
        vector_clock(vector_clock& source) = delete;
        vector_clock(vector_clock&& source) = delete;
        vector_clock& operator =(vector_clock& source) = delete;

        // todo: fix compile error: friend void check_clock(const vector_clock *clock);
        const std::vector<value_t>& get_times() const
        {
            return times;
        }

        int get_id() const
        {
            return id;
        }

        std::vector<value_t> send(int to_id)
        {
            check_id("to_id", to_id);
            assert(id != to_id);

            std::vector<value_t> result;
            event_inner([&]() -> void
                {
                    result = times;
                });

            return result;
        }

        void receive(const std::vector<value_t>& received_times, int from_id)
        {
            check_id("from_id", from_id);
            assert(id != from_id);

            event_inner([&]() -> void
                {
                    assert(received_times.size() == times.size());

                    const auto s = size();
                    for (auto i = 0; i < s; ++i)
                        times[i] = std::max(times[i], received_times[i]);
                });
        }

        void event()
        {
            event_inner();
        }
    };
}