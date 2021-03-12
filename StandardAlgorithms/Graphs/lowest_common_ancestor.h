#pragma once
#include"../Numbers/to_unsigned.h"
#include"../Utilities/require_utilities.h"
#include"tree_utilities.h"
#include<vector>

namespace Standard::Algorithms::Trees
{
    template<class node_t>
    [[nodiscard]] constexpr auto node_to(const node_t &node) -> std::int32_t
    {
        return static_cast<std::int32_t>(node);
    }

    // Lowest common ancestor LCA for a tree in O(n * log(n)).
    // Nodes start from 1.
    // Use sample:
    //
    // auto size = 32;
    // lowest_common_ancestor<std::int32_t> lca2(node_to<std::int32_t>);
    // auto &tree = lca2.reset(size);
    template<class edge_t, class get_to_t // = std::int32_t(*)(const edge_t&)
        >
    struct lowest_common_ancestor
    {
        constexpr explicit lowest_common_ancestor(get_to_t get_tod)
            : Get_tod(get_tod)
        {
        }

        constexpr lowest_common_ancestor(const lowest_common_ancestor &) = default;
        constexpr auto operator= (const lowest_common_ancestor &) & -> lowest_common_ancestor & = default;
        constexpr lowest_common_ancestor(lowest_common_ancestor &&) noexcept = default;
        constexpr auto operator= (lowest_common_ancestor &&) &noexcept -> lowest_common_ancestor & = default;

        constexpr virtual ~lowest_common_ancestor() noexcept = default;

        [[nodiscard]] constexpr auto reset(const std::int32_t new_size) & -> std::vector<std::vector<edge_t>> &
        {
            require_greater(new_size, 1, "new size");

            Tree.clear();
            Tree.resize(new_size + 1LL);
            return Tree;
        }

        constexpr virtual void dfs(const std::int32_t one_based_root)
        {
            assert(0 < one_based_root && one_based_root <= n());

            const auto size = n() + 1LL;
            Firsts.resize(size);
            Lasts.resize(size);
            Depths.resize(size);

            Parents.clear();
            Parents.resize(size);

            Tour.clear();
            Tour.reserve(size << 1U);

            Time = 0;
            dfs1(one_based_root);
        }

        [[nodiscard]] constexpr auto lca(std::int32_t one, std::int32_t two) const noexcept(false) -> std::int32_t
        {
            assert(0 < one && 0 < two);

            if (Depths.at(two) < Depths.at(one))
            {
                std::swap(one, two);
            }

            equal_depth(one, two);

            if (one == two)
            {
                return two;
            }

            move_up(one, two);

            return Parents.at(two).at(0);
        }

        // todo(p3): test is_parent_child - not used.
        [[nodiscard]] constexpr auto are_parent_child(
            const std::int32_t parent, const std::int32_t child) const noexcept -> bool
        {
            assert(!(parent < 0) && parent <= n() && !(child < 0) && child <= n() && parent != child);

            const auto are = Firsts[parent] <= Firsts[child] && Lasts[child] <= Lasts[parent];
            return are;
        }

        // todo(p3): test lca_distance - not used.
        [[nodiscard]] constexpr auto lca_distance(const std::int32_t one, const std::int32_t two) const noexcept(false)
            -> std::int32_t
        {
            assert(!(one < 0) && one <= n() && !(two < 0) && two <= n());

            const auto common = lca(one, two);
            const auto dist = Depths[one] + Depths[two] - 2 * Depths[common];
            assert(!(dist < 0));

            return dist;
        }

        // todo(p3): test is_path - not used.
        // start <- mid <- stop
        // start -> mid -> stop
        [[nodiscard]] constexpr auto is_path(
            const std::int32_t start, const std::int32_t mid, const std::int32_t stop) const noexcept(false) -> bool
        {
            const auto a_c = lca_distance(start, stop);
            const auto a_b = lca_distance(start, mid);
            const auto b_c = lca_distance(mid, stop);

            const auto isp = a_c == a_b + b_c;
            return isp;
        }

        // todo(p3): test first_meet_point - not used.
        // Return the first meet point of 2 paths: (s..a), (w..a).
        [[nodiscard]] constexpr auto first_meet_point(std::int32_t sss, std::int32_t www, const std::int32_t aaa) const
            noexcept(false) -> std::int32_t
        {
            assert(sss != www || www == aaa);

            if (Depths[www] < Depths[sss])
            {
                std::swap(www, sss);
            }

            // s <- a <- w
            if (are_parent_child(sss, aaa) && are_parent_child(aaa, www))
            {
                return aaa;
            }

            //     sw - a
            //     ^^
            //   /  |
            // s    |
            //      w
            if (const auto s_w = lca(sss, www); is_path(sss, s_w, aaa) && is_path(www, s_w, aaa))
            {
                return s_w;
            }

            // 1 - s
            // |;
            // aw
            // ^^
            // | \;
            // a  w
            //
            // s <- w <- a
            if (const auto a_w = lca(aaa, www); is_path(sss, a_w, aaa))
            {
                return a_w;
            }

            // 1 - w
            //  \;
            //  as
            //   ^^
            //   | \;
            //  a   s
            if (const auto a_s = lca(aaa, sss); is_path(www, a_s, aaa)) [[likely]]
            {
                return a_s;
            }

            throw std::runtime_error("Error in first_meet_point(" + std::to_string(sss) + ", " + std::to_string(www) +
                ", " + std::to_string(aaa) + ") must not occur.");
        }

        // todo(p3): test count_shared_nodes - not used.
        [[nodiscard]] constexpr auto count_shared_nodes(const std::int32_t sss, const std::int32_t www,
            std::int32_t const aaa) const noexcept(false) -> std::int32_t
        {
            const auto meet_point = first_meet_point(sss, www, aaa);
            const auto dist = lca_distance(meet_point, aaa) + 1;
            return dist;
        }

        [[nodiscard]] constexpr auto parents() const &noexcept -> const std::vector<std::vector<std::int32_t>> &
        {
            return Parents;
        }

        [[nodiscard]] constexpr auto depths() const &noexcept -> const std::vector<std::int32_t> &
        {
            return Depths;
        }

        [[nodiscard]] constexpr auto get_to() const &noexcept -> const get_to_t &
        {
            return Get_tod;
        }

        [[nodiscard]] constexpr auto tree() const &noexcept -> const std::vector<std::vector<edge_t>> &
        {
            return Tree;
        }

        [[nodiscard]] constexpr auto tour() &noexcept -> std::vector<std::int32_t> &
        {
            return Tour;
        }

protected:
        [[nodiscard]] constexpr auto n() const noexcept -> std::int32_t
        {
            assert(0U < Tree.size());

            const auto siz = static_cast<std::int32_t>(Tree.size()) - 1;
            return siz;
        }

        constexpr virtual void visit_child(std::int32_t current, std::int32_t child)
        {
            // NOLINTNEXTLINE NOLINT
            if (current == child) // Pacify the compiler.
            {
            }
        }

private:
        constexpr void equal_depth(const std::int32_t one, std::int32_t &two) const
        {
            auto diff = Depths[two] - Depths[one];
            assert(!Parents[two].empty() && !(diff < 0));

            while (0 < diff)
            {
                const auto uns_dif = static_cast<std::uint32_t>(diff);
                const auto bit = ::Standard::Algorithms::Numbers::to_unsigned(__builtin_ctz(uns_dif));

                const auto ind = 1U << bit;
                assert((ind & uns_dif) != 0U && bit < Parents[two].size());

                two = Parents[two][bit];
                diff = ::Standard::Algorithms::Numbers::to_signed(uns_dif ^ ind);
            }

            assert(!Parents[two].empty() && Depths[one] == Depths[two]);
        }

        constexpr void move_up(std::int32_t one, std::int32_t &two) const
        {
            auto ind = static_cast<std::int32_t>(Parents[two].size()) - 1;

            do
            {
                assert(!Parents[one].empty() && Parents[one].size() == Parents[two].size());

                ind = std::min(ind, static_cast<std::int32_t>(Parents[one].size()) - 1);

                const auto &par1 = Parents[one][ind];
                const auto &par2 = Parents[two][ind];
                if (par1 != par2)
                {
                    one = par1, two = par2;
                }

                assert(!Parents[one].empty() && Parents[one].size() == Parents[two].size());
            } while (!(--ind < 0));
        }

        constexpr void parents(const std::int32_t cur, std::int32_t prev)
        {
            assert(cur != prev && 0 < cur && !(prev < 0));

            auto &prs = Parents[cur];
            prs.push_back(prev);

            for (std::size_t index{};; ++index)
            {
                const auto &ps2 = Parents[prev];

                if (!(index < ps2.size()))
                {
                    break;
                }

                prs.push_back(prev = ps2[index]);

                assert(cur != prev && !(prev < 0));
            }
        }

        constexpr void dfs1(const std::int32_t cur, const std::int32_t prev = 0)
        {
            assert(0 < cur && cur <= n() && !(prev < 0) && prev <= n() && cur != prev);

            Tour.push_back(cur);
            Firsts[cur] = ++Time;
            Depths[cur] = Depths[prev] + 1;

            parents(cur, prev);

            for (const auto &chis = Tree[cur]; const auto &chi : chis)
            {
                const auto tod = Get_tod(chi);

                if (tod == prev)
                {
                    continue;
                }

                dfs1(tod, cur);
                Tour.push_back(cur);
                visit_child(cur, tod);
            }

            Lasts[cur] = ++Time;
        }

        get_to_t Get_tod;
        std::vector<std::vector<edge_t>> Tree{};

        // Sparse table for log(n) retrieval.
        std::vector<std::vector<std::int32_t>> Parents{};
        std::vector<std::int32_t> Firsts{};
        std::vector<std::int32_t> Lasts{};
        std::vector<std::int32_t> Depths{};

        std::int32_t Time{};
        std::vector<std::int32_t> Tour{};
    };
} // namespace Standard::Algorithms::Trees
