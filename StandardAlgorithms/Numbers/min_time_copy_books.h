#pragma once
#include"../Utilities/check_size.h"
#include"../Utilities/require_utilities.h"
#include<numeric> // std::midpoint
#include<vector>

namespace Standard::Algorithms::Numbers
{
    // Given books, each having books[i] pages, and c > 0 copiers,
    // compute min time to copy all books:
    // - a copier copies a page per second,
    // - a copier can copy a sequential book range e.g. from the 10-th book and to the book 200,
    // - book movements are banned,
    // - several copiers can work independently in parallel.
    // For example, given 3 books with the numbers of pages {10, 20, 5}, and 3 copiers, these are possible copier
    // assignments:
    // - use just 1 copier, time = max(10 + 20 + 5) = 35;
    // - use 2 copiers, the first does {10, 20}, the second - {5}, time = max(10 + 20, 5) = 30;
    // - use 2 copiers: {10} and {20, 5}, time = max(10, 20 + 5) = 25;
    // -- note that {5, 10} and {20} is not a valid copier distribution - no book can be moved;
    // - use 3 copiers, each gets a single book, time = max(10, 20, 5) = 20.
    // Time O(n*log(sum(books))), space O(1).
    template<std::integral long_int_t, std::integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto min_time_copy_books_using_binary_search(
        const std::vector<int_t> &books, const long_int_t &copiers) -> long_int_t
    {
        const auto *const name = "books size";

        const auto size = Standard::Algorithms::Utilities::check_size<long_int_t>(name, books.size());
        require_positive(size, name);

        require_all_positive(books, "books");
        require_positive(copiers, "copiers");

        if (copiers == 1)
        {// todo(p4): overflow check.
            auto sum = std::accumulate(books.cbegin(), books.cend(), long_int_t{});

            require_positive(sum, "sum");

            return sum;
        }

        if (size <= copiers)
        {
            const auto &dearest = *std::max_element(books.cbegin(), books.cend());

            assert(int_t{} < dearest);

            return static_cast<long_int_t>(dearest);
        }

        long_int_t left{};
        long_int_t right{};

        for (const auto &book : books)
        {
            left = std::max(left, static_cast<long_int_t>(book));

            [[maybe_unused]] const auto old = right;

            right += book;

            assert(long_int_t{} < left && left <= right);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(old < right);
            }
        }

        while (left <= right)
        {
            const auto mid = std::midpoint(left, right);

            long_int_t cops{};
            long_int_t sum{};

            for (const auto &book : books)
            {
                sum += book;

                if (mid < sum)
                {// Must end the previous copier work, then start a new one.
                    sum = book;
                    ++cops;
                }

                if (sum == mid)
                {
                    sum = {};
                    ++cops;
                }
            }

            cops += long_int_t{} < sum ? 1 : 0;

            assert(long_int_t{} < cops && long_int_t{} < mid);

            if (cops <= copiers)
            {
                right = static_cast<long_int_t>(mid - 1);
            }
            else
            {
                left = static_cast<long_int_t>(mid + 1);
            }
        }

        assert(long_int_t{} <= right);

        ++right;

        assert(long_int_t{} < right);

        return right;
    }

    // Time O(n*n*copiers), space can be O(n).
    template<std::integral long_int_t, std::integral int_t>
    requires(sizeof(int_t) <= sizeof(long_int_t))
    [[nodiscard]] constexpr auto min_time_copy_books_slow(const std::vector<int_t> &books, long_int_t copiers)
        -> long_int_t
    {
        const auto *const name = "books size";

        const auto size = Standard::Algorithms::Utilities::check_size<long_int_t>(name, books.size());
        require_positive(size, name);

        require_all_positive(books, "books");
        require_positive(copiers, "copiers");

        copiers = std::min(copiers, size);

        // copiers minus one, book index -> min time
        std::vector<std::vector<long_int_t>> buffers(copiers, std::vector<long_int_t>(size + 1));

        {// 1 copier
            auto &buf = buffers.at(0);

            for (long_int_t index{}; index < size; ++index)
            {
                const auto &book = books[index];
                const auto &pre = buf[index];

                auto &cur = buf[index + 1];
                cur = static_cast<long_int_t>(pre + book);

                assert(long_int_t{} <= pre && int_t{} < book && pre < cur && book <= cur);
            }
        }

        for (long_int_t cops = 2; cops <= copiers; ++cops)
        {
            const auto &prevs = buffers[cops - 2];
            auto &curs = buffers[cops - 1];

            for (long_int_t index{}; index < size; ++index)
            {
                // pages \ copiers
                //        1    2    3    4
                // 10 | 10   10   10   10
                // 20 | 30   20a  20   20
                // 05 | 35   25b  20c   20
                // 08 | 43   30d  20e   20
                // The buffers.
                // Note. 30 = min(max(8, 35), max(8+5, 30), max(8+5+20, 10)).
                // As soon as (8+5+20) = 33 is not less than the current best 30, stop.
                auto best = prevs[index + 1];

                for (long_int_t cur_sum{}, ind_2 = index; long_int_t{} < ind_2; --ind_2)
                {
                    cur_sum += books[ind_2];
                    assert(long_int_t{} < cur_sum);

                    if (!(cur_sum < best))
                    {
                        break;
                    }

                    auto cand = std::max(prevs[ind_2], cur_sum);
                    best = std::min(best, cand);
                }

                curs[index + 1] = best;
            }
        }

        const auto &result = buffers.back().back();

        assert(long_int_t{} < result);

        return result;
    }
} // namespace Standard::Algorithms::Numbers
