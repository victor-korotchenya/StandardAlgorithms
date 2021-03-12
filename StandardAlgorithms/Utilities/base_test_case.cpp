#include"base_test_case.h"
#include"is_class_helper.h"
#include"print_utilities.h"

namespace
{
    template<class t>
    constexpr bool pass_move_test = Standard::Algorithms::is_move_constructor_noexcept<t> &&
        Standard::Algorithms::is_move_assignment_operator_noexcept<t> &&
        !Standard::Algorithms::is_assignment_operator_noexcept<t>;

    static_assert(pass_move_test<std::string>);
    static_assert(pass_move_test<Standard::Algorithms::Tests::base_test_case>);
} // namespace

auto Standard::Algorithms::Tests::operator<< (std::ostream &str, const base_test_case &test) -> std::ostream &
{
    ::Standard::Algorithms::print_value("Name", str, test.name());
    test.print(str);
    return str;
}
