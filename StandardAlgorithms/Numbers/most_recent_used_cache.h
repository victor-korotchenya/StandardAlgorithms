#pragma once
#include <list>
#include <unordered_map>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Numbers
{
    // Least recently used items are removed when the maximum capacity is exceeded.
    template<class key_t, class value_t>
    struct most_recent_used_cache final
    {
        explicit most_recent_used_cache(size_t size_max, bool shall_reserve = true)
            : size_max(RequirePositive(size_max, "size_max"))
        {
            if (shall_reserve)
                m.reserve(size_max);
#if _DEBUG
            check();
#endif
        }

        void add(const key_t& key, const value_t& value)
        {
#if _DEBUG
            check();
#endif
            {
                auto it = m.find(key);
                if (it != m.end())
                {
                    auto list_it = it->second;
                    to_front(list_it, key, value);
                    return;
                }
            }

            if (size() == size_max)
            {// Evict the oldest recently used item.
                const auto& k2 = values.back().first;
#if _DEBUG
                const auto deleted =
#endif
                    m.erase(k2);

#if _DEBUG
                assert(deleted == 1);
#endif
                values.pop_back();
            }

            add_front(key, value);
        }

        bool find(const key_t& key, value_t& value)
        {
#if _DEBUG
            check();
#endif
            const auto it = m.find(key);
            if (it == m.end())
                return false;

            auto list_it = it->second;
            assert(list_it->first == key);
            value = list_it->second;

            to_front(list_it, key, value);
            return true;
        }

        size_t size() const
#ifndef _DEBUG
            noexcept
#endif
        {
#if _DEBUG
            check();
#endif
            return m.size();
        }

        std::pair<key_t, value_t> front() const
        {
#if _DEBUG
            check();
#endif
            if (values.empty())
                throw std::runtime_error("Cannot return front on empty mru.");

            return values.front();
        }

    private:
        void add_front(const key_t& key, const value_t& value)
        {
            assert(!m.count(key));

            values.emplace_front(key, value);
            m[key] = values.begin();
#if _DEBUG
            check();
#endif
        }

        template<class it_t>
        void to_front(it_t it, const key_t& key, const value_t& value)
        {
            assert(size() && it != values.end() && it->first == key);
#if _DEBUG
            const auto deleted =
#endif
                m.erase(key);
#if _DEBUG
            assert(deleted == 1);
#endif

            values.erase(it);
            add_front(key, value);
        }

#if _DEBUG
        void check() const
        {
            assert(size_max && m.size() == values.size());
        }
#endif

        size_t size_max;
        std::list<std::pair<key_t, value_t>> values;
        std::unordered_map<key_t, decltype(values.begin())> m;
    };

    template<class key_t, class value_t>
    struct most_recent_used_cache_slow final
    {
        explicit most_recent_used_cache_slow(size_t size_max)
            : size_max(RequirePositive(size_max, "size_max"))
        {
        }

        void add(const key_t& key, const value_t& value)
        {
            assert(size() <= size_max);

            for (auto it = mru.begin(); it != mru.end(); ++it)
            {
                if (it->first == key)
                {
                    it->second = value;
                    to_front(it);
                    return;
                }
            }

            if (size() == size_max)
                mru.pop_back();

            mru.emplace_front(key, value);
            assert(size() <= size_max);
        }

        bool find(const key_t& key, value_t& value)
        {
            for (auto it = mru.begin(); it != mru.end(); ++it)
            {
                if (it->first == key)
                {
                    value = it->second;
                    to_front(it);
                    return true;
                }
            }

            return false;
        }

        size_t size() const noexcept
        {
            return mru.size();
        }

        std::pair<key_t, value_t> front() const
        {
            if (mru.empty())
                throw std::runtime_error("Cannot return front on empty slow mru.");

            return *mru.begin();
        }

    private:
        template<class it_t>
        void to_front(it_t it)
        {
            assert(size());

            auto p = *it;
            mru.erase(it);
            mru.push_front(p);
        }

        size_t size_max;
        std::list<std::pair<key_t, value_t>> mru;
    };
}