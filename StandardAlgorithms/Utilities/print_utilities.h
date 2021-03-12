#pragma once
#include"is_debug.h"
#include"w_stream.h"
#include"zu_string.h"
#include<algorithm>
#include<array>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<cstdint>
#include<functional>
#include<list>
#include<map>
#include<optional>
#include<ostream>
#include<set>
#include<stdexcept>
#include<string>
#include<tuple>
#include<type_traits>
#include<unordered_map>
#include<unordered_set>
#include<utility>
#include<vector>

namespace Standard::Algorithms
{
    namespace Inner
    {
        constexpr auto default_max_separator_counter = 10;
    } // namespace Inner


    // The print declarations in order to allow printing a pair of a vector of maps and another vector recursively.
    template<class item_t>
    void print(const std::string &name, const item_t &item, std::ostream &str);

    template<class item_t>
    void print_value(const std::string &name, std::ostream &str, const item_t &value); // todo(p3): del. print instead.

    template<class key_t, class value_t>
    void print(const std::string &name, const std::pair<key_t, value_t> &par, std::ostream &str);

    template<class t0, class t1, class t2>
    void print(const std::string &name, const std::tuple<t0, t1, t2> &tup, std::ostream &str);

    template<class t>
    void print(const std::string &name, const std::optional<t> &option, std::ostream &str);

    template<class iterator_t, class print_it>
    void print(print_it pri, const std::string &name, iterator_t begin_inclusive, iterator_t end_exclusive,
        std::ostream &str, std::uint32_t max_separator_counter, const std::string &datum_separator,
        bool require_not_empty_name, const std::string &after_name_separator, const std::string &ending);

    template<class iterator_t>
    void print(const std::string &name, iterator_t begin_inclusive, iterator_t end_exclusive, std::ostream &str,
        std::uint32_t max_separator_counter = Inner::default_max_separator_counter,
        const std::string &datum_separator = ", ", bool require_not_empty_name = true,
        const std::string &after_name_separator = " {", const std::string &ending = "},");

    template<class t>
    void print(const std::string &name, const std::vector<t> &data, std::ostream &str,
        std::uint32_t max_separator_counter = Inner::default_max_separator_counter,
        const std::string &datum_separator = ", ", bool require_not_empty_name = true);

    template<class t>
    void print(const std::string &name, const std::vector<std::vector<t>> &data_of_data, std::ostream &str,
        std::uint32_t max_separator_counter = Inner::default_max_separator_counter,
        const std::string &datum_separator = ", ");

    // operator<< declarations

    template<class key_t, class value_t>
    auto operator<< (std::ostream &str, const std::pair<key_t, value_t> &par) -> std::ostream &;

    template<class t0, class t1, class t2>
    auto operator<< (std::ostream &str, const std::tuple<t0, t1, t2> &tup) -> std::ostream &;

    template<class t>
    auto operator<< (std::ostream &str, const std::optional<t> &option) -> std::ostream &;

    template<class item_t>
    auto operator<< (std::ostream &str, const std::list<item_t> &data) -> std::ostream &;

    template<class item_t>
    auto operator<< (std::ostream &str, const std::set<item_t> &data) -> std::ostream &;

    template<class k_t, class v_t>
    auto operator<< (std::ostream &str, const std::map<k_t, v_t> &data) -> std::ostream &;

    template<class k_t, class v_t>
    auto operator<< (std::ostream &str, const std::unordered_map<k_t, v_t> &data) -> std::ostream &;

    template<class item_t, class hasher_t = std::hash<item_t>, class item_equal = std::equal_to<item_t>,
        class alloc = std::allocator<item_t>>
    auto operator<< (std::ostream &str, const std::unordered_set<item_t, hasher_t, item_equal, alloc> &data)
        -> std::ostream &;

    template<class t, std::size_t size>
    auto operator<< (std::ostream &str, const std::array<t, size> &data) -> std::ostream &;

    template<class t>
    auto operator<< (std::ostream &str, const std::vector<t> &data) -> std::ostream &;

    template<class t>
    auto operator<< (std::ostream &str, const std::vector<std::vector<t>> &data) -> std::ostream &;

    // Implementations.
    template<class key_t, class value_t>
    void print(const std::string &name, const std::pair<key_t, value_t> &par, std::ostream &str)
    {
        assert(!name.empty());

        str << par << ' ' << name;
    }

    template<class key_t, class value_t>
    auto operator<< (std::ostream &str, const std::pair<key_t, value_t> &par) -> std::ostream &
    {
        str << '(' << par.first << ',' << par.second << ')';
        return str;
    }

    template<class t0, class t1, class t2>
    void print(const std::string &name, const std::tuple<t0, t1, t2> &tup, std::ostream &str)
    {
        assert(!name.empty());

        str << tup << ' ' << name;
    }

    template<class t0, class t1, class t2>
    auto operator<< (std::ostream &str, const std::tuple<t0, t1, t2> &tup) -> std::ostream &
    {
        str << '(' << std::get<0>(tup) << ',' << std::get<1>(tup) << ',' << std::get<2>(tup) << ')';
        return str;
    }

    template<class t>
    void print(const std::string &name, const std::optional<t> &option, std::ostream &str)
    {
        assert(!name.empty());

        str << option << ' ' << name;
    }

    template<class t>
    auto operator<< (std::ostream &str, const std::optional<t> &option) -> std::ostream &
    {
        if (option.has_value())
        {
            const auto &value = option.value();
            str << value;
        }
        else
        {
            str << "null optional";
        }
        return str;
    }

    inline void append_separator(std::ostream &str, const std::string &separator = ", ")
    {
        str << separator;
    }

    inline void print_name(std::ostream &str, const std::string &name)
    {
        if (name.empty())
        {
            return;
        }

        const auto &first = name.front();

        if (const auto print_comma = ',' != first; print_comma)
        {
            str << ", ";
        }

        str << name;

        const auto &last = name.back();

        if (const auto print_eq = '=' != last && ' ' != last && ':' != last; print_eq)
        {
            str << '=';
        }
    }

    template<class item_t>
    void print(const std::string &name, const item_t &item, std::ostream &str)
    {
        print_name(str, name);
        str << item;
    }

    template<class t_value>
    inline void print_value(const std::string &name, std::ostream &str, const t_value &value)
    {
        print_name(str, name);
        str << value;
    }

    template<>
    inline void print_value(const std::string &name, std::ostream &str, const std::string &value)
    {
        print_name(str, name);

        const auto value_separator = '\'';
        str << value_separator << value << value_separator;
    }

    template<class iterator_t, class print_it>
    void print(print_it pri, const std::string &name, const iterator_t begin_inclusive, const iterator_t end_exclusive,
        std::ostream &str, const std::uint32_t max_separator_counter, const std::string &datum_separator,
        const bool require_not_empty_name, const std::string &after_name_separator, const std::string &ending)
    {
        const auto empty_name = name.empty();
        if (require_not_empty_name && empty_name) [[unlikely]]
        {
            throw std::runtime_error("The 'name' is empty in print.");
        }

        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (datum_separator.empty()) [[unlikely]]
            {
                throw std::runtime_error("The 'datumSeparator' is empty in debug print.");
            }
        }
        else
        {
            assert(!datum_separator.empty());
        }

        if (begin_inclusive == end_exclusive)
        {
            str << name << " No data.\n";
            return;
        }

        if (!empty_name)
        {
            print_name(str, name);
        }

        str << after_name_separator;

        auto separator_counter = 0U;
        const auto shall_use_separator_counter = 0U < max_separator_counter;
        auto has_before = false;

        for (auto ite = begin_inclusive; end_exclusive != ite; ++ite)
        {
            if (has_before)
            {
                str << datum_separator;
            }
            else
            {
                has_before = true;
            }

            pri(str, ite);

            if (shall_use_separator_counter && max_separator_counter <= ++separator_counter)
            {
                separator_counter = 0U;
                str << '\n';
            }
        }

        str << ending << '\n';
    }

    template<class iterator_t>
    void print(const std::string &name, const iterator_t begin_inclusive, const iterator_t end_exclusive,
        std::ostream &str, const std::uint32_t max_separator_counter, const std::string &datum_separator,
        const bool require_not_empty_name, const std::string &after_name_separator, const std::string &ending)
    {
        const auto pri = [](std::ostream &str2, iterator_t iter) -> void
        {
            str2 << *iter;
        };

        print<iterator_t>(pri, name, begin_inclusive, end_exclusive, str, max_separator_counter, datum_separator,
            require_not_empty_name, after_name_separator, ending);
    }

    template<class item_t>
    auto operator<< (std::ostream &str, const std::list<item_t> &data) -> std::ostream &
    {
        const auto *const name = "Unknown list name";
        print(name, data.cbegin(), data.cend(), str);
        return str;
    }

    template<class k_t, class v_t>
    auto operator<< (std::ostream &str, const std::map<k_t, v_t> &data) -> std::ostream &
    {
        const auto *const name = "Unknown map name";
        print(name, data.cbegin(), data.cend(), str);
        return str;
    }

    template<class k_t, class v_t>
    auto operator<< (std::ostream &str, const std::unordered_map<k_t, v_t> &data) -> std::ostream &
    {
        const auto *const name = "Unknown unordered map name";
        print(name, data.cbegin(), data.cend(), str);
        return str;
    }

    template<class item_t, class hasher_t, class item_equal_t, class alloc_t>
    auto operator<< (std::ostream &str, const std::unordered_set<item_t, hasher_t, item_equal_t, alloc_t> &data)
        -> std::ostream &
    {
        const auto *const name = "Unknown unordered set name";
        print(name, data.cbegin(), data.cend(), str);
        return str;
    }

    template<class item_t>
    auto operator<< (std::ostream &str, const std::set<item_t> &data) -> std::ostream &
    {
        const auto *const name = "Unknown set name";
        print(name, data.cbegin(), data.cend(), str);
        return str;
    }

    template<>
    inline void print(const std::string &name, const std::vector<char>::const_iterator begin_inclusive,
        const std::vector<char>::const_iterator end_exclusive, std::ostream &str,
        const std::uint32_t max_separator_counter, const std::string &datum_separator,
        const bool require_not_empty_name, const std::string &after_name_separator, const std::string &ending)
    {
        const auto pri = [](std::ostream &str2, const std::vector<char>::const_iterator iter) -> void
        {
            str2 << static_cast<std::int32_t>(*iter);
        };

        print<const std::vector<char>::const_iterator>(pri, name, begin_inclusive, end_exclusive, str,
            max_separator_counter, datum_separator, require_not_empty_name, after_name_separator, ending);
    }

    template<class t>
    void print(const std::string &name, const std::vector<t> &data, std::ostream &str,
        const std::uint32_t max_separator_counter, const std::string &datum_separator,
        const bool require_not_empty_name)
    {
        const auto begin_inclusive = data.cbegin();
        const auto end_exclusive = data.cend();

        print(
            name, begin_inclusive, end_exclusive, str, max_separator_counter, datum_separator, require_not_empty_name);
    }

    template<class t>
    void print(const std::string &name, const std::vector<std::vector<t>> &data_of_data, std::ostream &str,
        const std::uint32_t max_separator_counter, const std::string &datum_separator)
    {
        const auto empty_name = name.empty();

        if constexpr (::Standard::Algorithms::is_debug)
        {
            if (datum_separator.empty()) [[unlikely]]
            {
                throw std::runtime_error("The 'datumSeparator' is empty in debug print.");
            }

            if (empty_name) [[unlikely]]
            {
                throw std::runtime_error("The 'name' is empty in debug print.");
            }
        }

        assert(!datum_separator.empty() && !empty_name);

        if (data_of_data.empty())
        {
            str << name << " No data.\n";
            return;
        }

        print_name(str, name);
        str << '\n';

        for (const auto &dat : data_of_data)
        {
            print("Item", dat, str, max_separator_counter, datum_separator, false);
        }
    }

    inline void print(const std::string &name, const std::string &value, std::ostream &str)
    {// todo(p3): fix.
        print_value(name, str, value);
    }

    template<class t>
    auto operator<< (std::ostream &str, const std::vector<t> &data) -> std::ostream &
    {
        const auto *const name = "Unknown vector name";

        print(name, data, str);
        return str;
    }

    template<class t, std::size_t size>
    auto operator<< (std::ostream &str, const std::array<t, size> &data) -> std::ostream &
    {
        const auto *const name = "Unknown array name";
        print(name, data.cbegin(), data.cend(), str);

        return str;
    }

    template<class t>
    auto operator<< (std::ostream &str, const std::vector<std::vector<t>> &data) -> std::ostream &
    {
        const auto *const name = "Unknown name2";

        print(name, data, str);
        return str;
    }
} // namespace Standard::Algorithms
