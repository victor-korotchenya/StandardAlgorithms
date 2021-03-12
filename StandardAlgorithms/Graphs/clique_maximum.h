#pragma once
#include"../Utilities/iota_vector.h"
#include"../Utilities/project_constants.h"
#include"../Utilities/require_utilities.h"
#include<numeric>
#include<vector>

namespace Standard::Algorithms::Graphs::Inner // Slow.
{
    [[nodiscard]] constexpr auto is_clique(const auto &edges, [[maybe_unused]] const std::size_t graph_size,
        const auto &clique_all_includes) noexcept -> bool
    {
        assert(0U < graph_size);

        for (const auto &tod : clique_all_includes)
        {
            assert(tod < graph_size);

            if (!edges[tod])
            {
                return false;
            }
        }

        return true;
    }

    template<class clique_t>
    inline constexpr void try_add_clique(std::vector<clique_t> &max_cliques, const clique_t &curr)
    {
        assert(!max_cliques.empty() && !curr.empty());

        const auto prev_size = max_cliques[0].size();
        if (curr.size() < prev_size)
        {
            return;
        }

        if (prev_size < curr.size())
        {
            max_cliques.clear();
        }

        max_cliques.push_back(curr);
    }

    [[nodiscard]] inline constexpr auto clique_go_deeper(
        const std::size_t current_clique_size, const std::pair<std::size_t, std::size_t> &alls_somes) noexcept -> bool
    {
        const auto &[alls, somes] = alls_somes;

        if (0U == somes)
        {
            return false;
        }

        const auto potential_future = alls + somes;
        auto shall = current_clique_size <= potential_future;
        return shall;
    }

    [[nodiscard]] inline constexpr auto clique_go_deeper_slow(const std::size_t ide,
        const std::vector<std::vector<std::size_t>> &max_cliques, const std::vector<std::size_t> &all_includes,
        const std::size_t graph_size) -> bool
    {
        assert(ide < graph_size && !max_cliques.empty());

        const auto current_clique_size = max_cliques.at(0).size();

        assert(current_clique_size <= graph_size && all_includes.size() <= graph_size);

        const auto leftover_size = graph_size - (ide + 1ZU);
        auto shall = ide + 1ZU < graph_size &&
            clique_go_deeper(current_clique_size, std::make_pair(all_includes.size(), leftover_size));

        return shall;
    }

    [[nodiscard]] constexpr auto enum_cliques_slow(const auto &graph, const std::size_t ide,
        std::vector<std::vector<std::size_t>> &max_cliques, std::vector<std::size_t> &all_includes
#if _DEBUG
        ,
        std::uint32_t depth = {}
#endif
        ) -> std::uint64_t
    {
#if _DEBUG
        if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
        {
            throw std::runtime_error("Too deep stack in enumerate cliques slow.");
        }
#endif

        assert(ide < graph.size() && graph[ide].size() == graph.size());
        assert(!max_cliques.empty() && all_includes.size() < graph.size());

        std::uint64_t steps{ 1 };

        if (const auto &edges = graph[ide]; is_clique(edges, graph.size(), all_includes))
        {// Add cliques in the sorted order.
            all_includes.push_back(ide);

            if (clique_go_deeper_slow(ide, max_cliques, all_includes, graph.size()))
            {
                steps += enum_cliques_slow(graph, ide + 1ZU, max_cliques, all_includes
#if _DEBUG
                    ,
                    depth
#endif
                );
            }

            try_add_clique(max_cliques, all_includes);

            assert(!all_includes.empty() && all_includes.back() == ide);
            all_includes.pop_back();
        }

        if (clique_go_deeper_slow(ide, max_cliques, all_includes, graph.size()))
        {// Omit.
            steps += enum_cliques_slow(graph, ide + 1ZU, max_cliques, all_includes
#if _DEBUG
                ,
                depth
#endif
            );
        }

        return steps;
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs::Inner // fast
{
    template<class graph_t1, std::unsigned_integral int_t1, class clique_t1>
    struct clique_maximum_context final
    {
        using graph_t = graph_t1;
        using int_t = int_t1;
        using clique_t = clique_t1;

        explicit constexpr clique_maximum_context(const graph_t &graph)
            : Graph(graph)
            , Some_candidates(Standard::Algorithms::Utilities::iota_vector<int_t>(graph.size()))
        {
            const auto size1 = size();
            assert(0U < size1);

            All_includes.reserve(size1);
            None_bans.reserve(size1);
        }

        [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
        {
            return Graph.size();
        }

        [[nodiscard]] constexpr auto clique_size() const -> std::size_t
        {
            assert(!Max_cliques.empty());

            return Max_cliques.at(0).size();
        }

        [[nodiscard]] constexpr auto shall_go_deeper() const -> bool
        {
            if (Some_candidates.empty())
            {
                return false;
            }

            const auto current_clique_size = clique_size();
            auto shall =
                clique_go_deeper(current_clique_size, std::make_pair(All_includes.size(), Some_candidates.size()));
            return shall;
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const graph_t &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto max_cliques() &noexcept -> std::vector<clique_t> &
        {
            return Max_cliques;
        }

        [[nodiscard]] constexpr auto all_includes() &noexcept -> clique_t &
        {
            return All_includes;
        }

        [[nodiscard]] constexpr auto some_candidates() &noexcept -> clique_t &
        {
            return Some_candidates;
        }

        [[nodiscard]] constexpr auto none_bans() &noexcept -> clique_t &
        {
            return None_bans;
        }

private:
        const graph_t &Graph;
        std::vector<clique_t> Max_cliques = std::vector<clique_t>(1); // Add an empty clique to simplify the code.
        clique_t All_includes{};
        clique_t Some_candidates;
        clique_t None_bans{};
    };

    template<class clique_t, std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto ban_unlinked_candidates(
        clique_t &froms, const int_t &ide, const auto &edges, clique_t &tods) -> std::size_t
    {
        assert(ide < edges.size());

        const auto old_size = tods.size();

        for (int_t index{}; index < froms.size();)
        {
            auto &cand = froms[index];
            assert(cand != ide && cand < edges.size());

            if (edges[cand])
            {
                ++index;
                continue;
            }

            // Don't preserve the original order as the ides will be back inserted later.
            tods.push_back(cand);

            cand = froms.back();
            froms.pop_back();
        }

        return old_size;
    }

    template<class clique_t, std::unsigned_integral int_t>
    constexpr void push_leftovers(clique_t &froms, const int_t &old_size, clique_t &tods)
    {
        assert(&froms != &tods);
        assert(old_size <= froms.size());

        while (old_size < froms.size())
        {// The order might change.
            auto &back1 = froms.back();
            tods.push_back(back1);
            froms.pop_back();
        }
    }

    template<class context_t>
    [[nodiscard]] constexpr auto enum_cliques(context_t &context
#if _DEBUG
        ,
        std::uint32_t depth = {}
#endif
        ) -> std::uint64_t
    {
#if _DEBUG
        if (::Standard::Algorithms::Utilities::stack_max_size < ++depth) [[unlikely]]
        {
            throw std::runtime_error("Too deep stack in enumerate cliques.");
        }
#endif

        using int_t = typename context_t::int_t;
        using clique_t = typename context_t::clique_t;

        const auto old_bans_size = context.none_bans().size();

        assert(old_bans_size < context.size());
        assert(context.shall_go_deeper());
        assert(!context.some_candidates().empty() && context.some_candidates().size() <= context.size());
        assert(context.all_includes().size() < context.size() && context.clique_size() < context.size());

        std::uint64_t steps{ 1 };

        do
        {
            const auto ide = context.some_candidates().back(); // todo(p3): choose the thickest node.
            assert(ide < context.size() && context.graph()[ide].size() == context.size());

            const auto &edges = context.graph()[ide];
            assert(is_clique(edges, context.size(), context.all_includes()));

            context.none_bans().push_back(ide);
            context.all_includes().push_back(ide);
            context.some_candidates().pop_back();

            const auto mib_size =
                ban_unlinked_candidates<clique_t, int_t>(context.some_candidates(), ide, edges, context.none_bans());

            if (context.shall_go_deeper())
            {
                steps += enum_cliques<context_t>(context
#if _DEBUG
                    ,
                    depth
#endif
                );
            }
            else
            {
                try_add_clique(context.max_cliques(), context.all_includes());
            }

            assert(!context.all_includes().empty() && ide == context.all_includes().back());
            context.all_includes().pop_back();

            push_leftovers<clique_t, int_t>(context.none_bans(), mib_size, context.some_candidates());
        } while (context.shall_go_deeper());

        assert(old_bans_size < context.none_bans().size());
        push_leftovers<clique_t, int_t>(context.none_bans(), old_bans_size, context.some_candidates());

        return steps;
    }
} // namespace Standard::Algorithms::Graphs::Inner

namespace Standard::Algorithms::Graphs
{
    constexpr auto slow_clique_max_size = 20U;

    // A graph must be simple: no self-loops, no multi-edges.
    // todo(p4): Bron+Kerbosch.
    // Still slow time O(3**(n/3) * n).

    template<class graph_t, std::unsigned_integral int_t = std::size_t, class clique_t = std::vector<int_t>>
    [[nodiscard]] constexpr auto clique_maximum_still(const graph_t &graph)
        -> std::pair<std::vector<clique_t>, std::uint64_t>
    {
        const auto size = static_cast<std::size_t>(graph.size());
        if (size <= 1U)
        {
            return { { size, clique_t(size, 0) }, 0 };
        }

        {
            constexpr auto max_size = 40U;
            static_assert(slow_clique_max_size < max_size);

            require_less_equal(size, max_size, "too large size");
        }

        Inner::clique_maximum_context<graph_t, int_t, clique_t> context(graph);
        auto &max_cliques = context.max_cliques();
        assert(!max_cliques.empty() && max_cliques.front().empty());

        const auto steps = Inner::enum_cliques(context);
        assert(!max_cliques.empty() && !max_cliques.front().empty());

        [[maybe_unused]] const auto clique_size = max_cliques.front().size();

        for (auto &clique : max_cliques)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                assert(clique_size == clique.size());
            }

            std::sort(clique.begin(), clique.end());
        }

        std::sort(max_cliques.begin(), max_cliques.end());
        return { std::move(max_cliques), steps };
    }

    // Slow time O(2**n * n).
    [[nodiscard]] constexpr auto clique_maximum_slow(const auto &graph)
        -> std::pair<std::vector<std::vector<std::size_t>>, std::uint64_t>
    {
        const auto size = static_cast<std::size_t>(graph.size());
        if (0U == size)
        {
            return {};
        }

        require_less_equal(size, slow_clique_max_size, "too large size");

        std::vector<std::vector<std::size_t>> max_cliques(1); // Add an empty clique to simplify the code.
        assert(!max_cliques.empty() && max_cliques.front().empty());

        std::vector<std::size_t> all_includes;
        all_includes.reserve(size);

        const auto steps = Inner::enum_cliques_slow(graph, 0ZU, max_cliques, all_includes);
        assert(!max_cliques.empty() && !max_cliques.front().empty());

        return { std::move(max_cliques), steps };
    }
} // namespace Standard::Algorithms::Graphs
