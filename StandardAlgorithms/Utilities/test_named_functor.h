#pragma once
#include"named_functor.h"
#include"test_run_context.h"

namespace Standard::Algorithms::Utilities
{
    class test_named_functor final : public named_functor<void, void (*)(), Tests::test_run_context *>
    {
        using void_func_t = void (*)();
        using parent_t = named_functor<void, void_func_t, Tests::test_run_context *>;

        std::int32_t total_tests_log10{};

public:
        explicit test_named_functor(
            std::string &&name = {}, void_func_t test_function = {}, std::int32_t total_tests = {});

        void operator() (Tests::test_run_context *context) & override;
    };
} // namespace Standard::Algorithms::Utilities
