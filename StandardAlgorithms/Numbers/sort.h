#pragma once
// "sort.h"
#include"../Utilities/is_class_helper.h"
#include"../Utilities/require_utilities.h"
#include"digita.h"
#include<array>
#include<cstring>
#include<functional>
#include<span>
#include<typeinfo> // std::type_info
#include<vector>

namespace Standard::Algorithms::Numbers::Inner
{
    template<std::unsigned_integral int_t, class collection_t>
    requires(std::is_same_v<int_t, std::remove_cvref_t<decltype(std::declval<collection_t>()[0])>>)
    constexpr void propagate_counts(const int_t &max_value_inclusive, collection_t &counts)
    {
        {
            constexpr auto max_size = std::numeric_limits<int_t>::max();

            require_greater(max_size, max_value_inclusive, "max value inclusive");
        }

        assert(int_t{} < max_value_inclusive);

        int_t index{};
        int_t previous{}; // todo(p3): del previous.

        do
        {
            auto &cnt = counts[index];
            const auto next = static_cast<int_t>(previous + cnt);
            cnt = previous;
            previous = next;
        } while (++index <= max_value_inclusive);
    }

    template<std::unsigned_integral int_t, class collection_t, class get_size_t>
    [[nodiscard]] constexpr auto empty_count_max_length(const collection_t &collection, get_size_t get_size)
        -> std::pair<int_t, int_t>
    {
        int_t empty_count{};
        int_t element_max_length{};

        for (const auto &item : collection)
        {
            const auto raw = get_size(item);
            const auto item_size = static_cast<int_t>(raw);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (item_size != raw)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The type '" << typeid(int_t).name() << "' is too small to store " << raw << ".";

                    throw_exception(str);
                }
            }

            if (0U == item_size)
            {
                ++empty_count;
            }
            else if (element_max_length < item_size)
            {
                element_max_length = item_size;
            }
        }

        return { empty_count, element_max_length };
    }

    template<class collection_t, std::integral int_t, class get_size_t>
    constexpr void move_empty_to_start(
        collection_t &collection, get_size_t get_size, int_t empty_count, collection_t &temp)
    {
        require_positive(empty_count, "empty count");

        const auto size = static_cast<int_t>(collection.size());
        require_greater(size, empty_count, "collection size vs. empty count");

        temp.resize(size);
        --empty_count;

        int_t index{};
        auto empty = int_t{} - static_cast<int_t>(1);

        do
        {
            const auto &item = collection[index];
            const auto pos = 0 == get_size(item) ? ++empty : ++empty_count;
            temp[pos] = item;
        } while (++index < size);

        std::copy(temp.cbegin(), temp.cend(), collection.begin());
    }

    template<class collection_t, digita digit_t, std::unsigned_integral int_t, class get_size_t, class get_item_t>
    constexpr void count_at_digit(const int_t digit, get_size_t get_size, get_item_t get_item, const int_t max_value,
        const collection_t &collection, std::vector<int_t> &counts, int_t index)
    {
        constexpr int_t one = 1;
        {
            constexpr auto limerick_poetry = std::numeric_limits<int_t>::max();
            assert(max_value < limerick_poetry);
        }

        counts.assign(max_value + one, int_t{});
        const auto size = static_cast<int_t>(collection.size());

        do
        {
            const auto &item = collection[index];
            const auto raw = get_size(item);
            const auto pos = std::min(digit, static_cast<int_t>(raw - one));

            if constexpr (::Standard::Algorithms::is_debug)
            {
                require_positive(raw, "collection[" + std::to_string(index) + "] size");
                require_less_equal(pos, max_value, "pos at digit " + std::to_string(digit));
            }

            const auto symbol = static_cast<std::size_t>(get_item(item, pos));
            ++counts.at(symbol);
        } while (++index < size);
    }

    template<class collection_t, digita digit_t, std::unsigned_integral int_t, class get_size_t, class get_item_t>
    constexpr void copy_to_buffer(get_size_t get_size, get_item_t get_item, const collection_t &collection,
        const int_t digit, collection_t &temp, std::vector<int_t> &counts, int_t index)
    {
        const auto size = static_cast<int_t>(collection.size());

        do
        {
            constexpr int_t one = 1;

            const auto &item = collection[index];
            const auto raw_size = get_size(item);
            const auto pos = std::min(digit, static_cast<int_t>(raw_size - one));
            const auto symbol = static_cast<std::size_t>(get_item(item, pos));
            auto &cnt = counts.at(symbol);
            temp[cnt] = item;
            ++cnt;
        } while (++index < size);
    }

    template<class collection_t, digita digit_t, std::unsigned_integral int_t, class get_size_t, class get_item_t>
    constexpr void sort_by_digit(const int_t digit, get_size_t get_size, const int_t max_value, get_item_t get_item,
        const int_t empty_count, collection_t &collection, std::vector<int_t> &counts, collection_t &temp)
    {
        assert(int_t{} < max_value);

        count_at_digit<collection_t, digit_t, int_t, get_size_t, get_item_t>(
            digit, get_size, get_item, max_value, collection, counts, empty_count);

        propagate_counts<int_t>(max_value, counts);

        copy_to_buffer<collection_t, digit_t, int_t, get_size_t, get_item_t>(
            get_size, get_item, collection, digit, temp, counts, empty_count);

        std::copy(temp.cbegin(), temp.cend(), collection.begin() + empty_count);
    }
} // namespace Standard::Algorithms::Numbers::Inner

namespace Standard::Algorithms::Numbers
{
    // Partially sorts the item at the index in an array having at least 2 items.
    // The array items before the index, if any, must be sorted.
    // E.g. given {20, 30, 40, 25, ..} and index = 3,
    // 25 will be moved to the left step-by-step
    // where the question mark '?' indicates "recently moved from":
    // {20, 30, 40, 25, ..} // The initial array.
    // {20, 30, 40, ?, ..}
    // {20, 30, ?, 40, ..}
    // {20, ?, 30, 40, ..} // Stop moving.
    // {20, 25, 30, 40, ..} // The resulting, partially sorted array of the 3+1 = 4 first items.
    template<class item_t, class less_t = std::less<item_t>, std::size_t extent = std::dynamic_extent>
    constexpr void insertion_sort_step(std::span<item_t, extent> spa, std::size_t index, less_t less1 = {}) noexcept(
        std::is_move_assignable_v<item_t> &&std::is_move_constructible_v<item_t>
            &&is_move_assignment_operator_noexcept<item_t> &&std::is_nothrow_move_constructible_v<item_t>)
    {
        assert(spa.data() != nullptr && 0U < index && index < spa.size());

        if (!less1(spa[index], spa[index - 1U]))
        {
            return;
        }

        auto current = std::move(spa[index]);

        do
        {
            spa[index] = std::move(spa[index - 1U]);
            --index;
        } while (0U < index && less1(current, spa[index - 1U]));

        spa[index] = std::move(current);
    }

    // Time O(n) for already or almost sorted data.
    // Worst time O(n*n).
    template<class item_t, class less_t = std::less<item_t>, std::size_t extent = std::dynamic_extent>
    constexpr void insertion_sort(std::span<item_t, extent> spa, less_t less1 = {})
    {
        const auto size = spa.size();

        for (std::size_t index = 1; index < size; ++index)
        {
            insertion_sort_step<item_t, less_t, extent>(spa, index, less1);
        }
    }

    template<class t>
    constexpr void swap_move(t &one, t &two) noexcept
    {
        auto temp = std::move(one);
        one = std::move(two);
        two = std::move(temp);
    }

    // Return stably the minimum of 3 elements.
    template<class t>
    [[nodiscard]] constexpr auto min3(t &one, t &two, t &three) noexcept -> t &
    {
        if (two < one)
        {// Ignore one.
            return three < two ? three : two;
        }

        // Ignore two.
        return three < one ? three : one;
    }

    template<class t>
    constexpr void swap_if_greater(t &first, t &second)
    {
        if (!(second < first))
        {
            return;
        }

        auto temp = std::move(first);
        first = std::move(second);
        second = std::move(temp);
    }

    // The array must have 3 elements to sort.
    template<class item_t, std::size_t extent = std::dynamic_extent>
    constexpr void fast_sort3(std::span<item_t, extent> spa)
    {
        assert(spa.data() != nullptr && 3U <= spa.size());

        // swap_if_greater(spa[0], spa[1]);
        ////swap_if_greater(spa[1], spa[2]);
        ////swap_if_greater(spa[0], spa[1]);
        //
        // auto temp = spa[1];
        // if (spa[2] < temp)
        //{
        //  spa[1] = spa[2];
        //  spa[2] = temp;
        //
        //  swap_if_greater(spa[0], spa[1]);
        //}

        // The fastest code may be longer.
        if (spa[1] < spa[0])
        {// 1 < 0.
            if (spa[2] < spa[0])
            {// 1 < 0. // 2 < 0.
                if (spa[2] < spa[1])
                {// 2 < 1. // 1 < 0. // 2 < 0.
                  // 2 < 1 < 0.
                    auto temp = std::move(spa[0]);
                    spa[0] = std::move(spa[2]);
                    spa[2] = std::move(temp);
                }
                else
                {// 1 < 0. // 2 < 0. // 1 <= 2.
                  // 1 <= 2 < 0.
                    auto temp = std::move(spa[0]);
                    spa[0] = std::move(spa[1]);
                    spa[1] = std::move(spa[2]);
                    spa[2] = std::move(temp);
                }
            }
            else
            {// 1 < 0. // 0 <= 2.
              // 1 < 0 <= 2.
                auto temp = std::move(spa[0]);
                spa[0] = std::move(spa[1]);
                spa[1] = std::move(temp);
            }
        }
        else if (spa[2] < spa[1])
        {// 0 <= 1. // 2 < 1.
            if (spa[2] < spa[0])
            {// 0 <= 1. // 2 < 1. // 2 < 0.
              // 2 < 0 <= 1.
                auto temp = std::move(spa[0]);
                spa[0] = std::move(spa[2]);
                spa[2] = std::move(spa[1]);
                spa[1] = std::move(temp);
            }
            else
            {// 0 <= 1. // 2 < 1. // 0 <= 2.
              // 0 <= 2 < 1.
                auto temp = std::move(spa[1]);
                spa[1] = std::move(spa[2]);
                spa[2] = std::move(temp);
            }
        }

        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (const auto is_ok = !(spa[1] < spa[0]) && !(spa[2] < spa[1]); !is_ok) [[unlikely]]
            {
                throw std::runtime_error("Error in fast_sort3.");
            }
        }
    }

    // The array must have 4 elements to sort.
    template<class item_t, std::size_t extent = std::dynamic_extent>
    constexpr void fast_sort4(std::span<item_t, extent> spa)
    {
        assert(spa.data() != nullptr && 4U <= spa.size());

        // todo(p4): faster.
        swap_if_greater(spa[0], spa[1]);
        swap_if_greater(spa[2], spa[3]);

        // Min
        swap_if_greater(spa[0], spa[2]);

        // Max
        swap_if_greater(spa[1], spa[3]);

        // Middle
        swap_if_greater(spa[1], spa[2]);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (const auto is_ok = !(spa[1] < spa[0]) && !(spa[2] < spa[1]) && !(spa[3] < spa[2]); !is_ok) [[unlikely]]
            {
                throw std::runtime_error("Error in fast_sort4.");
            }
        }
    }

    // The array must have 5 elements to sort.
    template<class item_t, std::size_t extent = std::dynamic_extent>
    constexpr void fast_sort5(std::span<item_t, extent> spa)
    {
        assert(spa.data() != nullptr && 5U <= spa.size());

        // todo(p5): faster.
        swap_if_greater(spa[0], spa[1]);
        swap_if_greater(spa[3], spa[4]);

        // Min
        swap_if_greater(spa[0], spa[2]);
        swap_if_greater(spa[0], spa[3]);

        swap_if_greater(spa[1], spa[2]);
        swap_if_greater(spa[2], spa[3]);

        swap_if_greater(spa[1], spa[4]);
        swap_if_greater(spa[1], spa[2]);

        swap_if_greater(spa[3], spa[4]);

        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (const auto is_ok = !(spa[1] < spa[0]) && !(spa[2] < spa[1]) && !(spa[3] < spa[2]) && !(spa[4] < spa[3]);
                !is_ok) [[unlikely]]
            {
                throw std::runtime_error("Error in fast_sort5.");
            }
        }
    }

    // Minimize the number of moves in case of a stable sort.
    // Time O(n*n).
    template<class item_t, std::size_t extent = std::dynamic_extent>
    constexpr void selection_sort(std::span<item_t, extent> spa,
        std::int32_t (*comparer)(const item_t &, const item_t &), const bool use_stable_sort)
    {
        throw_if_null("comparer", comparer);

        auto *base = spa.data();

        if (base == nullptr || spa.size() <= 1U)
        {
            return;
        }

        for (const auto *const end_incl = base + spa.size() - 1U; base < end_incl; ++base)
        {
            auto *min_value_address = base;

            for (auto *cand_address = base + 1; cand_address <= end_incl; ++cand_address)
            {
                if (const auto comparison_result = comparer(*cand_address, *min_value_address); comparison_result < 0)
                {
                    min_value_address = cand_address;
                }
            }

            if (!use_stable_sort)
            {
                swap_move(*base, *min_value_address);
                continue;
            }

            // Stable sorting should be slower.
            auto min_value = std::move(*min_value_address);

            // Shift sub-array of values from [startingAddress, minValueAddress - 1]
            //  to the right by one element to preserve the order.
            for (auto *shift_address = min_value_address; base < shift_address; --shift_address)
            {
                *shift_address = std::move(*(shift_address - 1));
            }

            *base = std::move(min_value);
        }
    }

    // This is 15 per cent slower than "selection_sort".
    template<class iterator>
    constexpr void selection_sort_slow(iterator beginen, iterator enden)
    {
        while (beginen != enden)
        {
            auto minette = std::min_element(beginen, enden);
            std::iter_swap(beginen, minette);
            ++beginen;
        }
    }

    // Stable count sort. Usually the "index_max" is relatively small.
    // The buffer size must be >= (stop - start).
    template<std::unsigned_integral int_t, int_t index_max,
        // std::function(int_t(int_t))
        class to_index_t, class iter_t, class iter_t2>
    requires(sizeof(int_t) <= sizeof(std::size_t) && 1U < index_max &&
        index_max < std::numeric_limits<std::size_t>::max() - 2U)
    constexpr void counting_sort(iter_t start, iter_t stop, to_index_t to_index, iter_t2 buf)
    {
        assert(!(stop < start));

        const auto size = stop - start;
        if (size <= 1)
        {
            return;
        }

        using larger_type = std::size_t;

        std::array<larger_type, index_max> counts{};
        {
            auto sta = start;

            do
            {
                const auto index = to_index(sta);
                assert(index < index_max);

                ++counts.at(index);
            } while (++sta != stop);
        }

        Inner::propagate_counts(static_cast<larger_type>(index_max - 1U), counts);

        {
            auto sta = start;

            do
            {
                const auto index = to_index(sta);
                assert(index < index_max);

                auto &cnt = counts.at(index);
                *(buf + cnt) = *sta;
                ++cnt;
            } while (++sta != stop);
        }

        std::copy(buf, buf + size, start);
    }

    // todo(p3): Optimize variable length integers: firstly, split the numbers by digit count - use the count sort.
    // Note that smaller digit count numbers come first e.g. 999 < 1000, or 3 < 4 (in digits).
    template<std::unsigned_integral int_t,
        // 255 when using 1-byte digits.
        std::size_t mask = std::numeric_limits<std::uint8_t>::max(), std::size_t extent = std::dynamic_extent,
        std::size_t extent2 = std::dynamic_extent>
    requires(0U < (mask & 1U) && mask < std::numeric_limits<std::size_t>::max() &&
        (std::is_same_v<int_t, std::uint8_t> || std::is_same_v<int_t, std::uint16_t> ||
            std::is_same_v<int_t, std::uint32_t> || std::is_same_v<int_t, std::uint64_t>))
    constexpr void radix_sort_unsigned(std::span<int_t, extent> spa, std::span<int_t, extent2> buffer)
    {
        const auto size = spa.size();

        if (spa.data() == nullptr || size <= 1U)
        {
            return;
        }

        assert(buffer.data() != nullptr && spa.data() != buffer.data() && size <= buffer.size());

        // No initializer because the array will be filled in below.
        std::array<std::size_t, mask + 1LLU> counts;
        std::size_t digit{};

        do
        {
            counts.fill(std::size_t{});

            const auto pos = digit << 3U;
            {
                std::size_t index{};

                do
                {
                    const auto &item = spa[index];
                    const auto masked_pos = (item >> pos) & mask;
                    ++counts[masked_pos];
                } while (++index < size);
            }
            {// sum up.
                // todo(p3): propagate_counts
                std::size_t index{};
                std::size_t prev{};

                do
                {
                    const auto next = prev + counts[index];
                    counts[index] = prev;
                    prev = next;
                } while (++index <= mask);
            }
            {
                std::size_t index{};

                do
                {
                    const auto &item = spa[index];
                    const auto item_masked = (item >> pos) & mask;
                    auto &cnt = counts[item_masked];
                    buffer[cnt] = item;
                    ++cnt;
                } while (++index < size);
            }

            std::copy(buffer.begin(), buffer.begin() + size, spa.begin());
        } while (++digit < sizeof(int_t));
    }

    template<class collection_t, digita digit_t,
        // "std::uint32_t" can be used if both the collection and each item
        // have fewer than (power(2, 32) - 1) items
        // in order to save memory.
        std::unsigned_integral int_t, class get_size_t, class get_item_t,
        const int_t max_value = static_cast<int_t>(max_digit<digit_t>)>
    requires(sizeof(digit_t) <= sizeof(int_t) && 0U < max_value && max_value < std::numeric_limits<int_t>::max())
    constexpr void radix_sort(collection_t &collection, get_size_t get_size = {}, get_item_t get_item = {})
    {
        if (std::numeric_limits<int_t>::max() <= collection.size())
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The type '" << typeid(int_t).name() << "' is too small to have " << collection.size() << " items.";

            throw_exception<std::out_of_range>(str);
        }

        const auto size = static_cast<int_t>(collection.size());
        if (size <= 1U) // Already sorted.
        {
            return;
        }

        const auto [empty_count, max_length] =
            Inner::empty_count_max_length<int_t, collection_t, get_size_t>(collection, get_size);

        if (size == empty_count)
        {// Already sorted as all items are empty.
            return;
        }

        collection_t temp{};

        if (0U < empty_count)
        {
            Inner::move_empty_to_start<collection_t, int_t, get_size_t>(collection, get_size, empty_count, temp);
        }

        temp.resize(size - empty_count);

        std::vector<int_t> counts(max_value + 1LLU);

        // From least- to most- significant digit.
        auto digit = static_cast<int_t>(max_length - 1U);

        do
        {
            Inner::sort_by_digit<collection_t, digit_t, int_t, get_size_t, get_item_t>(
                digit, get_size, max_value, get_item, empty_count, collection, counts, temp);
        } while (0U < digit--);
    }

    // See also quick_sort

    // todo(p3): stable sort of std::vector<std::string>; measure time, compare with std::sort.
    // Note "0" < "01" < "1" < "10".
    // Let i=0; split by i-th char/digit, empty strings come first.
    // For each char, if >1 aren't empty, ++i, do a recursive call.
    //
    // To have fewer recursive calls:
    // - set range = [0, size)
    // - while(0 < size)
    // -- set the largest length sub-range = {};
    // --- fund the largest length sub-range while counting;
    // --- call recursively on smaller sub-ranges; break ties randomly;
    // -- set range = the largest sub-range; // tail elimination.
} // namespace Standard::Algorithms::Numbers
