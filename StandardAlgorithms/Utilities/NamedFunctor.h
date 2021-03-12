#pragma once
#include <string>
#include <functional>

namespace Privet::Algorithms::Utilities
{
    template <typename TReturn = void,
        typename TFunction = TReturn(*)(void),
        typename ... Arguments>
        class NamedFunctor
    {
        std::string _Name;
        TFunction _Function;

    protected:
        NamedFunctor(const std::string& name = "", TFunction theFunction = nullptr)
            : _Name(name), _Function(theFunction)
        {
        }

    public:
        virtual ~NamedFunctor()
        {
        }

        using TheFunction = std::function<TReturn(Arguments ...)>;

        virtual TReturn operator()(Arguments ... arguments) = 0;

        const std::string& get_Name() const
        {
            return _Name;
        }

        TFunction GetFunction() const
        {
            return _Function;
        }
    };
}