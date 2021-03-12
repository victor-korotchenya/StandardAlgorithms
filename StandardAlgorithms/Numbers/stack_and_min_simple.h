#pragma once
#include<cassert>
#include<cstddef>
#include<functional> // less
#include<stack>
#include<stdexcept>
#include<string>

namespace Standard::Algorithms::Numbers
{
    // Stack, returning minimum value.
    template<class t, class less_t = std::less<t>>
    struct stack_and_min_simple final
    {
        constexpr explicit stack_and_min_simple(less_t less1 = {})
            : Less(less1)
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

        [[nodiscard]] constexpr auto top() const & -> const t &
        {
            require_non_empty();

            const auto &result = Data.top();
            return result;
        }

        [[nodiscard]] constexpr auto minimum() const & -> const t &
        {
            require_non_empty();

            const auto &result = Minimums.top();
            return result;
        }

        constexpr void push(const t &value)
        {
            assert(Minimums.size() <= Data.size());

            const auto has_minimum = Minimums.empty() || !Less(Minimums.top(), value);

            if (has_minimum)
            {
                Minimums.push(value);
            }

            try
            {
                Data.push(value);
            }
            catch (...)
            {
                if (has_minimum)
                {// Repair.
                    Minimums.pop();
                }

                throw;
            }
        }

        constexpr void pop()
        {
            require_non_empty();

            if (const auto has_minimum = !Less(Minimums.top(), Data.top()); has_minimum)
            {
                Minimums.pop();
            }

            Data.pop();
        }

private:
        constexpr void require_non_empty() const
        {
            if (Minimums.empty()) [[unlikely]]
            {
                throw std::out_of_range("The simple min stack is empty.");
            }

            if (const auto data_size = Data.size(), mini_size = Minimums.size(); data_size < mini_size) [[unlikely]]
            {
                auto err = "The simple min stack is broken: data size " + std::to_string(data_size) + " < " +
                    std::to_string(mini_size) + " min size.";

                throw std::out_of_range(err);
            }
        }

        less_t Less;
        std::stack<t> Data{};
        std::stack<t> Minimums{};
    };
} // namespace Standard::Algorithms::Numbers
