#pragma once
#include"../Utilities/require_utilities.h"
#include<numeric> // std::midpoint
#include<vector>

namespace Standard::Algorithms::Trees::Inner
{
    inline constexpr void pers_check_range(
        const std::size_t size, const std::pair<std::size_t, std::size_t> &left_right_incl)
    {
        const auto &left = left_right_incl.first;
        const auto &right_incl = left_right_incl.second;

        require_greater(size, right_incl, "right_incl");
        require_greater(right_incl + 1U, left, "right_incl+1");

        assert(left <= right_incl && right_incl < size);
    }
} // namespace Standard::Algorithms::Trees::Inner

namespace Standard::Algorithms::Trees
{
    struct sum_query final
    {
        std::size_t version{};
        std::size_t left{};
        std::size_t right_incl{};
    };

    // Temporal, persistence, persistent data structure.
    //
    // Given an array of n zeros, answer m queries:
    // - Sum L R V : return sum[L..R] using an existing version V.
    // - Add L R A : add A to[L..R], creating a new version; demand A != 0.
    // Versions are 0 based.
    // Nodes are 1 based.
    // Limitation: the size is constant here.
    // The queries could be processed offline.
    //
    // See also "segment_tree.h".

    /* // todo(p3): finish.
    template<class int_t, std::unsigned_integral siz_t = std::size_t>
    struct persistent_segment_tree final
    {
        // Assuming size=4, we need 8 nodes (with 0 unused node):
        //        1        ;root
        //    2       3    ;inner nodes
        //  4   5   6   7  ;value nodes
        //
        // When size=5, 10 nodes (with 0 node) are needed:
        //        1        ;root
        //    2       3    ;inner nodes
        //  4   5   6   7  ;inner 4, value 5,6,7
        // 8 9             ;value nodes
        constexpr explicit persistent_segment_tree(const siz_t size)
        : //Size(size)),
            Data(require_positive(size << 1U, "tree size"))
        {
            // todo(p3): use 0th position memory.

            // todo(p3): check (size << 1U, size, "tree size * 2")),


            assert(1U < Data.size());

            for (siz_t index = 1; index < Data.size(); ++index)
            {
                auto& datum = Data[index];
                datum.left = index << 1U;
                datum.right = (index << 1U) | 1;
            }
        }

        //[[nodiscard]] constexpr auto size() const& noexcept -> const siz_t& { return Size; }
        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t { return Data.size(); }

        [[nodiscard]] constexpr auto max_version() const& noexcept -> const siz_t& { return Max_version; }

        constexpr void add(std::size_t left, const int_t& val, const std::size_t right_incl)
        {
            Inner::pers_check_range(size(), {left, right_incl});

            if (val == int_t{})
            {
                return;
            }

            constexpr auto maxv = std::numeric_limits<siz_t>::max();
            require_greater(maxv, Max_version, "version");

            ++Max_version;
        }

        [[nodiscard]] constexpr auto sum(std::size_t left, const std::size_t right_incl, const siz_t version) const ->
int_t
        {
            Inner::pers_check_range(size(), {left, right_incl});

            require_less_equal(version, Max_version, "version");

            int_t acc{};

            return acc;
        }

private:
        struct node_t final
        {
            int_t value{};
            siz_t left{};
            siz_t right{};
        };

        // siz_t Size;
        siz_t Max_version{};
        std::vector<node_t> Data{};
        //std::vector<siz_t> roots(1, 1);
    }; */

    template<class int_t>
    struct persistent_segment_tree_other final
    {
        // Assuming n=5, need 10 nodes (with 0 unused node):
        //              1[5010]
        //            /          \;
        //       2[3003]          3[2007]
        //        /    \          /       \;
        //     4[2001]  5v[1002] 8v[1003]  9v[1004]
        //     /    \;
        // 6v[1000] 7v[1001]  ; value nodes: 6,7,5,8,9.
        constexpr explicit persistent_segment_tree_other(std::size_t size)
            : version_roots(1, 1)
            , Size(require_positive(size, "tree size"))
        {
            {
                constexpr auto maxim = std::numeric_limits<std::size_t>::max() / 2 - 1U;

                require_greater(maxim, size, "tree size");
            }

            Data.reserve(Size << 1U);
            Data.resize(2); // 0-th value won't be used.

            build_tree({ 1U, 0U, Size - 1U });
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Size;
        }

        [[nodiscard]] constexpr auto max_version() const noexcept -> std::size_t
        {
            return version_roots.size() - 1U;
        }

        constexpr void add(const std::size_t left, const int_t &val, const std::size_t right_incl)
        {
            check_range_todo_del(left, right_incl);

            if (int_t{} == val)
            {
                return;
            }

            const auto root_id0 = version_roots.back();
            const auto root_id1 = Data.size();
            assert(root_id0 < root_id1);

            Data.push_back({});
            // todo(p3): make more reliable - strong guarantee.
            version_roots.push_back(root_id1);

            // todo(p2): Del the limitation: left === right_incl, and use right_incl!
            add_impl({ root_id0, root_id1 }, { left, val }, { 0U, Size - 1U });
        }

        [[nodiscard]] constexpr auto sum(const sum_query &query) const -> int_t
        {
            check_range_todo_del(query.left, query.right_incl);

            require_less_equal(query.version, max_version(), "version");

            const auto &root_id = version_roots[query.version];

            const auto acc = sum_impl({ 0U, Size - 1U }, root_id, { query.left, query.right_incl });

            return acc;
        }

private:
        constexpr void check_range_todo_del(const std::size_t left, const std::size_t right_incl) const
        {
            assert(left == right_incl); // todo(p2): del left == right_incl

            Inner::pers_check_range(size(), { left, right_incl });
        }

        struct node_t final
        {
            int_t value{};
            std::size_t left{};
            std::size_t right{};
        };

        // Time O(n)
        constexpr void build_tree(const sum_query &query)
        {
            const auto &node_id = query.version;
            const auto &low = query.left;
            const auto &high = query.right_incl;

            if (low == high)
            {
                // auto &node = Data[node_id];
                // node.value = source[low];
                return;
            }

            // [[assume(low < high)]];
            assert(low < high);

            const auto ide = Data.size();
            assert(node_id < ide);

            Data.push_back({});
            Data.push_back({});

            {
                auto &node = Data.at(node_id);
                node.left = ide;
                node.right = ide + 1U;
            }

            const auto mid = std::midpoint(low, high);
            build_tree({ ide, low, mid });
            build_tree({ ide + 1U, mid + 1U, high });

            // auto &node = Data.at(node_id);
            // node.value = Data.at(ide).value + Data.at(ide + 1U).value;
        }

        // Time, space O(log(n)).
        constexpr void add_impl(const std::pair<std::size_t, std::size_t> &prev_cur_vers,
            const std::pair<std::size_t, int_t> &index_val, const std::pair<std::size_t, std::size_t> &low_high)
        {
            const auto &index = index_val.first;

            const auto &low = low_high.first;
            const auto &high = low_high.second;

            if (high < low || index < low || high < index)
            {
                return;
            }

            const auto &previous_version_id = prev_cur_vers.first;
            assert(0U < previous_version_id); // todo(p3): del?

            const auto &cur_version_id = prev_cur_vers.second;
            assert(previous_version_id < cur_version_id);

            const auto &val = index_val.second;

            if (low == high)
            {
                const auto &prev = Data.at(previous_version_id);

                auto &cur = Data.at(cur_version_id);
                cur.value = static_cast<int_t>(prev.value + val);

                return;
            }

            const auto ide = Data.size();
            Data.push_back({});

            {
                const auto &prev = Data.at(previous_version_id);
                auto &cur = Data.at(cur_version_id);

                const auto mid = std::midpoint(low, high);

                if (index <= mid)
                {
                    cur.left = ide;
                    cur.right = prev.right;

                    add_impl({ prev.left, cur.left }, index_val, { low, mid });
                }
                else
                {
                    cur.left = prev.left;
                    cur.right = ide;

                    add_impl({ prev.right, cur.right }, index_val, { mid + 1U, high });
                }
            }

            auto &cur = Data.at(cur_version_id);
            assert(0U < cur.left && 0U < cur.right);

            const auto &left = Data.at(cur.left);
            const auto &right = Data.at(cur.right);
            cur.value = static_cast<int_t>(left.value + right.value);
        }

        // Time, space O(log(n)).
        [[nodiscard]] constexpr auto sum_impl(const std::pair<std::size_t, std::size_t> &low_high,
            const std::size_t node_id, const std::pair<std::size_t, std::size_t> &left_right) const -> int_t
        {
            const auto &low = low_high.first;
            const auto &high = low_high.second;

            const auto &left = left_right.first;
            const auto &right = left_right.second;

            if (high < low || high < left || right < low)
            {
                return {};
            }

            assert(0U < node_id);

            const auto &node = Data.at(node_id);

            if (left <= low && high <= right)
            {
                return node.value;
            }

            const auto mid = std::midpoint(low, high);

            const auto one = sum_impl({ low, mid }, node.left, left_right);
            const auto two = sum_impl({ mid + 1U, high }, node.right, left_right);
            const auto acc = static_cast<int_t>(one + two);
            return acc;
        }

        std::vector<node_t> Data{};
        std::vector<std::size_t> version_roots;
        std::size_t Size;
    };

    // Slow - copy all from the previous version.
    template<class int_t>
    struct persistent_segment_tree_slow final
    {
        constexpr explicit persistent_segment_tree_slow(std::size_t size)
            : Data(1, std::vector<int_t>(require_positive(size, "tree size"), int_t{}))
        {
        }

        [[nodiscard]] constexpr auto size( // todo(p3): add an argument 'version'?
        ) const noexcept -> std::size_t
        {
            return Data[0].size();
        }

        [[nodiscard]] constexpr auto max_version() const noexcept -> std::size_t
        {
            return Data.size() - 1U;
        }

        constexpr void add(std::size_t left, const int_t &val, const std::size_t right_incl)
        {
            Inner::pers_check_range(size(), { left, right_incl });

            if (int_t{} == val)
            {
                return;
            }

            Data.reserve(Data.size() + 1U);
            Data.push_back(Data.back());

            auto &arr = Data.back();

            while (left <= right_incl)
            {
                arr[left++] += val;
            }
        }

        [[nodiscard]] constexpr auto sum(const sum_query &query) const -> int_t
        {
            const auto &version = query.version;
            const auto &right_incl = query.right_incl;
            auto left = query.left;

            Inner::pers_check_range(size(), { left, right_incl });

            require_less_equal(version, max_version(), "version");

            const auto &arr = Data.at(version);

            int_t acc{};

            while (left <= right_incl)
            {
                acc += arr[left++];
            }

            return acc;
        }

private:
        std::vector<std::vector<int_t>> Data;
    };
} // namespace Standard::Algorithms::Trees
