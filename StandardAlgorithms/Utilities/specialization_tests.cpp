#include"specialization_tests.h"
#include"simple_test_case.h"
#include"test_utilities.h"
#include<cstdint>
#include<stdexcept>

namespace
{
    using int_t = std::int32_t;

    struct ein_gigantisches_stueck_amphibienscheisse final : std::exception
    {
    };

    struct dangerous_tag
    {
    };

    using dangerous_case = Standard::Algorithms::Tests::simple_test_case<dangerous_tag, int_t>;

    struct dangerous_tag_empty_namespace
    {
    };

    using dangerous_case_empty_namespace =
        Standard::Algorithms::Tests::simple_test_case<dangerous_tag_empty_namespace, int_t>;

    constexpr void simple_validate(const dangerous_case_empty_namespace &test)
    {// A non-template validate function.
        const auto &input = test.input();
        if (input < 0)
        {
            throw ein_gigantisches_stueck_amphibienscheisse{};
        }
    }

    using long_int_t = std::int64_t;

    static_assert(sizeof(int_t) != sizeof(long_int_t), "The types must be different, not just an alias of each other.");

    struct silent_tag
    {
    };

    // By default, the silent case class does not throw on the validate().
    using silent_case = Standard::Algorithms::Tests::simple_test_case<silent_tag, long_int_t>;
} // namespace

// A specialization for the dangerous case class.
template<>
constexpr void Standard::Algorithms::Tests::simple_validate<dangerous_case>(const dangerous_case &test)
{
    const auto &input = test.input();
    if (input < 0)
    {
        throw ein_gigantisches_stueck_amphibienscheisse{};
    }
}

namespace
{
    constexpr int_t zero{};
    constexpr int_t positiv = 10;
    constexpr int_t negativ = -positiv;

    static_assert(negativ < zero && zero < positiv);

    template<class test_t>
    constexpr auto validate_test(const int_t &input, bool is_expected_to_throw = false) -> bool
    {
        using input_t = typename test_t::input_t;

        const auto *const type_name = sizeof(input_t) == sizeof(int_t) ? "dangerous" : "silent";
        assert(type_name != nullptr);

        const auto name = std::string(type_name) + (input < 0 ? " negativ" : " positiv");

        const test_t test{ name + "", // todo(p3): del "+" when std::string becomes constexpr.
            input, input };
        bool actual{};

        try
        {
            test.validate();
            actual = !is_expected_to_throw;
        }
        catch (const ein_gigantisches_stueck_amphibienscheisse &)
        {
            actual = is_expected_to_throw;
        }

        {
            constexpr auto success = true;
            ::Standard::Algorithms::ert::are_equal(success, actual, name + " success");
        }

        return true;
    }
} // namespace

void Standard::Algorithms::Utilities::Tests::specialization_tests()
{
    static_assert(validate_test<dangerous_case>(positiv));
    validate_test<dangerous_case>(negativ, true); // todo(p4): static_assert in C++29?

    static_assert(validate_test<dangerous_case_empty_namespace>(positiv));
    validate_test<dangerous_case_empty_namespace>(negativ, true); // todo(p4): static_assert in C++32?

    static_assert(validate_test<silent_case>(positiv));
    static_assert(validate_test<silent_case>(negativ));
}
