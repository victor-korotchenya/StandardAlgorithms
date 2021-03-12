#pragma once
#include"../Utilities/require_utilities.h"
#include<list>
#include<optional>
#include<vector>

namespace Standard::Algorithms::Heaps
{
    // A priority queue where the average edge weight is a small positive integer
    // which is less than (plus infinity / |vertices|).
    // Vertex duplicates are not allowed - must be unique and be pushed/inserted maximum once.
    // The extracted weight cannot decrease.
    // Both push and decrease_key are O(1).
    // Both top and pop are O(1) in average.
    // todo(p3): decrease memory use (Weight_item_lists) when max edge weight is small - keep only what's necessary.
    // Can be used to run Dijkstra's algorithm in O(|edges| + |vertices|) when the edge weights are small.
    template<class item_t1, std::integral int_t1>
    requires(sizeof(int_t1) <= sizeof(std::size_t) && std::convertible_to<int_t1, std::size_t>)
    struct priority_queue_limited_size final
    {
        using item_t = item_t1;
        using item_list_t = std::list<item_t>;
        using iter_t = decltype(std::declval<item_list_t>().cbegin());
        using int_t = int_t1;

        static constexpr int_t min_weight{};

        constexpr priority_queue_limited_size(const int_t &vertices_count, const int_t &plus_infinity)
            : Weight_item_lists(check_args_return_inf(vertices_count, plus_infinity))
            , Vertex_iters(vertices_count, std::nullopt)
        {
            self_check();
        }

        [[nodiscard]] constexpr auto plus_infinity() const -> int_t
        {
            self_check();

            auto inf = static_cast<int_t>(Weight_item_lists.size());
            require_positive(inf, "plus_infinity");

            return inf;
        }

        [[nodiscard]] constexpr auto vertices_count() const -> int_t
        {
            self_check();

            auto vcount = static_cast<int_t>(Vertex_iters.size());
            require_positive(vcount, "vertices count");

            return vcount;
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            self_check();

            return Size;
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool
        {
            return 0ZU == size();
        }

        constexpr void clear() // todo(p3): noexcept
        {
            self_check();

            const auto inf = plus_infinity();
            const auto vcount = vertices_count();

            for (auto &item_list : Weight_item_lists)
            {
                assert(item_list.size() <= Size);

                Size -= item_list.size();
                item_list.clear();
            }

            assert(Size == 0ZU);
            Size = {};
            Current_weight = {};
            Vertex_iters.assign(vcount, std::nullopt);

            self_check();
        }

        constexpr void push(const item_t &item)
        {
            self_check();

            const auto weight = check_value(static_cast<int_t>(item.weight), "push item weight", plus_infinity());

            const auto vertex = check_value(static_cast<int_t>(item.vertex), "push item vertex", vertices_count());

            if (weight < Current_weight)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "A new vertex " << vertex << " with weight " << weight
                    << " cannot be less then the current weight " << Current_weight << ".";

                throw_exception(str);
            }

            auto &optio = Vertex_iters.at(vertex);
            if (optio.has_value()) [[unlikely]]
            {
                auto err = "Cannot push again a vertex " + std::to_string(vertex);
                throw std::runtime_error(err);
            }

            auto &item_list = Weight_item_lists.at(weight);
            item_list.push_front(item);

            const auto iter = item_list.cbegin();

            try
            {
                optio = std::make_optional(iter);
            }
            catch (...)
            {
                item_list.erase(iter);
                throw;
            }

            ++Size;
            assert(0ZU < Size);

            self_check();
        }

        [[nodiscard]] constexpr auto top() & -> const item_t &
        {
            self_check();

            require_less_equal(min_weight, Current_weight, "Current_weight in pop");

            const auto inf = plus_infinity();

            while (Current_weight < inf)
            {
                const auto &item_list = Weight_item_lists.at(Current_weight);
                if (item_list.empty())
                {
                    ++Current_weight;
                    continue;
                }

                assert(0ZU < Size);

                const auto &item = item_list.front();
                return item;
            }

            throw std::runtime_error("No top in an empty priority queue of limited size.");
        }

        constexpr void pop()
        {
            self_check();

            require_less_equal(min_weight, Current_weight, "Current_weight in pop");

            const auto inf = plus_infinity();

            while (Current_weight < inf)
            {
                auto &item_list = Weight_item_lists.at(Current_weight);
                if (item_list.empty())
                {
                    ++Current_weight;
                    continue;
                }

                assert(0ZU < Size);
                item_list.pop_front();
                --Size;
                // todo(p3): maybe use 2 bits to signal done? Now, don't clear the Vertex_iters here.
                self_check();

                return;
            }

            throw std::runtime_error("Cannot pop from an empty priority queue of limited size.");
        }

        constexpr void decrease_key(const item_t &item) &
        {
            self_check();

            const auto inf = plus_infinity();
            const auto weight = check_value(static_cast<int_t>(item.weight), "decrease item weight", inf);

            const auto vertex = check_value(static_cast<int_t>(item.vertex), "decrease item vertex", vertices_count());

            if (weight < Current_weight)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Vertex " << vertex << " new weight " << weight << " cannot be less then the current weight "
                    << Current_weight << ".";

                throw_exception(str);
            }

            auto &optio = Vertex_iters.at(vertex);
            if (!optio.has_value()) [[unlikely]]
            {
                auto err = "Cannot decrease key of a never pushed vertex " + std::to_string(vertex);
                throw std::runtime_error(err);
            }

            const auto old_iter = optio.value();
            const auto old_weight = check_value(static_cast<int_t>(old_iter->weight), "old item weight", inf);

            if (!(weight < old_weight))
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Vertex " << vertex << " must have decreased the weight from old " << old_weight << " to new "
                    << weight << ".";

                throw_exception(str);
            }

            auto &old_item_list = Weight_item_lists.at(old_weight);
            auto &item_list = Weight_item_lists.at(weight);

            item_list.push_front(item);
            // todo(p3): more safety.
            {
                const auto iter = item_list.cbegin();
                optio = std::make_optional(iter);
            }
            old_item_list.erase(old_iter);

            self_check();
        }

        [[nodiscard]] constexpr auto try_get_item(const item_t &current_item, item_t &queue_item) const & -> bool
        {
            self_check();

            const auto vertex =
                check_value(static_cast<int_t>(current_item.vertex), "try get item vertex", vertices_count());

            const auto &optio = Vertex_iters.at(vertex);
            if (optio.has_value())
            {
                queue_item = *(optio.value());
                return true;
            }

            return false;
        }

private:
        [[nodiscard]] static constexpr auto check_args_return_inf(
            const int_t &vertices_count, const int_t &plus_infinity) -> const int_t &
        {
            require_positive(vertices_count, "vertices count");

            require_greater(plus_infinity, vertices_count, "plus infinity vs vertices count");

            return plus_infinity;
        }

        constexpr void self_check() const noexcept
        {
            if constexpr (!::Standard::Algorithms::is_debug)
            {
                return;
            }

            assert(!Vertex_iters.empty() && Vertex_iters.size() < Weight_item_lists.size());

            assert(!(Vertex_iters.size() < Size));

            assert(!(Current_weight < int_t{}));
            assert(!(Weight_item_lists.size() < static_cast<std::size_t>(Current_weight)));
        }

        [[nodiscard]] static constexpr auto check_value(int_t value, const char *const name, const int_t &inf) -> int_t
        {
            assert(name != nullptr && int_t{} < inf);

            if constexpr (std::is_signed_v<int_t>)
            {
                require_less_equal(min_weight, value, name);
            }

            require_greater(inf, value, name);

            return value;
        }

        // A vector of bool could save memory, but it is not cache friendly - let's use std::optional.

        std::vector<item_list_t> Weight_item_lists;
        std::vector<std::optional<iter_t>> Vertex_iters;
        std::size_t Size{};
        int_t Current_weight{};
    };
} // namespace Standard::Algorithms::Heaps
