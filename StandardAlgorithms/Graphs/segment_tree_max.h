#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Trees
{
    // Segment tree with lazy propagation.
    //
    // max on range, add to range.
    template<class value_t = int64_t, int size_log = 18>
    class segment_tree_max final
    {
        static_assert(size_log >= 2 && size_log < 30);

        std::vector<value_t> ar, debt;

    public:
        segment_tree_max() : ar(size_t(2u) << size_log), debt(ar.size() >> 1)
        {
        }

        void clear()
        {
            ar.assign(ar.size(), {});
            debt.assign(debt.size(), {});
        }

        int size() const noexcept
        {
            return static_cast<int>(debt.size());
        }

        std::vector<value_t>& data() noexcept
        {
            return ar;
        }

        // Warn. Not tested!
        void init(const std::vector<value_t>& source, bool shall_clear = true)
        {
            const auto ss = static_cast<int>(source.size());
            RequirePositive(ss, "source.size");
            RequireNotGreater(ss, size(), "source.size");

            if (shall_clear)
                clear();

            std::copy(source.begin(), source.end(), ar.begin() + size());

            auto i = ss + size() - 1;

            do
                ar[i] = std::max(ar[i << 1], ar[i << 1 | 1]);
            while (--i);
        }

        void add_range(const value_t& v, int left, int rex)
        {
            assert(left >= 0 && left < rex&& rex <= size());

            left += size();
            rex += size();

            const auto lin = left, rin = rex - 1;
            while (left < rex)
            {
                if (left & 1)
                    add1(left++, v);

                if (rex & 1)
                    add1(rex - 1, v);

                left >>= 1;
                rex >>= 1;
            }

            push_up(lin);
            if (lin != rin)
                push_up(rin);
        }

        value_t get_max(int left, int rex)
        {
            assert(left >= 0 && left < rex&& rex <= size());

            auto r = std::numeric_limits<value_t>::min();
            left += size();
            rex += size();

            push_down(left);
            if (left != rex - 1)
                push_down(rex - 1);

            while (left < rex)
            {
                if (left & 1)
                    r = std::max(r, ar[left++]);

                if (rex & 1)
                    r = std::max(ar[rex - 1ll], r);

                left >>= 1;
                rex >>= 1;
            }

            return r;
        }

    private:
        void add1(const int index, const value_t& value)
        {
            ar[index] += value;
            if (index < size())
                debt[index] += value;
        }

        void push_down(const int index)
        {
            auto h = size_log;
            do
            {
                const auto parent = index >> h;
                if (debt[parent])
                {
                    add1(parent << 1, debt[parent]);
                    add1(parent << 1 | 1, debt[parent]);
                    debt[parent] = 0;
                }
            } while (--h);
        }

        void push_up(int index)
        {
            while (index > 1)
            {
                index >>= 1;
                ar[index] = std::max(ar[index << 1],
                    ar[index << 1 | 1]) + debt[index];
            }
        }
    };

    // from chef.
    template<class value_t = int64_t, int log_size = 18>
    class segment_tree_get_max_add_range final
    {
        static_assert(log_size >= 2 && log_size < 30);

        std::array<value_t, 2 << log_size> _ar = {};
        std::array<value_t, 1 << log_size> _debt = {};
        const int n;

    public:
        explicit segment_tree_get_max_add_range(int size) : n(size)
        {
            assert(n <= 1 << log_size);
        }

        int size() const noexcept
        {
            return static_cast<int>(_debt.size());
        }

        void add_range(const value_t& x, const int left, const int right)
        {
            assert(left >= 0 && left <= right && right < n);

            add(x, left, right, n);
        }

        value_t get_max(const int left, const int right)
        {
            assert(left >= 0 && left <= right && right < n);

            const auto t = get(left, right, n);
            return t;
        }

    private:
        void add1(const int i, const value_t& x)
        {
            _ar[i] += x;
            if (i < size())
                _debt[i] += x;
        }

        void push_down1(const int i)
        {
            add1(i << 1, _debt[i]);
            add1(i << 1 | 1, _debt[i]);
            _debt[i] = 0;
        }

        void add(const value_t& x, const int qle, const int qri,
            const int right, const int left = 0, const int i = 1)
        {
            if (qle <= left && right <= qri)
            {
                add1(i, x);
                return;
            }

            const auto mid = (left + right) >> 1;
            push_down1(i);
            if (qle <= mid)
                add(x, qle, qri, mid, left, i << 1);

            if (mid < qri)
                add(x, qle, qri, right, mid + 1, i << 1 | 1);

            _ar[i] = std::max(_ar[i << 1], _ar[i << 1 | 1]);
        }

        value_t get(const int qle, const int qri,
            const int right, const int left = 0, const int i = 1)
        {
            assert(qle >= 0 && qle <= qri && qri < n&& left <= right && right <= n);

            if (qle <= left && right <= qri)
                // Whole range included.
                return _ar[i];

            const auto mid = (left + right) >> 1;
            push_down1(i);

            const auto lm = qle <= mid ? get(qle, qri, mid, left, i << 1)
                : std::numeric_limits<value_t>::min();

            const auto rm = mid < qri ? get(qle, qri, right, mid + 1, i << 1 | 1)
                : std::numeric_limits<value_t>::min();

            const auto mv = std::max(lm, rm);
            return mv;
        }
    };
}