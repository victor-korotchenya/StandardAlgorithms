#pragma once

namespace Privet::Algorithms::Utilities
{
    enum class RunResultKind
    {
        OK = 0,
        //The error has been processed, and re-thrown as LaunchException.
        ErrorInFunctorCaught,
        //The error has been thrown by the function.
        ErrorInFunctorThrown,
        //The error has been thrown by the function, but
        // could not be caught or there was not enough memory at the time.
        ErrorCannotCatch,
    };
}