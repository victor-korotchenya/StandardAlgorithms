#pragma once
#include <cassert>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"
// todo: p1. end
namespace Privet::Algorithms::Trees
{
    // Sum chosen cells times their widths, which might be different and are positive.
    // Initial state (width, chosen count) of 6 cells:
    //  10 0,  3 0,  20 3,  5 0,  10 1,  40 0
    // Add count from 1 to 5 exclusive:
    //      ,   +1,    +1,   +1,    +1,
    // Final state:
    //  10 0,  3 1,  20 4,  5 1,  10 2,  40 0
    // Cells that had 0 count before:
    //      ,  3 0,      ,  5 0,      ,
    // So, the added width is 8.
    //
    // A cell cannot have a negative count.
    // Indexes start from 0.
    // No overflow checks.
    template<class number_t, class count_t = int>
    class segment_tree_add_bool final
    {
        std::vector<count_t> counts;
        const std::vector<number_t> widths;

    public:
        explicit segment_tree_add_bool(std::vector<number_t>&& widths)
            : counts(RequirePositive(widths.size(), "widths.size")),
            widths(std::forward<std::vector<number_t>>(widths))
        {
#if _DEBUG
            for (const auto& width : this->widths) assert(width > 0);
#endif
        }

        explicit segment_tree_add_bool(const std::vector<number_t>& widths)
            : counts(RequirePositive(widths.size(), "widths.size")),
            widths(widths)
        {
#if _DEBUG
            for (const auto& width : this->widths) assert(width > 0);
#endif
        }

        void clear()
        {
            counts.assign(widths.size(), {});
        }

        int size() const noexcept
        {
            return static_cast<int>(widths.size());
        }

        // Return cells, that have become positive, times their widths.
        number_t add(int left, const int rex)
        {
            assert(left >= 0 && left < rex&& rex <= size());

            number_t result{};
            while (left < rex)
            {
                assert(counts[left] >= 0 && widths[left] > 0);
                if (!counts[left])
                {
                    result += widths[left];
                    assert(result > 0);
                }

                ++counts[left];
                assert(counts[left] > 0);

                ++left;
            }
            return result;
        }

        // Cells, that have become zero, times their widths.
        number_t remove(int left, const int rex)
        {
            assert(left >= 0 && left < rex&& rex <= size());

            number_t result{};
            while (left < rex)
            {
                assert(counts[left] > 0 && widths[left] > 0);
                if (!--counts[left])
                {
                    result += widths[left];
                    assert(result > 0);
                }

                assert(counts[left] >= 0);

                ++left;
            }
            return result;
        }

        number_t get(int left, const int rex) const
        {
            assert(left >= 0 && left < rex&& rex <= size());

            number_t result{};
            while (left < rex)
            {
                assert(counts[left] >= 0 && widths[left] > 0);
                if (counts[left] > 0)
                    result += widths[left];
                ++left;
            }
            return result;
        }
    };

    // Slow.
    template<class number_t, class count_t = int>
    class segment_tree_add_bool_slow final
    {
        std::vector<count_t> counts;
        const std::vector<number_t> widths;

    public:
        explicit segment_tree_add_bool_slow(std::vector<number_t>&& widths)
            : counts(RequirePositive(widths.size(), "widths.size")),
            widths(std::forward<std::vector<number_t>>(widths))
        {
#if _DEBUG
            for (const auto& width : this->widths) assert(width > 0);
#endif
        }

        void clear()
        {
            counts.assign(widths.size(), {});
        }

        int size() const
        {
            return static_cast<int>(widths.size());
        }

        // Return cells, that have become positive, times their widths.
        number_t add(int left, const int rex)
        {
            assert(left >= 0 && left < rex&& rex <= size());

            number_t result{};
            while (left < rex)
            {
                assert(counts[left] >= 0 && widths[left] > 0);
                if (!counts[left])
                {
                    result += widths[left];
                    assert(result > 0);
                }

                ++counts[left];
                assert(counts[left] > 0);

                ++left;
            }
            return result;
        }

        // Cells, that have become zero, times their widths.
        number_t remove(int left, const int rex)
        {
            assert(left >= 0 && left < rex&& rex <= size());

            number_t result{};
            while (left < rex)
            {
                assert(counts[left] > 0 && widths[left] > 0);
                if (!--counts[left])
                {
                    result += widths[left];
                    assert(result > 0);
                }

                assert(counts[left] >= 0);

                ++left;
            }
            return result;
        }

        number_t get(int left, const int rex) const
        {
            assert(left >= 0 && left < rex&& rex <= size());

            number_t result{};
            while (left < rex)
            {
                assert(counts[left] >= 0 && widths[left] > 0);
                if (counts[left] > 0)
                    result += widths[left];
                ++left;
            }
            return result;
        }
    };
}