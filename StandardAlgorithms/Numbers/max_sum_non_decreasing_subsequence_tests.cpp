#include"max_sum_non_decreasing_subsequence_tests.h"
#include"../Utilities/test_utilities.h"
#include"max_sum_non_decreasing_subsequence.h"

namespace
{
    using int_t = std::uint16_t;
    using long_int_t = std::uint64_t;
    using vec_t = std::vector<int_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, vec_t &&input, vec_t &&output = {}) noexcept
            : base_test_case(std::move(name))
            , Input(std::move(input))
            , Output(std::move(output))
        {
        }

        [[nodiscard]] constexpr auto input() const &noexcept -> const vec_t &
        {
            return Input;
        }

        [[nodiscard]] constexpr auto output() const &noexcept -> const vec_t &
        {
            return Output;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(Input.size(), "Input size");
            ::Standard::Algorithms::ert::are_equal(size, Output.size(), "Output size");
            Standard::Algorithms::require_all_positive(Input, "Input");

            int_t last_output{};

            for (std::size_t index{}; index < size; ++index)
            {
                input_vs_output(index);
                require_increasing_output(index, last_output);
            }

            Standard::Algorithms::require_positive(last_output, "Last output");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::append_separator(str);
            ::Standard::Algorithms::print("Input", Input, str);
            ::Standard::Algorithms::print("Output", Output, str);
        }

private:
        constexpr void input_vs_output(const std::size_t index) const
        {
            const auto &inp = Input.at(index);
            const auto &outp = Output.at(index);

            if (0U == outp || !(inp < outp))
            {
                return;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Output[" << index << "] " << outp << " cannot exceed " << inp << " input.";

            Standard::Algorithms::throw_exception(str);
        }

        constexpr void require_increasing_output(const std::size_t index, int_t &last_output) const
        {
            const auto &new_output = Output.at(index);

            if (0U == new_output)
            {
                return;
            }

            Standard::Algorithms::require_less_equal(last_output, new_output, "last output at " + std::to_string(index));

            last_output = new_output;
        }

        vec_t Input;
        vec_t Output;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "SecondZero", // NOLINTNEXTLINE
            { 5, 2 }, { 5, 0 } });

        test_cases.push_back({ "BothEqual", // NOLINTNEXTLINE
            { 5, 3 }, { 3, 3 } });

        test_cases.push_back({ "MiddleZero", // NOLINTNEXTLINE
            { 65533, 1234, 65535 }, { 65533, 0, 65535 } });

        test_cases.push_back({ "MiddleZero2", // NOLINTNEXTLINE
            { 65535, 1234, 65533 }, { 65533, 0, 65533 } });

        test_cases.push_back({ "Trivial1", // NOLINTNEXTLINE
            { 1 }, { 1 } });

        test_cases.push_back({ "Trivial 20", // NOLINTNEXTLINE
            { 20 }, { 20 } });

        test_cases.push_back({ "Increasing", // NOLINTNEXTLINE
            { 10, 20 }, { 10, 20 } });

        test_cases.push_back({ "Two same", // NOLINTNEXTLINE
            { 21, 21 }, { 21, 21 } });

        test_cases.push_back({ "Three same", // NOLINTNEXTLINE
            { 20, 20, 20 }, { 20, 20, 20 } });

        test_cases.push_back({ "Longer", // NOLINTNEXTLINE
            { 40, 20, 30, 10, 20, 50, 30, 100, 20 }, // NOLINTNEXTLINE
            { 20, 20, 20, 0, 20, 30, 30, 100, 0 } });
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual =
            Standard::Algorithms::Numbers::max_sum_non_decreasing_subsequence<int_t, long_int_t>::compute(test.input());

        ::Standard::Algorithms::ert::are_equal(test.output(), actual, "max_sum_non_decreasing_subsequence");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::max_sum_non_decreasing_subsequence_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
