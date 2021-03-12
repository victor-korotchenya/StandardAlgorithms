#include"van_emde_boas_tree_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"van_emde_boas_tree.h"
#include<set>

namespace
{
    constexpr auto half = 1U;
    constexpr auto size_plus_one = (half << 1U) | 1U;

    template<class tree_t, class int_t = typename tree_t::int_t>
    constexpr void empty_tree_test(const tree_t &tree)
    {
        const std::pair<int_t, bool> expected{};

        for (int_t index{}; index <= size_plus_one + 2U; ++index)
        {
            const auto name = std::to_string(index);
            {
                const auto has = tree.contains(index);

                ::Standard::Algorithms::ert::are_equal(false, has, "Empty tree, contains(" + name + ')');
            }
            {
                const auto actual = tree.prev(index);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "Empty tree, prev(" + name + ')');
            }
            {
                const auto actual = tree.next(index);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "Empty tree, next(" + name + ')');
            }
        }

        {
            const auto min = tree.min();
            ::Standard::Algorithms::ert::are_equal(false, min.second, "Empty tree, min()");
        }
        {
            const auto max = tree.max();
            ::Standard::Algorithms::ert::are_equal(false, max.second, "Empty tree, max()");
        }
    }

    template<class tree_t, class int_t = typename tree_t::int_t>
    constexpr void push_tree_test(tree_t &tree)
    {
        const std::pair<int_t, bool> nop{};

        for (int_t key = half; key <= half + 1U; ++key)
        {
            const auto key_name = "key " + std::to_string(key) + ", index ";

            constexpr auto max_attempts = 3;

            for (std::int32_t att{}; att < max_attempts; ++att)
            {
                const auto name = key_name + std::to_string(att);
                const auto has_before = tree.contains(key) ? 1U : 0U;
                const auto exists = att != 0;
                ::Standard::Algorithms::ert::are_equal(exists, has_before, "Has before push " + name);

                const auto added = tree.push(key);
                ::Standard::Algorithms::ert::are_equal(!exists, added, "Added after push " + name);

                const auto has_after = tree.contains(key);
                ::Standard::Algorithms::ert::are_equal(true, has_after, "Has after push " + name);
            }

            {
                const auto actual = tree.min();
                const auto expected = std::make_pair(static_cast<int_t>(half), true);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "min " + key_name);
            }
            {
                const auto actual = tree.max();
                const auto expected = std::make_pair(key, true);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "max " + key_name);
            }
            {
                const auto actual = tree.prev(key);

                const auto expected = key == half ? nop : std::make_pair(static_cast<int_t>(key - 1U), true);

                ::Standard::Algorithms::ert::are_equal(expected, actual, "prev " + key_name);
            }
            {
                const auto key1 = static_cast<int_t>(key - 1U);
                const auto actual = tree.next(key1);
                const auto expected = std::make_pair(key, true);

                ::Standard::Algorithms::ert::are_equal(expected, actual, "next " + std::to_string(key1));
            }
        }
    }

    template<class tree_t, class int_t = typename tree_t::int_t>
    constexpr void pop_tree_test(tree_t &tree)
    {
        for (int_t key = half; key <= half + 1U; ++key)
        {
            const auto key_name = "pop key " + std::to_string(key) + ", index ";

            constexpr auto max_attempts = 3;

            for (std::int32_t att{}; att < max_attempts; ++att)
            {
                const auto name = key_name + std::to_string(att);
                const auto has_before = tree.contains(key) ? 1U : 0U;
                const auto exists = att == 0;
                ::Standard::Algorithms::ert::are_equal(exists, has_before, "Has before " + name);

                const auto deleted = tree.pop(key);
                ::Standard::Algorithms::ert::are_equal(exists, deleted, "Deleted " + name);

                const auto has = tree.contains(key);
                ::Standard::Algorithms::ert::are_equal(false, has, "Has after " + name);
            }
        }
    }

    template<class tree_t>
    constexpr void simple_test()
    {
        tree_t tree(size_plus_one);

        try
        {
            empty_tree_test(tree);
            push_tree_test(tree);
            pop_tree_test(tree);
        }
        catch (const std::exception &exc)
        {
            const auto mag = std::string(exc.what()) + ", simple fast " + std::to_string(tree_t::prefer_speed);

            throw std::runtime_error(mag);
        }
    }

    enum class op_kind : std::uint8_t
    {
        add,
        add_not,
        erase,
        erase_not,
    };

    template<class int_t, class tree_t, class rg_t>
    constexpr void random_push_pop(
        rg_t &rnd, std::set<int_t> &set1, std::vector<std::pair<int_t, op_kind>> &steps, tree_t &tree)
    {
        const auto value = static_cast<int_t>(rnd());
        assert(value <= tree.domain_size());

        if (const auto is_add = static_cast<bool>(rnd); is_add)
        {
            const auto added = set1.insert(value).second;

            steps.emplace_back(value, added ? op_kind::add : op_kind::add_not);

            const auto actual1 = tree.push(value);
            ::Standard::Algorithms::ert::are_equal(added, actual1, "Push");

            return;
        }

        const auto erased = 0U < set1.erase(value);
        steps.emplace_back(value, erased ? op_kind::erase : op_kind::erase_not);

        const auto actual = tree.pop(value);
        ::Standard::Algorithms::ert::are_equal(erased, actual, "Pop");
    }

    template<class int_t, class iter_t, class iter_t2>
    [[nodiscard]] constexpr auto to_pair(const iter_t &iter, const iter_t2 &end1) -> std::pair<int_t, bool>
    {
        return iter == end1 ? std::pair<int_t, bool>{} : std::make_pair(static_cast<int_t>(*iter), true);
    }

    template<class int_t>
    [[nodiscard]] constexpr auto set_max(const std::set<int_t> &set1) -> std::pair<int_t, bool>
    {
        const auto beg = set1.cbegin();
        const auto end1 = set1.cend();
        const auto empty = beg == end1;
        return to_pair<int_t>(empty ? end1 : std::prev(end1), end1);
    }

    template<class int_t, class tree_t, class rg_t>
    constexpr void random_check(rg_t &rnd, const std::set<int_t> &set1, const tree_t &tree)
    {
        assert(0U < tree.domain_size());
        {
            const auto expected = set1.empty();
            const auto actual = tree.is_empty();
            ::Standard::Algorithms::ert::are_equal(expected, actual, "is empty");
        }

        const auto beg = set1.cbegin();
        const auto end1 = set1.cend();
        {
            const auto expected = to_pair<int_t>(beg, end1);
            const auto actual = tree.min();
            ::Standard::Algorithms::ert::are_equal(expected, actual, "min()");
        }
        {
            const auto expected = set_max(set1);
            const auto actual = tree.max();
            ::Standard::Algorithms::ert::are_equal(expected, actual, "max()");
        }

        const auto key = static_cast<int_t>(rnd());
        assert(key <= tree.domain_size());

        const auto name = std::to_string(key);
        {
            const auto expected = set1.contains(key);
            const auto actual = tree.contains(key);

            ::Standard::Algorithms::ert::are_equal(expected, actual, "contains(" + name + ')');
        }

        const auto range = set1.equal_range(key);
        {
            const auto iter = range.first != beg ? std::prev(range.first) : end1;
            const auto expected = to_pair<int_t>(iter, end1);
            const auto actual = tree.prev(key);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "prev(" + name + ')');
        }
        {
            const auto expected = to_pair<int_t>(range.second, end1);
            const auto actual = tree.next(key);
            ::Standard::Algorithms::ert::are_equal(expected, actual, "next(" + name + ')');
        }
    }

    template<class int_t, bool prefer_speed>
    [[nodiscard]] constexpr auto random_domain_size() -> int_t
    {
        constexpr auto max_domain_size =
            static_cast<int_t>(prefer_speed ? 70U : std::numeric_limits<int_t>::max() / 3U);

        Standard::Algorithms::Utilities::random_t<int_t> rnd(1U, max_domain_size);

        auto domain_size1 = rnd();
        assert(0U < domain_size1 && domain_size1 <= max_domain_size);

        return domain_size1;
    }

    template<class int_t, bool prefer_speed>
    constexpr void random_tree_test()
    {
        const auto domain_size1 = random_domain_size<int_t, prefer_speed>();

        Standard::Algorithms::Utilities::random_t<int_t> rnd(0, static_cast<int_t>(domain_size1 - 1U));

        ::Standard::Algorithms::ert::greater(domain_size1, int_t{}, "domain size");

        constexpr auto edge = static_cast<int_t>(int_t{} - 1U);

        using tree_t = Standard::Algorithms::Graphs::van_emde_boas_tree<int_t, prefer_speed, edge>;

        tree_t tree(domain_size1);

        std::set<int_t> set1;
        std::vector<std::pair<int_t, op_kind>> steps;

        constexpr auto max_index = 100U;
        steps.reserve(max_index);

        for (std::uint32_t index{}; index < max_index; ++index)
        {
            try
            {
                random_push_pop<int_t>(rnd, set1, steps, tree);
                random_check<int_t>(rnd, set1, tree);
            }
            catch (...)
            {
                //                const auto err = "Random van Emde Boas tree domain size " +
                //                    std::to_string(tree.domain_size()) + ", fast" +
                //                    std::to_string(tree_t::prefer_speed) + ". Steps";
                //
                // todo(p2): ::Standard::Algorithms::print(steps, err);
                throw;
            }
        }
    }

    constexpr void van_emde_boas_tree_tests_random()
    {
        constexpr auto max_attempts = 1;

        for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
        {
            random_tree_test<std::uint16_t, true>();
            random_tree_test<std::uint64_t, false>();
            // todo(p2): random_tree_test<uint128_t, false, 0U -(uint128_t)1>();
        }
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::van_emde_boas_tree_tests()
{
    simple_test<van_emde_boas_tree<std::uint16_t, true>>();
    simple_test<van_emde_boas_tree<std::uint32_t, true>>();
    simple_test<van_emde_boas_tree<std::uint64_t, false>>();

    van_emde_boas_tree_tests_random();
}
