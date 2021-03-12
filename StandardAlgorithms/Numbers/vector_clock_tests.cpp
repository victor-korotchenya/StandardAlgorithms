#include"vector_clock_tests.h"
#include"../Utilities/check_size.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"vector_clock.h"
#include<memory>
#include<thread>

#if defined(__clang__)
// todo(p2): remove when Clang has C++20 std::jthread class.
#include"../Utilities/jthread_not.h"
#endif

namespace
{
    using int_t = std::int32_t;

    using clock_ty = Standard::Algorithms::Numbers::vector_clock<int_t>;

    struct vector_clock_test_context
    {
        std::vector<std::unique_ptr<clock_ty>> &clocks;
        std::mutex mutex{};
        std::string error_message{};
        bool has_error{};
    };

    constexpr void check_clock(const clock_ty &clock, const int_t &size)
    {
        Standard::Algorithms::require_positive(size, "size");

        const auto &times = clock.times();

        ::Standard::Algorithms::ert::are_equal(static_cast<std::size_t>(size), times.size(), "times size");

        const auto ide = clock.ide();
        const auto prefix = "times[" + ::Standard::Algorithms::Utilities::zu_string(ide) + "][";
        const auto max_time = (size - 1) * 2LL;

        for (int_t index{}; index < size; ++index)
        {
            const auto name = prefix + ::Standard::Algorithms::Utilities::zu_string(index) + "]";
            const auto &time = times.at(index);

            if (index == ide)
            {
                ::Standard::Algorithms::ert::are_equal(max_time, time, name);
            }
            else
            {
                Standard::Algorithms::require_positive(time, name);

                ::Standard::Algorithms::ert::greater_or_equal(max_time, time, name);
            }
        }
    }

    constexpr void check_clocks(const std::vector<std::unique_ptr<clock_ty>> &clocks)
    {
        const auto *const name_siz = "clocks size";

        const auto size = Standard::Algorithms::Utilities::check_size<int_t>(name_siz, clocks.size());

        Standard::Algorithms::require_positive(size, name_siz);

        for (int_t index{}; index < size; ++index)
        {
            const auto name = "Clock at " + ::Standard::Algorithms::Utilities::zu_string(index);
            const auto *const clock = clocks[index].get();

            Standard::Algorithms::throw_if_null(name, clock);

            ::Standard::Algorithms::ert::are_equal(index, clock->ide(), name);

            check_clock(*clock, size);
        }
    }

    void launch_clock_thread(
        std::vector<std::jthread> &threads, vector_clock_test_context &context, const int_t &clo_index)
    {
        ::Standard::Algorithms::ert::are_equal(context.clocks.size(), threads.size(), "threads size");

        auto func = [&context, clo_index]() -> void
        {
            try
            {
                auto ids = ::Standard::Algorithms::Utilities::iota_vector<int_t>(context.clocks.size());
                Standard::Algorithms::Utilities::shuffle_data(ids);

                auto *const clock_source = context.clocks.at(clo_index).get();
                Standard::Algorithms::throw_if_null("clock source", clock_source);

                for (const auto &ide : ids)
                {
                    if (clo_index == ide)
                    {
                        continue;
                    }

                    const auto sent_times = clock_source->send(ide);
                    auto *const clock_destination = context.clocks.at(ide).get();
                    Standard::Algorithms::throw_if_null("clock destination", clock_destination);

                    clock_destination->receive(sent_times, clo_index);
                }
            }
            catch (const std::exception &exc)
            {
                context.has_error = true;

                try
                {
                    const std::lock_guard lock(context.mutex);

                    context.error_message = "Error in launch_clock_thread(" +
                        ::Standard::Algorithms::Utilities::zu_string(clo_index) + "): " + exc.what();
                }
                catch (...)
                {// Nothing can be done.
                }
            }
            catch (...)
            {
                context.has_error = true;
            }
        };

        threads.at(clo_index) = std::jthread(func);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::vector_clock_tests()
{
    constexpr int_t min_size = 2;
    constexpr int_t max_size = 16;

    Standard::Algorithms::Utilities::random_t<int_t> rnd(min_size, max_size);
    const auto size = rnd();

    std::vector<std::unique_ptr<clock_ty>> clocks(size);

    for (int_t index{}; index < size; ++index)
    {
        clocks[index] = std::make_unique<clock_ty>(std::make_pair(size, index));
    }

    vector_clock_test_context context{ clocks };
    {
        std::vector<std::jthread> threads(size);

        for (int_t index{}; index < size; ++index)
        {
            launch_clock_thread(threads, context, index);
        }
    }

    check_clocks(clocks);

    ::Standard::Algorithms::ert::are_equal(std::string{}, context.error_message, "error message");

    ::Standard::Algorithms::ert::are_equal(false, context.has_error, "has error");
}
