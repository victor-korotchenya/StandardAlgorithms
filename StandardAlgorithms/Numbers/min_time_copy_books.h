#pragma once
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Given m <= 1000 books each having bi pages, and c <= m copiers,
            // compute min time to copy all books:
            // - a copier can copy a sequential book range,
            // - a copier copies a page a second.
            // Time O(n*log(n*max(books))), space O(1).
            template<class int_t2, class int_t>
            int_t2 min_time_copy_books_using_binary_search(const std::vector<int_t>& books, const int_t copiers)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t2));

                const auto size = static_cast<int_t>(books.size());
                RequirePositive(size, "books.size");
                RequireAllPositive(books, "books");
                RequirePositive(copiers, "copiers");

                if (copiers == 1)
                {
                    const auto sum = std::accumulate(books.begin(), books.end(), int_t2());
                    return sum;
                }

                if (size <= copiers)
                {
                    const auto& maxe = *std::max_element(books.begin(), books.end());
                    return maxe;
                }

                int_t2 left{}, right{};

                for (const auto& book : books)
                    left = std::max<int_t2>(left, book),
                    right += book;

                while (left <= right)
                {
                    const auto mid = left + ((right - left) >> 1);
                    int_t2 cops = 0, sum = 0;

                    for (const auto& book : books)
                    {
                        sum += book;
                        if (sum > mid)
                        {// Must end the previous.
                            sum = book;
                            assert(sum <= mid);
                            ++cops;
                        }

                        if (sum == mid)
                        {
                            sum = 0;
                            ++cops;
                        }
                    }

                    cops += sum > 0;
                    assert(cops > 0);

                    if (cops <= copiers)
                        right = mid - 1;
                    else
                        left = mid + 1;
                }

                assert(right >= 0);
                return right + 1;
            }

            // Time O(n*n*copiers), space can be O(n).
            template<class int_t2, class int_t>
            int_t2 min_time_copy_books_slow(const std::vector<int_t>& books, int_t copiers)
            {
                static_assert(sizeof(int_t) <= sizeof(int_t2));

                const auto size = static_cast<int_t>(books.size());
                RequirePositive(size, "books.size");
                RequireAllPositive(books, "books");
                RequirePositive(copiers, "copiers");

                //if (size <= copiers)
                //{
                //    const auto &maxe = *std::max_element(books.begin(), books.end());
                //    return maxe;
                //}
                //
                //if (copiers == 1)
                //{
                //    const auto sum = std::accumulate(books.begin(), books.end(), int_t2());
                //    return sum;
                //}

                if (size < copiers)
                    copiers = size;

                // cops,index -> min_time
                std::vector<std::vector<int_t2>> buf(copiers + 1,
                    std::vector<int_t2>(size + 1));

                {// 1 copier
                    auto& cur = buf[1];

                    for (auto i = 0; i < size; ++i)
                        cur[i + 1] = cur[i] + books[i];
                }

                for (auto cops = 2; cops <= copiers; ++cops)
                {
                    const auto& prev = buf[cops - 1];
                    auto& cur = buf[cops];

                    for (auto i = 0; i < size; ++i)
                    {
                        //           prev  cur
                        // vals \ cops  1    2    3    4
                        // 10          10   10   10   10
                        // 20          30   20a  20   20
                        //  5          35   25b  20   20
                        //  8          43   30c  20   20 ; 30c = max(30, 5+8)
                        auto best = prev[i + 1];
                        int_t2 cur_sum{};

                        for (auto j = i; j >= 0; --j)
                        {
                            cur_sum += books[j];
                            if (best <= cur_sum)
                                break;

                            const auto cand = std::max(prev[j], cur_sum);
                            if (best > cand)
                                best = cand;
                        }

                        cur[i + 1] = best;
                    }
                }

                const auto& result = buf.back().back();
                assert(result > 0);
                return result;
            }
        }
    }
}