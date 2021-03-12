#pragma once
// "number_splitter.h"
#include<cstddef>
#include<cstdint>
#include<functional>
#include<iterator> // std::make_move_iterator
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given an array:
    // {1, 5, -7, 10, 0, -2, -8, 0, 4, 0},
    // it must become:
    // {-7, -2, -8, 0, 0, 0, 1, 5, 10, 4}.
    //
    // The relative order must be maintained:
    // A) First, place all negative numbers.
    // B) Then zeros.
    // C) Last, positive numbers.
    //
    // See also dutch_flag_split.h which is unstable.

    // todo(p4): It is incorrect, O(n) time and O(1) space?
    // template<class int_t, class less_t = std::less<int_t>>
    // static void number_splitter_fast(std::vector<int_t> &data, less_t less_ = {})
    //{
    // This does not handle zeros: have to make TWO calls.
    //
    // std::stable_partition(data.begin(), data.end(),
    //  [&less_](const auto &val)
    //  {
    //    return less_(val, zero);
    //  });
    //}

    // Slow space O(n).
    template<class int_t, class less_t = std::less<int_t>>
    constexpr void number_splitter_slow(std::vector<int_t> &data, less_t less1 = {}, const int_t &zero = {})
    {
        // todo(p3): about less_t. May <=> be faster?
        if (const auto already_sorted = data.size() <= 1U; already_sorted)
        {
            return;
        }

        std::vector<int_t> negatives;
        std::vector<int_t> zeros;
        // todo(p4): join negatives and zeros: put at the start and the end of an array of size == data.size().
        // todo(p4): if an object is hard to copy, maybe first count how many.

        std::vector<int_t> positives;

        for (auto &old : data)
        {
            auto &&old_item = std::move(old);
            auto &item = old_item; // The other way around is not allowed - std::move is needed.

            if (less1(item, zero))
            {
                negatives.push_back(item);
            }
            else if (less1(zero, item))
            {
                positives.push_back(item);
            }
            else
            {
                zeros.push_back(item);
            }
        }

        data.clear();

        auto pusher = [&data](auto &source)
        {
            auto begin0 = std::make_move_iterator(source.begin());
            auto end1 = std::make_move_iterator(source.end());

            data.insert(data.end(), begin0, end1);
        };

        pusher(negatives);
        pusher(zeros);
        pusher(positives);
    }
} // namespace Standard::Algorithms::Numbers
