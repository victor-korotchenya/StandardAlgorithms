#pragma once
#include<atomic>
#include<cstddef>
#include<mutex>

namespace Standard::Algorithms::Tests
{
    struct test_run_context final
    {
        using mutex_t = std::mutex;

        // The mutex must be used for making changes to an instance of this class and
        // printing onto std::cout.
        mutex_t console_mutex{};

        std::atomic<std::size_t> long_runing_tests{};
        bool shall_print_line{};
    };
} // namespace Standard::Algorithms::Tests
