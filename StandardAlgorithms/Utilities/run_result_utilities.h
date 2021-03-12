#pragma once
#include"launch_exception.h"
#include"run_result_kind.h"
#include<string>
#include<utility>

namespace Standard::Algorithms::Utilities
{
    struct run_result_utilities final
    {
        run_result_utilities() = delete;

        inline static constexpr auto has_printable_error(const run_result_kind &kind) noexcept -> bool
        {
            using enum run_result_kind;

            return error_in_functor_thrown == kind || error_cannot_catch == kind;
        }

        inline static constexpr auto has_error(const run_result_kind &kind) noexcept -> bool
        {
            return run_result_kind::okay != kind;
        }

        inline static constexpr auto has_error_uncaught(const run_result_kind &kind) noexcept -> bool
        {
            return run_result_kind::error_cannot_catch == kind;
        }

        using run_result_message_t = std::pair<run_result_kind, std::string>;

        template<class function_t, class... arguments_t>
        [[nodiscard]] inline static constexpr auto run_safe(const function_t &func, arguments_t... arguments)
            -> run_result_message_t
        {
            try
            {
                // no std::forward<function_t>(func)(std::forward<arguments_t>(arguments)...);
                func(arguments...);

                return { run_result_kind::okay, "" };
            }
            catch (const launch_exception &exc)
            {
                try
                {
                    return { run_result_kind::error_in_functor_caught, exc.what() };
                }
                catch (...)
                {
                }
            }
            catch (const std::exception &exc)
            {
                try
                {
                    return { run_result_kind::error_in_functor_thrown, exc.what() };
                }
                catch (...)
                {
                }
            }
            catch (...)
            {
            }

            return { run_result_kind::error_cannot_catch, "Unknown possibly uncaught error occurred." };
        }
    };
} // namespace Standard::Algorithms::Utilities
