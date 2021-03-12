#include"uniq_name_tests.h"
#include<cstring> // std::strcmp
#include<stdexcept>
#include<typeinfo> // std::type_info

namespace
{
    // A failed proof of concept for "simple_test_case.h".
    // It seams that Cppcon authors, as we all, make mistakes.
    template<auto uni =
                 []
        {
        }>
    struct immer_uniq_gena_test final
    {
    };

    // It appears to work sporadically e.g. when the types are in the same file.
    // todo(p4): Try again C++32 in 2032.
    using test_1 = immer_uniq_gena_test<>;
    using test_2 = immer_uniq_gena_test<>;

    [[nodiscard]]
#ifndef __clang__ // todo(p4): Remove when Clangs is C++23 compatible.
    constexpr
#endif
        auto
        test_auto_generated_names_are_unique() noexcept -> bool
    {
        return typeid(test_1) != typeid(test_2);
    }
} // namespace

void Standard::Algorithms::Utilities::Tests::uniq_name_tests()
{
#ifndef __clang__ // todo(p4): Remove when Clangs is C++23 compatible.
    static_assert(test_auto_generated_names_are_unique());
#else
    if (!test_auto_generated_names_are_unique()) [[unlikely]]
    {
        throw std::runtime_error("Error in test_auto_generated_names_are_unique");
    }
#endif
}
