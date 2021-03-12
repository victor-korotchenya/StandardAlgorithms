#include"xor_queue_tests.h"
#include"../Utilities/ert.h"
#include"xor_queue.h"
#include<array>

namespace
{
    using int_t = std::int32_t;

    constexpr int_t initial_value = 1024;

    struct node_t final
    {
        int_t value{};
        node_t *next{};
    };

    auto output_value(std::ostream &str, const node_t &node) -> std::ostream &
    {
        str << "value " << node.value << ", has next " << (node.next != nullptr);

        return str;
    }

    constexpr void empty_queue_test(const std::string &name, Standard::Algorithms::Numbers::xor_queue<node_t> &que)
    {
        assert(!name.empty());

        {
            constexpr auto expected_size = 0U;

            ::Standard::Algorithms::ert::are_equal(expected_size, que.size(), "empty queue size. " + name);
        }
        {
            const auto *const node = que.dequeue();

            const auto value = node != nullptr ? node->value : int_t{};

            ::Standard::Algorithms::ert::is_null_ptr(
                node, "empty queue dequeue. " + name + ", value " + ::Standard::Algorithms::Utilities::zu_string(value));
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::xor_queue_tests()
{
    // todo(p5): random test.
    constexpr auto count = 5;

    std::array<node_t, count> items{};
    Standard::Algorithms::Numbers::xor_queue<node_t> que{};

    empty_queue_test("after queue creation", que);

    for (int_t key{}; key < count; ++key)
    {
        auto &item = items.at(key);

        ::Standard::Algorithms::ert::are_equal(
            nullptr, item.next, "fresh node Next at " + ::Standard::Algorithms::Utilities::zu_string(key));

        item.value = key + initial_value;
        que.enqueue(item);

        const auto expected_size = static_cast<std::size_t>(key + 1LL);
        ::Standard::Algorithms::ert::are_equal(expected_size, que.size(), "queue size after en-queuing");
    }

    for (int_t key{}; key < count; ++key)
    {
        const auto name = ::Standard::Algorithms::Utilities::zu_string(key);
        {
            const auto *const node = que.dequeue();

            ::Standard::Algorithms::ert::is_not_null_ptr(node, name);

            ::Standard::Algorithms::ert::are_equal(key + initial_value, node->value, "key after de-queuing");
        }
        {
            const auto expected_size = static_cast<std::size_t>(count - 1LL - key);

            ::Standard::Algorithms::ert::are_equal(expected_size, que.size(), "queue size after de-queuing");
        }
    }

    empty_queue_test("before queue ~", que);
}
