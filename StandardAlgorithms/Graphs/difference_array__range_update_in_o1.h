#pragma once
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Trees
{
    // Given an array, and range update operations(left, rex, val)
    // 1) run each range update in O(1).
    // 2) restore the resulting array.
    template <typename t>
    class difference_array__range_update_in_o1 final
    {
        std::vector<t> diffs;

    public:
        using value_type = t;

        explicit difference_array__range_update_in_o1(
            const std::vector<t>& source)
            : diffs(RequirePositive(source.size(), "source size"))
        {
            const auto n1 = count() - 1;

            diffs[0] = source[0];
            for (auto i = 0; i < n1; i++)
                diffs[i + 1] = source[i + 1] - source[i];
        }

        int count() const
        {
            return static_cast<int>(diffs.size());
        }

        void update(int left, int rex, const t& value)
        {
            RequireNotGreater(0, left, "left");
            RequireGreater(rex, left, "rex");
            RequireNotGreater(rex, count(), "rex");

            diffs[left] += value;
            if (rex < count())
                diffs[rex] -= value;
        }

        void restore(std::vector<t>& ar)
        {
            ar.resize(count());
            ar[0] = diffs[0];

            const auto size = count();
            for (auto i = 1; i < size; i++)
                ar[i] = diffs[i] + ar[i - 1];
        }
    };

    template <typename t>
    class difference_array_slow final
    {
        std::vector<t> data;

    public:
        using value_type = t;

        explicit difference_array_slow(const std::vector<t>& source)
            : data(source)
        {
            RequirePositive(source.size(), "source size");
        }

        int count() const
        {
            return static_cast<int>(data.size());
        }

        void update(int left, int rex, const t& value)
        {
            RequireNotGreater(0, left, "left");
            RequireGreater(rex, left, "rex");
            RequireNotGreater(rex, count(), "rex");

            for (auto i = left; i < rex; ++i)
                data[i] += value;
        }

        void restore(std::vector<t>& ar)
        {
            ar = data;
        }
    };
}