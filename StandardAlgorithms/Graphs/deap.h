#pragma once
#include"binary_tree_utilities.h"
#include"one_based_index_t.h"
#include<bit>
#include<vector>

namespace Standard::Algorithms::Heaps
{
    // Deap heap, or double-ended priority queue, DEPQ.
    // The root stores no item; its left(right) subtree is min(max) heap.
    // When (1 < size), for any left node i
    // there is a corresponding node j (or its parent) in the right part,
    // and (i.item <= j.item).
    // O(log(n)) time for many operations.
    template<class item_t>
    struct deap final
    {
        // Time O(1).
        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            assert(!Data.empty());
            return Data.size() - empty_tree_size;
        }

        // Time O(1).
        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool
        {
            return size() == 0U;
        }

        // Time O(1).
        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<item_t> &
        {
            assert(!Data.empty());
            return Data;
        }

        // Time O(1).
        [[nodiscard]] constexpr auto min() const &noexcept -> const item_t &
        {
            assert(!is_empty());
            return Data.at(empty_tree_size);
        }

        // Time O(1).
        [[nodiscard]] constexpr auto max() const &noexcept -> const item_t &
        {
            const auto siz = size();
            const auto index = std::min(siz, empty_tree_size * 2U);
            assert(0U < index && index < Data.size());

            return Data.at(index);
        }

        // todo(p2): template<class item_t2> void push(item_t2 &&item) _data.push_back(std::forward<item_t2>(item));
        //
        // todo(p2): make strong guarantee.
        constexpr void push(const item_t &item) noexcept(false)
        {
            assert(!Data.empty());

            const auto old_size = size();
            if (old_size == 0U)
            {
                Data.push_back(item);
                return;
            }

            const auto par = push_identify_subtree(old_size, item);
            push_up_towards_root(item, par.first, par.second);
        }

        // todo(p2): merge(deap& other)

        // todo(p2): push_many in O(n).

        // todo(p2): void decrease_key(node_t& node, const item_t &item)

        constexpr void pop_min()
        {
            pop(true);
        }

        constexpr void pop_max()
        {
            pop(false);
        }

        constexpr void validate() const noexcept(false)
        {
            assert(!Data.empty());

            const auto size1 = size();

            if (1U < size1)
            {
                const auto &min_item = Data[1];
                const auto &max_item = Data[2];

                if (max_item < min_item) [[unlikely]]
                {
                    throw std::runtime_error("Error in deap data, 1-based indexes: (Max item at 2) < (min item at 1).");
                }
            }

            //                     1 (root)
            //         2                             3
            // 4* (first possible node index)
            for (auto index = one_based_index_t(3U); index.zero_based() <= size1; ++index)
            {
                const auto &item = Data.at(index.zero_based());
                auto sym_ind = symmetric_index(index); // todo(p3): faster compute.
                const auto is_right = sym_ind < index;
                {
                    const auto par = parent(index);
                    assert(0U < par.zero_based());

                    const auto &parent_item = Data[par.zero_based()];
                    if (is_right)
                    {
                        if (parent_item < item) [[unlikely]]
                        {
                            throw std::runtime_error("Error in deap max subtree, 1-based indexes: (Larger item at " +
                                std::to_string(par.one_based()) + ") < (smaller item at " +
                                std::to_string(index.one_based()) + ").");
                        }

                        continue; // Already verified.
                    }

                    if (item < parent_item) [[unlikely]]
                    {
                        throw std::runtime_error("Error in deap min subtree, 1-based indexes: (Smaller item at " +
                            std::to_string(par.one_based()) + ") > (larger item at " +
                            std::to_string(index.one_based()) + ").");
                    }
                }
                {
                    if (size1 < sym_ind.zero_based())
                    {
                        assert(!is_right);
                        sym_ind = parent(sym_ind);
                    }

                    assert(0U < sym_ind.zero_based() && sym_ind.zero_based() <= size1);

                    const auto &right_item = Data.at(sym_ind.zero_based());
                    if (right_item < item) [[unlikely]]
                    {
                        throw std::runtime_error("Error in deap max subtree, 1-based indexes: (Larger item at " +
                            std::to_string(sym_ind.one_based()) + ") < (smaller item at " +
                            std::to_string(index.one_based()) + ").");
                    }
                }
            }
        }

private:
        [[nodiscard]] static constexpr auto is_better(const item_t &one, const bool is_less, const item_t &two) noexcept
        {
            const auto bet = (is_less && one < two) || (!is_less && two < one);
            return bet;
        }

        [[nodiscard]] constexpr auto push_identify_subtree(const std::size_t old_size, const item_t &item) noexcept(
            false) -> std::pair<one_based_index_t, bool>
        {
            require_positive(old_size, "old_size");

            // Simplest 1-based sample. One node excluding the root:
            //     1 (root)
            //   /             \;
            // 2 (node)    3* (new item)
            const auto new_index = one_based_index_t(old_size + 1U);
            auto index = symmetric_index(new_index);
            auto is_right = index < new_index;

            if (const auto must_go_to_parent = !is_right; must_go_to_parent)
            {
                assert(Data.size() < index.zero_based());

                index = parent(index);
            }

            const auto zeb = index.zero_based();
            assert(0U < zeb && zeb < Data.size());

            auto &cur = Data.at(zeb);
            const auto shall_change_subtree = is_better(item, is_right, cur);
            if (shall_change_subtree)
            {
                // Add to left:
                //       10                     (20 cur)
                //    /        \;
                // 12       (300* item)
                //
                //       10                     (300* cur)
                //    /        \;
                // 12       20
                //
                //
                // Add to right:
                //        10                   20
                //    /        \               /;
                // (12 cur) 15    (11* item)
                //
                //        10                   20
                //    /        \               /;
                // (11* cur) 15    12
                Data.push_back(std::move(cur));
                is_right = !is_right;
            }
            else
            {
                Data.push_back(item); // todo(p2): optim
                index = new_index;
            }

            return { index, is_right };
        }

        constexpr void push_up_towards_root(const item_t &item, one_based_index_t index, const bool is_right)
        {
            assert(!Data.empty());

            for (;;)
            {
                const auto zeb = index.zero_based();
                assert(0U < zeb && zeb < Data.size());

                auto &cur = Data.at(zeb);
                const auto par = parent(index);
                auto &parent_item = Data[par.zero_based()];

                if (par.zero_based() == 0U || (is_right && !(parent_item < item)) ||
                    (!is_right && !(item < parent_item)))
                {
                    cur = item; // todo(p2): might throw - revert.
                    return;
                }

                cur = std::move(parent_item);
                index = par;
            }
        }

        [[nodiscard]] constexpr auto pop_small_size(const std::size_t old_size, const bool is_minimum) -> bool
        {
            if (small_size < old_size)
            {
                return false;
            }

            if (old_size == 3U)
            {
                //        0 root
                //    1       2
                // 3
                Data[is_minimum ? 1 : 2] = std::move(Data.back());
            }
            else if (old_size == 2U && is_minimum)
            {
                Data[1] = std::move(Data.back());
            }
            else if (old_size == 0U) [[unlikely]]
            {
                static const auto *const min_error = "Cannot pop min from an empty deap.";
                static const auto *const max_error = "Cannot pop max from an empty deap.";

                throw std::runtime_error(is_minimum ? min_error : max_error);
            }

            Data.pop_back();
            assert(!Data.empty());

            return true;
        }

        //         root
        //     3           6* (pop max)
        // 5     4?
        //
        //     3           4* (not done)
        // 5?
        //
        //     3           5
        // 4
        //
        //
        //                       root
        //     10* (pop min)      20
        // 12     16                14   18?
        //
        //     18*          20
        // 12?    16   14
        //
        //     12           20
        // 18*   16   14?
        //
        //     12           20
        // 14   16   18
        //
        //
        //                 root
        //         2                   34* (pop max)
        //     4       6           22     30
        // 18 14   8 21   20   16?
        //
        //         2                   16*
        //     4       6           22     30?
        // 18 14   8 21   20
        //
        //         2                   30
        //     4       6           22     16*
        // 18 14   8 21?  20
        //
        //         2                   30
        //     4       6           22     21
        // 18 14   8 16   20
        //
        //
        //                          root
        //       2* (pop min)        30
        //    4     6                22          26
        // 18 8  14 16      18   22   24?
        //
        //       24*                       30
        //    4?      6             22          26
        // 18 8  14 16      18   22
        //
        //       4                         30
        //    24*    6             22          26
        // 18 8? 14 16      18   22
        //
        //       4                         30
        //    8           6              22          26
        // 18 24*  14 16      18   22?
        //
        //       4                         30
        //    8          6              22?         26
        // 18 22   14 16      18   24* (to be moved up)
        //
        //       4                         30
        //    8          6              24          26
        // 18 22   14 16      18   22
        [[nodiscard]] constexpr auto pop_move_hole_down(const std::size_t old_size, const bool is_minimum)
            -> one_based_index_t
        {
            assert(small_size < old_size);

            const auto &back1 = Data.back();

            for (auto hole = one_based_index_t(is_minimum ? 1U : 2U);;)
            {
                assert(hole.zero_based() < old_size);

                const auto left = left_child(hole);
                if (old_size <= left.zero_based())
                {
                    return hole;
                }

                auto &left_item = Data.at(left.zero_based());
                auto right = left + one_based_index_t(1U);

                if (const auto only_left_child = old_size == right.zero_based(); only_left_child)
                {
                    if (is_better(left_item, is_minimum, back1))
                    {
                        auto &cur = Data.at(hole.zero_based());
                        cur = std::move(left_item);
                        hole = left;
                    }

                    return hole;
                }

                if (const auto &temp = Data.at(right.zero_based()); is_better(left_item, is_minimum, temp))
                {
                    right = left;
                }

                auto &right_item = Data.at(right.zero_based());
                if (!is_better(right_item, is_minimum, back1))
                {
                    return hole;
                }

                auto &cur2 = Data.at(hole.zero_based());
                cur2 = std::move(right_item);
                hole = right;
            }
        }

        [[nodiscard]] constexpr auto pop_swap_corresponding(
            const std::size_t old_size, one_based_index_t sym_ind, bool &is_minimum, one_based_index_t &hole) -> bool
        {
            assert(sym_ind.zero_based() <= old_size);

            if (const auto left = left_child(sym_ind); left.zero_based() < old_size)
            {
                if (is_better(Data[left.zero_based()], is_minimum, Data[sym_ind.zero_based()]))
                {
                    sym_ind = left;
                }

                if (const auto right = left + one_based_index_t(1); right.zero_based() < old_size &&
                    is_better(Data[right.zero_based()], is_minimum, Data[sym_ind.zero_based()]))
                {
                    sym_ind = right;
                }
            }

            assert(sym_ind.zero_based() <= old_size);

            const auto &back1 = Data.back();

            auto &symm_item = Data.at(sym_ind.zero_based());
            if (!is_better(symm_item, is_minimum, back1))
            {
                return false;
            }

            assert(sym_ind.zero_based() < old_size);

            auto &cur = Data.at(hole.zero_based());
            cur = std::move(symm_item);
            hole = sym_ind;
            is_minimum = !is_minimum;

            return true;
        }

        constexpr void pop_move_up(const std::size_t old_size, const bool is_minimum, one_based_index_t &hole)
        {
            const auto &back1 = Data.back();

            for (;;)
            {
                assert(0U < hole.zero_based() && hole.zero_based() < old_size);

                const auto par = parent(hole);
                if (par.zero_based() == 0U)
                {
                    return;
                }

                auto &par_item = Data.at(par.zero_based());
                if (!is_better(back1, is_minimum, par_item))
                {
                    return;
                }

                auto &cur = Data.at(hole.zero_based());
                cur = std::move(par_item);
                hole = par;
            }
        }

        constexpr void pop(bool is_minimum)
        {
            const auto old_size = size();
            if (pop_small_size(old_size, is_minimum))
            {
                return;
            }

            auto hole = pop_move_hole_down(old_size, is_minimum);
            assert(0U < hole.zero_based() && hole.zero_based() < old_size);

            auto sym_ind = symmetric_index(hole);

            if (const auto must_go_to_parent = is_minimum && old_size <= sym_ind.zero_based(); must_go_to_parent)
            {
                sym_ind = parent(sym_ind);
            }

            assert(sym_ind.zero_based() <= old_size);

            if (pop_swap_corresponding(old_size, sym_ind, is_minimum, hole))
            {
                pop_move_up(old_size, is_minimum, hole);
            }

            auto &back1 = Data.back();
            auto &cur = Data.at(hole.zero_based());
            cur = std::move(back1);
            Data.pop_back();

            assert(!Data.empty());
        }

        static constexpr std::size_t empty_tree_size = 1; // todo(p2): use the _data[0] as left tree.
        static constexpr auto small_size = 3U;

        using data_t = std::vector<item_t>;
        data_t Data = data_t(empty_tree_size);
    };
} // namespace Standard::Algorithms::Heaps
