#include"tarjan_scc.h"
#include<algorithm>
#include<cassert>
#include<cstring>
#include<stdexcept>
#include<string>

namespace
{
    constexpr auto scc_max_size = Standard::Algorithms::Graphs::scc_max_size;

    struct scc_context final
    {
        // Input: a directed graph.
        std::array<std::vector<std::int32_t>, scc_max_size> graph{};
        std::int32_t size{};

        std::int32_t stack_size{};
        std::array<std::int32_t, scc_max_size> stack1{};

        // Output:
        std::int32_t comp_cnt{};
        std::array<std::int32_t, scc_max_size> vert_to_comp{};

        // often only one needed of {vert_to_comp, components}.
        std::vector<std::vector<std::int32_t>> components{};

        // Temporary variables:
        std::int32_t visit_cnt{};
        std::array<std::int32_t, scc_max_size> lower_index{};
        std::array<std::int32_t, scc_max_size> discovery_time{};
        std::array<bool, scc_max_size> in_stack{}; // todo(p3): vector<bool>
    };

    constexpr void clear(scc_context &context)
    {
        context.comp_cnt = 0;
        context.visit_cnt = 0;

        std::fill(context.discovery_time.begin(), context.discovery_time.end(), 0);
        std::fill(context.in_stack.begin(), context.in_stack.end(), false);

        context.stack_size = 0;
        context.components.clear();
        context.components.resize(context.size);
    }

    constexpr void visit(scc_context &context, const std::int32_t pare
#if _DEBUG
        ,
        std::uint32_t depth = {}
#endif
    )
    {
#if _DEBUG
        if (static_cast<std::uint32_t>(scc_max_size) < ++depth) [[unlikely]]
        {
            throw std::runtime_error("Too large stack depth " + std::to_string(depth));
        }
#endif

        assert(0 <= pare && pare < scc_max_size);

        context.discovery_time.at(pare) = context.lower_index.at(pare) = ++context.visit_cnt;
        context.stack1.at(context.stack_size++) = pare;
        context.in_stack.at(pare) = true;

        auto &low_par = context.lower_index.at(pare);

        for (const auto &tods = context.graph.at(pare); const auto &tod : tods)
        {
            if (0 == context.discovery_time.at(tod))
            {
                visit(context, tod
#if _DEBUG
                    ,
                    depth
#endif
                );

                low_par = std::min(low_par, context.lower_index.at(tod));
            }
            else if (context.in_stack.at(tod))
            {
                low_par = std::min(low_par, context.discovery_time.at(tod));
            }
        }

        if (low_par != context.discovery_time.at(pare))
        {
            return;
        }

        auto &compo = context.components.at(context.comp_cnt);
        compo.clear();

        std::int32_t from{};

        do
        {
            assert(0 < context.stack_size);

            from = context.stack1.at(context.stack_size - 1);
            context.in_stack.at(from) = false;
            context.vert_to_comp.at(from) = context.comp_cnt;
            compo.push_back(from);
            --context.stack_size;
        } while (pare != from);

        ++context.comp_cnt;
    }

    // small size test only.
    // NOLINTNEXTLINE
    scc_context test_context{};
} // namespace

[[nodiscard]] auto Standard::Algorithms::Graphs::tarjan_graph_ptr() noexcept
    -> std::array<std::vector<std::int32_t>, scc_max_size> &
{
    return test_context.graph;
}

[[nodiscard]] auto Standard::Algorithms::Graphs::tarjan_graph_size() noexcept -> std::int32_t &
{
    return test_context.size;
}

[[nodiscard]] auto Standard::Algorithms::Graphs::tarjan_component_cnt() noexcept -> std::int32_t
{
    return test_context.comp_cnt;
}

// todo(p3): test.
[[nodiscard]] auto Standard::Algorithms::Graphs::tarjan_vertex_to_component() noexcept
    -> const std::array<std::int32_t, scc_max_size> &
{
    return test_context.vert_to_comp;
}

[[nodiscard]] auto Standard::Algorithms::Graphs::tarjan_components() noexcept -> std::vector<std::vector<std::int32_t>> &
{
    return test_context.components;
}

void Standard::Algorithms::Graphs::tarjan_scc()
{
    assert(0 <= test_context.size);

    clear(test_context);

    for (std::int32_t parent{}; parent < test_context.size; ++parent)
    {
        if (0 == test_context.discovery_time.at(parent))
        {
            visit(test_context, parent);
        }
    }

    test_context.components.resize(test_context.comp_cnt);
}
