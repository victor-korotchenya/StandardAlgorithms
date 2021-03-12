#include"full_class_tests.h"
#include"../Utilities/test_utilities.h"
#include"full_class.h"
#include"magic_pendel.h"

namespace
{
    using magic_pendel_t = Standard::Algorithms::Strings::magic_pendel;
    using full_class_t = Standard::Algorithms::Strings::full_class<magic_pendel_t>;
    using funct_t = void (*)(full_class_t &ins);

    // cppcheck-suppress [constParameterCallback]
    constexpr void pass_by_ref(full_class_t &ins)
    {
        Standard::Algorithms::Strings::append(ins.pendel(), "pass by ref " + ins.ids());
    }

    constexpr void pass_by_ref_ref(full_class_t &&ins)
    {
        Standard::Algorithms::Strings::append(ins.pendel(), "pass by ref_ref " + ins.ids());
    }

    // Ignore clang-tidy: Pass by value is needed here.
    // NOLINTNEXTLINE
    constexpr void pass_by_value(full_class_t ins)
    {
        Standard::Algorithms::Strings::append(ins.pendel(), "pass by value " + ins.ids());
    }

    constexpr void run_test_case(funct_t funct, const std::string &expected_journal)
    {
        magic_pendel_t pendel;
        {
            full_class_t ins(pendel);
            funct(ins);
        }
        ::Standard::Algorithms::ert::are_equal(expected_journal, pendel.journal, "journal");
    }

    // cppcheck-suppress [constParameterCallback]
    constexpr void copy_test(full_class_t &ins)
    {
        auto two(ins);
        auto three = two;
    }

    // cppcheck-suppress [constParameterCallback]
    constexpr void copy_test_2(full_class_t &ins)
    {
        auto two = ins;
        two = ins;
    }

    constexpr void move_test(full_class_t &ins)
    {
        ins = full_class_t(ins.pendel());
    }

    constexpr void move_test_2(full_class_t &ins)
    {
        pass_by_value(std::move(ins));
    }

    // cppcheck-suppress [constParameterCallback]
    constexpr void pass_by_value_test(full_class_t &ins)
    {
        pass_by_value(ins);
    }

    // cppcheck-suppress [constParameterCallback]
    constexpr void pass_by_value_test_2(full_class_t &ins)
    {
        pass_by_value(full_class_t(ins.pendel()));
    }

    constexpr void pass_by_ref_ref_test(full_class_t &ins)
    {
        pass_by_ref_ref(std::move(ins));
    }

    // cppcheck-suppress [constParameterCallback]
    constexpr void pass_by_ref_ref_test_2(full_class_t &ins)
    {
        pass_by_ref_ref(full_class_t(ins.pendel()));
    }

    [[nodiscard]] constexpr auto run_ful_tests() -> bool
    {
        run_test_case(&pass_by_ref, "1 ctor; pass by ref 1; 1 dtor; ");

        run_test_case(&copy_test, "1 ctor; 2 copy ctor from 1; 3 copy ctor from 2; 3 dtor; 2 dtor; 1 dtor; ");

        run_test_case(&copy_test_2, "1 ctor; 2 copy ctor from 1; 2 copy op from 1; 2 dtor; 1 dtor; ");

        run_test_case(&move_test, "1 ctor; 2 ctor; 1 move op from 2; 2 dtor; 1 dtor; ");

        run_test_case(&move_test_2, "1 ctor; 2 move ctor from 1; pass by value 2; 2 dtor; 1 dtor; ");

        run_test_case(&pass_by_value_test, "1 ctor; 2 copy ctor from 1; pass by value 2; 2 dtor; 1 dtor; ");

        run_test_case(&pass_by_value_test_2, "1 ctor; 2 ctor; pass by value 2; 2 dtor; 1 dtor; ");

        run_test_case(&pass_by_ref_ref_test, "1 ctor; pass by ref_ref 1; 1 dtor; ");

        run_test_case(&pass_by_ref_ref_test_2, "1 ctor; 2 ctor; pass by ref_ref 2; 2 dtor; 1 dtor; ");

        return true;
    }
} // namespace

void Standard::Algorithms::Strings::Tests::full_class_tests()
{
    static_assert(run_ful_tests());
}
