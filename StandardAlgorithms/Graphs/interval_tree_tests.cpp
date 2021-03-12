#include"interval_tree_tests.h"
#include"../Numbers/hash_utilities.h"
#include"../Utilities/random_generator_pair.h"
#include"../Utilities/remove_duplicates.h"
#include"bst_tests.h"
#include"interval_tree.h"
#include<initializer_list>
#include<optional>
#include<unordered_set>

namespace
{
    using coord_t = std::int16_t;
    using key_t = std::pair<coord_t, coord_t>;
} // namespace

namespace Standard::Algorithms::Trees
{
    template<class coord_t1, class key_t1>
    auto operator<< (std::ostream &str, const interval_node<coord_t1, key_t1> &node) -> std::ostream &
    {
        str << "interval ['" << node.key.first << "', '" << node.key.second << "', count " << node.count << ", height "
            << node.height << ", has left " << (node.left != nullptr) << ", has right " << (node.right != nullptr)
            << ", max " << node.subtree_max;
        return str;
    }
} // namespace Standard::Algorithms::Trees

constexpr void Standard::Algorithms::Trees::avl_update_height(interval_node<coord_t, key_t> &node)
{
    avl_update_height_int<coord_t, key_t>(node);
}

namespace
{
    using key_rand_t = Standard::Algorithms::Utilities::random_pair_t<coord_t>;
    using key_set_t = std::unordered_set<key_t, Standard::Algorithms::Numbers::pair_hash>;

    constexpr auto random_attempts = 9ZU;

    [[nodiscard]] consteval auto is_valid_interval_tests() noexcept -> bool
    {
        const auto one = Standard::Algorithms::Trees::is_valid_interval(key_t(1, 2));
        const auto two = Standard::Algorithms::Trees::is_valid_interval(key_t(1, 1));
        const auto three = !Standard::Algorithms::Trees::is_valid_interval(key_t(2, 1));

        auto okay = one && two && three;
        return okay;
    }

    [[nodiscard]] constexpr auto run_intervals_overlapping_test(
        const key_t &interval_one, const key_t &interval_two, const bool expected = false) noexcept -> bool
    {
        if (!Standard::Algorithms::Trees::is_valid_interval(interval_one) ||
            !Standard::Algorithms::Trees::is_valid_interval(interval_two))
        {
            return false;
        }

        const auto actual = Standard::Algorithms::Trees::are_intervals_overlapping(interval_one, interval_two);

        const auto rev = Standard::Algorithms::Trees::are_intervals_overlapping(interval_two, interval_one);

        auto good = expected == actual && expected == rev;
        return good;
    }

    [[nodiscard]] consteval auto intervals_overlapping_tests() noexcept -> bool
    {
        using input_t = std::pair<key_t, key_t>;
        using test_t = std::pair<input_t, bool>;

        const std::initializer_list<test_t> tests{
            { { { -2, -1 }, { 1, 2 } }, false },
            { { { -1, -1 }, { 1, 1 } }, false },
            { { { -1, 0 }, { 1, 1 } }, false },
            { { { -1, 0 }, { 1, 2 } }, false },
            { { { -1, 0 }, { 2, 2 } }, false },
            { { { -1, 0 }, { 2, 3 } }, false },
            { { { 0, 1 }, { 1, 1 } }, true },
            { { { 0, 2 }, { 1, 1 } }, true },
            { { { 0, 3 }, { 1, 1 } }, true },
            { { { 0, 3 }, { 1, 2 } }, true },
            { { { 0, 3 }, { 1, 3 } }, true },
            { { { 1, 1 }, { 1, 1 } }, true },
            { { { 1, 1 }, { 1, 2 } }, true },
            { { { 1, 1 }, { 2, 2 } }, false },
            { { { 1, 2 }, { 1, 2 } }, true },
            { { { 1, 2 }, { 2, 2 } }, true },
        };

        auto good = std::all_of(tests.begin(), tests.end(),
            [] [[nodiscard]] (const auto &test) noexcept
            {
                const auto &[input, expected] = test;

                auto okay1 = run_intervals_overlapping_test(input.first, input.second, expected);
                return okay1;
            });

        return good;
    }

    static_assert(is_valid_interval_tests());
    static_assert(intervals_overlapping_tests());
} // namespace

// todo(p2): del it.
[[nodiscard]] auto std::to_string(const key_t &key) -> std::string
{
    return "[" + std::to_string(key.first) + ", " + std::to_string(key.second) + "]";
}

// todo(p2): del it.
auto std::operator<< (std::ostream &str, const std::pair<std::int16_t, std::int16_t> &par) -> std::ostream &
{
    str << '(' << par.first << ',' << par.second << ')';
    return str;
}

namespace
{
    using tree_t = Standard::Algorithms::Trees::interval_tree<coord_t, key_t>;
    using node_t = tree_t::node_t;

    static_assert(Standard::Algorithms::Trees::Inner::can_count_v<node_t>);

    [[nodiscard]] auto all_overlaping_intervals_slow(const key_set_t &source_keys, const key_t &source_interval)
        -> std::optional<std::vector<key_t>>
    {
        Standard::Algorithms::Trees::require_valid_interval(source_interval);

        std::vector<key_t> matches;

        for (const auto &key : source_keys)
        {
            Standard::Algorithms::Trees::require_valid_interval(key);

            const auto crossing = Standard::Algorithms::Trees::are_intervals_overlapping(source_interval, key);
            {
                const auto rev = Standard::Algorithms::Trees::are_intervals_overlapping(key, source_interval);
                ::Standard::Algorithms::ert::are_equal(crossing, rev, "are_intervals_overlapping and rev");
            }

            if (crossing)
            {
                matches.push_back(key);
            }
        }

        if (matches.empty())
        {
            return std::nullopt;
        }

        Standard::Algorithms::sort_remove_duplicates(matches);

        return matches;
    }

    [[nodiscard]] auto build_tree_keys() -> key_set_t
    {
        return {// NOLINTNEXTLINE
            { 5, 5 }, { 10, 20 }, { 10, 21 }, { 10, 22 }, // NOLINTNEXTLINE
            { 20, 40 }, { 25, 25 }, { 100, 125 }, { 125, 130 }, // NOLINTNEXTLINE
            { 200, 225 }, { 226, 230 }, { 300, 325 }, { 327, 330 }
        };
    }

    void fill_tree(const key_set_t &keys, tree_t &tree)
    {
        Standard::Algorithms::throw_if_empty("keys", keys);

        for (const auto &key : keys)
        {
            const auto actual = tree.insert(key).second;

            ::Standard::Algorithms::ert::are_equal(
                true, actual, "Add interval [" + std::to_string(key.first) + ", " + std::to_string(key.second) + "]");
        }
    }

    [[nodiscard]] constexpr auto build_overlaping_cases()
    {
        using expected_t = std::vector<key_t>;
        using record_t = std::pair<key_t, std::optional<expected_t>>;

        return std::vector<record_t>{
            // NOLINTNEXTLINE
            { { -10, 0 }, std::nullopt }, // NOLINTNEXTLINE
            { { -10, 4 }, std::nullopt }, // NOLINTNEXTLINE
            { { 0, 0 }, std::nullopt }, // NOLINTNEXTLINE
            { { 0, 4 }, std::nullopt }, // NOLINTNEXTLINE
            { { 5, 5 }, expected_t{ { 5, 5 } } }, // NOLINTNEXTLINE
            { { 6, 6 }, std::nullopt }, // NOLINTNEXTLINE
            { { 6, 8 }, std::nullopt }, // NOLINTNEXTLINE
            { { 6, 9 }, std::nullopt }, // NOLINTNEXTLINE
            { { 6, 9 }, std::nullopt }, // NOLINTNEXTLINE
            { { 8, 10 }, expected_t{ { 10, 20 }, { 10, 21 }, { 10, 22 } } }, // NOLINTNEXTLINE
            { { 8, 20 }, expected_t{ { 10, 20 }, { 10, 21 }, { 10, 22 }, { 20, 40 } } }, // NOLINTNEXTLINE
            { { 10, 10 }, expected_t{ { 10, 20 }, { 10, 21 }, { 10, 22 } } }, // NOLINTNEXTLINE
            { { 10, 12 }, expected_t{ { 10, 20 }, { 10, 21 }, { 10, 22 } } }, // NOLINTNEXTLINE
            { { 125, 125 }, expected_t{ { 100, 125 }, { 125, 130 } } }, // NOLINTNEXTLINE
            { { 225, 225 }, expected_t{ { 200, 225 } } }, // NOLINTNEXTLINE
            { { 225, 226 }, expected_t{ { 200, 225 }, { 226, 230 } } }, // NOLINTNEXTLINE
            { { 225, 227 }, expected_t{ { 200, 225 }, { 226, 230 } } }, // NOLINTNEXTLINE
            { { 226, 226 }, expected_t{ { 226, 230 } } }, // NOLINTNEXTLINE
            { { 226, 227 }, expected_t{ { 226, 230 } } }, // NOLINTNEXTLINE
            { { 326, 326 }, std::nullopt }, // NOLINTNEXTLINE
        };
    }

    constexpr void run_test(const key_set_t &source_keys, const tree_t &tree, const std::size_t index,
        const key_t &input, const std::optional<std::vector<key_t>> &outputs, std::vector<key_t> &matches)
    {
        {
            Standard::Algorithms::Trees::all_overlaping_intervals(tree, input, matches);

            if (outputs.has_value())
            {
                ::Standard::Algorithms::ert::are_equal(outputs.value(), matches, "all_overlaping_intervals");
            }
            else
            {
                ::Standard::Algorithms::ert::empty(matches, "all_overlaping_intervals");
            }
        }

        const auto *const node = Standard::Algorithms::Trees::some_overlaping_interval(tree, input);

        if (node == nullptr)
        {
            if (!outputs.has_value()) [[likely]]
            {
                return;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "some_overlaping_interval at test index " << index;
            ::Standard::Algorithms::print(", input ", input, str);
            ::Standard::Algorithms::print(", got null, expected any of outputs", outputs.value(), str);

            throw std::runtime_error(str.str());
        }

        if (!Standard::Algorithms::Trees::is_valid_interval((node->key))) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "some_overlaping_interval at test index " << index;
            ::Standard::Algorithms::print(", input ", input, str);
            ::Standard::Algorithms::print(", got invalid interval ", node->key, str);

            throw std::runtime_error(str.str());
        }

        if (!outputs.has_value()) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "some_overlaping_interval at test index ";
            ::Standard::Algorithms::print(", input", input, str);
            ::Standard::Algorithms::print(", expected null, got", node->key, str);

            throw std::runtime_error(str.str());
        }

        const auto &values = outputs.value();

        if (const auto iter = std::find(values.cbegin(), values.cend(), node->key); iter == values.cend()) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "some_overlaping_interval at test index " << index;
            ::Standard::Algorithms::print(", input", input, str);
            ::Standard::Algorithms::print(", expected any of outputs", values, str);
            ::Standard::Algorithms::print(", got unexpected", node->key, str);

            throw std::runtime_error(str.str());
        }

        if (!source_keys.contains(node->key)) [[unlikely]]
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "some_overlaping_interval at test index " << index;
            ::Standard::Algorithms::print(", input", input, str);
            ::Standard::Algorithms::print(", Wrong expected outputs", values, str);

            throw std::runtime_error(str.str());
        }
    }

    void run_preset_tests()
    {
        const auto source_keys = build_tree_keys();
        tree_t tree{};
        fill_tree(source_keys, tree);
        std::vector<key_t> matches;

        const auto tests = build_overlaping_cases();

        std::size_t index{};
        --index;

        for (const auto &[input, outputs] : tests)
        {
            ++index;

            {
                const auto slow = all_overlaping_intervals_slow(source_keys, input);
                ::Standard::Algorithms::ert::are_equal(
                    outputs, slow, "all_overlaping_intervals_slow at " + std::to_string(index));
            }

            run_test(source_keys, tree, index, input, outputs, matches);
        }
    }

    void random_tests(key_set_t &source_keys, tree_t &tree)
    {
        key_rand_t key_rand{};
        std::vector<key_t> matches;

        for (std::size_t att{}; att < random_attempts; ++att)
        {
            {
                const auto input = key_rand();
                Standard::Algorithms::Trees::require_valid_interval(input);

                const auto outputs = all_overlaping_intervals_slow(source_keys, input);
                run_test(source_keys, tree, att, input, outputs, matches);
            }
            {
                const auto interval = key_rand();
                Standard::Algorithms::Trees::require_valid_interval(interval);

                const auto exists = source_keys.insert(interval).second;
                const auto actual = tree.insert(interval).second;
                ::Standard::Algorithms::ert::are_equal(exists, actual,
                    "Add interval [" + std::to_string(interval.first) + ", " + std::to_string(interval.second) + "]");
            }
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::interval_tree_tests()
{
    {
        constexpr auto min_coord = std::numeric_limits<coord_t>::min();
        constexpr auto max_coord = std::numeric_limits<coord_t>::max();

        constexpr key_t min_key{ min_coord, min_coord };
        constexpr key_t max_key{ max_coord, max_coord };

        bst_tests<key_t, tree_t, key_rand_t, node_t>(nullptr, min_key, max_key);
    }

    run_preset_tests();
    {
        key_set_t keys{};
        tree_t tree{};

        try
        {
            random_tests(keys, tree);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            ::Standard::Algorithms::print("random keys", keys, str);
            str << " Error: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }
}
