#pragma once
#include <algorithm>
#include <cassert>
#include <vector>

namespace Privet::Algorithms::Trees
{
    // Segment tree with lazy propagation.
    //
    // sum on range, assign to range.
    template<class value_t = int64_t>
    class segment_tree_assign_sum final
    {
        int _size_log, _size;
        std::vector<value_t> _ar;
        std::vector<std::pair<value_t, bool>> _debt;

    public:
        explicit segment_tree_assign_sum(int size_log = 18) : _size_log(size_log),
            _size(1 << _size_log), _ar(_size << 1), _debt(_size)
        {
            assert(size_log > 1 && size_log <= 31);
        }

        void clear()
        {
            _ar.assign(_ar.size(), {});
            _debt.assign(_debt.size(), {});
        }

        int size() const noexcept
        {
            assert(_size > 0);
            return _size;
        }

        std::vector<value_t>& data()
        {
            return _ar;
        }

        //void init(const std::vector<value_t>&v)
        void init()
        {
            //assert(static_cast<int>(v.size()) <= _size);
            //std::copy(v.begin(), v.end(), _ar.begin() + _size);

            for (auto i = _size - 1; i; --i)
                _ar[i] = _ar[i << 1] + _ar[i << 1 | 1];
        }

        void add(const value_t& v, int left, int rex)
        {
            assert(left >= 0 && left < rex&& rex <= _size);

            left += _size;
            rex += _size;

            const auto lin = left, rin = rex - 1;
            push_down(left);
            if (left != rin)
                push_down(rin);

            value_t k = 1;
            while (left < rex)
            {
                if (left & 1)
                    add_mult(left++, v, k);

                if (rex & 1)
                    add_mult(rex - 1, v, k);

                left >>= 1;
                rex >>= 1;
                k <<= 1;
            }

            push_up(lin);
            if (lin != rin)
                push_up(rin);
        }

        value_t get_sum(int left, int rex)
        {
            assert(left >= 0 && left < rex&& rex <= _size);

            left += _size;
            rex += _size;

            push_down(left);
            if (left != rex - 1)
                push_down(rex - 1);

            value_t r{};
            while (left < rex)
            {
                if (left & 1)
                    r += _ar[left++];

                if (rex & 1)
                    r += _ar[rex - 1];

                left >>= 1;
                rex >>= 1;
            }

            return r;
        }

    private:
        void add_mult(const int index, const value_t& value, const value_t& mult)
        {
            _ar[index] = value * mult;
            if (index < _size)
                _debt[index] = { value, true };
        }

        void push_down(int left)
        {
            assert(left >= _size && left < _size << 1);

            value_t k = value_t(1) << (_size_log - 1);
            auto h = _size_log;
            do
            {
                const auto p = left >> h;
                if (_debt[p].second)
                {
                    add_mult(p << 1, _debt[p].first, k);
                    add_mult(p << 1 | 1, _debt[p].first, k);
                    _debt[p].second = false;
                }

                k >>= 1;
            } while (--h);
        }

        void push_up(int left)
        {
            value_t mult = 2;
            for (;;)
            {
                left >>= 1;
                if (!left)
                    break;

                _ar[left] = _debt[left].second
                    ? _debt[left].first * mult
                    : _ar[left << 1] + _ar[left << 1 | 1];

                mult <<= 1;
            }
        }
    };
}