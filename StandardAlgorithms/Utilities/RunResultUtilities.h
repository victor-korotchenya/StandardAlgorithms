#pragma once
#include <string>
#include <utility>
#include "RunResultKind.h"
#include "LaunchException.h"

namespace Privet::Algorithms::Utilities
{
    class RunResultUtilities final
    {
        RunResultUtilities() = delete;

    public:
        inline static bool constexpr HasPrintableError(const RunResultKind& kind)
            noexcept
        {
            return RunResultKind::ErrorInFunctorThrown == kind || RunResultKind::ErrorCannotCatch == kind;
        }

        inline static bool HasError(const RunResultKind& kind)
        {
            return RunResultKind::OK != kind;
        }

        inline static bool HasErrorUncaught(const RunResultKind& kind)
        {
            return RunResultKind::ErrorCannotCatch == kind;
        }

        using RunResult_Message = std::pair<RunResultKind, std::string>;

        template <typename FunctionType, typename ... Arguments>
        inline static RunResult_Message
            RunSafe(const FunctionType& function, Arguments... arguments)
        {
            try
            {
                function(arguments ...);
                return RunResult_Message(RunResultKind::OK, "");
            }
            catch (const LaunchException& ex)
            {
                try
                {
                    return RunResult_Message(RunResultKind::ErrorInFunctorCaught, ex.what());
                }
                catch (...)
                {
                }
            }
            catch (const std::exception& ex)
            {
                try
                {
                    return RunResult_Message(RunResultKind::ErrorInFunctorThrown, ex.what());
                }
                catch (...)
                {
                }
            }
            catch (...)
            {
            }

            return RunResult_Message(RunResultKind::ErrorCannotCatch,
                "Unknown possibly uncaught error occurred.");
        }
    };
}