#pragma once
#include <algorithm>
#include <climits>
#include <cassert>

namespace Privet::Algorithms::Trees
{
    template<class value_t>
    struct avl_node_t final
    {
        avl_node_t* left, * right;
        value_t key;
        int32_t count;
        int8_t height;

        explicit avl_node_t(const value_t& key = {}) : left(nullptr), right(nullptr), key(key), count(1), height(1)
        {
        }

        void update_height()
        {
            height = static_cast<int8_t>(std::max(left ? left->height : 0, right ? right->height : 0) + 1);
            assert(height >= 0 && height < std::numeric_limits<int8_t>::max());

            count = (left ? left->count : 0) + (right ? right->count : 0) + 1;
            assert(count >= 0 && count < std::numeric_limits<int32_t>::max());
        }

        int get_balance() const
        {
            const auto b = (right ? right->height : 0) - (left ? left->height : 0);
            assert(b >= -2 && b <= 2);
            return b;
        }

        //bool operator <(const avl_node_t &b) const
        //{
        //    const auto result = key < b.key;
        //    return result;
        //}
    };
}