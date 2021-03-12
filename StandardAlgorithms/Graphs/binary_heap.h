#pragma once
#include"../Utilities/require_utilities.h"
#include"binary_tree_utilities.h"
#include<cstring>
#include<functional>
#include<span>
#include<vector>

namespace Standard::Algorithms::Heaps
{
    // Minimum binary heap.
    template<class item_t, class container_t = std::vector<item_t>, class comparer_t = std::less_equal<item_t>>
    struct binary_heap
    {
        constexpr explicit binary_heap(const std::size_t capacity = {})
        {
            if (0U < capacity)
            {
                Data.reserve(capacity);
            }
        }

        constexpr virtual ~binary_heap() noexcept = default;

        constexpr binary_heap(const binary_heap &) = default;

        constexpr auto operator= (const binary_heap &other) &noexcept(false) -> binary_heap &
        // #if (1 == 2) noexcept(false) = default; // todo(p3): It does not work - fix.
        {
            // Problem - had to implement explicitly. todo(p3): can use default?
            if (this != &other)
            {
                Data = other.Data;
                Comparer = other.Comparer;
            }

            return *this;
        }

        constexpr binary_heap(binary_heap &&) noexcept = default;

        constexpr auto operator= (binary_heap &&other) &noexcept -> binary_heap &
        {
            if (this != &other)
            {
                Data = std::move(other.Data);
                Comparer = std::move(other.Comparer);
            }

            return *this;
        }

        [[nodiscard]] constexpr auto size() const noexcept
        {
            return Data.size();
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept
        {
            return Data.empty();
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const container_t &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto top() const & -> const item_t &
        {
            if (Data.empty()) [[unlikely]]
            {
                throw std::runtime_error("The heap is empty - cannot get the top element.");
            }

            return Data[0];
        }

        constexpr virtual void clear() noexcept
        {
            Data.clear();
        }

        // Clear and build the heap from the given array of elements.
        //
        // If an exception is thrown,
        // the instance may have the corrupted data: call clear() to fix it up.
        constexpr virtual void clear_and_build(const std::span<const item_t, std::dynamic_extent> spa)
        {
            throw_if_null("source in clear_and_build.", spa.data());
            const auto count = require_positive(spa.size(), "count in clear_and_build");

            clear();
            copy_data(spa);

            if (1U == count)
            {// 1 item is already sorted.
                return;
            }

            auto parent_index = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(count - 1);

            do
            {
                repair_downwards(parent_index);
            } while (0U < parent_index--);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                this->validate("clear_and_build.");
            }
        }

        // Sort the array using the heap sort algorithm.
        //
        // If an exception is thrown,
        // the instance may have the corrupted data: call clear() to fix it up.
        //
        // The comparer_t should be "greater_equal< T >" in
        // order to sort in the ascending order.
        constexpr void heap_sort(std::vector<item_t> &data)
        {
            const auto count = data.size();
            if (count <= 1U)
            {
                return;
            }

            // todo(p2): Sort can be done without data copy.
            const auto spa = std::span(data.data(), count);
            clear_and_build(spa);
            assert(0U < count);

            auto index = count - 1U;
            do
            {
                data[index] = top();
                pop();
            } while (0U < index--);
        }

        // Add an item.
        constexpr virtual void push(const item_t &item)
        {
            const auto last_item_index = Data.size();
            Data.push_back(item);

            if (0U < last_item_index)
            {// There are at least 2 elements.
                repair_upwards(last_item_index);
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                validate("Add.");
            }
        }

        template<class... args_t>
        constexpr void emplace(args_t &&...args)
        {
            item_t item{ std::forward<args_t>(args)... }; // todo(p3): del hack.
            push(item);
        }

        // Delete the top element.
        constexpr virtual void pop()
        {
            const auto initial_size = Data.size();
            if (0U == initial_size) [[unlikely]]
            {
                throw std::runtime_error("The heap is empty - cannot delete the top element.");
            }

            constexpr auto new_position = 0U;

            const std::size_t new_size = initial_size - 1ZU;

            if (0U < new_size)
            {
                Data[new_position] = Data[new_size];
            }

            Data.pop_back();

            if (1U < new_size)
            {// There are at least 2 elements left after removal.
                repair_downwards(new_position);
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                validate("Delete top.");
            }
        }

        constexpr void validate(const std::string &message) const
        {
            constexpr std::size_t root_has_no_parent = 1;

            const auto siz = Data.size();

            for (auto index = root_has_no_parent; index < siz; ++index)
            {
                const auto &current_item = Data[index];

                const std::size_t parent_index = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(index);
                const auto &parent_item = Data[parent_index];
                const auto is_valid = compare(parent_item, current_item);

                if (!is_valid) [[unlikely]]
                {
                    constexpr auto beginc = '{';
                    constexpr auto endc = '}';

                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The heap is broken at index " << index << ", parentItem " << beginc << parent_item << endc
                        << ", currentItem " << beginc << current_item << endc << ". " << message;

                    throw_exception(str);
                }
            }
        }

protected:
        [[nodiscard]] constexpr auto data() &noexcept -> container_t &
        {
            return Data;
        }

        inline constexpr virtual void swap(const std::size_t index1, const std::size_t index2) noexcept(
            !::Standard::Algorithms::is_debug)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (index1 == index2)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error: index1 == index2 = " << index1 << " in binary_heap::swap.";
                    throw_exception(str);
                }
            }

            std::swap(Data[index1], Data[index2]);
        }

        // If an exception is thrown,
        // the instance may have the corrupted data: call clear() to fix it up.
        constexpr virtual void copy_data(const std::span<const item_t, std::dynamic_extent> spa)
        {
            const auto *const source = throw_if_null("source in copy_data.", spa.data());
            const auto count = require_positive(spa.size(), "count in copy_data");

            Data.clear();
            Data.reserve(count);

            std::copy(source, source + count, std::back_inserter(Data));
        }

        // Whether parentItem <= currentItem.
        template<class t2>
        [[nodiscard]] constexpr auto compare(const t2 &parent_item, const t2 &current_item) const -> bool
        {
            auto result = Comparer(parent_item, current_item);
            return result;
        }

        constexpr void repair_upwards(std::size_t index)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (0U == index) [[unlikely]]
                {
                    throw std::runtime_error("0 == index in repair_upwards.");
                }
            }

            do
            {
                const auto parent_index = Standard::Algorithms::Trees::binary_tree_utilities<>::parent_node(index);
                if (const auto is_good_heap = compare(Data[parent_index], Data[index]); is_good_heap)
                {
                    break;
                }

                // todo(p1): (2*moves - 1) fewer moves instead of swaps.
                swap(index, parent_index);
                index = parent_index;
            } while (0U < index);
        }

        constexpr void repair_downwards(const std::size_t parent_index)
        {
            const std::size_t siz = Data.size();

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (siz <= parent_index) [[unlikely]]
                {
                    throw std::runtime_error("size <= parent_index in repair_downwards.");
                }
            }

            std::size_t parent = parent_index;
            for (;;)
            {
                const std::size_t left = Standard::Algorithms::Trees::binary_tree_utilities<>::left_child(parent);
                if (siz <= left)
                {
                    return;
                }

                const std::size_t right = Standard::Algorithms::Trees::binary_tree_utilities<>::right_child(parent);
                if (siz == right)
                {// There is only one child: left.
                    if (!compare(Data[parent], Data[left]))
                    {
                        swap(parent, left);
                    }

                    return;
                }

                // There are both children.
                auto change_index = right;

                if (compare(Data[parent], Data[left]))
                {
                    if (compare(Data[parent], Data[right]))
                    {// Everything is in place.
                        return;
                    }

                    //"right < parent <= left" - only right sub-tree must be fixed.
                }
                else
                {// left < parent
                    if (!compare(Data[right], Data[left]))
                    {// left < right
                        change_index = left;
                    }
                }

                swap(parent, change_index);
                parent = change_index;
            }
        }

private:
        comparer_t Comparer{};
        container_t Data{};
    };
} // namespace Standard::Algorithms::Heaps
