#include"min_cut_paste_to_sort_permutation_tests.h"
#include"../Graphs/trie_map.h"
#include"../Utilities/random.h"
#include"../Utilities/remove_duplicates.h"
#include"../Utilities/test_utilities.h"
#include"min_cut_paste_to_sort_permutation.h"
#include<unordered_set>

namespace
{
    using string_t = std::string;
    using char_t = string_t::value_type;

    constexpr char_t min_key = '1';
    constexpr char_t max_key = '9';

    // distance+1, is destination
    using dist1_isdest_t = std::pair<std::int32_t, bool>;

    using node_t = Standard::Algorithms::Trees::simple_trie_map_node_t<char_t, dist1_isdest_t, min_key, max_key>;

    // Could be made 3x faster when using 'simple_trie_map_node_t'.
    using map_t = Standard::Algorithms::Trees::trie_map<char_t, dist1_isdest_t, node_t>;

    using slow_node_t = Standard::Algorithms::Trees::simple_trie_map_node_t<char_t, std::int32_t, min_key, max_key>;

    using slow_map_t = Standard::Algorithms::Trees::trie_map<char_t, std::int32_t, slow_node_t>;

    constexpr auto not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::int32_t expected) noexcept
            : base_test_case(std::move(name))
            , Expected(expected)
        {
        }

        [[nodiscard]] constexpr auto expected() const noexcept -> std::int32_t
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto &name = this->name();

            const auto size = Standard::Algorithms::require_positive(name.size(), "source size");
            {
                constexpr auto max_size = Standard::Algorithms::Numbers::mcp_sort_perm_max_size;

                ::Standard::Algorithms::ert::greater_or_equal(max_size, size, "source size");
            }
            {
                const auto [min_it, max_it] = std::minmax_element(name.cbegin(), name.cend());

                ::Standard::Algorithms::ert::are_equal(*min_it, min_key, "min key");
                Standard::Algorithms::require_less_equal(*max_it, max_key, "max key");
                {
                    const auto max_digit1 = (*max_it) - min_key + 1;

                    ::Standard::Algorithms::ert::are_equal(
                        size, static_cast<std::size_t>(max_digit1), "size vs max digit");
                }
            }
            {
                auto cop = name;

                Standard::Algorithms::sort_remove_duplicates(cop);

                ::Standard::Algorithms::ert::are_equal(
                    name.size(), cop.size(), "name must be a permutation 1, 2, .. , n.");
            }

            Standard::Algorithms::require_greater(Expected, not_computed - 1, "expected");

            Standard::Algorithms::require_greater(static_cast<std::int32_t>(size), Expected, "expected");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::int32_t Expected;
    };

    void add_random(std::vector<test_case> &test_cases)
    {
        Standard::Algorithms::require_positive(test_cases.size(), "Random tests must come at the end.");

        std::unordered_set<string_t> names;

        for (const auto &test : test_cases)
        {
            const auto [iter, added] = names.insert(test.name());

            ::Standard::Algorithms::ert::are_equal(true, added, "Test names must be unique.");
        }

        constexpr auto min_size = 2;
        constexpr auto max_size = 5;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto size = rnd();

            string_t source(size, 0);

            for (std::int32_t index{}; index < size; ++index)
            {
                source[index] = static_cast<char_t>(index + min_key);
            }

            Standard::Algorithms::Utilities::shuffle_data(source);

            if (const auto [iter, added] = names.insert(source); !added)
            {
                continue;
            }

            test_cases.emplace_back(std::move(source), not_computed);
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("54321", 3);
        test_cases.emplace_back("4321", 3);
        test_cases.emplace_back("4132", 2);
        test_cases.emplace_back("2341", 1);
        test_cases.emplace_back("321", 2);
        test_cases.emplace_back("132", 1);
        test_cases.emplace_back("231", 1);

        // Sorted permutations.
        test_cases.emplace_back("1", 0);
        test_cases.emplace_back("12", 0);
        test_cases.emplace_back("123", 0);
        test_cases.emplace_back("1234", 0);
        test_cases.emplace_back("12345", 0);
        test_cases.emplace_back("123456", 0);

        // Must be the last.
        add_random(test_cases);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto &name = test.name();

        const auto still =
            Standard::Algorithms::Numbers::min_cut_paste_to_sort_permutation_slow_still<map_t, string_t>(name);

        if (not_computed < test.expected())
        {
            ::Standard::Algorithms::ert::are_equal(
                test.expected(), still.first, "min_cut_paste_to_sort_permutation_slow_still");
        }

        const auto slow =
            Standard::Algorithms::Numbers::min_cut_paste_to_sort_permutation_slow<slow_map_t, string_t>(name);

        ::Standard::Algorithms::ert::are_equal(still.first, slow.first, "min_cut_paste_to_sort_permutation_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::min_cut_paste_to_sort_permutation_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests<all_cores>(run_test_case, generate_test_cases);
}
