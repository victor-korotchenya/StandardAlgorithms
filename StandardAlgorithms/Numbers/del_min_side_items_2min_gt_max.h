#pragma once
#include<cassert>
#include<set>
#include<vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Given n integers, remove minimum number of items from sides so that (2*min > max) in the remaining subarray.
            // Return number of deleted on the left, and right.
            // Time O(n*log(n)), space O(n).
            template <class int_t2, class int_t>
            std::pair<int, int> del_min_side_items_2min_gt_max(const std::vector<int_t>& ar)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t2));

                const auto size = static_cast<int>(ar.size());
                if (size <= 0)
                    return {};

                auto left_del = 0, right_del = size;
                std::multiset<int_t> ms;

                for (auto left = 0, right = 0; right < size; ++right)
                {
                    const auto& a = ar[right];
                    if (a <= 0)
                    {
                        ms.clear();
                        left = right + 1;
                        continue;
                    }

                    ms.insert(a);
                    int_t2 mi = *ms.begin(), ma = *ms.rbegin();

                    while ((mi << 1) <= ma)
                    {
                        assert(left < right&& ms.size() >= 2);
                        const auto& b = ar[left];

                        auto it = ms.find(b);
                        assert(it != ms.end());
                        ms.erase(it);

                        mi = *ms.begin(), ma = *ms.rbegin();
                        ++left;
                    }

                    const auto rc = size - 1 - right;
                    if (left_del + right_del > left + rc)
                        left_del = left, right_del = rc;
                }

                return { left_del, right_del };
            }

            // Slow time O(n*n).
            template <class int_t2, class int_t>
            std::pair<int, int> del_min_side_items_2min_gt_max_slow(const std::vector<int_t>& ar)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t2));
                const auto size = static_cast<int>(ar.size());
                if (size <= 0)
                    return {};

                auto ib = 0, jb = -1;

                for (auto i = 0; i < size; ++i)
                {
                    auto mi = ar[i];
                    if (mi <= 0)
                        continue;

                    auto ma = mi;
                    auto j = i;
                    do
                    {
                        const auto& a = ar[j];
                        if (mi > a)
                            mi = a;
                        else if (ma < a)
                            ma = a;
                    } while ((static_cast<int_t2>(mi) << 1) > ma && ++j < size);

                    --j;

                    if (jb - ib < j - i)
                        jb = j, ib = i;
                }

                if (jb < 0)
                    return { size, 0 };

                return { ib, size - 1 - jb };
            }
        }
    }
}