#pragma once
#include<algorithm>
#include<cassert>
#include<set>

namespace Standard::Algorithms::Heaps
{
    // Simple double-ended priority queue, DEPQ.
    // O(log(n)) time for many operations.
    template<class item_t>
    struct multiset_heap_depq final
    {
        // Time O(1).
        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Data.size();
        }

        // Time O(1).
        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool
        {
            return Data.empty();
        }

        // Time O(1).
        [[nodiscard]] constexpr auto data() const &noexcept -> const std::multiset<item_t> &
        {
            return Data;
        }

        // Time O(1).
        [[nodiscard]] constexpr auto min() const &noexcept -> const item_t &
        {
            assert(!is_empty());

            const auto &skinny = *Data.cbegin();
            return skinny;
        }

        // Time O(1).
        [[nodiscard]] constexpr auto max() const &noexcept -> const item_t &
        {
            assert(!is_empty());
            const auto &lard = *Data.crbegin();
            return lard;
        }

        constexpr void push(const item_t &item) noexcept(false)
        {
            Data.insert(item);
        }

        // todo(p2): merge(multiset_heap_depq& other)

        // todo(p2): push_many in O(n).

        // todo(p2): void decrease_key(node_t*const node, const item_t &item)

        constexpr void pop_min()
        {
            assert(!is_empty());

            const auto ite = Data.cbegin();
            Data.erase(ite);
        }

        constexpr void pop_max()
        {
            assert(!is_empty());

            auto ite = std::prev(Data.end());
            Data.erase(ite);
        }

        // todo(p3):  void validate() const

private:
        std::multiset<item_t> Data{};
    };
} // namespace Standard::Algorithms::Heaps
