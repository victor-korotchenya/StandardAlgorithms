#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Trees
{
    // Sum chosen cells times their widths, which are positive and might be different.
    // Initial position (width, chosen count) of 6 cells:
    //  10 0,  3 0,  20 3,  5 0,  10 1,  40 0
    // Add count from 1 to 5 exclusive:
    //      ,   +1,    +1,   +1,    +1,
    // Final position:
    //  10 0,  3 1,  20 4,  5 1,  10 2,  40 0
    // Cells that had 0 count before:
    //      ,  3 0,      ,  5 0,      ,       .
    // So, the added width is 8.
    //
    // A cell cannot have a negative count.
    // Indexes start from 0.
    // todo(p4): make overflow checks.


    // Slow. See "segment_tree_add_bool".
    template<std::unsigned_integral int_t1, std::unsigned_integral long_int_t1 = int_t1>
    requires(sizeof(int_t1) <= sizeof(long_int_t1))
    struct segment_tree_add_bool_slow final
    {
        using int_t = int_t1;
        using long_int_t = long_int_t1;

        constexpr explicit segment_tree_add_bool_slow(std::vector<long_int_t> &&widths)
            : counts(require_positive(widths.size(), "widths size"))
            , widths(std::move(require_all_positive(widths, "widths")))
        {
        }

        constexpr explicit segment_tree_add_bool_slow(const std::vector<long_int_t> &widths)
            : counts(require_positive(widths.size(), "widths size"))
            , widths(require_all_positive(widths, "widths"))
        {
        }

        constexpr void clear()
        {
            counts.assign(size(), {});
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            assert(0U < widths.size() && counts.size() == widths.size());

            return widths.size();
        }

        // Return cells, that have become positive, times their widths.
        constexpr auto add(int_t left, const int_t rex) -> long_int_t
        {
            assert(left < rex && rex <= size());

            long_int_t result{};

            while (left < rex)
            {
                assert(0U < widths[left]);

                auto &cnt = counts[left];

                if (cnt == 0U)
                {
                    result += widths[left];
                    assert(0U < result);
                }

                ++cnt;
                assert(0U < cnt);

                ++left;
            }

            return result;
        }

        // Cells, that have become zero, times their widths.
        constexpr auto remove(int_t left, const int_t rex) -> long_int_t
        {
            assert(left < rex && rex <= size());

            long_int_t result{};

            while (left < rex)
            {
                auto &cnt = counts[left];

                assert(0U < cnt && 0U < widths[left]);

                if (--cnt == 0U)
                {
                    result += widths[left];
                    assert(0U < result);
                }

                ++left;
            }

            return result;
        }

        [[nodiscard]] constexpr auto get(int_t left, const int_t rex) const -> long_int_t
        {
            assert(left < rex && rex <= size());

            long_int_t result{};

            while (left < rex)
            {
                assert(0U < widths[left]);

                if (0U < counts[left])
                {
                    result += widths[left];
                    assert(0U < result);
                }

                ++left;
            }

            return result;
        }

private:
        std::vector<int_t> counts;
        const std::vector<long_int_t> widths;
    };

    // todo(p3): finish the fast one.
    template<std::unsigned_integral int_t, std::unsigned_integral long_int_t = int_t>
    using segment_tree_add_bool = segment_tree_add_bool_slow<int_t, long_int_t>;
} // namespace Standard::Algorithms::Trees
