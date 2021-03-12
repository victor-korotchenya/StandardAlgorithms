#pragma once
#include"../Numbers/prefix_sum.h"
#include"../Utilities/is_debug.h"
#include"../Utilities/require_utilities.h"
#include"../Utilities/same_sign_leq_size.h"
#include<algorithm>
#include<cstddef>
#include<limits>
#include<vector>

namespace Standard::Algorithms::Trees
{
    namespace Inner
    {
        // When size=3, the sums will have 8 values with the index specified in ():
        // 0(0)     hit[0](2)           hit[1](4)           hit[2](6)
        //         /             \          /           \             /       \;
        //   miss[0](1)    miss[1](3)       miss[2](5)      miss[2](7)
        //
        // sum(1, 3) -> sums[7] - sums[2]
        // sum(1, 3) -> (hit[1] + hit[2])  +  (miss[1] + miss[2] + miss[3])
        template<class long_int_t, class hit_t, class miss_t>
        constexpr auto compute_hit_miss_sums(const std::size_t size, const hit_t &hit_probabilities,
            const miss_t &miss_probabilities) -> std::vector<long_int_t>
        {
            {
                using int_t1 = decltype(hit_probabilities[0]);
                using int_t2 = decltype(miss_probabilities[0]);

                static_assert(sizeof(int_t1) <= sizeof(long_int_t) && sizeof(int_t2) <= sizeof(long_int_t));
            }

            assert(0U < size);
            std::vector<long_int_t> sums((size + 1U) << 1U);

            sums.at(0) = long_int_t{};
            sums.at(1) = miss_probabilities[0];

            for (std::size_t index{}; index < size; ++index)
            {
                const auto index_2 = index << 1U;
                const auto &pre = sums.at(index_2 | 1U);
                auto &cur = sums.at(index_2 + 2U);
                cur = pre + hit_probabilities[index];

                auto &nex = sums.at(index_2 + 3U);
                nex = cur + miss_probabilities[index + 1U];
            }

            return sums;
        }

        template<class long_int_t, class hit_t, class miss_t>
        constexpr void init_roots_bufs(const std::size_t size, const hit_t &hit_probabilities,
            const miss_t &miss_probabilities, std::vector<std::vector<long_int_t>> &range_min_costs,
            const long_int_t &inf, std::vector<std::vector<std::size_t>> &roots,
            const std::size_t npos = 0U - static_cast<std::size_t>(1))
        {
            assert(0U < size);

            range_min_costs.assign(size + 1U, std::vector<long_int_t>(size + 1U, inf));

            constexpr auto needed_for_slow = 1U;

            roots.assign(size + needed_for_slow, std::vector<std::size_t>(size + needed_for_slow, npos));

            constexpr long_int_t depth = 2;

            for (std::size_t index{}; index < size; ++index)
            {
                roots[index][index] = index;
                range_min_costs[index][index] = miss_probabilities[index];

                //         hit[index]
                //       /        \;
                // miss[index]    miss[index + 1U]
                range_min_costs[index][index + 1U] = static_cast<long_int_t>(hit_probabilities[index]) +
                    depth *
                        (static_cast<long_int_t>(miss_probabilities[index]) +
                            static_cast<long_int_t>(miss_probabilities[index + 1U]));
            }

            roots[size][size] = size;
            range_min_costs[size][size] = miss_probabilities[size];
        }
    } // namespace Inner

    template<class key_t>
    struct optimal_bst_node final
    {
        key_t key{};
        std::size_t index{};
        std::size_t depth{};
        std::size_t left{};
        std::size_t right{};
    };

    // Given n>0 sorted and distinct keys, each having a non-negative hit cost (or weight),
    // and (n + 1) miss costs between the keys including -infinity and +infinity,
    // build an OBST, Optimal BST (binary search tree) with minimal total cost TC.
    // Each key with cost c>0 has depth d>0, and contributes (c*d) to TC.
    // The root node depth is 1.
    // It might work incorrectly because of float/double rounding errors? Solution: use gold old integers.
    // Time O(n*n).
    template<class int_t, class long_int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    constexpr auto optimal_bst(const std::vector<int_t> &hit_probabilities, const long_int_t &inf,
        const std::vector<int_t> &miss_probabilities, std::vector<optimal_bst_node<int_t>> &nodes)
        -> std::pair<long_int_t, optimal_bst_node<int_t>> // total cost, root node.
    {
        const auto size = require_positive(hit_probabilities.size(), "hit probabilities size");
        require_positive(inf, "inf");

        if (const auto misi = miss_probabilities.size(); size + 1ZU != misi) [[unlikely]]
        {
            throw std::invalid_argument("The hit probabilities size (" + std::to_string(size) +
                ") + 1 must be miss probabilities size (" + std::to_string(misi) + ").");
        }

        if constexpr (std::is_signed_v<int_t>)
        {
            require_all_non_negative(hit_probabilities, "hit probabilities");
            require_all_non_negative(miss_probabilities, "miss probabilities");
        }

        nodes.clear();

        // todo(p3): fill nodes.
        nodes.reserve(size + 2U);
        nodes.push_back({}); // zero pointer.

        const auto sums = Inner::compute_hit_miss_sums<long_int_t>(size, hit_probabilities, miss_probabilities);

        assert(4U <= sums.size() && !(sums.back() < long_int_t{}) && sums.back() < inf);

        std::vector<std::vector<long_int_t>> range_min_costs;
        std::vector<std::vector<std::size_t>> roots;

        Inner::init_roots_bufs<long_int_t>(
            size, hit_probabilities, miss_probabilities, range_min_costs, long_int_t{}, roots, 0U);

        [[maybe_unused]] std::uint64_t steps{}; // To check performance.

        for (std::size_t length = 2U; length <= size; ++length)
        {
            for (std::size_t from{}; from <= size - length; ++from)
            {
                const auto tod = from + length;

                // Total cost = left subtree cost + right subtree cost + subtree cost
                // where, of course, each subtree must be optimal,
                // and the subtree cost is a constant here.
                const auto subtree_cost = ::Standard::Algorithms::Numbers::calculate_substring_sum<long_int_t>(
                    from << 1U, sums, (tod << 1U) | 1U);

                auto &best_cost = range_min_costs[from][tod];
                best_cost = inf;

                // After the computation, due to Professor Knuth optimization, monotonicity:
                // roots[from][tod - 1] <= roots[from][tod]
                // roots[from][tod] <= roots[from + 1][tod]

                const auto to_inclusive = tod - 1U;

                //     |   max_root |;
                // |    root        |;
                // |    best_root    |;
                const auto &max_root = roots[from + 1U][to_inclusive];
                auto root = roots[from][to_inclusive - 1U];

                assert(!(subtree_cost < long_int_t{}) && subtree_cost < inf && max_root < size && root <= max_root);

                auto &best_root = roots[from][to_inclusive];

                do
                {
                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        ++steps;
                    }

                    const auto &left = range_min_costs[from][root];
                    const auto &right = range_min_costs[root + 1U][tod];
                    const long_int_t cand = subtree_cost + left + right;

                    assert(!(left < long_int_t{}) && !(right < long_int_t{}) &&
                        !(cand < std::max({ subtree_cost, left, right })) && cand < inf);

                    if (cand < best_cost)
                    {
                        best_cost = cand;
                        best_root = root;
                    }
                } while (++root <= max_root);
            }
        }

        const auto &best_total_cost = range_min_costs.at(0).back();

        assert(!(best_total_cost < long_int_t{}) && best_total_cost < inf);

        return std::make_pair(best_total_cost, nodes.at(0));
    }

    // Slow time O(n*n*n).
    template<class int_t, class long_int_t>
    requires(same_sign_leq_size<int_t, long_int_t>)
    constexpr auto optimal_bst_slow(const std::vector<int_t> &hit_probabilities, const long_int_t &inf,
        const std::vector<int_t> &miss_probabilities, std::vector<optimal_bst_node<int_t>> &nodes)
        -> std::pair<long_int_t, optimal_bst_node<int_t>> // total cost, root node.
    {
        const auto size = require_positive(hit_probabilities.size(), "hit probabilities size");
        require_positive(inf, "inf");

        if (const auto misi = miss_probabilities.size(); size + 1U != misi) [[unlikely]]
        {
            throw std::invalid_argument("The hit probabilities size (" + std::to_string(size) +
                ") + 1 must be miss probabilities size (" + std::to_string(misi) + ").");
        }

        if constexpr (std::is_signed_v<int_t>)
        {
            require_all_non_negative(hit_probabilities, "hit probabilities");
            require_all_non_negative(miss_probabilities, "miss probabilities");
        }

        nodes.clear();

        // todo(p3): fill nodes.
        nodes.reserve(size + 2U);
        nodes.push_back({}); // zero pointer.

        const auto sums = Inner::compute_hit_miss_sums<long_int_t>(size, hit_probabilities, miss_probabilities);

        assert(4U <= sums.size() && !(sums.back() < long_int_t{}) && sums.back() < inf);

        std::vector<std::vector<long_int_t>> range_min_costs;
        std::vector<std::vector<std::size_t>> roots;

        Inner::init_roots_bufs<long_int_t>(size, hit_probabilities, miss_probabilities, range_min_costs, inf, roots);

        [[maybe_unused]] std::uint64_t steps{}; // To check performance.

        // Bottom to top might perform unneeded computations.
        for (std::size_t length = 2U; length <= size; ++length)
        {
            for (std::size_t from{}; from <= size - length; ++from)
            {
                const auto tod = from + length;
                auto &min_cost = range_min_costs[from][tod];
                auto &best_root = roots[from][tod];

                for (auto mid = from; mid < tod; ++mid)
                {
                    if constexpr (::Standard::Algorithms::is_debug)
                    {
                        ++steps;
                    }

                    const auto &left_cost = range_min_costs[from][mid];
                    const auto &right_cost = range_min_costs[mid + 1U][tod];
                    const long_int_t cand = left_cost + right_cost;

                    assert(!(left_cost < long_int_t{}) && !(right_cost < long_int_t{}) &&
                        !(cand < std::max({ long_int_t{}, left_cost, right_cost })) &&
                        std::max({ left_cost, right_cost, cand }) < inf);

                    if (cand < min_cost)
                    {
                        min_cost = cand;
                        best_root = mid;
                    }
                }

                const auto subtree_cost = ::Standard::Algorithms::Numbers::calculate_substring_sum<long_int_t>(
                    from << 1U, sums, (tod << 1U) | 1U);

                const long_int_t cur = subtree_cost + min_cost;

                assert(!(min_cost < long_int_t{}) && !(subtree_cost < long_int_t{}) &&
                    !(cur < std::max(min_cost, subtree_cost)) && std::max({ subtree_cost, min_cost, cur }) < inf);

                min_cost = cur;
            }
        }

        const auto &best_total_cost = range_min_costs.at(0).back();

        assert(!(best_total_cost < long_int_t{}) && best_total_cost < inf);

        return std::make_pair(best_total_cost, nodes.at(0));
    }
} // namespace Standard::Algorithms::Trees
