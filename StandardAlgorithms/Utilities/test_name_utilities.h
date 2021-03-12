#pragma once
#include"find_first_repetition.h"
#include"require_utilities.h"
#include<stdexcept>
#include<string>
#include<vector>

namespace Standard::Algorithms::Utilities
{
    class test_name_utilities final
    {
        template<class item_t>
        [[nodiscard]] static constexpr auto name(const item_t &item) noexcept(noexcept(item.name()))
            -> const std::string &
        {
            return item.name();
        }

public:
        test_name_utilities() = delete;

        // Return true only if there is a repetition
        // or the 'data' is empty.
        template<class item_t, class get_name_function_t = const std::string &(*)(const item_t &)>
        static constexpr auto require_unique_names(const std::vector<item_t> &data,
            const std::string &extra_error_message, const bool throw_on_exception = true,
            get_name_function_t get_name_function = {}) -> bool
        {
            assert(!extra_error_message.empty());

            if (data.empty())
            {
                if (throw_on_exception)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "The input must be not empty.";

                    throw_exception(str, extra_error_message);
                }

                return true;
            }

            using iterator_t = typename std::vector<item_t>::const_iterator;

            if (nullptr == get_name_function)
            {
                get_name_function = &name<item_t>;
            }

            std::size_t first_repeat_index{};

            auto result =
                Standard::Algorithms::Utilities::find_first_repetition<std::string, iterator_t, get_name_function_t>(
                    data.begin(), first_repeat_index, data.end(), extra_error_message, throw_on_exception, get_name_function);

            return result;
        }
    };
} // namespace Standard::Algorithms::Utilities
