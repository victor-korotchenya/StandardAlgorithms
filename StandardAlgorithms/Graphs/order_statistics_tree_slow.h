#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>
#include <stdexcept>

namespace Privet::Algorithms::Trees
{
    template <typename t, typename less_t = std::less<t>>
    class order_statistics_tree_slow final
    {
        std::vector<t> data;
        less_t less;

        void sort()
        {
            std::sort(data.begin(), data.end(), less);
        }

        bool eq(const t& a, const t& b) const
        {
            const auto r = !less(a, b) && !less(b, a);
            return r;
        }

    public:
        explicit order_statistics_tree_slow(less_t comparer = {})
            : less(comparer)
        {
        }

        int size() const noexcept
        {
            return static_cast<int>(data.size());
        }

        // todo: return rank?
        bool insert(const t& v)
        {
            for (const auto& it : data)
            {
                if (eq(v, it))
                    return false;
            }

            data.push_back(v);
            return true;
        }

        bool erase(const t& v)
        {
            const auto s = size();
            for (auto i = 0; i < s; ++i)
            {
                if (eq(v, data[i]))
                {
                    data.erase(data.begin() + 1);
                    return true;
                }
            }

            return false;
        }

        // todo: del it for 'rank()'?
        bool find(const t& v) const
        {
            for (const auto& it : data)
            {
                if (eq(v, it))
                    return true;
            }

            return false;
        }

        const t& find_k_smallest(int rank)
        {
            if (rank < 0 || rank >= size())
                throw std::runtime_error("invalid rank");

            sort();
            return data[rank];
        }

        int rank(const t& v)
        {
            sort();

            const auto s = size();
            for (auto i = 0; i < s; ++i)
            {
                if (eq(v, data[i]))
                    return i;
            }
            return -1;
        }
    };
}