#include"text_recognition_tests.h"
#include"../Utilities/stringer.h"
#include"../Utilities/test_utilities.h"
#include"text_recognition.h"
#include<unordered_set>

namespace
{
    using int_t = std::size_t;
    using probability_t = Standard::Algorithms::floating_point_type;
    using text_t = std::string;
    using character_t = text_t::value_type;

    [[nodiscard]] constexpr auto is_valid_character(const character_t &cha) noexcept -> bool
    {
        constexpr character_t min_char = 'a';
        constexpr character_t max_char = 'z';

        auto good = !(cha < min_char) && !(max_char < cha);
        return good;
    }

    static_assert(is_valid_character('a'));
    static_assert(!is_valid_character('!'));

    [[nodiscard]] constexpr auto character_to_string(const character_t &cha) -> std::string
    {
        return ::Standard::Algorithms::Utilities::zu_string(static_cast<std::int32_t>(cha));
    }

    constexpr void require_character(const character_t &cha)
    {
        if (is_valid_character(cha)) [[likely]]
        {
            return;
        }

        auto err = "The character '" + character_to_string(cha) + "' is not allowed.";

        throw std::runtime_error(err);
    }

    constexpr void require_text(const text_t &text)
    {
        Standard::Algorithms::require_positive(text.size(), "text size");

        for (const auto &cha : text)
        {
            require_character(cha);
        }
    }

    constexpr void require_probability_initial(const std::string &message, const std::floating_point auto &probability)
    {
        assert(!message.empty());

        constexpr probability_t min_probability{ 0.01 };
        static_assert(probability_t{} < min_probability);

        Standard::Algorithms::require_between(
            min_probability, probability, ::Standard::Algorithms::Numbers::max_probability, message);
    }

    constexpr void require_probability_computed(const std::string &message, const std::floating_point auto &probability)
    {
        assert(!message.empty());

        Standard::Algorithms::require_positive(probability, message);

        Standard::Algorithms::require_less_equal(probability, ::Standard::Algorithms::Numbers::max_probability, message);
    }

    struct probab_node_t final
    {
        int_t tod{};
        probability_t probability{};
    };

    auto operator<< (std::ostream &str, const probab_node_t &node) -> std::ostream &
    {
        str << node.tod << " " << node.probability;
        return str;
    }

    using to_links_t = std::vector<probab_node_t>;
    using edges_t = std::unordered_map<character_t, to_links_t>;
    using graph_t = std::vector<edges_t>;

    template<class set_t>
    constexpr void check_probab_graph(const graph_t &graph)
    {
        const auto size = Standard::Algorithms::require_positive(graph.size(), "graph size");

        Standard::Algorithms::Utilities::stringer message{};
        set_t used_nodes{};

        for (int_t from{}; from < size; ++from)
        {
            const auto &edges = graph[from];

            message.initialize(std::string("edge from ")
                                   .append(::Standard::Algorithms::Utilities::zu_string(from))
                                   .append(" character "));

            for (const auto &[cha, to_links] : edges)
            {
                require_character(cha);

                message.reset_append(character_to_string(cha));
                message.append(" link ");
                Standard::Algorithms::throw_if_empty(message, to_links);

                used_nodes.clear();
                probability_t prob_sum{};

                for (const auto &link : to_links)
                {
                    require_probability_initial(message, link.probability);
                    prob_sum += link.probability;
                    Standard::Algorithms::require_greater(size, link.tod, message);

                    if (const auto par = used_nodes.insert(link.tod); !par.second) [[unlikely]]
                    {
                        message.append(" the node ");
                        message.append(::Standard::Algorithms::Utilities::zu_string(link.tod));
                        message.append(" has already been added");

                        throw std::runtime_error(message);
                    }
                }

                message.append(" probability sum");
                require_probability_initial(message, prob_sum);
            }
        }
    }

    [[nodiscard]] // todo(p3): constexpr
    auto
    demand_link_exists(const int_t from, const graph_t &graph, const int_t tod, const character_t &cha)
        -> const probab_node_t &
    {
        const auto size = graph.size();
        Standard::Algorithms::require_greater(size, from, "from node");
        Standard::Algorithms::require_greater(size, tod, "to node");
        require_character(cha);

        const auto &edges = graph.at(from);

        if (const auto links_iter = edges.find(cha); links_iter != edges.end()) [[likely]]
        {
            const auto &links = links_iter->second;

            if (const auto found = std::find_if(links.cbegin(), links.cend(),
                    [tod](const auto &link)
                    {// todo(p4): faster?
                        return link.tod == tod;
                    });
                found != links.cend()) [[likely]]
            {
                return *found;
            }
        }

        auto err = "The edge from " + ::Standard::Algorithms::Utilities::zu_string(from) + " to " +
            ::Standard::Algorithms::Utilities::zu_string(tod) + " via character " + character_to_string(cha) +
            " not found";

        throw std::runtime_error(err);
    }

    [[nodiscard]] constexpr auto compute_trail_probability(
        const text_t &text, const graph_t &graph, const std::vector<int_t> &trail, int_t prev) -> probability_t
    {
        Standard::Algorithms::require_positive(text.size(), "text size");
        ::Standard::Algorithms::ert::are_equal(text.size() + 1LLU, trail.size(), "trail size");
        ::Standard::Algorithms::ert::are_equal(prev, trail.at(0), "trail[0]");

        const auto size = Standard::Algorithms::require_positive(graph.size(), "graph size");
        probability_t probability{ 1 };

        for (int_t index = 1; index < trail.size(); ++index)
        {
            const auto &cha = text.at(index - 1U);
            const auto &cur = trail[index];
            Standard::Algorithms::require_greater(size, cur, "trail node");

            {
                const auto link = demand_link_exists(prev, graph, cur, cha);
                ::Standard::Algorithms::ert::are_equal(cur, link.tod, "edge to");
                require_probability_initial("found link probability", link.probability);

                probability *= link.probability;
            }

            prev = cur;
        }

        require_probability_computed("computed trail probability", probability);

        return probability;
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, text_t &&text, graph_t &&graph, probability_t probability = {},
            std::vector<int_t> &&trail = {}, int_t starting_node = {}) noexcept
            : base_test_case(std::move(name))
            , Text(std::move(text))
            , Graph(std::move(graph))
            , Trail(std::move(trail))
            , Probability(probability)
            , Starting_node(starting_node)
        {
        }

        [[nodiscard]] constexpr auto text() const &noexcept -> const text_t &
        {
            return Text;
        }

        [[nodiscard]] constexpr auto graph() const &noexcept -> const graph_t &
        {
            return Graph;
        }

        [[nodiscard]] constexpr auto starting_node() const noexcept -> int_t
        {
            return Starting_node;
        }

        [[nodiscard]] constexpr auto probability() const noexcept -> probability_t
        {
            return Probability;
        }

        [[nodiscard]] constexpr auto is_unlikely() const noexcept -> bool
        {
            auto unlikely = probability_t{} == Probability;
            return unlikely;
        }

        [[nodiscard]] constexpr auto trail() const &noexcept -> const std::vector<int_t> &
        {
            return Trail;
        }

        void print(std::ostream &str) const override
        {
            str << ", Text '" << Text << "', starting node " << Starting_node;
            ::Standard::Algorithms::print("Graph", Graph, str);

            ::Standard::Algorithms::print("expected trail", Trail, str);
            ::Standard::Algorithms::print_value("expected probability", str, Probability);
        }

        void validate() const override
        {
            base_test_case::validate();

            require_text(Text);
            check_probab_graph<std::unordered_set<int_t>>(Graph);

            if (is_unlikely())
            {
                ::Standard::Algorithms::ert::are_equal(0U, Trail.size(), "Trail size when no solution");
                return;
            }

            require_probability_computed("Expected probability", Probability);

            const auto actual_probability = compute_trail_probability(Text, Graph, Trail, Starting_node);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(Probability, actual_probability, "actual probability");
        }

private:
        text_t Text;
        graph_t Graph;
        std::vector<int_t> Trail;
        probability_t Probability;
        int_t Starting_node;
    };

    // todo(p3): constexpr
    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr probability_t half = 0.5;

        test_cases.emplace_back("trivial single start node 2", "t",
            graph_t{
                edges_t{}, // 0
                edges_t{ { 't', to_links_t{ { 2, ::Standard::Algorithms::Numbers::max_probability } } } }, // 1
                edges_t{ { 't', to_links_t{ { 0, half }, { 1, half } } },
                    { 'b', to_links_t{ { 1, half }, { 2, half } } } }, // 2
                edges_t{} // 3
            },
            half, std::vector<int_t>{ 2, 1 }, 2);

        test_cases.emplace_back("trivial single", "t",
            graph_t{
                edges_t{ { 't', to_links_t{ { 1, half } } } }, // 0
                edges_t{} // 1
            },
            half, std::vector<int_t>{ 0, 1 });

        test_cases.emplace_back("impossible trivial", "t", graph_t(1));

        test_cases.emplace_back("impossible trivial 2", "t",
            graph_t{
                edges_t{ { 'i', to_links_t{ { 1, half } } } }, // 0
                edges_t{} // 1
            });

        {
            constexpr probability_t prob_0_1 = 0.1;
            constexpr probability_t prob_1_2 = 0.2;
            constexpr probability_t prob_0_3 = 0.17;
            constexpr probability_t prob_3_2 = 0.3;

            constexpr auto best_prob = prob_0_3 * prob_3_2;

            test_cases.emplace_back("best of 2", "bt",
                // 0 -> 1 -> 2, prob = 0.1 * 0.2
                // 0 -> 3 -> 2, prob = 0.17 * 0.3 ; It is max.
                graph_t{
                    edges_t{ { 'b', to_links_t{ { 1, prob_0_1 }, { 3, prob_0_3 } } } }, // 0
                    edges_t{ { 't', to_links_t{ { 2, prob_1_2 } } } }, // 1
                    edges_t{}, // 2
                    edges_t{ { 't', to_links_t{ { 2, prob_3_2 } } } } // 3
                },
                best_prob, std::vector<int_t>{ 0, 3, 2 });

            constexpr auto rev_best_prob_2 = prob_0_1 * prob_1_2;

            test_cases.emplace_back("rev best of 2 another", "bt",
                // 0 -> 1 -> 2, prob = 0.1 * 0.2 ; It is max.
                // 0 -> 3 -> 2, prob = 0.17/2 * 0.3/2
                graph_t{
                    edges_t{ { 'b', to_links_t{ { 1, prob_0_1 }, { 3, prob_0_3 * half } } } }, // 0
                    edges_t{ { 't', to_links_t{ { 2, prob_1_2 } } } }, // 1
                    edges_t{}, // 2
                    edges_t{ { 't', to_links_t{ { 2, prob_3_2 * half } } } } // 3
                },
                rev_best_prob_2, std::vector<int_t>{ 0, 1, 2 });
        }
        // todo(p3): more tests.
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<int_t> trail{};

        const auto actual_probability = Standard::Algorithms::Strings::text_recognition<probability_t, text_t, graph_t>(
            test.text(), test.graph(), trail, test.starting_node());

        ::Standard::Algorithms::ert::are_equal_with_epsilon(
            test.probability(), actual_probability, "computed probability");

        if (!test.is_unlikely())
        {
            ::Standard::Algorithms::ert::are_equal(test.trail(), trail, "computed trail");
        }
    }
} // namespace

void Standard::Algorithms::Strings::Tests::text_recognition_tests()
{
    ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
