#pragma once
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Trees
{
    // Temporal, persistence, persistent data structure.
    //
    // Given an array of n zeros, answer m queries:
    // - Sum L R V : return sum[L..R] using an existing version V.
    // - Add L R A : add A to[L..R], creating a new version; demand A != 0.
    // Versions are 0 based..
    // Note. The queries could be processed offline.
    //
    // todo: p1. Del current limitation: L=R.
    // See also 'SegmentTree.h'.
    template<class int_t, class s_t = size_t>
    struct persistent_segment_tree final
    {
        // Assuming n=4, need 8 nodes (with 0 unused node):
        //        1        ;root
        //    2       3    ;inner nodes
        //  4   5   6   7  ;value nodes
        //
        // When n=5, need 10 nodes (with 0 node):
        //        1        ;root
        //    2       3    ;inner nodes
        //  4   5   6   7  ;inner 4, value 5,6,7
        // 8 9             ;value nodes

        explicit persistent_segment_tree(const s_t n)
            : n(RequirePositive(n, "n")),
            _version(0),
            data(RequirePositive(n << 1, "2n")),
            roots(1, 1)
        {
            assert(data.size() >= 2);

            for (s_t i = 1; i < this->n; ++i)
            {
                auto& d = data[i];
                d.left = i << 1, d.right = i << 1 | 1;
            }
        }

        s_t size() const noexcept
        {
            return n;
        }

        auto version() const noexcept
        {
            return _version;
        }

        void add(int left, const int right_incl, const int_t& a)
        {
            check_range(left, right_incl);

            assert(this->_version < std::numeric_limits<decltype(this->_version)>::max());

            if (!a)
                return;

            // todo: p2. end.
            ++this->_version;
        }

        int_t sum(int left, const int right_incl, const s_t version) const
        {
            check_range(left, right_incl);

            assert(version <= this->_version);
            RequireNotGreater(version, this->version(), "Version");

            int_t su{};

            return su;
        }

    private:
        struct node_t final
        {
            s_t left = {}, right = {};
            int_t value = {};
        };

        void check_range(const int left, const int right_incl) const
        {
            assert(left >= 0 && left <= right_incl && right_incl < static_cast<int>(size()));

            RequireNotGreater(0, left, "left");
            RequireGreater(size(), right_incl, "right_incl");
            RequireGreater(right_incl + 1, left, "right_incl+1");
        }

        s_t n;
        s_t _version;
        std::vector<node_t> data;
        std::vector<s_t> roots;
    };

    // They.
    template<class int_t>
    struct persistent_segment_tree_they final
    {
        // Assuming n=5, need 10 nodes (with 0 unused node):
        //              1[5010]
        //            /          \ _
        //       2[3003]          3[2007]
        //        /    \          /       \ _
        //     4[2001]  5v[1002] 8v[1003]  9v[1004]
        //     /    \ _
        // 6v[1000] 7v[1001]   ;value nodes: 6,7,5,8,9.

        explicit persistent_segment_tree_they(size_t n)
            : version_roots(1, 1),
            n(RequirePositive(n, "n"))
        {
            data.reserve(n << 1);
            data.resize(2); // 0-th value won't be used.
            build(1, 0, n - 1);
        }

        size_t size() const noexcept
        {
            return n;
        }

        auto version() const noexcept
        {
            return version_roots.size() - 1u;
        }

        void add(const size_t left, const size_t right_incl, const int_t& a)
        {
            check_range(left, right_incl);
            if (!a)
                return;

            const auto root_id0 = version_roots.back();
            const auto root_id1 = data.size();
            assert(root_id0 < root_id1);

            data.push_back({});
            version_roots.push_back(root_id1);

            add_impl(root_id0, root_id1, 0, n - 1, left, a);
        }

        int_t sum(size_t left, const size_t right_incl, const size_t version) const
        {
            check_range(left, right_incl);

            assert(version <= this->version());
            RequireNotGreater(version, this->version(), "Version");

            const auto& root_id = version_roots[version];
            const auto su = sum_impl(root_id, 0, n - 1, left, right_incl);
            return su;
        }

    private:
        void check_range(const size_t left, const size_t right_incl) const
        {
            assert(left == right_incl); // todo: del p2. left == right_incl

            assert(left >= 0 && left <= right_incl && right_incl < static_cast<int>(size()));

            RequireNotGreater(0, left, "left");
            RequireGreater(static_cast<int>(size()), right_incl, "right_incl");
            RequireGreater(right_incl + 1, left, "right_incl+1");
        }

        struct node_t final
        {
            size_t left = {}, right = {};
            int_t value = {};
        };

        std::vector<node_t> data;
        std::vector<size_t> version_roots;
        size_t n;

        // Time O(n)
        void build(const size_t node_id, const size_t low, const size_t high)
        {
            if (low == high)
            {
                //auto &node = data[node_id];
                //node.value = low + 1000;
                //node.value = source[low];
                return;
            }

            const auto id = data.size();
            data.push_back({});
            data.push_back({});

            {
                auto& node = data[node_id];
                node.left = id, node.right = id + 1;
            }

            const auto mid = (low + high) >> 1;
            build(id, low, mid);
            build(id + 1, mid + 1, high);

            //auto &node = data[node_id];
            //const auto &le = data[id], &ri = data[id + 1];
            //node.value = le.value + ri.value;
        }

        // Time, space O(log(n)).
        void add_impl(const size_t previous_version_id, const size_t cur_version_id, const size_t low, const size_t high,
            const size_t index, const int_t& value)
        {
            if (low > high || index < low || index > high)
                return;

            assert(cur_version_id > 0 && previous_version_id > 0);
            if (low == high)
            {
                const auto& prev = data[previous_version_id];
                auto& node = data[cur_version_id];
                node.value = prev.value + value;
                return;
            }

            const auto id = data.size();
            data.push_back({});

            {
                const auto& prev = data[previous_version_id];
                auto& cur = data[cur_version_id];

                const auto mid = (low + high) >> 1;
                if (index <= mid)
                {
                    cur.left = id;
                    cur.right = prev.right;

                    add_impl(prev.left, cur.left, low, mid, index, value);
                }
                else
                {
                    cur.left = prev.left;
                    cur.right = id;

                    add_impl(prev.right, cur.right, mid + 1, high, index, value);
                }
            }

            auto& cur = data[cur_version_id];
            assert(cur.left > 0 && cur.right > 0);

            const auto& le = data[cur.left], & ri = data[cur.right];
            cur.value = le.value + ri.value;
        }

        // Time, space O(log(n)).
        int_t sum_impl(const size_t node_id, const size_t low, const size_t high, const size_t left, const size_t right) const
        {
            if (low > high || left > high || right < low)
                return 0;

            assert(node_id > 0);
            const auto& node = data[node_id];

            if (left <= low && high <= right)
                return node.value;

            const auto mid = (low + high) >> 1;
            const int_t a = sum_impl(node.left, low, mid, left, right),
                b = sum_impl(node.right, mid + 1, high, left, right),
                c = a + b;
            return c;
        }
    };

    // Slow.
    template<class int_t>
    struct persistent_segment_tree_slow final
    {
        explicit persistent_segment_tree_slow(size_t n)
            : data(1, std::vector<int_t>(RequirePositive(n, "n")))
        {
        }

        size_t size() const noexcept
        {
            return data[0].size();
        }

        auto version() const noexcept
        {
            return data.size() - 1u;
        }

        void add(int left, const int right_incl, const int_t& a)
        {
            check_range(left, right_incl);
            if (!a)
                return;

            data.push_back(data.back());
            auto& ar = data.back();

            while (left <= right_incl)
                ar[left++] += a;
        }

        int_t sum(int left, const int right_incl, const size_t version) const
        {
            check_range(left, right_incl);

            assert(version <= this->version());
            RequireNotGreater(version, this->version(), "Version");

            const auto& ar = data[version];

            int_t su{};

            while (left <= right_incl)
                su += ar[left++];

            return su;
        }

    private:
        void check_range(const int left, const int right_incl) const
        {
            assert(left >= 0 && left <= right_incl && right_incl < static_cast<int>(size()));

            RequireNotGreater(0, left, "left");
            RequireGreater(static_cast<int>(size()), right_incl, "right_incl");
            RequireGreater(right_incl + 1, left, "right_incl+1");
        }

        std::vector<std::vector<int_t>> data;
    };
}