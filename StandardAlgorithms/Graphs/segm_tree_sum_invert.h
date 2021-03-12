#pragma once
#include <cassert>

namespace Privet::Algorithms::Trees
{
    // Given bool[n]:
    // - get sum over [b, e).
    // - invert [b, e) i.e. false => true and vv.
    // Note. Indexes are 0 based.
    template<int logsi>
    class segm_tree_sum_invert final
    {
        static_assert(0 < logsi && logsi < 31);
        static constexpr int size = int(1) << logsi;
        int su[size << 1];
        char ups[size << 1];

    public:
        void clear();

        // "build" must be called aterwards.
        void init1(const int pos, const char b);

        void build();

        int get8(int le, int ri_x);

        void invert8(int le, int ri_x);

    private:
        static int calc_divs(const int index, int vals[logsi]);
        bool top_down(const int index, const int vals[logsi], const int cnt);
        void bottom_up(const int vals[logsi], const int cnt);
        void puto(const int index);
    };

    template <int logsi>
    void segm_tree_sum_invert<logsi>::clear()
    {
        memset(su, 0, sizeof su);
        memset(ups, 0, sizeof ups);
    }

    template <int logsi>
    void segm_tree_sum_invert<logsi>::init1(const int pos, const char b)
    {
        su[pos + size] = b;
    }

    template <int logsi>
    void segm_tree_sum_invert<logsi>::build()
    {
        for (auto i = size - 1; i; --i)
        {
            su[i] = su[i << 1] + su[(i << 1) | 1];
        }
    }

    template <int logsi>
    int segm_tree_sum_invert<logsi>::get8(int le, int ri_x)
    {
        le += size;
        ri_x += size;
        puto(le);
        puto(ri_x - 1);

        int r = 0;
        while (le < ri_x)
        {
            if (le & 1)
                r += su[le++];

            if (ri_x & 1)
                r += su[ri_x - 1];

            le >>= 1;
            ri_x >>= 1;
        }
        return r;
    }

    template <int logsi>
    void segm_tree_sum_invert<logsi>::invert8(int le, int ri_x)
    {
        le += size;
        ri_x += size;
        const auto be = le, enn = ri_x - 1;
        while (le < ri_x)
        {
            if (le & 1)
                ups[le++] ^= 1;

            if (ri_x & 1)
                ups[ri_x - 1] ^= 1;

            le >>= 1;
            ri_x >>= 1;
        }
        puto(be);
        puto(enn);
    }

    template <int logsi>
    int segm_tree_sum_invert<logsi>::calc_divs(
        const int index, int vals[logsi])
    {
        int cnt = 0, i2 = index;
        while (i2 >>= 1)
            vals[cnt++] = i2;

        assert(0 < cnt && cnt <= logsi);
        assert(index / 2 == vals[0]);
        assert(1 == vals[cnt - 1]);
        return cnt;
    }

    template <int logsi>
    bool segm_tree_sum_invert<logsi>::top_down(
        const int index, const int vals[logsi], const int cnt)
    {
        auto any = false;

        for (auto i = cnt - 1, tree_size = 1 << logsi;
            0 <= i;
            --i, tree_size >>= 1)
        {// from 1 to (index / 2)
            const auto& v = vals[i];
            assert(0 < v && v < size);

            for (const auto& pos : { v, v ^ 1 })
                if (ups[pos])
                {
                    auto& sup = su[pos];
                    assert(0 <= sup && sup <= tree_size);

                    sup = tree_size - sup;
                    assert(0 <= sup && sup <= tree_size);

                    ups[pos] = 0;
                    ups[pos << 1] ^= 1;
                    ups[(pos << 1) | 1] ^= 1;
                    any = true;
                }
        }

        {// leaves
            for (const auto& chi : { index, index ^ 1 })
                if (ups[chi])
                {
                    ups[chi] = 0;
                    su[chi] ^= 1;
                    any = true;
                }
        }
        return any;
    }

    template <int logsi>
    void segm_tree_sum_invert<logsi>::bottom_up(
        const int vals[logsi], const int cnt)
    {
        for (int i = 0; i < cnt; ++i)
        {
            const auto& par = vals[i];
            const auto c0 = par << 1, c1 = c0 | 1;
            su[par] = su[c0] + su[c1];
        }
    }

    template <int logsi>
    void segm_tree_sum_invert<logsi>::puto(const int index)
    {
        assert(size <= index && index < size * 2);
        int vals[logsi];
        const auto cnt = calc_divs(index, vals);
        if (top_down(index, vals, cnt))
            bottom_up(vals, cnt);
    }
}