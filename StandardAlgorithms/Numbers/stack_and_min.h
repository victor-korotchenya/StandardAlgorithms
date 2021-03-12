#pragma once
#include<algorithm>
#include<concepts>
#include<cstddef>
#include<functional> // less
#include<stack>
#include<stdexcept>

namespace Standard::Algorithms::Numbers
{
    // Stack, returning minimum value.
    // Although it is more memory efficient, than "stack_and_min_simple",
    //  the default implementation does NOT check for an overflow!
    template<std::signed_integral int_t,
        // To check for add overflow.
        class add_t = int_t (*)(const int_t &, const int_t &),
        // To check for subtract overflow.
        class subtract_t = add_t, class less_t = std::less<int_t>>
    struct stack_and_min final
    {
        explicit constexpr stack_and_min(
            // This implementation does NOT check for an overflow.
            add_t add = [] [[nodiscard]] (const int_t &one, const int_t &two) -> int_t
            {
                return one + two;
            },
            // Here is NO overflow check.
            subtract_t subtract = [] [[nodiscard]] (const int_t &one, const int_t &two) -> int_t
            {
                return one - two;
            },
            less_t less1 = {})
            : Add(add)
            , Subtract(subtract)
            , Less(less1)
        {
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool
        {
            return Data.empty();
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Data.size();
        }

        [[nodiscard]] constexpr auto top() const & -> const int_t &
        {
            require_non_empty();

            const auto &mimo = Data.top();
            const auto &result = Less(mimo, Minimum) ? Minimum : mimo;
            return result;
        }

        [[nodiscard]] constexpr auto minimum() const & -> const int_t &
        {
            require_non_empty();
            return Minimum;
        }

        constexpr void push(const int_t &value)
        {
            if (Data.empty())
            {
                Minimum = value;
                Data.push(value);
                return;
            }

            if (!Less(value, Minimum))
            {
                Data.push(value);
                return;
            }

            // val < min; 2*val < 2*min; 2*val - min < min.
            // Let min=10, value=9, then newValue = 2*9-10 = 8.
            {
                auto double_trouble = Add(value, value);
                auto new_value = Subtract(double_trouble, Minimum);
                Data.push(std::move(new_value));
            }

            Minimum = value;
        }

        constexpr void pop()
        {
            require_non_empty();

            if (const auto &mimo = Data.top(); Less(mimo, Minimum))
            {
                auto bikini = Add(Minimum, Minimum);
                Minimum = Subtract(bikini, mimo);
            }

            Data.pop();
        }

private:
        inline constexpr void require_non_empty() const
        {
            if (Data.empty()) [[unlikely]]
            {
                throw std::out_of_range("The min stack is empty.");
            }
        }

        add_t Add;
        subtract_t Subtract;
        less_t Less;

        std::stack<int_t> Data{};
        int_t Minimum{};
    };
} // namespace Standard::Algorithms::Numbers
