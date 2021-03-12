#pragma once
// "require_utilities.h"
#include"print.h"
#include"print_utilities.h"
#include"throw_exception.h"
#include<algorithm>
#include<exception>
#include<limits>
#include<stdexcept>
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms
{
    // todo(p3): c++23 pointer concept?
    template<class ptr_t, class = std::enable_if_t<std::is_pointer_v<ptr_t>>>
    constexpr auto throw_if_null(const std::string &argument_name, ptr_t value
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
            ) -> ptr_t
    {
        assert(!argument_name.empty());

        if (nullptr != value) [[likely]]
        {
            return value;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The '" << argument_name << "' argument must be not null.";

#ifndef __clang__
        Standard::Algorithms::Utilities::print(str, loc);
#endif

        // Just 'throw' to avoid CPP check issues.
        throw std::runtime_error(str.str());
    }

    // todo(p3): c++23 pointer concept?
    template<class ptr_t, class = std::enable_if_t<std::is_pointer_v<ptr_t>>>
    constexpr void throw_if_not_null(
        const std::string &argument_name, ptr_t value, const std::string &extra_message = ""
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
    )
    {
        assert(!argument_name.empty());

        if (nullptr == value) [[likely]]
        {
            return;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The '" << argument_name << "' argument must be null, but is not.";

#ifndef __clang__
        Standard::Algorithms::Utilities::print(str, loc);
#endif

        throw throw_exception(str, extra_message);
    }

    template<class typ>
    constexpr void throw_if_null_skip_non_pointer(const std::string &argument_name, const typ &value
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
    )
    {
        if constexpr (std::is_pointer_v<typ>)
        {
            throw_if_null<typ>(argument_name, value
#ifndef __clang__
                ,
                loc
#endif
            );
        }
    }

    constexpr void throw_if_empty(
        const std::string &argument_name, const auto &collection, const std::string &extra_message = ""
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
    )
    {
        assert(!argument_name.empty());

        if (collection.empty()) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The '" << argument_name << "' argument must be not empty.";

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw throw_exception(str, extra_message);
        }
    }

    template<class item_t, class item_t2>
    constexpr void require_equal(const item_t &one, const std::string &message, const item_t2 &two
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
    )
    {
        assert(!message.empty());

        if (one == two) [[likely]]
        {
            return;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The (" << one << ") must be equal to (" << two << ").";

#ifndef __clang__
        Standard::Algorithms::Utilities::print(str, loc);
#endif

        throw throw_exception(str, message);
    }

    template<class item_t, class item_t2>
    constexpr void require_not_equal(const item_t &one, const std::string &message, const item_t2 &two
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
    )
    {
        assert(!message.empty());

        if (one == two) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The (" << one << ") must Not be equal to (" << two << ").";

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw throw_exception(str, message);
        }
    }

    template<class int_t>
    constexpr auto require_positive(const int_t &value, const std::string &message
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
            ) -> const int_t &
    {
        assert(!message.empty());

        if (int_t{} < value) [[likely]]
        {
            return value;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The value (" << value << ") must be positive.";

#ifndef __clang__
        Standard::Algorithms::Utilities::print(str, loc);
#endif

        throw throw_exception<std::out_of_range>(str, message);
    }

    template<std::signed_integral int_t>
    constexpr auto require_non_negative(const int_t &value, const std::string &message
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
            ) -> const int_t &
    {
        assert(!message.empty());

        if (value < int_t{}) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The value (" << value << ") must be non-negative.";

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw throw_exception<std::out_of_range>(str, message);
        }

        return value;
    }

    template<class int_t, class int_t_2>
    constexpr auto require_less_equal(const int_t &value, const int_t_2 &max_value, const std::string &message
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
            ) -> const int_t &
    {
        assert(!message.empty());

        if (max_value < value) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The value (" << value << ") must non exceed " << max_value << ".";

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw throw_exception<std::out_of_range>(str, message);
        }

        return value;
    }

    template<class int_t, class int_t_2>
    constexpr auto require_greater(const int_t &value, const int_t_2 &edge_value, const std::string &message
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
            ) -> const int_t &
    {
        assert(!message.empty());

        if (edge_value < value) [[likely]]
        {
            return value;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The value (" << value << ") cannot be less than or equal to " << edge_value << ".";

#ifndef __clang__
        Standard::Algorithms::Utilities::print(str, loc);
#endif

        throw throw_exception<std::out_of_range>(str, message);
    }

    template<class int_t, class int_t_2, class int_t_3>
    constexpr void require_between(
        const int_t &mini, const int_t_2 &value, const int_t_3 &maxi, const std::string &message
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
    )
    {
        assert(!message.empty());
        assert(!(maxi < mini));

        require_less_equal(mini, value, message
#ifndef __clang__
            ,
            loc
#endif
        );

        require_less_equal(value, maxi, message
#ifndef __clang__
            ,
            loc
#endif
        );
    }

    template<class iter_t, class iter_t2, class int_t = typename iter_t::value_type>
    constexpr void require_all_positive(iter_t begin1, iter_t2 end1, const std::string &message
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
    )
    {
        assert(!message.empty());

        constexpr int_t zero{};

        const auto bad = std::find_if(begin1, end1,
            [zero](const auto &num) -> bool
            {
                return !(zero < num);
            });

        if (end1 == bad) [[likely]]
        {
            return;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The value (" << (*bad) << ") at index " << (bad - begin1) << " must be strictly positive.";

#ifndef __clang__
        Standard::Algorithms::Utilities::print(str, loc);
#endif

        throw throw_exception(str, message);
    }

    template<class int_t>
    constexpr auto require_all_positive(const std::vector<int_t> &data, const std::string &message
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
            ) -> const std::vector<int_t> &
    {
        require_all_positive(data.cbegin(), data.cend(), message
#ifndef __clang__
            ,
            loc
#endif
        );

        return data;
    }

    template<class int_t>
    requires(std::is_signed_v<int_t> || std::is_floating_point_v<int_t>)
    constexpr auto require_all_non_negative(const std::vector<int_t> &data, const std::string &message
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
            ) -> const std::vector<int_t> &
    {
        assert(!message.empty());

        constexpr int_t zero{};

        const auto bad = std::find_if(data.cbegin(), data.cend(),
            [zero](const auto &num)
            {
                return num < zero;
            });

        if (data.cend() == bad) [[likely]]
        {
            return data;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The value (" << (*bad) << ") at index " << (bad - data.cbegin()) << " must be non-negative.";

#ifndef __clang__
        Standard::Algorithms::Utilities::print(str, loc);
#endif

        throw throw_exception(str, message);
    }

    constexpr void require_sorted(
        const auto &collection, const std::string &message, const bool shall_require_unique = false
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
    )
    {
        assert(!message.empty());

        const auto size = collection.size();
        if (size <= 1ZU)
        {
            return;
        }

        for (std::size_t index{}; index < size - 1ZU; ++index)
        {
            const auto &pre = collection[index];
            const auto &curr = collection[index + 1U];

            if (curr < pre) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The element " << curr << " at index " << (index + 1U) << " is less than " << pre << " at index "
                    << index << "; size " << size << ".";

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw throw_exception(str, message);
            }

            // todo(p3): <=>
            if (shall_require_unique && curr == pre) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The element " << curr << " at index " << (index + 1U) << " is equal to " << pre << " at index "
                    << index << "; size " << size << ".";

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw throw_exception(str, message);
            }
        }
    }

    // Require the "data" to be distinct, unique, non-repeating.
    template<class v_t, class item_t = typename v_t::value_type, class set_t = std::unordered_set<item_t>>
    constexpr auto require_unique(const v_t &data, const std::string &message, const bool throw_on_error = true
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
            ) -> bool
    {
        assert(!message.empty());

        if (data.size() <= 1ZU)
        {
            return true;
        }

        set_t uniq{};

        for (const auto &datum : data)
        {
            if (uniq.insert(datum).second) [[likely]]
            {
                continue;
            }

            if (!throw_on_error)
            {
                return false;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The value (" << datum << ") must not repeat.";

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw throw_exception(str, message);
        }

        return true;
    }

    template<class typ>
    [[noreturn]] auto throw_type_mismatch(
#ifndef __clang__
        const std::source_location &loc = std::source_location::current()
#endif
            ) -> std::runtime_error
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "Type mismatch for '" << typeid(typ).name() << "'.";

#ifndef __clang__
        Standard::Algorithms::Utilities::print(str, loc);
#endif

        throw std::runtime_error(str.str());
    }
} // namespace Standard::Algorithms
