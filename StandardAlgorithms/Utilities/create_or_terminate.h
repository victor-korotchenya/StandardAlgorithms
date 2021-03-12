#pragma once
#include<cassert>
#include<exception> // std::terminate
#include<iostream>

namespace Standard::Algorithms
{
    template<class creator_t = void (*)()>
    [[nodiscard]] auto create_or_terminate(creator_t creator) noexcept -> decltype(auto)
    {
        assert(creator);

        auto printed = false;

        try
        {
            return creator();
        }
        catch (const std::exception &exc)
        {
            try
            {
                std::cout << "Unexpected exception in create or terminate: " << exc.what() << "\n";

                printed = true;
            }
            catch (...)
            {// Nothing can be done.
            }
        }
        catch (...)
        {// Nothing can be done.
        }

        if (!printed)
        {
            try
            {
                std::cout << "Got a really unexpected exception in create or terminate.\n";
            }
            catch (...)
            {// Nothing can be done.
            }
        }

        std::terminate();
    }
} // namespace Standard::Algorithms
