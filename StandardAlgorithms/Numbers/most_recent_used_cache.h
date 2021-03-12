#pragma once
#include"../Utilities/require_utilities.h"
#include<list>
#include<unordered_map>

namespace Standard::Algorithms::Numbers
{
    // Least recently used (LRU) items are evicted when the maximum capacity is exceeded.
    template<class key_t, class value_t>
    struct most_recent_used_cache final
    {
        using key_value_t = std::pair<key_t, value_t>;
        using list_t = std::list<key_value_t>;
        using iter_t = decltype(std::declval<list_t>().begin());

        constexpr explicit most_recent_used_cache(std::size_t max_size, bool shall_reserve = true)
            : Max_size(require_positive(max_size, "max size"))
        {
            if (shall_reserve)
            {
                Map.reserve(max_size);
            }

            nkvd();
        }

        [[nodiscard]] constexpr auto size() const noexcept(!::Standard::Algorithms::is_debug) -> std::size_t
        {
            nkvd();

            return Map.size();
        }

        [[nodiscard]] constexpr auto front() const & -> const key_value_t &
        {
            nkvd();

            if (Values.empty()) [[unlikely]]
            {
                throw std::runtime_error("Cannot return front on empty MRU.");
            }

            return Values.front();
        }

        constexpr void add(const key_t &key, const value_t &value)
        {
            nkvd();

            if (auto iter = Map.find(key); iter != Map.end())
            {
                auto list_it = iter->second;
                assert(!Values.empty() && list_it != Values.end());

                list_it->second = value;
                to_front(key, list_it);

                return;
            }

            if (const auto evict_oldest_recently_used = size() == Max_size; evict_oldest_recently_used)
            {
                assert(!Values.empty());

                const auto &last_key = Values.back().first;

                [[maybe_unused]] const auto deleted = Map.erase(last_key);
                assert(deleted == 1U);

                Values.pop_back();
            }

            Values.emplace_front(key, value);

            try
            {
                Map.emplace(key, Values.begin());
            }
            catch (...)
            {
                Values.pop_front();
                throw;
            }

            nkvd();
        }

        constexpr auto move_to_front(const key_t &key, value_t &out_value) -> bool
        {
            nkvd();

            auto iter = Map.find(key);
            if (iter == Map.end())
            {
                return false;
            }

            auto list_it = iter->second;
            assert(!Values.empty() && list_it != Values.end());

            out_value = list_it->second; // Might throw.
            to_front(key, list_it);

            return true;
        }

private:
        constexpr void nkvd() const noexcept
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {// Values.size() takes O(n) time in the Debug build.
                assert(0U < Max_size && Map.size() == Values.size());
            }
        }

        // Time O(1).
        constexpr void to_front([[maybe_unused]] const key_t &key, iter_t list_it) noexcept
        {
            assert(0U < size() && list_it != Values.end() && list_it->first == key);

            if (auto insert_before_it = Values.begin(); list_it != insert_before_it)
            {
                auto next_after_list_it = std::next(list_it);

                Values.splice(insert_before_it, Values, list_it, next_after_list_it);
            }

            nkvd();
        }

        std::size_t Max_size;
        list_t Values{};
        std::unordered_map<key_t, iter_t> Map{};
    };

    template<class key_t, class value_t>
    struct most_recent_used_cache_slow final
    {
        using key_value_t = std::pair<key_t, value_t>;
        using list_t = std::list<key_value_t>;
        using iter_t = decltype(std::declval<list_t>().begin());

        constexpr explicit most_recent_used_cache_slow(std::size_t max_size)
            : Max_size(require_positive(max_size, "max size"))
        {
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            // todo(p4): list.size() is slow - add a variable?
            return Mru.size();
        }

        // Time O(1).
        [[nodiscard]] constexpr auto front() const & -> const key_value_t &
        {
            if (Mru.empty()) [[unlikely]]
            {
                throw std::runtime_error("Cannot return front on empty slow MRU.");
            }

            return *Mru.begin();
        }

        // Slow time O(n).
        constexpr void add(const key_t &key, const value_t &value)
        {
            check_order();

            for (auto iter = Mru.begin(); iter != Mru.end(); ++iter)
            {
                if (iter->first == key)
                {
                    iter->second = value;
                    to_front(iter);

                    return;
                }
            }

            if (size() == Max_size)
            {
                Mru.pop_back();
            }

            Mru.emplace_front(key, value);

            check_order();
        }

        // Slow time O(n).
        constexpr auto move_to_front(const key_t &key, value_t &value) -> bool
        {
            check_order();

            auto iterat = std::find_if(Mru.begin(), Mru.end(),
                [&key](const auto &item)
                {
                    return item.first == key;
                });

            if (iterat == Mru.end())
            {
                return false;
            }

            value = iterat->second;
            to_front(iterat);

            return true;
        }

private:
        constexpr void check_order() const noexcept
        {
            assert(size() <= Max_size && 0U < Max_size);
        }

        constexpr void to_front(iter_t iter)
        {
            assert(0U < size() && iter != Mru.end());

            auto kvp = std::move(*iter);
            Mru.erase(iter);
            Mru.push_front(std::move(kvp));

            check_order();
        }

        std::size_t Max_size;
        list_t Mru{};
    };
} // namespace Standard::Algorithms::Numbers
