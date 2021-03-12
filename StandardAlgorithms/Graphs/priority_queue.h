#pragma once
// "priority_queue.h"
#include"../Numbers/arithmetic.h"
#include"../Utilities/require_utilities.h"
#include"binary_heap.h"
#include<unordered_map>
#include<unordered_set>

namespace Standard::Algorithms::Heaps
{
    // A minimum heap that can decrease keys.
    // Can be used in MST and Dijkstra algorithms.
    // Be careful: each datum must have a unique index.
    template<class item_t, class get_index_function_t, class container_t = std::vector<item_t>,
        class comparer_t = std::less_equal<item_t>, class base_t = binary_heap<item_t, container_t, comparer_t>>
    struct priority_queue final : base_t
    {
        static constexpr auto infinity_position = 0ZU - static_cast<std::size_t>(1);

        constexpr explicit priority_queue(
            get_index_function_t get_index_function = {}, const std::size_t initial_capacity = {})
            : base_t(initial_capacity)
            , Positions(initial_capacity, infinity_position)
            , Get_index_function(throw_if_null("get index function", get_index_function))
        {
        }

        constexpr auto operator= (const base_t &bas) & -> priority_queue &
        {
            if (this == &bas)
            {
                return *this;
            }

            auto *converted = dynamic_cast<const priority_queue *>(&bas);
            if (converted == nullptr) [[unlikely]]
            {
                throw throw_type_mismatch<base_t>();
            }

            base_t::operator= (static_cast<const base_t &>(bas));

            Positions = converted->Positions;
            Get_index_function = converted->Get_index_function;

#if _DEBUG
            Used_indexes = converted->Used_indexes;
#endif

            return *this;
        }

        constexpr auto operator= (base_t &&bas) & -> priority_queue &
        {
            if (this == &bas)
            {
                return *this;
            }

            auto *converted = dynamic_cast<const priority_queue *>(&bas);
            if (converted == nullptr) [[unlikely]]
            {
                throw throw_type_mismatch<base_t>();
            }

            Positions = std::move(converted->Positions);
            Get_index_function = std::move(converted->Get_index_function);

#if _DEBUG
            Used_indexes = std::move(converted->Used_indexes);
#endif

            return *this;
        }

        constexpr void clear() noexcept override
        {
            base_t::clear();

            Positions.clear();

#if _DEBUG
            Used_indexes.clear();
#endif
        }

        // Indexes start from 0.
        constexpr void ensure_indexes_size(const std::size_t max_index)
        {
            const auto indexes_size = Positions.size();
            if (max_index < indexes_size)
            {
                return;
            }

            const auto new_size_raw = max_index << 1U;
            const auto new_size = Standard::Algorithms::Numbers::round_to_greater_power_of_two(new_size_raw);
            require_greater(new_size, max_index, "newSize");

            Positions.resize(new_size, infinity_position);
        }

        // Add an item.
        // Careful: each datum must have a unique index.
        constexpr void push(const item_t &item) override
        {
            const std::size_t index = Get_index_function(item);
            ensure_indexes_size(index);

            const auto data_size_old = this->data().size();

#if _DEBUG
            if (const auto inserted = Used_indexes.insert(index); !inserted.second) [[unlikely]]
            {
                if (Positions.size() <= index)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The existing item index " << index << " exceeds the Positions.size() " << Positions.size()
                        << "; and the new item '" << item << "' maps to the same index.";

                    throw_exception(str);
                }

                const auto &old_index = Positions[index];

                if (data_size_old <= old_index)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The existing item at index " << index << " positions to oldIndex " << old_index
                        << " which exceeds the dataSizeOld " << data_size_old << "; and the new item '" << item
                        << "' maps to the same index.";

                    throw_exception(str);
                }

                const auto &old_item = this->data()[old_index];

                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The existing item '" << old_item << "' at " << old_index << " and the new item '" << item
                    << "' have the same index " << index << ".";
                throw_exception(str);
            }
#endif

            Positions[index] = data_size_old;

            try
            {
                this->data().push_back(item);
            }
            catch (...)
            {// Restore.
                Positions[index] = infinity_position;

#if _DEBUG
                Used_indexes.erase(index);
#endif

                throw;
            }

            if (0U < data_size_old)
            {
                this->repair_upwards(data_size_old);
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                this->validate("Add.");
            }
        }

        // todo(p3): emplace(..).

        constexpr void pop() override
        {
            const auto initial_size = require_positive(this->data().size(), "Cannot pop from an empty priority queue.");

            const auto new_size = initial_size - 1U;
            auto last_item = std::move(this->data()[new_size]);

            const auto index = get_index(last_item);
            update_top_position();

            constexpr std::size_t new_position{};

            this->data()[new_position] = std::move(last_item);
            Positions[index] = new_position;

            // Cannot remove from Positions because the last item might have the (N-1) position.
            this->data().pop_back();

            if (2U <= new_size)
            {
                this->repair_downwards(new_position);
            }

            if constexpr (::Standard::Algorithms::is_debug)
            {
                this->validate("pop.");
            }
        }

        constexpr void decrease_key(const item_t &item_to_update) &
        {
            const auto index = get_index(item_to_update);
            const auto &position = Positions[index];

            if constexpr (::Standard::Algorithms::is_debug)
            {
                require_greater(this->data().size(), position, "decrease_key position");

                if (const auto has_decreased_or_same = this->compare(item_to_update, this->data()[position]);
                    !has_decreased_or_same)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "_DEBUG: item at " << position << " has not decreased key, "
                        << "old item " << this->data()[position] << ", new item " << item_to_update << ".";

                    throw_exception(str);
                }
            }

            this->data()[position] = item_to_update;

            if (0U < position)
            {// There are at least 2 elements.
                this->repair_upwards(position);
            }
        }

        [[nodiscard]] constexpr auto try_get_item(const item_t &current_item, item_t &queue_item) const & -> bool
        {
            const auto index = get_index(current_item);
            const auto &position = Positions[index];
            auto result = position != infinity_position;

            if (result)
            {
                if constexpr (::Standard::Algorithms::is_debug)
                {
                    require_greater(this->data().size(), position, "try_get_item position");
                }

                queue_item = this->data()[position];
            }

            return result;
        }

protected:
        inline constexpr void swap(const std::size_t index1, const std::size_t index2) noexcept(false) override
        {
            auto temp = this->data().at(index1);

            const auto index_a = get_index(temp);
            const auto index_b = get_index(this->data().at(index2));

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (index1 == index2) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error: index1 == index2 = " << index2 << " in priority_queue::swap.";

                    throw_exception(str);
                }

                if (index_a == index_b) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Error: indexA == indexB " << index_b << " in priority_queue::swap; index1 " << index1
                        << ", index2 " << index2;

                    throw_exception(str);
                }
            }

            this->data()[index1] = this->data()[index2];
            this->data()[index2] = std::move(temp);

            Positions[index_a] = index2;
            Positions[index_b] = index1;
        }

        // If an exception is thrown,
        // the instance may have the corrupted data: call clear() to fix it up.
        constexpr void copy_data(const std::span<const item_t, std::dynamic_extent> spa) override
        {
            base_t::copy_data(spa);

            throw_if_null("source in copy_data.", spa.data());
            const auto count = require_positive(spa.size(), "count in copy_data");

            auto max_index = count - 1U;
            ensure_indexes_size(max_index);

#ifdef _DEBUG
            std::unordered_map<std::size_t, std::size_t> unique_indexes;
#endif

            for (std::size_t ind2{}; ind2 < count; ++ind2)
            {
                const auto &new_item = spa[ind2];
                const std::size_t index = Get_index_function(new_item);

#if _DEBUG
                if (const auto inserted = unique_indexes.insert({ index, ind2 }); !inserted.second) [[unlikely]]
                {
                    const auto &old_index = inserted.first->second;
                    require_greater(count, old_index, "copy_data oldIndex");

                    const auto &old_item = spa[old_index];

                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "At least two items '" << old_item << "' at " << old_index << " and '" << new_item << "' at "
                        << ind2 << " have the same index " << index << ".";

                    throw_exception(str);
                }
#endif

                if (max_index < index)
                {
                    ensure_indexes_size(index);
                    max_index = Positions.size() - 1U;
                }

                Positions[index] = ind2;
            }
        }

private:
        constexpr void update_top_position()
        {
            const auto index = get_index(this->data()[0]);

#if _DEBUG
            if (const auto deleted = Used_indexes.erase(index); 0U == deleted)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Used indexes must have the index " << index << " while deleting the top item '"
                    << this->data()[0] << "'.";
                throw_exception(str);
            }
#endif

            Positions[index] = infinity_position;
        }

        [[nodiscard]] inline constexpr auto get_index(const item_t &item) const -> std::size_t
        {
            const auto raw = Get_index_function(item);
            auto index = static_cast<std::size_t>(raw);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                require_greater(Positions.size(), index, "index");
            }

            return index;
        }

        std::vector<std::size_t> Positions;
        get_index_function_t Get_index_function;

#ifdef _DEBUG
        // To ensure unique indexes.
        std::unordered_set<std::size_t> Used_indexes{};
#endif
    };
} // namespace Standard::Algorithms::Heaps
