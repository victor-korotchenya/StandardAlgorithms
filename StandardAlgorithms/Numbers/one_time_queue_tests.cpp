#include"one_time_queue_tests.h"
#include"../Utilities/ert.h"
#include"one_time_queue.h"

namespace
{
    using item_type = std::int32_t;
    using size_type = std::uint64_t;
    using queue_t = Standard::Algorithms::Numbers::one_time_queue<item_type, size_type>;

    constexpr item_type init_value = 17;

    constexpr size_type capacity = 5;
    constexpr size_type half_cap = capacity / 2;

    constexpr void fill_empty_que(queue_t &que, const std::string &name, const size_type size1)
    {
        Standard::Algorithms::require_positive(size1, name + " fill que size");

        ::Standard::Algorithms::ert::are_equal(size_type{}, que.size(), name + " initial size");

        for (size_type index{}; index < size1; ++index)
        {
            const auto value = static_cast<item_type>(init_value + index);
            que.push(value);

            ::Standard::Algorithms::ert::are_equal(init_value, que.top(), name + " top");

            ::Standard::Algorithms::ert::are_equal(index + 1, que.size(), name + " size");
        }
    }

    // todo(p3):  constexpr auto simple_que_test() -> bool
    constexpr void simple_que_test()
    {
        queue_t que(capacity);
        fill_empty_que(que, "before reset", half_cap);

        que.reset();
        fill_empty_que(que, "after reset", capacity);

        {
            auto lambda = [&que]() -> void
            {
                que.push(item_type{});
            };

            ::Standard::Algorithms::ert::expect_exception<std::runtime_error>(
                "One time queue is full, capacity " + ::Standard::Algorithms::Utilities::zu_string(capacity) + ".",
                lambda, "Enqueueing into a full queue.");
        }

        for (size_type index{}; index < capacity; ++index)
        {
            const auto value = static_cast<item_type>(init_value + index);
            {
                const auto top = que.top();
                ::Standard::Algorithms::ert::are_equal(value, top, "top before pop");
                ::Standard::Algorithms::ert::are_equal(capacity - index, que.size(), "size before pop");
            }
            {
                const auto popped = que.pop();
                ::Standard::Algorithms::ert::are_equal(value, popped, "value after pop");
                ::Standard::Algorithms::ert::are_equal(capacity - index - 1, que.size(), "size after pop");
            }
        }

        {
            auto lambda = [&que]() -> void
            {
                que.pop();
            };

            ::Standard::Algorithms::ert::expect_exception<std::runtime_error>(
                "Cannot pop from an empty One time queue.", lambda, "Dequeuing from an empty queue.");
        }
        // todo(p3): return true;
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::one_time_queue_tests()
{
    // todo(p3):  static_assert(simple_que_test());
    simple_que_test();
}
