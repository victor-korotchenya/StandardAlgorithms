#include"stack_and_min_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"arithmetic.h"
#include"stack_and_min.h"
#include"stack_and_min_simple.h"
#include<vector>

namespace
{
    using int_t = std::int64_t;
    using add_function_t = int_t (*)(const int_t &, const int_t &);

    constexpr void test_impl(const std::vector<int_t> &numbers, auto &stack1, auto &stack_simple, std::size_t &step)
    {
        const auto size = Standard::Algorithms::require_positive(numbers.size(), "size");
        {
            auto mina = numbers.at(0);

            for (std::size_t index{}; index < size; ++index, ++step)
            {
                const auto &number = numbers[index];
                stack1.push(number);

                const auto name = std::to_string(index);
                ::Standard::Algorithms::ert::are_equal(number, stack1.top(), "Top after push #" + name);

                stack_simple.push(number);
                ::Standard::Algorithms::ert::are_equal(number, stack_simple.top(), "Simple top after push #" + name);

                mina = std::min(number, mina);
                ::Standard::Algorithms::ert::are_equal(mina, stack1.minimum(), "Minimum after push #" + name);
                ::Standard::Algorithms::ert::are_equal(
                    mina, stack_simple.minimum(), "Simple minimum after push #" + name);
            }
        }

        for (std::size_t index{}; index < size - 1U; ++index, ++step)
        {
            const auto name = std::to_string(index);
            {
                const auto prev_index = size - 2U - index;
                const auto &expected = numbers.at(prev_index);
                stack1.pop();
                ::Standard::Algorithms::ert::are_equal(expected, stack1.top(), "Top after pop #" + name);

                stack_simple.pop();
                ::Standard::Algorithms::ert::are_equal(expected, stack_simple.top(), "Simple top after pop #" + name);
            }

            ::Standard::Algorithms::ert::are_equal(
                stack1.minimum(), stack_simple.minimum(), "Minimum after pop #" + name);
        }

        ::Standard::Algorithms::ert::are_equal(false, stack1.is_empty(), "non-empty stack");
        stack1.pop();
        ::Standard::Algorithms::ert::are_equal(true, stack1.is_empty(), "empty stack");

        ::Standard::Algorithms::ert::are_equal(false, stack_simple.is_empty(), "non-empty simple stack");
        stack_simple.pop();
        ::Standard::Algorithms::ert::are_equal(true, stack1.is_empty(), "empty simple stack");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::stack_and_min_tests()
{
    stack_and_min<int_t, add_function_t> stack1(add_signed<int_t>, subtract_signed<int_t>);
    ::Standard::Algorithms::ert::are_equal(0U, stack1.size(), "Stack size after creation.");

    stack_and_min_simple<int_t> stack_simple;
    ::Standard::Algorithms::ert::are_equal(0U, stack_simple.size(), "Simple stack size after creation.");

    constexpr std::size_t size = 20;

    // To avoid an overflow.
    constexpr auto cutter = static_cast<int_t>(size * 50);

    std::vector<int_t> numbers(size);
    Standard::Algorithms::Utilities::fill_random(numbers, size, cutter);

    std::size_t step{};

    try
    {
        test_impl(numbers, stack1, stack_simple, step);
    }
    catch (const std::exception &exc)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "Step " << step << ", numbers " << numbers << ", error: " << exc.what();

        throw std::runtime_error(str.str());
    }
}
