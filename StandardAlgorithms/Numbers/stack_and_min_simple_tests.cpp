#include"stack_and_min_simple_tests.h"
#include"../Utilities/ert.h"
#include"stack_and_min_simple.h"
#include<vector>

namespace
{
    using int_t = std::int16_t;
} // namespace

void Standard::Algorithms::Numbers::Tests::stack_and_min_simple_tests()
{
    const std::vector<int_t> numbers{// NOLINTNEXTLINE
        259, 152, 201, -140
    };
    const std::vector<int_t> minimums{ numbers[0], numbers[1], numbers[1], numbers[3] };

    ::Standard::Algorithms::ert::are_equal(numbers.size(), minimums.size(), "numbers size");

    stack_and_min_simple<int_t> stack1;
    ::Standard::Algorithms::ert::are_equal(0U, stack1.size(), "Stack size after creation.");

    {
        auto min_value = numbers.at(0);

        for (std::size_t index{}; index < numbers.size(); ++index)
        {
            const auto &number = numbers[index];
            stack1.push(number);

            const auto name = std::to_string(index);
            ::Standard::Algorithms::ert::are_equal(number, stack1.top(), "Top after push #" + name);

            min_value = std::min(number, min_value);
            ::Standard::Algorithms::ert::are_equal(minimums[index], min_value, "Minimum at " + name);

            ::Standard::Algorithms::ert::are_equal(min_value, stack1.minimum(), "Minimum after push #" + name);
        }
    }

    for (std::size_t index{}; index < numbers.size() - 1U; ++index)
    {
        stack1.pop();

        const auto name = std::to_string(index);
        const auto prev_index = numbers.size() - index - 2U;

        ::Standard::Algorithms::ert::are_equal(numbers.at(prev_index), stack1.top(), "Top after pop #" + name);

        ::Standard::Algorithms::ert::are_equal(minimums[prev_index], stack1.minimum(), "Minimum after pop #" + name);
    }

    ::Standard::Algorithms::ert::are_equal(false, stack1.is_empty(), "stack non-empty");
    stack1.pop();
    ::Standard::Algorithms::ert::are_equal(true, stack1.is_empty(), "stack empty");
}
