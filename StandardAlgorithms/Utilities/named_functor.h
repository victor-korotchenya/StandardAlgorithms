#pragma once
#include<functional>
#include<string>

namespace Standard::Algorithms::Utilities
{
    template<class return_t = void, class function_t = return_t (*)(void), class... arguments_t>
    struct named_functor
    {
protected:
        constexpr explicit named_functor(std::string &&name1 = {}, function_t func = {})
            : Name(std::move(name1))
            , Func(func)
        {
        }

public:
        constexpr virtual ~named_functor() noexcept = default;

        constexpr named_functor(const named_functor &) = default;
        constexpr auto operator= (const named_functor &) & -> named_functor & = default;
        constexpr named_functor(named_functor &&) noexcept = default;
        constexpr auto operator= (named_functor &&) &noexcept -> named_functor & = default;

        using full_function_t = std::function<return_t(arguments_t...)>;

        [[nodiscard]] constexpr virtual auto operator() (arguments_t... arguments) & -> return_t = 0;

        [[nodiscard]] constexpr auto name() const &noexcept -> const std::string &
        {
            return Name;
        }

        [[nodiscard]] constexpr auto func() const &noexcept -> function_t
        {
            return Func;
        }

private:
        std::string Name;
        function_t Func;
    };
} // namespace Standard::Algorithms::Utilities
