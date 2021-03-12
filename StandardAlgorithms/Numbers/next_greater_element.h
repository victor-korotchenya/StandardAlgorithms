#pragma once
#include"../Utilities/check_size.h"
#include<stack>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // For each array item, find the next greater, or -1 if none.
    // Time O(n).
    template<class item_t, std::signed_integral size_t1>
    constexpr void next_greater_element(const std::vector<item_t> &arr, std::vector<size_t1> &result)
    {
        result.resize(arr.size());

        if (arr.empty())
        {
            return;
        }

        const auto size = Standard::Algorithms::Utilities::check_size<size_t1>("source size", arr.size());

        std::stack<size_t1> sta;

        for (size_t1 index{}; index < size; ++index)
        {
            while (!sta.empty() && arr[sta.top()] < arr[index])
            {
                const auto &top = sta.top();
                assert(size_t1{} <= top && top < size);

                result[top] = index;
                sta.pop();
            }

            sta.push(index);
        }

        while (!sta.empty())
        {
            constexpr size_t1 no_next = -static_cast<size_t1>(1);
            static_assert(no_next < size_t1{});

            const auto &top = sta.top();
            assert(size_t1{} <= top && top < size);

            result[top] = no_next;
            sta.pop();
        }
    }

    // Slow time O(n*n).
    template<class item_t, std::signed_integral size_t1>
    constexpr void next_greater_element_slow(const std::vector<item_t> &arr, std::vector<size_t1> &result)
    {
        result.resize(arr.size());

        if (arr.empty())
        {
            return;
        }

        const auto size = Standard::Algorithms::Utilities::check_size<size_t1>("source size", arr.size());

        for (size_t1 index{}; index < size; ++index)
        {
            constexpr size_t1 no_next = -static_cast<size_t1>(1);
            static_assert(no_next < size_t1{});

            const auto &pre = arr[index];
            auto match = no_next;

            for (auto ind_2 = index + 1; ind_2 < size; ++ind_2)
            {
                if (pre < arr[ind_2])
                {
                    match = ind_2;
                    break;
                }
            }

            result[index] = match;
        }
    }
} // namespace Standard::Algorithms::Numbers
