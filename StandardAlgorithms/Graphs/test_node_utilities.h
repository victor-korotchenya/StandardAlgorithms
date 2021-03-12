#pragma once
// "test_node_utilities.h"
#include"../Utilities/ert.h"
#include"binary_tree_node.h"
#include<vector>

namespace Standard::Algorithms::Trees
{
    // A cheap alternative to produce pointers.
    template<class node_t, std::integral int_t>
    [[nodiscard]] constexpr auto set_value_get_node(std::vector<node_t> &nodes, const int_t &ide) -> node_t *
    {
        require_greater(nodes.size(), static_cast<std::uint64_t>(ide), "ide");

        auto *result = &nodes[ide];

        ::Standard::Algorithms::ert::are_equal(int_t{}, result->value,
            "result->value is already set to " + std::to_string(result->value) + " for id " + std::to_string(ide));

        result->value = ide;

        return result;
    }

    template<class pair_t>
    constexpr void require_isomorphic(const pair_t &par, const std::string &message = "")
    {
        if (const auto equal1 = nullptr == par.first && nullptr == par.second; equal1)
        {
            return;
        }

        auto str = ::Standard::Algorithms::Utilities::w_stream();

        auto print_node = [&str](const auto *const node)
        {
            if (nullptr == node)
            {
                str << "null";
            }
            else
            {
                str << (*node);
            }
        };

        str << " Trees are not equal, not isomorphic: first node '";
        print_node(par.first);

        str << "', second node '";
        print_node(par.second);
        str << "'.";

        throw_exception(str, message);
    }
} // namespace Standard::Algorithms::Trees
