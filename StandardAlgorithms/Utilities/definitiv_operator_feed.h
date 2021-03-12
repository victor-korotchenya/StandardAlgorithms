#pragma once
#include<cstddef>
#include<stdexcept>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Utilities
{
    // Can be used in tests to provide sort of "random", preset input.
    // See also enumerate_choose.
    template<class int_t1>
    struct definitiv_operator_feed final
    {
        using int_t = int_t1;

public:
        constexpr explicit definitiv_operator_feed(const std::vector<int_t> &init) noexcept
            : Source_ref(init)
        {
        }

        constexpr explicit definitiv_operator_feed(std::vector<int_t> &&init = {}) noexcept
            : Source(std::move(init))
            , Source_ref(Source)
        {
        }

        constexpr explicit definitiv_operator_feed(const auto &init)
            : Source(init.begin(), init.end())
            , Source_ref(Source)
        {
        }

        [[nodiscard]] constexpr auto operator() () & -> const int_t &
        {
            if (Index < Source_ref.size()) [[likely]]
            {
                return Source_ref[Index++];
            }

            throw std::runtime_error("No more free cheese.");
        }

        // Prefer to use the other operator without arguments which is faster.
        [[nodiscard]] constexpr auto operator() (
            const int_t & /*min_inclusive*/, const int_t & /*max_inclusive*/) & -> const int_t &
        {
            return this->operator() ();
        }

        constexpr void reset() noexcept
        {
            Index = {};
        }

        [[nodiscard]] constexpr auto source() const &noexcept -> const std::vector<int_t> &
        {
            return Source_ref;
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Source_ref.size();
        }

private:
        std::size_t Index{};
        std::vector<int_t> Source{};
        const std::vector<int_t> &Source_ref;
    };

    // Deduction guide.
    template<class container_t>
    definitiv_operator_feed(container_t) -> definitiv_operator_feed<typename container_t::value_type>;
} // namespace Standard::Algorithms::Utilities
