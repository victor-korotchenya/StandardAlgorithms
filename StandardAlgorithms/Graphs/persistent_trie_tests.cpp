#include"persistent_trie_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"persistent_trie.h"

namespace
{
    using string_t = std::string;

    struct oper_t final
    {
        std::string str{};
        std::size_t version{};
        bool is_add{};
        bool expected{};
    };

    constexpr void validate_operation(const oper_t &oper)
    {
        Standard::Algorithms::require_positive(oper.str.size(), "string size");

        if (oper.is_add)
        {
            Standard::Algorithms::require_positive(oper.version, "version");
        }
    }

    auto operator<< (std::ostream &str, const oper_t &oper) -> std::ostream &
    {
        if (oper.is_add)
        {
            str << "Add '" << oper.str << "' " << oper.expected << " new version " << oper.version;
        }
        else
        {
            str << "Contains '" << oper.str << "' version " << oper.version << " " << oper.expected;
        }

        return str;
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<oper_t> &&operations) noexcept
            : base_test_case(std::move(name))
            , Operations(std::move(operations))
        {
        }

        [[nodiscard]] constexpr auto operations() const &noexcept -> const std::vector<oper_t> &
        {
            return Operations;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Operations.size(), "operations");

            for (const auto &oper : Operations)
            {
                validate_operation(oper);
            }

            ::Standard::Algorithms::ert::are_equal(false, Operations.back().is_add, "last operation");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("operations", Operations, str);
        }

private:
        std::vector<oper_t> Operations;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr auto val5 = 5;
        constexpr auto val6 = 6;

        test_cases.emplace_back("base1",
            std::vector<oper_t>{ { "a", 0, false, false }, { "a", 1, true, true }, { "a", 1, false, true },
                { "a", 1, true, false }, // 2nd add
                { "a", 1, false, true }, // 2nd search
                //
                { "ab", 2, true, true }, { "cd", 3, true, true }, { "ce", 4, true, true }, { "cd", 4, true, false },
                // Searches
                { "a", 1, false, true }, { "a", 2, false, true }, { "a", 3, false, true }, { "a", 4, false, true },
                { "c", 3, false, false }, { "c", 4, false, false }, { "cd", 2, false, false }, { "cd", 3, false, true },
                { "cd", 4, false, true }, { "ce", 3, false, false }, { "ce", 4, false, true },
                { "aB", 4, false, false }, { "ab", 1, false, false }, { "ab", 2, false, true },
                { "ab", 4, false, true }, { "abc", 4, false, false },
                // Add more
                { "ghj", val5, true, true }, { "gh", val5, false, false }, { "ghj", val5, false, true },
                { "gh", val6, true, true }, { "gh", val6, true, false }, { "ghj", val6, true, false },
                { "gh", val6, true, false }, { "gh", val5, false, false }, { "gh", val6, false, true },
                { "ghj", val5, false, true }, { "ghj", val6, false, true } });

        constexpr auto min_size = 1;
        constexpr auto max_size = 10;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);
        std::unordered_set<string_t> uniq{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto size = rnd();
            std::vector<oper_t> ops(size);
            std::size_t version{};

            uniq.clear();

            for (std::int32_t index{}; index < size; ++index)
            {
                constexpr auto min_size_str = 1;
                constexpr auto max_size_str = 5;

                auto str = Standard::Algorithms::Utilities::random_string(rnd, min_size_str, max_size_str);

                const auto is_add = index != size - 1 && static_cast<bool>(rnd);
                const auto expected = is_add ? uniq.insert(str).second : uniq.contains(str);

                version += is_add && expected ? 1U : 0U;

                ops[index] = { std::move(str), version, is_add, expected };
            }

            test_cases.emplace_back("Random" + std::to_string(att), std::move(ops));
        }
    }

    template<class tree_a, class tree_b>
    constexpr void test_add(tree_a &tree, tree_b &tree_slow, const oper_t &oper, std::size_t &version)
    {
        ::Standard::Algorithms::ert::are_equal(version, tree.max_version(), "before tree add");
        ::Standard::Algorithms::ert::are_equal(version, tree_slow.max_version(), "before slow tree add");

        const auto fast = tree.add(oper.str);
        ::Standard::Algorithms::ert::are_equal(oper.expected, fast, "tree add");

        const auto slow = tree_slow.add(oper.str);
        ::Standard::Algorithms::ert::are_equal(fast, slow, "slow tree add");

        version += fast;
        ::Standard::Algorithms::ert::are_equal(version, oper.version, "add version");
        ::Standard::Algorithms::ert::are_equal(version, tree.max_version(), "after tree add ");
        ::Standard::Algorithms::ert::are_equal(version, tree_slow.max_version(), "after slow tree add");
    }

    template<class tree_a, class tree_b>
    constexpr void test_contains(
        const tree_a &tree, const tree_b &tree_slow, const oper_t &oper, const std::size_t version)
    {
        ::Standard::Algorithms::ert::greater_or_equal(version, oper.version, "sum version");

        const auto fast = tree.contains(oper.str, oper.version);
        ::Standard::Algorithms::ert::are_equal(oper.expected, fast, "tree contains");

        const auto slow = tree_slow.contains(oper.str, oper.version);
        ::Standard::Algorithms::ert::are_equal(fast, slow, "tree slow contains");
    }

    constexpr void run_test_case(const test_case &test)
    {
        Standard::Algorithms::Trees::persistent_trie<string_t> tree{};
        ::Standard::Algorithms::ert::are_equal(0U, tree.max_version(), "tree max version");

        Standard::Algorithms::Trees::persistent_trie_slow<string_t> tree_slow{};
        ::Standard::Algorithms::ert::are_equal(0U, tree_slow.max_version(), "slow tree max version");

        std::size_t version{};
        std::size_t index{};

        for (const auto &oper : test.operations())
        {
            try
            {
                if (oper.is_add)
                {
                    test_add(tree, tree_slow, oper, version);
                }
                else
                {
                    test_contains(tree, tree_slow, oper, version);
                }
            }
            catch (const std::exception &exc)
            {
                const auto err = exc.what() + std::string(" at operation index ") + std::to_string(index);
                throw std::runtime_error(err);
            }

            ++index;
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::persistent_trie_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
