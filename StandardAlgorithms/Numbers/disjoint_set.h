#pragma once
#include <algorithm>
#include <numeric>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

struct DisjointSetException final : std::exception
{
    explicit DisjointSetException(const char* const& message)
        : std::exception(message)
    {
    }
};

// Disjoint set union-find structure.
template <class int_t>
class disjoint_set final
{
    std::vector<int_t> parents, sizes;
    int_t disjoint_set_count;

public:
    explicit disjoint_set(const int_t size)
        : parents(RequirePositive(size, "size")), sizes(size, 1), disjoint_set_count(size)
    {
        std::iota(parents.begin(), parents.end(), int_t());
    }

    int_t get_parent(int_t node)
    {
        check_index(node);

        // It should be faster on average then
        // setting the root for all predecessors.
        while (parents[node] != node)
            node = parents[node] = parents[parents[node]];

        return node;
    }

    // Some children 'sizes' might get broken - not a big deal.
    void set_parent(const int_t node)
    {
        check_index(node);

        const auto root = get_parent(node);
        if (root == node)
            return;

        assert(sizes[node] < sizes[root]);
        std::swap(sizes[node], sizes[root]);

        parents[root] = node;
        parents[node] = node;
    }

    bool unite(int_t a, int_t b)
    {
        a = get_parent(a);
        b = get_parent(b);
        if (a == b)
            return false;

        if (sizes[a] < sizes[b])
            std::swap(a, b);

        parents[b] = a;
        sizes[a] += sizes[b];

        --disjoint_set_count;
        return true;
    }

    const int_t& count() const noexcept
    {
        return disjoint_set_count;
    }

    bool are_connected(int_t a, int_t b)
    {
        a = get_parent(a);
        b = get_parent(b);
        const auto result = a == b;
        return result;
    }

private:
    void check_index(const int_t index) const
    {
        if (!(0 <= index && index < static_cast<int_t>(sizes.size())))
        {
            const auto err = "disjoint_set bad index " + std::to_string(index) + ".";
            throw DisjointSetException(err.c_str());
        }
    }
};