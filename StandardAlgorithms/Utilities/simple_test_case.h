#pragma once
// "simple_test_case.h"
#include"base_test_case.h"
#include"print_utilities.h"

namespace Standard::Algorithms::Tests
{
    template<class input_t, class output_t>
    concept is_stdream_printable =
        // Avoid 70 chars long prefixes. // NOLINTNEXTLINE
        requires(stdream &str, const input_t &input, const output_t &output) {
            {
                str << input
                } -> std::same_as<stdream &>;
            {
                str << output
                } -> std::same_as<stdream &>;
        };

    // Please specialize to validate.
    template<class test_case_t>
    constexpr void simple_validate( // NOLINTNEXTLINE
        [[maybe_unused]] const test_case_t &test)
    {
    }

    template< // Use a unique tag type to avoid an accidentally wrong validation when both types match.
        class tag_t1, // auto uni =[]{} // See "uniq_name_tests.h" for the details. Clang fails on 20+ classes.
        class input_t1, class output_t1 = input_t1>
    struct simple_test_case final : base_test_case
    {
        using tag_t = tag_t1;
        using input_t = input_t1;
        using output_t = output_t1;

        template<class string_t, class input_t2, class output_t2 = output_t>
        constexpr simple_test_case(string_t &&name, input_t2 &&input, output_t2 &&output = {})
            : base_test_case(std::forward<string_t>(name))
            , Input(std::forward<input_t2>(input))
            , Output(std::forward<output_t2>(output))
        {
        }

#if defined(__GNUG__) && !defined(__clang__) // todo(p3): del this garbage after G++ is fixed.
        // Avoid an error: 'virtual constexpr {anonymous}::test_case::~test_case()' used before its definition.
        constexpr simple_test_case(const simple_test_case &) = default;
        constexpr auto operator= (const simple_test_case &) & -> simple_test_case & = default;
        constexpr simple_test_case(simple_test_case &&) noexcept = default;
        constexpr auto operator= (simple_test_case &&) &noexcept -> simple_test_case & = default;

        constexpr virtual ~simple_test_case() noexcept = default;
#endif // todo(p3): End of: del this garbage after G++ is fixed.

        [[nodiscard]] constexpr auto input() const &noexcept -> const input_t &
        {
            return Input;
        }

        [[nodiscard]] constexpr auto output() const &noexcept -> const output_t &
        {
            return Output;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();
            simple_validate(*this);
        }

        inline constexpr void print(stdream &str) const override
        {
            if constexpr (is_stdream_printable<input_t, output_t>) // todo(p3): del "if" after the fixing.
            {
                str << " Input " << Input << ", output " << Output;
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::append_separator(str);
            ::Standard::Algorithms::print("Input", Input, str);

            ::Standard::Algorithms::append_separator(str);
            ::Standard::Algorithms::print("Output", Output, str);
        }

private:
        input_t Input;
        output_t Output;
    };
} // namespace Standard::Algorithms::Tests
