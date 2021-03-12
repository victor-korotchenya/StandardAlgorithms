#pragma once
#include"../Numbers/max_probability.h"
#include"../Utilities/require_utilities.h"
#include<unordered_map>
#include<vector>

namespace Standard::Algorithms::Strings::Inner
{
    template<std::floating_point probability_t, std::unsigned_integral int_t>
    [[nodiscard]] constexpr auto find_most_probable_link(const int_t &graph_size, const auto &currents) noexcept
        -> std::pair<int_t, std::pair<int_t, probability_t>>
    {
        assert(0U < graph_size && !currents.empty());

        probability_t maximum_probability{};
        auto best_from = graph_size;
        auto best_tod = graph_size;

        for (const auto &[tod, from_prob] : currents)
        {
            const auto &[from, prob] = from_prob;

            assert(probability_t{} < prob && prob <= ::Standard::Algorithms::Numbers::max_probability);
            assert(tod < graph_size && from < graph_size);

            if (maximum_probability < prob)
            {
                maximum_probability = prob;
                best_from = from;
                best_tod = tod;
            }
        }

        assert(probability_t{} < maximum_probability &&
            maximum_probability <= ::Standard::Algorithms::Numbers::max_probability);

        assert(best_from < graph_size && best_tod < graph_size);

        return { best_tod, { best_from, maximum_probability } };
    }

    template<std::floating_point probability_t>
    constexpr void fill_currents(const auto &prevs, const auto &graph, const auto &cha, auto &currents)
    {
        assert(!prevs.empty());

        for (const auto &[from, penult_prob] : prevs)
        {
            const auto &prev_prob = penult_prob.second;

            assert(probability_t{} < prev_prob && prev_prob <= ::Standard::Algorithms::Numbers::max_probability);

            const auto &edges = graph.at(from);
            const auto cha_iter = edges.find(cha);

            if (cha_iter == edges.end())
            {
                continue;
            }

            const auto &to_links = cha_iter->second;
            assert(!to_links.empty());

            for (const auto &[tod, link_probability] : to_links)
            {
                assert(probability_t{} < link_probability &&
                    link_probability <= ::Standard::Algorithms::Numbers::max_probability);

                auto cand = prev_prob * link_probability;

                if (constexpr probability_t unprob{}; cand == unprob)
                {// Multiplying too many small values.
                    continue;
                }

                assert(probability_t{} < cand && cand <= ::Standard::Algorithms::Numbers::max_probability);

                auto &cur = currents[tod];
                auto &probab = cur.second;

                if (probab < cand)
                {
                    auto &node_from = cur.first;
                    node_from = from;
                    probab = cand;
                }
            }
        }
    }

    template<std::floating_point probability_t, std::unsigned_integral int_t>
    constexpr void backtrack_text_recog_nodes(
        const int_t &text_size, const auto &umba, const int_t &graph_size, std::vector<int_t> &trail, int_t cur_node)
    {
        require_positive(text_size, "text size");
        require_positive(graph_size, "graph size");

        for (int_t index = text_size - 1U; int_t{} < index; --index)
        {
            const auto &currents = umba.at(index);
            const auto iter = currents.find(cur_node);

            if (iter == currents.end()) [[unlikely]]
            {
                auto err = "While backtracking at " + std::to_string(index) + " not found the node " +
                    std::to_string(cur_node);

                throw std::runtime_error(err);
            }

            const auto &prev_node = iter->second.first;

            assert(iter->first == cur_node && prev_node < graph_size && cur_node < graph_size);

            assert(probability_t{} < iter->second.second &&
                iter->second.second <= ::Standard::Algorithms::Numbers::max_probability);

            trail.push_back(prev_node);
            cur_node = prev_node;
        }

        std::reverse(trail.begin(), trail.end());
    }
} // namespace Standard::Algorithms::Strings::Inner

namespace Standard::Algorithms::Strings
{
    // todo(p3): Markov hidden model. Viterbi

    // Speech, text recognition. Given a non-empty text,
    // a directed graph, where each edge has a character and positive probability,
    // and a starting node,
    // the aim is to compute the most probable trail in the graph with the trail labels, equal to the text.
    // If the text cannot be produced, return 0 probability.
    // The graph might have self-loops,
    // but multi-edges with the same 'to node' and character are not allowed.
    template<std::floating_point probability_t, class text_t, class graph_t, std::unsigned_integral int_t = std::size_t>
    [[nodiscard]] constexpr auto text_recognition(const text_t &text, const graph_t &graph, std::vector<int_t> &trail,
        const int_t &starting_node = {}) -> probability_t
    {
        const auto text_size = require_positive(text.size(), "text size");
        const auto graph_size = require_positive(graph.size(), "graph size");
        require_between(int_t{}, starting_node, graph_size - 1ZU, "starting node");

        trail.clear();

        using from_prob_t = std::pair<int_t, probability_t>;
        using to_map_t = std::unordered_map<int_t, from_prob_t>;

        std::vector<to_map_t> umba(text_size + 1LLU);
        umba.at(0)[starting_node] = std::make_pair(starting_node, ::Standard::Algorithms::Numbers::max_probability);

        for (int_t index{}; index < text_size; ++index)
        {
            const auto &cha = text[index];
            const auto &prevs = umba.at(index);
            auto &currents = umba.at(index + 1LLU);

            Inner::fill_currents<probability_t>(prevs, graph, cha, currents);

            if (currents.empty())
            {
                return {};
            }
        }

        const auto &last_currents = umba.at(text_size);
        const auto [best_tod, from_maxprob] =
            Inner::find_most_probable_link<probability_t, int_t>(graph_size, last_currents);

        const auto [best_from, maximum_probability] = from_maxprob;

        assert(probability_t{} < maximum_probability &&
            maximum_probability <= ::Standard::Algorithms::Numbers::max_probability);

        assert(best_from < graph_size && best_tod < graph_size);

        trail.push_back(best_tod);
        trail.push_back(best_from);

        Inner::backtrack_text_recog_nodes<probability_t, int_t>(text_size, umba, graph_size, trail, best_from);

        assert(trail.at(0) == starting_node);

        return maximum_probability;
    }
} // namespace Standard::Algorithms::Strings
