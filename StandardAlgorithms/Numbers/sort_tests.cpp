#include"sort_tests.h"
#include"../Graphs/binary_heap.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"merge_sort.h"
#include"quick_sort.h"
#include"sort.h"
#include<iostream>
#include<list>

namespace
{
    constexpr auto default_max_size = 17;
    constexpr auto shall_print_sort_report = false;

    template<std::integral int_t, class lama_t>
    constexpr void sortir_tiesto( // todo(p3): std::source_location
        const std::string &name, lama_t panama, const int_t &min_size = 0, const int_t &max_size = default_max_size)
    {
        Standard::Algorithms::throw_if_empty("name", name);

        if constexpr (std::is_signed_v<int_t>)
        {
            Standard::Algorithms::require_less_equal(int_t{}, min_size, "min size");
        }

        Standard::Algorithms::require_less_equal(min_size, max_size, "min size");
        Standard::Algorithms::require_positive(max_size, "max size");

        constexpr int_t min_value = 0;
        constexpr int_t max_value = 109;
        static_assert(min_value < max_value);

        Standard::Algorithms::Utilities::random_t<int_t> rnd(min_value, max_value);

        const auto source = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);

        auto good_cop = source;
        auto bad_cop = source;

        const Standard::Algorithms::elapsed_time_ns std_tim;
        std::sort(good_cop.begin(), good_cop.end());
        [[maybe_unused]] const auto std_elapsed = std_tim.elapsed();

        try
        {
            const Standard::Algorithms::elapsed_time_ns me_tim;
            panama(bad_cop);
            [[maybe_unused]] const auto me_elapsed = me_tim.elapsed();

            ::Standard::Algorithms::ert::are_equal(good_cop, bad_cop, "sorted data");

            if constexpr (shall_print_sort_report)
            {
                const auto ratio = ::Standard::Algorithms::ratio_compute(me_elapsed, std_elapsed);

                std::cout << "size " << source.size() << ", std " << std_elapsed << ", me " << me_elapsed << ", std/me "
                          << ratio << "\n";
            }
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << name << " has failed. ";
            ::Standard::Algorithms::print("source", source, str);
            str << "\nError: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }

    constexpr void test_insertion_sort()
    {
        using int_t = std::int64_t;

        const auto lam = [](auto &data)
        {
            // No constant
            // NOLINTNEXTLINE
            std::span spa = data;

            Standard::Algorithms::Numbers::insertion_sort(spa);
        };

        sortir_tiesto<int_t>("insertion_sort", lam);
    }

    constexpr void test_heap_sort()
    {
        using int_t = std::uint8_t;

        const auto lam = [](auto &data)
        {
            // To sort in the ascending order, max heap must be used.
            Standard::Algorithms::Heaps::binary_heap<int_t, std::vector<int_t>, std::greater_equal<>> heap;

            heap.heap_sort(data);
        };

        sortir_tiesto<int_t>("heap_sort", lam);
    }

    constexpr void test_merge_sort()
    {
        using int_t = std::int32_t;

        {
            const auto lam = [](auto &data)
            {
                Standard::Algorithms::Numbers::merge_sort(data);
            };

            sortir_tiesto<int_t>("merge_sort", lam);
        }
        {
            const auto lam = [](auto &data)
            {
                Standard::Algorithms::Numbers::merge_sort_recursive(data);
            };

            sortir_tiesto<int_t>("merge_sort_recursive", lam);
        }
        {
            const auto lam = [](auto &data)
            {
                Standard::Algorithms::Numbers::parallel_merge_sort(data);
            };

            sortir_tiesto<int_t>("parallel_merge_sort", lam);
        }
    }

    constexpr void test_quick_sort()
    {
        using int_t = std::int64_t;

        const auto lam = [](auto &data)
        {
            const std::span spa = data;
            Standard::Algorithms::Numbers::quick_sort(spa);
        };

        sortir_tiesto<int_t>("quick_sort", lam);
    }

    constexpr void test_selection_sort()
    {
        using int_t = std::uint16_t;

        const auto lam = [](auto &data)
        {
            // No constant
            // NOLINTNEXTLINE
            std::span spa = data;

            Standard::Algorithms::Numbers::selection_sort(spa, &Standard::Algorithms::Numbers::compare<int_t>, false);
        };

        sortir_tiesto<int_t>("selection_sort", lam);
    }

    constexpr void test_selection_sort_slow()
    {
        using int_t = std::int16_t;

        const auto lam = [](auto &data)
        {
            Standard::Algorithms::Numbers::selection_sort_slow(data.begin(), data.end());
        };

        sortir_tiesto<int_t>("selection_sort_slow", lam);
    }

    constexpr void test_selection_sort_slow_list()
    {
        using int_t = std::uint64_t;

        const auto lam = [](auto &data)
        {
            std::list<int_t> lisa(data.begin(), data.end());

            Standard::Algorithms::Numbers::selection_sort_slow(lisa.begin(), lisa.end());

            std::copy(lisa.cbegin(), lisa.cend(), data.begin());
        };

        sortir_tiesto<int_t>("selection_sort_slow list", lam);
    }

    constexpr void test_sort_n(const std::int32_t length, void (*function1)(std::span<std::int32_t>))
    {
        Standard::Algorithms::require_positive(length, "length");
        Standard::Algorithms::throw_if_null("function", function1);

        using int_t = std::int32_t;

        const auto lam = [&function1](auto &data)
        {
            // No constant
            // NOLINTNEXTLINE
            std::span spa = data;
            function1(spa);
        };

        sortir_tiesto<int_t>("TestSort_N " + ::Standard::Algorithms::Utilities::zu_string(length), lam, length, length);
    }

    constexpr void test_radix_sort_unsigned()
    {
        using int_t = std::uint32_t;

        const auto lam = [](auto &data)
        {
            // No constant
            // NOLINTNEXTLINE
            std::span spa = data;

            auto bugger = data;
            // No constant
            // NOLINTNEXTLINE
            std::span temp = bugger;

            Standard::Algorithms::Numbers::radix_sort_unsigned(spa, temp);
        };

        constexpr auto size = 1'000U;

        sortir_tiesto<int_t>("radix_sort_unsigned", lam, size, size);
    }

    template<class item, Standard::Algorithms::Numbers::digita digit_t, class get_size_t, class get_item_t,
        class collection_t>
    constexpr void radix_sort_helper(const std::string &name, const collection_t &data_original, get_size_t get_size,
        get_item_t get_item, const collection_t &expected)
    {
        ::Standard::Algorithms::ert::are_equal(data_original.size(), expected.size(), "size for " + name);

        Standard::Algorithms::require_sorted(expected, name);

        using int_t = std::uint32_t;

        auto actual = data_original;

        Standard::Algorithms::Numbers::radix_sort<collection_t, digit_t, int_t, get_size_t, get_item_t>(
            actual, get_size, get_item);

        ::Standard::Algorithms::ert::are_equal(expected, actual, "expected_" + name);
    }

    constexpr void test_radix_sort_string()
    {
        const auto *const func_name = static_cast<const char *>(__FUNCTION__);
        const auto name = std::string(func_name);

        using item_t = std::string;
        using digit_t = unsigned char;
        using collection_t = std::vector<item_t>;
        using int_t = std::uint32_t;

        const collection_t data_original{ "BBC", "X", "ok", "AAA", "", "AAA", "ABCD", "AA\xFF", "", "AAA" };

        const collection_t expected{ "", "", "AAA", "AAA", "AAA", "AA\xFF", "ABCD", "BBC", "X", "ok" };

        const auto get_size = [](const item_t &item) noexcept -> std::size_t
        {
            return item.size();
        };

        auto get_item = [](const item_t &item, const int_t index) -> digit_t
        {
            return item[index];
        };

        radix_sort_helper<item_t, digit_t, decltype(get_size), decltype(get_item)>(
            name, data_original, get_size, get_item, expected);
    }

    constexpr void test_radix_sort_int()
    {
        const auto *const func_name = static_cast<const char *>(__FUNCTION__);
        const auto name = std::string(func_name);

        using item_t = std::uint32_t;
        using digit_t = std::uint8_t;
        using collection_t = std::vector<item_t>;
        using int_t = std::uint32_t;

        const collection_t data_original{// NOLINTNEXTLINE
            3'457, 0, std::numeric_limits<item_t>::max(), std::numeric_limits<item_t>::min(),
            // NOLINTNEXTLINE
            798, 679, 57'809, 981'238'546,
            // NOLINTNEXTLINE
            0xFfFfFfFf, 6'780, 571'234'354
        };

        const collection_t expected{ 0, std::numeric_limits<item_t>::min(),
            // NOLINTNEXTLINE
            679, 798, 3'457, 6'780, 57'809,
            // NOLINTNEXTLINE
            571'234'354, 981'238'546, 0xFfFfFfFf, std::numeric_limits<item_t>::max() };

        const auto get_size = [](const item_t &) noexcept -> item_t
        {
            return sizeof(item_t);
        };

        auto get_item = [](const item_t &item, const int_t index) noexcept -> digit_t
        {
            constexpr auto mask = 0xFFU;

            const auto shift = (3U - index) << 3U;
            auto dig = static_cast<digit_t>(mask & (item >> shift));
            return dig;
        };

        radix_sort_helper<item_t, digit_t, decltype(get_size), decltype(get_item)>(
            name, data_original, get_size, get_item, expected);
    }

    void test_counting_sort()
    {
        using int_t = char;
        using item_t = std::pair<int_t, int_t>;

        const std::vector<item_t> arr{ { 3, 2 }, { 3, 1 }, { 3, 3 }, { 1, 0 }, { 2, 1 }, { 1, 1 }, { 2, 0 } };

        const std::vector<item_t> expected{ { 1, 0 }, { 1, 1 }, { 2, 1 }, { 2, 0 }, { 3, 2 }, { 3, 1 }, { 3, 3 } };

        {
            auto cop = arr;

            std::stable_sort(cop.begin(), cop.end(),
                [](const auto &one, const auto &two)
                {
                    return one.first < two.first;
                });

            ::Standard::Algorithms::ert::are_equal(expected, cop, "stable_sort");
        }
        {
            const auto to_index = [](const auto &item) -> std::uint32_t
            {
                return static_cast<std::uint32_t>(item->first);
            };

            using to_index_t = decltype(to_index);

            constexpr auto index_max = 256U;

            auto cop = arr;
            auto bugger = arr;

            Standard::Algorithms::Numbers::counting_sort<std::uint32_t, index_max, const to_index_t &>(
                cop.begin(), cop.end(), to_index, bugger.begin());

            ::Standard::Algorithms::ert::are_equal(expected, cop, "counting_sort");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::sort_tests()
{
    // todo(p3): split into several tests.
    test_radix_sort_unsigned();
    test_counting_sort();
    test_selection_sort();
    test_selection_sort_slow();
    test_selection_sort_slow_list();

    test_sort_n(3, &fast_sort3<std::int32_t>);
    test_sort_n(4, &fast_sort4<std::int32_t>);
    {
        constexpr auto siz = 5;
        test_sort_n(siz, &fast_sort5<std::int32_t>);
    }

    test_insertion_sort();
    test_heap_sort();
    test_merge_sort();
    test_quick_sort();
    test_radix_sort_string();
    test_radix_sort_int();
}
