#pragma once
#include <tuple>
#include <utility>
#include <vector>

namespace std
{
    struct pair_hash final
    {
        template<class f, class s>
        size_t operator()(const std::pair<f, s>& p) const noexcept
        {
            const size_t a = std::hash<f>()(p.first),
                b = std::hash<s>()(p.second),
                c = (a | (a << 5)) ^ b;
            return c;
        }
    };

    struct tuple_hash final
    {
        // todo: p2. C++20 tuple hash.
        template<class f, class s>
        size_t operator()(const std::tuple<f, s>& p) const noexcept
        {
            const size_t a = std::hash<f>()(std::get<0>(p)),
                b = std::hash<f>()(std::get<1>(p)),
                c = (a | a << 5) ^ b;
            return c;
        }

        template<class f, class s, class t>
        size_t operator()(const std::tuple<f, s, t>& p) const noexcept
        {
            const size_t a = std::hash<f>()(std::get<0>(p)),
                b = std::hash<f>()(std::get<1>(p)),
                c = std::hash<f>()(std::get<2>(p)),
                d = (a | a << 5) ^ b,
                e = (d | d << 5) ^ c;
            return e;
        }

        template<class f, class s, class t, class u>
        size_t operator()(const std::tuple<f, s, t, u>& p) const noexcept
        {
            const size_t a = std::hash<f>()(std::get<0>(p)),
                b = std::hash<f>()(std::get<1>(p)),
                c = std::hash<f>()(std::get<2>(p)),
                d = std::hash<f>()(std::get<3>(p)),
                e = (a | a << 5) ^ b,
                g = (c | c << 5) ^ d,
                h = (e | e << 5) ^ g;
            return h;
        }

        template<class f, class s, class t, class u, class v>
        size_t operator()(const std::tuple<f, s, t, u, v>& p) const noexcept
        {
            const size_t a = std::hash<f>()(std::get<0>(p)),
                b = std::hash<f>()(std::get<1>(p)),
                c = std::hash<f>()(std::get<2>(p)),
                d = std::hash<f>()(std::get<3>(p)),
                e = std::hash<f>()(std::get<4>(p)),
                g = (a | a << 5) ^ b,
                h = (c | c << 5) ^ d,
                i = (g | g << 5) ^ h;
            return i ^ e;
        }

        template<class f, class s, class t, class u, class v, class w>
        size_t operator()(const std::tuple<f, s, t, u, v, w>& p) const noexcept
        {
            const size_t a = std::hash<f>()(std::get<0>(p)),
                b = std::hash<f>()(std::get<1>(p)),
                c = std::hash<f>()(std::get<2>(p)),
                d = std::hash<f>()(std::get<3>(p)),
                e = std::hash<f>()(std::get<4>(p)),
                g = std::hash<f>()(std::get<5>(p)),
                h = (a | a << 5) ^ b,
                i = (c | c << 5) ^ d,
                j = (e | e << 5) ^ g,
                //
                k = (h | h << 5) ^ i,
                l = (k | k << 5) ^ j;
            return l;
        }
    };

    // unordered_set<vector<int>, vector_hash>;
    struct vector_hash final
    {
        template<class t>
        size_t operator()(const std::vector<t>& v) const noexcept
        {
            size_t r = 0;
            auto hash = std::hash<t>();
            for (const auto& a : v)
            {
                const auto h = hash(a);
                r = ((r << 5) | r) ^ h;
            }
            return r;
        }
    };
}