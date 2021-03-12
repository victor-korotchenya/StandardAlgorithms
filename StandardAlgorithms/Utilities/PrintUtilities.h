#pragma once
#include <array>
#include <map>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <stdexcept>

namespace Privet::Algorithms
{
    template<typename key_t, typename value_t>
    std::ostream& operator <<(std::ostream& str, const std::pair<key_t, value_t>& p)
    {
        str << '(' << p.first << ',' << p.second << ')';
        return str;
    }

    template<typename t0, typename t1, typename t2>
    std::ostream& operator <<(std::ostream& str, const std::tuple<t0, t1, t2>& t)
    {
        str << '(' << std::get<0>(t)
            << ',' << std::get<1>(t)
            << ',' << std::get<2>(t)
            << ')';
        return str;
    }

    inline void AppendSeparator(std::ostream& str, const std::string& separator = ", ")
    {
        str << separator;
    }

    inline void PrintName(std::ostream& str, const std::string& name)
    {
        if (name.empty())
            return;

        const auto& first = name.front();
        const auto& last = name.back();

        const auto print_comma = ',' != first,
            print_eq = '=' != last &&
            ' ' != last &&
            ':' != last;

        if (print_comma)
            str << ", ";

        str << name;

        if (print_eq)
            str << '=';
    }

    template<typename TValue>
    void PrintValue(std::ostream& str, const std::string& name, const TValue& value)
    {
        PrintName(str, name);
        str << value;
    }

    inline void PrintValue(std::ostream& str, const std::string& name, const std::string& value)
    {
        PrintName(str, name);

        const auto valueSeparator = '\'';
        str << valueSeparator << value << valueSeparator;
    }

    template <typename Iterator, class print_it>
    void Print(print_it pr, const std::string& name,
        const Iterator beginInclusive, const Iterator endExclusive,
        std::ostream& str, const unsigned maxSeparatorCounter,
        const std::string& datumSeparator, const bool requireNotEmptyName,
        const std::string& afterNameSeparator, const std::string& ending)
    {
        const auto emptyName = name.empty();
        if (requireNotEmptyName && emptyName)
            throw std::runtime_error("The 'name' is empty in Print.");

#ifdef _DEBUG
        if (datumSeparator.empty())
            throw std::runtime_error("The 'datumSeparator' is empty in Print.");
#endif
        if (beginInclusive == endExclusive)
        {
            str << name << " No data.\n";
            return;
        }

        if (!emptyName)
            PrintName(str, name);

        str << afterNameSeparator;

        auto separatorCounter = 0u;
        const auto shallUseSeparatorCounter = 0 < maxSeparatorCounter;
        auto has_before = false;

        for (auto it = beginInclusive; endExclusive != it; ++it)
        {
            if (has_before)
                str << datumSeparator;
            else
                has_before = true;

            pr(str, it);

            if (shallUseSeparatorCounter && maxSeparatorCounter <= ++separatorCounter)
            {
                separatorCounter = 0;
                str << '\n';
            }
        }

        str << ending << '\n';
    }

    template <typename Iterator>
    void Print(const std::string& name, const Iterator beginInclusive, const Iterator endExclusive, std::ostream& str,
        const unsigned maxSeparatorCounter = 10, const std::string& datumSeparator = ", ",
        const bool requireNotEmptyName = true,
        const std::string& afterNameSeparator = " {", const std::string& ending = "},")
    {
        auto pr = [](std::ostream& str2, Iterator it) -> void {
            str2 << *it;
        };

        Print<Iterator>(pr, name, beginInclusive, endExclusive, str,
            maxSeparatorCounter, datumSeparator, requireNotEmptyName,
            afterNameSeparator, ending);
    }

    template<class k, class v>
    std::ostream& operator << (std::ostream& str, const std::map<k, v>& data)
    {
        const auto name = "Unknown map name";
        Print(name, data.begin(), data.end(), str);
        return str;
    }

    template<class k, class v>
    std::ostream& operator << (std::ostream& str, const std::unordered_map<k, v>& data)
    {
        const auto name = "Unknown unordered map name";
        Print(name, data.begin(), data.end(), str);
        return str;
    }

    template <>
    void inline Print(const std::string& name, const std::vector<char>::const_iterator beginInclusive, const std::vector<char>::const_iterator endExclusive,
        std::ostream& str,
        const unsigned maxSeparatorCounter, const std::string& datumSeparator,
        const bool requireNotEmptyName,
        const std::string& afterNameSeparator, const std::string& ending)
    {
        auto pr = [](std::ostream& str2, const std::vector<char>::const_iterator it) -> void {
            str2 << static_cast<int>(*it);
        };

        Print<const std::vector<char>::const_iterator>(pr, name, beginInclusive, endExclusive, str, maxSeparatorCounter, datumSeparator,
            requireNotEmptyName, afterNameSeparator, ending);
    }

    template <typename T>
    void Print(const std::string& name, const std::vector<T>& data, std::ostream& str,
        const unsigned maxSeparatorCounter = 10, const std::string& datumSeparator = ", ", const bool requireNotEmptyName = true)
    {
        const auto beginInclusive = data.begin();
        const auto endExclusive = data.end();

        Print(name, beginInclusive, endExclusive, str, maxSeparatorCounter, datumSeparator, requireNotEmptyName);
    }

    template <typename T>
    void Print(const std::string& name,
        const std::vector<std::vector<T>>& dataOfData,
        std::ostream& str, const unsigned maxSeparatorCounter = 10, const std::string& datumSeparator = ", ")
    {
        const auto emptyName = name.empty();
#ifdef _DEBUG
        if (emptyName)
            throw std::runtime_error("The 'name' is empty in Print.");

        if (datumSeparator.empty())
            throw std::runtime_error("The 'datumSeparator' is empty in Print.");
#endif
        if (dataOfData.empty())
        {
            str << name << " No data.\n";
            return;
        }

        if (!emptyName)
        {
            PrintName(str, name);
            str << '\n';
        }

        for (const auto& d : dataOfData)
            Print("", d, str, maxSeparatorCounter, datumSeparator, false);
    }

    template<class t>
    std::ostream& operator << (std::ostream& str, const std::vector<t>& data)
    {
        const auto name = "Unknown vector name";

        Print(name, data, str);
        return str;
    }

    template<class t, int size>
    std::ostream& operator << (std::ostream& str, const std::array<t, size>& data)
    {
        const auto name = "Unknown array name";

        Print(name, data.begin(), data.end(), str);
        return str;
    }

    template<class t>
    std::ostream& operator << (std::ostream& str, const std::vector<std::vector<t>>& data)
    {
        const auto name = "Unknown name2";

        Print(name, data, str);
        return str;
    }
}