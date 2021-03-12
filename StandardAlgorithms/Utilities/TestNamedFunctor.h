#pragma once
#include "NamedFunctor.h"
#include "TestRunContext.h"

namespace Privet::Algorithms::Utilities
{
    class TestNamedFunctor final : public NamedFunctor<void, void(*)(), Tests::TestRunContext*>
    {
        using VoidFunc = void(*)();
        using TParent = NamedFunctor<void, VoidFunc, Tests::TestRunContext*>;

        const int total_tests_log10;

    public:
        TestNamedFunctor(const std::string& name = "", VoidFunc testFunction = nullptr, int total_tests = 0);

        void operator()(Tests::TestRunContext* testRunContext) override;
    };
}