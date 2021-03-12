#pragma once
#include"../Utilities/require_utilities.h"
#include<vector>

namespace Standard::Algorithms::Graphs
{
    // It is edge effective, but vertex defective -(.
    template<class int_t>
    struct sparse_graph final
    {
        constexpr explicit sparse_graph(const std::pair<int_t, int_t> &vertices_edges)
            : Values(require_positive(vertices_edges.second, "Edge count") + 1LL)
            , Previous(vertices_edges.second + 1LL)
            , Lasts(require_positive(vertices_edges.first, "Vertex count"))
        {
        }

        [[nodiscard]] constexpr auto counter() const noexcept -> const int_t &
        {
            return Counter;
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return static_cast<std::size_t>(Counter);
        }

        [[nodiscard]] constexpr auto values() const noexcept -> const std::vector<int_t> &
        {
            return Values;
        }

        [[nodiscard]] constexpr auto previous() const noexcept -> const std::vector<int_t> &
        {
            return Previous;
        }

        [[nodiscard]] constexpr auto lasts() const noexcept -> const std::vector<int_t> &
        {
            return Lasts;
        }

        constexpr void add_edge(const int_t &from, const int_t &tod)
        {
            if constexpr (std::is_signed_v<int_t>)
            {
                require_non_negative(from, "from");
                require_non_negative(tod, "to");
            }

            require_greater(
                Values.size(), size() + 1LLU, "Cannot add edges as the counter has reached the maximum value");

            require_greater(static_cast<int_t>(Lasts.size()), from, "from, to = " + std::to_string(tod));

            require_greater(static_cast<int_t>(Lasts.size()), tod, "to, from = " + std::to_string(from));

            ++Counter;

            Values[Counter] = tod;

            auto &last_from = Lasts[from];
            Previous[Counter] = last_from;
            last_from = Counter;
        }

private:
        std::vector<int_t> Values;
        std::vector<int_t> Previous;
        std::vector<int_t> Lasts;
        int_t Counter{};
    };
} // namespace Standard::Algorithms::Graphs
