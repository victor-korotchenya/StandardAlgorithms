#include"persistent_segment_tree_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"persistent_segment_tree.h"

namespace
{
    using int_t = std::int64_t;

    // Suppress a false warning: constructor does not initialize union fields.
    // NOLINTNEXTLINE
    struct oper_t final
    {
        std::size_t left{};
        std::size_t right_incl{};

        union
        {
            int_t summ{};
            // NOLINTNEXTLINE
            std::int32_t addd;
        };

        std::uint32_t version{};
        bool is_add{};
    };

    constexpr void validate(const oper_t &oper)
    {
        Standard::Algorithms::require_less_equal(oper.left, oper.right_incl, "right incl");

        if (oper.is_add)
        {
            Standard::Algorithms::require_positive(oper.version, "version");
            ::Standard::Algorithms::ert::not_equal(oper.addd, 0, "add");
        }
    }

    auto operator<< (std::ostream &str, const oper_t &oper) -> std::ostream &
    {
        if (oper.is_add)
        {
            str << "Add [" << oper.left << ", " << oper.right_incl << "] " << oper.addd << " new version "
                << oper.version;
        }
        else
        {
            str << "Sum [" << oper.left << ", " << oper.right_incl << "] version " << oper.version << " expect "
                << oper.summ;
        }

        return str;
    }

    constexpr auto add_oper(const std::int32_t add, const std::pair<std::size_t, std::size_t> &left_right_incl,
        const std::uint32_t version) -> oper_t
    {
        Standard::Algorithms::require_positive(version, "version");
        Standard::Algorithms::require_not_equal(add, "add", 0);

        const auto &left = left_right_incl.first;
        const auto &right_incl = left_right_incl.second;

        oper_t oper{};
        oper.left = left;
        oper.right_incl = right_incl;
        oper.addd = add;
        oper.version = version;
        oper.is_add = true;

        validate(oper);
        return oper;
    }

    constexpr auto accum_oper(const int_t sum, const std::pair<std::size_t, std::size_t> &left_right_incl,
        const std::uint32_t version) -> oper_t
    {
        const auto &left = left_right_incl.first;
        const auto &right_incl = left_right_incl.second;

        oper_t oper{};
        oper.left = left;
        oper.right_incl = right_incl;
        oper.summ = sum;
        oper.version = version;
        oper.is_add = false;

        validate(oper);
        return oper;
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::size_t tree_size, std::vector<oper_t> &&operations) noexcept
            : base_test_case(std::move(name))
            , Operations(std::move(operations))
            , Tree_size(tree_size)
        {
        }

        [[nodiscard]] constexpr auto operations() const &noexcept -> const std::vector<oper_t> &
        {
            return Operations;
        }

        [[nodiscard]] constexpr auto tree_size() const noexcept -> std::size_t
        {
            return Tree_size;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Tree_size, "tree size");
            Standard::Algorithms::require_positive(Operations.size(), "operations");

            ::Standard::Algorithms::ert::are_equal(false, Operations.back().is_add, "last operation");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("tree size", str, Tree_size);
            ::Standard::Algorithms::print("operations", Operations, str);
        }

private:
        std::vector<oper_t> Operations;
        std::size_t Tree_size;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("base1", 1,
            std::vector<oper_t>{ accum_oper(0, { 0, 0 }, 0),
                // NOLINTNEXTLINE
                add_oper(10, { 0, 0 }, 1),
                // NOLINTNEXTLINE
                add_oper(300, { 0, 0 }, 2),
                // NOLINTNEXTLINE
                accum_oper(0, { 0, 0 }, 0),
                // NOLINTNEXTLINE
                accum_oper(10, { 0, 0 }, 1),
                // NOLINTNEXTLINE
                accum_oper(310, { 0, 0 }, 2) });

        constexpr auto min_size = 1;
        constexpr auto max_size = 10;

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto tree_size = rnd();
            const auto ops_count = rnd();

            std::vector<oper_t> ops(ops_count);
            std::uint32_t version{};

            for (std::int32_t index{}; index < ops_count; ++index)
            {
                const auto left = rnd(0, tree_size - 1);

                // todo(p3): "Add L R A", now works only L=R.
                // right = rnd(left, tree_size - 1);
                const auto &right = left;

                constexpr auto min_add = 1;
                constexpr auto max_add = 20;

                const auto is_accum = index == ops_count - 1 || static_cast<bool>(rnd);

                ops[index] = is_accum ? accum_oper(0, { left, right }, version)
                                      : add_oper(rnd(min_add, max_add), { left, right }, ++version);
            }

            test_cases.emplace_back("Random" + std::to_string(att), tree_size, std::move(ops));
        }
    }

    template<class tree_a, class tree_b, class tree_c>
    constexpr void test_add(
        tree_a &tree, tree_b &tree_other, tree_c &tree_slow, const oper_t &oper, std::uint32_t &version)
    {
        ::Standard::Algorithms::ert::are_equal(version, tree.max_version(), "before tree add");
        ::Standard::Algorithms::ert::are_equal(version, tree_other.max_version(), "before tree_other add");
        ::Standard::Algorithms::ert::are_equal(version, tree_slow.max_version(), "before tree_slow add");

        {
            const auto &add = oper.addd;
            tree.add(oper.left, add, oper.right_incl);
            tree_other.add(oper.left, add, oper.right_incl);
            tree_slow.add(oper.left, add, oper.right_incl);
        }

        ++version;
        ::Standard::Algorithms::ert::are_equal(version, oper.version, "add oper version");
        ::Standard::Algorithms::ert::are_equal(version, tree.max_version(), "after tree add ");
        ::Standard::Algorithms::ert::are_equal(version, tree_other.max_version(), "after tree_other add ");
        ::Standard::Algorithms::ert::are_equal(version, tree_slow.max_version(), "after tree_slow add");
    }

    template<class tree_a, class tree_b, class tree_c>
    constexpr void test_sum(const tree_a &tree, const tree_b &tree_other, const tree_c &tree_slow, const oper_t &oper,
        const std::uint32_t version)
    {
        ::Standard::Algorithms::ert::greater_or_equal(version, oper.version, "sum version");

        const auto fast = tree.sum({ oper.version, oper.left, oper.right_incl });
        if (const auto &sum = oper.summ; sum != 0)
        {
            ::Standard::Algorithms::ert::are_equal(sum, fast, "tree sum");
        }

        const auto other = tree_other.sum({ oper.version, oper.left, oper.right_incl });
        ::Standard::Algorithms::ert::are_equal(fast, other, "tree_other sum");

        const auto slow = tree_slow.sum({ oper.version, oper.left, oper.right_incl });
        ::Standard::Algorithms::ert::are_equal(fast, slow, "tree_slow sum");
    }

    template<class tree_t>
    constexpr void check_initial(const test_case &test, const std::string &name, const tree_t &tree)
    {
        assert(!name.empty());

        ::Standard::Algorithms::ert::are_equal(test.tree_size(), tree.size(), name + " tree size");

        ::Standard::Algorithms::ert::are_equal(0U, tree.max_version(), name + " max version");
    }

    constexpr void run_test_case(const test_case &test)
    {
        // todo(p3): Standard::Algorithms::Trees::persistent_segment_tree<int_t> tree(test.tree_size());
        Standard::Algorithms::Trees::persistent_segment_tree_slow<int_t> tree(test.tree_size());
        check_initial(test, "persistent_segment_tree", tree);

        Standard::Algorithms::Trees::persistent_segment_tree_other<int_t> tree_other(test.tree_size());
        check_initial(test, "persistent_segment_tree_other", tree_other);

        Standard::Algorithms::Trees::persistent_segment_tree_slow<int_t> tree_slow(test.tree_size());
        check_initial(test, "persistent_segment_tree_slow", tree_slow);

        std::uint32_t version{};
        std::uint32_t index{};

        for (const auto &oper : test.operations())
        {
            try
            {
                if (oper.is_add)
                {
                    test_add(tree, tree_other, tree_slow, oper, version);
                }
                else
                {
                    test_sum(tree, tree_other, tree_slow, oper, version);
                }
            }
            catch (const std::exception &exc)
            {
                const auto err = exc.what() + std::string(" at oper index ") + std::to_string(index);
                throw std::runtime_error(err);
            }

            ++index;
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::persistent_segment_tree_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
