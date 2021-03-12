#include"binary_heap_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"binary_heap.h"
#include"k_smallest_element.h"
#include"priority_queue.h"

namespace
{
    using int_t = std::int32_t;

    constexpr void check_for_equality(const std::vector<int_t> &items_sorted, const std::size_t size,
        Standard::Algorithms::Heaps::binary_heap<int_t> &heap, const std::string &extra_error)
    {
        for (std::size_t index{}; index < size; ++index)
        {
            const auto expected = items_sorted[index];
            const auto actual = heap.top();
            heap.pop();

            if (expected == actual)
            {
                continue;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Items mismatch at index " << index << ": expect '" << expected << "', but actual '" << actual
                << "'.";
            if (!extra_error.empty())
            {
                str << " " << extra_error;
            }

            Standard::Algorithms::throw_exception(str);
        }

        const auto count = heap.size();
        ::Standard::Algorithms::ert::are_equal(0U, count, "The size after removing all items.");
    }

    void clear_and_build_test()
    {
        using data_type = std::uint16_t;

        const auto some_item = Standard::Algorithms::Utilities::random_t<data_type>()();

        const auto get_index = [] [[nodiscard]] (const auto &item) noexcept
        {
            return static_cast<std::size_t>(item);
        };

        using clear_and_build_test_get_index_function = std::size_t (*)(const data_type &vertex) noexcept;

        constexpr auto size = 15U;

        // All items must be unique.
        // No const. // NOLINTNEXTLINE
        Standard::Algorithms::Heaps::priority_queue<data_type, clear_and_build_test_get_index_function> heap(
            get_index, size);

        heap.push(some_item);
        heap.push(some_item + 1);
        heap.push(some_item + 2);
        heap.push(some_item + 3);

        std::vector<data_type> items{};
        Standard::Algorithms::Utilities::fill_random(items, size, data_type{}, true);

        auto items_copy = items;

        const auto *const data = items.data();
        const auto spa = std::span(data, size);
        heap.clear_and_build(spa);
        heap.validate("clear_and_build test");

        std::sort(items_copy.begin(), items_copy.end());

        Standard::Algorithms::Heaps::binary_heap<const data_type *, std::vector<const data_type *>,
            Standard::Algorithms::Numbers::less_equal_by_ptr<data_type>>
            heap_temp{};

        constexpr auto k_th_zero_based_index = size >> 1U;

        const auto &kth_item =
            Standard::Algorithms::Heaps::k_smallest_element<data_type>(heap, k_th_zero_based_index, heap_temp);

        ::Standard::Algorithms::ert::are_equal(items_copy[k_th_zero_based_index], kth_item, "k-th item");
    }
} // namespace

void Standard::Algorithms::Heaps::Tests::binary_heap_tests()
{
    clear_and_build_test();

    // NOLINTNEXTLINE
    std::size_t size = ::Standard::Algorithms::is_debug ? 15 : 64;

    constexpr auto max_attempts = 2;

    for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
    {
        std::vector<int_t> items;
        Standard::Algorithms::Utilities::fill_random(items, size);

        auto items_sorted = items;
        std::sort(items_sorted.begin(), items_sorted.end());

        {
            binary_heap<int_t> heap(size);
            heap.push(items[0]);

            ::Standard::Algorithms::ert::are_equal(items[0], heap.top(), "Top after 1st adding.");

            // The first item is already added.
            for (auto index = 1U; index < size; ++index)
            {
                const auto item = items[index];
                heap.push(item);
            }

            const auto count0 = heap.size();
            ::Standard::Algorithms::ert::are_equal(size, count0, "The size after adding all items.");

            check_for_equality(items_sorted, size, heap, std::string("The heap has just been built."));
        }
        {
            binary_heap<int_t> heap(size);
            heap.push(items[0]);

            ::Standard::Algorithms::ert::are_equal(items[0], heap.top(), "Top after 1st adding.");

            // The first item is already added.
            for (auto index = 1U; index < size; ++index)
            {
                const auto item = items[index];
                heap.push(item);
            }

            const auto count0 = heap.size();
            ::Standard::Algorithms::ert::are_equal(size, count0, "The size after adding all items.");

            check_for_equality(items_sorted, size, heap, std::string("The heap has just been built."));
        }

        size <<= 1U;
    }

    // constexpr int_t repetitions_size = 2; // NOLINTNEXTLINE
    // const array<int_t, repetitions_size> repetitions {3, 6};
    // for (const auto &titi :  repetitions)
    //{
    //     heap.add(items[titi]);
    // }
    //
    // const auto count1 = heap.size();
    // ::Standard::Algorithms::ert::are_equal(size + repetitions_size, count1, "The size after adding repetitions.");
    // todo(p3): finish.
}
