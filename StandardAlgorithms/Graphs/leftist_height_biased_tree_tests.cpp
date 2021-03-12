#include"leftist_height_biased_tree_tests.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"leftist_height_biased_tree.h"
#include"leftist_weight_biased_tree.h"
#include"pair_heap.h"
#include<array>
#include<ctime>
#include<numeric>
#include<set>
#include<typeinfo> // std::type_info
#include<vector>

namespace Standard::Algorithms::Trees
{
    template<class key_t, bool has_parent>
    auto operator<< (std::ostream &str, const lhb_node<key_t, has_parent> &node) -> std::ostream &
    {
        str << "lhb_node key " << node.key << ", height " << node.height << ", has left " << (node.left != nullptr)
            << ", has right " << (node.right != nullptr);

        if constexpr (has_parent)
        {
            str << ", has parent " << (node.parent != nullptr);
        }

        return str;
    }
} // namespace Standard::Algorithms::Trees

namespace Standard::Algorithms::Heaps
{
    template<class key_t>
    auto operator<< (std::ostream &str, const pair_heap_node<key_t> &node) -> std::ostream &
    {
        str << "pair_heap_node key " << node.key << ", has child " << (node.child != nullptr) << ", has prev "
            << (node.prev != nullptr) << ", has next " << (node.next != nullptr);

        return str;
    }
} // namespace Standard::Algorithms::Heaps

namespace
{
    using key_t = std::uint16_t;

    constexpr key_t mask = ::Standard::Algorithms::is_debug ? 15 : 255;

    constexpr std::array source_arr{// NOLINTNEXTLINE
        10, 20, 15
    };

    template<class tree_t>
    constexpr void test_pop(tree_t &lhb, const std::string &message)
    {
        auto ar2 = source_arr;
        std::sort(ar2.begin(), ar2.end());

        const auto name_prefix = message + " The top before popping # ";

        for (std::size_t index{}; index < ar2.size(); ++index)
        {
            lhb.validate(message);

            const auto name = name_prefix + std::to_string(index);
            const auto *top = lhb.top();
            ::Standard::Algorithms::ert::is_not_null_ptr(top, name);

            const auto &key = ar2.at(index);
            ::Standard::Algorithms::ert::are_equal(top->key, key, name);
            lhb.pop();
        }

        const auto last_name = message + " The top after popping all. ";
        lhb.validate(last_name);
        ::Standard::Algorithms::ert::is_null_ptr(lhb.top(), last_name);
    }

    template<class tree_t>
    constexpr void plain_lhb()
    {
        const std::string name(static_cast<const char *>(__func__));
        tree_t lhb;

        for (const auto &key : source_arr)
        {
            lhb.validate(name);
            lhb.push(key);
        }

        test_pop(lhb, name);
    }

    template<class t>
    [[nodiscard]] auto random_perm() -> std::vector<key_t>
    {
        Standard::Algorithms::Utilities::random_t<std::uint32_t> rnd{};

        static_assert(0U < mask);
        const auto size = static_cast<std::int32_t>(rnd(1U, mask));
        assert(0 < size);

        auto data = ::Standard::Algorithms::Utilities::iota_vector<key_t>(size);

        const auto seed = rnd();
        std::mt19937 eng(seed);

        std::shuffle(data.begin(), data.end(), eng);
        return data;
    }

    template<class tree_t>
    constexpr void check_top(const std::multiset<key_t> &sorted, const tree_t &lhb, std::string &message)
    {
        ::Standard::Algorithms::ert::greater(sorted.size(), 0U, message);

        const auto &mini = *sorted.cbegin();
        message += " check_top " + std::to_string(mini) + ",";

        const auto *const top = lhb.top();
        ::Standard::Algorithms::ert::is_not_null_ptr(top, message);
        ::Standard::Algorithms::ert::are_equal(mini, top->key, message);
    }

    template<class tree_t>
    constexpr void pop_off(std::multiset<key_t> &sorted, tree_t &lhb, std::string &message)
    {
        ::Standard::Algorithms::ert::greater(sorted.size(), 0U, message);

        const auto ite = sorted.cbegin();
        const auto mini = *ite;
        message += " pop " + std::to_string(mini) + ",";
        sorted.erase(ite);

        const auto *top = lhb.top();
        ::Standard::Algorithms::ert::is_not_null_ptr(top, message);
        ::Standard::Algorithms::ert::are_equal(mini, top->key, message);

        lhb.pop();
    }

    template<class tree_t>
    void random_perm_tests()
    {
        const auto data = random_perm<key_t>();
        assert(!data.empty());

        auto message = static_cast<const char *>(__func__) + std::string(": ");

        tree_t lhb;
        lhb.validate(message);
        {
            auto mini = data.at(0);

            for (const auto &key : data)
            {
                message += std::to_string(key) + ", ";

                const auto *const cur = lhb.push(key);
                ::Standard::Algorithms::ert::is_not_null_ptr(cur, message);
                ::Standard::Algorithms::ert::are_equal(key, cur->key, message);

                mini = std::min(mini, key);

                const auto *const top = lhb.top();
                ::Standard::Algorithms::ert::is_not_null_ptr(top, message);
                ::Standard::Algorithms::ert::are_equal(mini, top->key, message);
                lhb.validate(message);
            }
        }

        message += " Start popping ";

        for (key_t key{}; key < static_cast<key_t>(data.size()); ++key)
        {
            message += std::to_string(key) + ", ";

            const auto *const top = lhb.top();
            ::Standard::Algorithms::ert::is_not_null_ptr(top, message);
            ::Standard::Algorithms::ert::are_equal(key, top->key, message);
            lhb.pop();
            lhb.validate(message);
        }

        message += " The top after popping all.";
        ::Standard::Algorithms::ert::is_null_ptr(lhb.top(), message);
    }

    template<class tree_t>
    void random_lhb()
    {
        constexpr auto size = 32U;

        Standard::Algorithms::Utilities::random_t<std::uint32_t> rnd{};
        auto message = static_cast<const char *>(__func__) + std::string(": ");
        std::multiset<key_t> sorted;
        tree_t lhb;

        for (std::uint32_t index{}; index < size; ++index)
        {
            const auto is_add = lhb.top() == nullptr || static_cast<bool>(rnd);

            if (is_add)
            {
                const auto key = static_cast<key_t>(rnd() & mask);
                message += " add " + std::to_string(key) + ",";
                sorted.insert(key);

                const auto *const cur = lhb.push(key);
                ::Standard::Algorithms::ert::is_not_null_ptr(cur, message);
                ::Standard::Algorithms::ert::are_equal(key, cur->key, message);
            }
            else
            {
                check_top(sorted, lhb, message);
                pop_off(sorted, lhb, message);
            }

            lhb.validate(message);

            if (sorted.empty())
            {
                message += " empty,";

                const auto *const top = lhb.top();
                ::Standard::Algorithms::ert::is_null_ptr(top, message);
            }
            else
            {
                check_top(sorted, lhb, message);
            }
        }

        message += " Start popping ";

        while (!sorted.empty())
        {
            check_top(sorted, lhb, message);
            pop_off(sorted, lhb, message);
            lhb.validate(message);
        }

        message += " The top after popping all.\r\n";
        ::Standard::Algorithms::ert::is_null_ptr(lhb.top(), message);
        // std::cout << "\n" << message;
    }

    template<class tree_t>
    constexpr void push_many_lhb()
    {
        const std::string name(static_cast<const char *>(__func__));
        tree_t lhb{};
        lhb.push_many(source_arr.cbegin(), source_arr.cend());
        test_pop(lhb, name);
    }

    template<class tree_t>
    constexpr void merge_lhb()
    {
        const std::string name(static_cast<const char *>(__func__));
        tree_t lhb1{};
        tree_t lhb2{};

        const auto half = source_arr.size() >> 1U;

        for (std::size_t index{}; index < source_arr.size(); ++index)
        {
            auto &tree = index < half ? lhb1 : lhb2;
            tree.push(source_arr.at(index));
            tree.validate(name);
        }

        ::Standard::Algorithms::ert::is_not_null_ptr(lhb1.top(), name + "lhb1.top before merge");
        ::Standard::Algorithms::ert::is_not_null_ptr(lhb2.top(), name + "lhb2.top before merge");

        lhb1.merge(lhb2);
        ::Standard::Algorithms::ert::is_not_null_ptr(lhb1.top(), name + "lhb1.top after merge");
        ::Standard::Algorithms::ert::is_null_ptr(lhb2.top(), name + "lhb2.top after merge");
        lhb2.validate(name);

        test_pop(lhb1, name);
    }

    template<class tree_t>
    constexpr void decrease_key_lhb()
    {
        using node_t = typename tree_t::node_t;
        constexpr auto size = source_arr.size();

        const std::string name(static_cast<const char *>(__func__));
        tree_t lhb;
        std::array<node_t *, size> nodes{};

        for (std::size_t index{}; index < size; ++index)
        {
            constexpr auto val = std::numeric_limits<key_t>::max();
            nodes.at(index) = lhb.push(val);
            lhb.validate(name);
        }

        for (std::size_t index{}; index < size; ++index)
        {
            auto *node = nodes.at(index);
            Standard::Algorithms::throw_if_null("node in test", node);

            const auto &key = source_arr.at(index);
            lhb.decrease_key(*node, key);
            lhb.validate(name);
        }

        test_pop(lhb, name);
    }

    template<class tree_t, bool can_push_many = true, bool has_parent = tree_t::has_parent>
    constexpr void lhb_tests()
    {
        try
        {
            random_lhb<tree_t>();
            random_perm_tests<tree_t>();
            plain_lhb<tree_t>();
            merge_lhb<tree_t>();

            if constexpr (can_push_many)
            {
                push_many_lhb<tree_t>();
            }

            if constexpr (has_parent)
            {
                decrease_key_lhb<tree_t>();
            }
        }
        catch (const std::exception &exc)
        {
            const auto msg = typeid(tree_t).name() + std::string(" can_push_many ") + std::to_string(can_push_many) +
                std::string(", has_parent ") + std::to_string(has_parent) + ". " + std::string(exc.what());
            throw std::runtime_error(msg);
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::leftist_height_biased_tree_tests()
{
    using pair_heap_t = Standard::Algorithms::Heaps::pair_heap<key_t>;

    lhb_tests<pair_heap_t, false>();

    lhb_tests<leftist_weight_biased_tree<key_t, lhb_node<key_t, false>>>();

    lhb_tests<leftist_height_biased_tree<key_t, lhb_node<key_t, false>>>();

    lhb_tests<leftist_height_biased_tree<key_t, lhb_node<key_t, true>>>();
    // todo(p2): random, perf tests.
}
