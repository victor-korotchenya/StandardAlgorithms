#pragma once
#include<cstddef>
#include<string>
#include<utility>
#include<vector>

namespace Standard::Algorithms
{
    using test_function_t = void (*)();

    struct test_function final
    {
        // NOLINTNEXTLINE
        std::string name{};

        // NOLINTNEXTLINE
        test_function_t func{};

        // NOLINTNEXTLINE
        std::size_t algorithm_count{};

#if defined(__clang__) // todo(p3): del unnecessary ctors in C++26
        constexpr test_function() = default;

        template<class name_t>
        constexpr test_function(name_t &&nam, test_function_t func, std::size_t algorithm_count)
            : name(std::forward<name_t>(nam))
            , func(func)
            , algorithm_count(algorithm_count)
        {
        }
#endif
    };
} // namespace Standard::Algorithms
