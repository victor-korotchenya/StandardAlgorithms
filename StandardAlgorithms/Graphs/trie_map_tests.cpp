#include"trie_map_tests.h"
#include"../Numbers/bit_utilities.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"trie_map.h"
#include<iostream>

namespace
{
    using string_t = std::string;
    using key_t = string_t::value_type;
    using value_t = std::int32_t;

    constexpr auto trie_map_tests_load_flag = false;
    constexpr char is_add_flag = 1;
    constexpr char expected_flag = 2;

    struct oper_t final
    {
        string_t key{};
        value_t value{};
        char flags{};
    };

    // See also is_add_flag.
    [[nodiscard]] constexpr auto is_add(const oper_t &oper) noexcept -> bool
    {
        auto gasoline = ::Standard::Algorithms::Numbers::has_zero_bit(oper.flags);
        return gasoline;
    }

    // See also expected_flag.
    [[nodiscard]] constexpr auto is_expected(const oper_t &oper) noexcept -> bool
    {
        auto gas = ::Standard::Algorithms::Numbers::has_zero_bit(oper.flags / 2);
        return gas;
    }

    auto operator<< (std::ostream &str, const oper_t &oper) -> std::ostream &
    {
        str << (is_add(oper) ? "add '" : "find '") << oper.key << "' " << oper.value << " expect " << is_expected(oper);

        return str;
    }

    [[nodiscard]] constexpr auto add(const string_t &key, const value_t &value) -> oper_t
    {
        return { key, value, is_add_flag };
    }

    [[nodiscard]] constexpr auto fin(const string_t &key, const bool expected = {}, const value_t &value = {}) -> oper_t
    {
        return { key, value, expected ? expected_flag : char{} };
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

            Standard::Algorithms::require_positive(static_cast<std::int32_t>(Operations.size()), "operations size");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("operations", Operations, str);
        }

private:
        std::vector<oper_t> Operations;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.push_back({ "base1",
            { add("", 1), fin("c", false, 3), add("c", 4),
                // NOLINTNEXTLINE
                add("c", 5),
                // NOLINTNEXTLINE
                fin("c", true, 5),
                // NOLINTNEXTLINE
                fin("c", true, 5),
                // NOLINTNEXTLINE
                add("dfg", 60), fin("cd"), fin("d"), fin("df"), fin("f"), fin("fg"),
                // NOLINTNEXTLINE
                fin("dfg", true, 60), fin("dfgdfg"), fin("dfgd") } });
    }

    constexpr void run_test_case(const test_case &test)
    {
        Standard::Algorithms::Trees::trie_map<key_t, value_t> trie;

        for (std::size_t index{}; index < test.operations().size(); ++index)
        {
            const auto &oper = test.operations()[index];
            const auto &key = oper.key;

            if (is_add(oper))
            {
                trie[key] = oper.value;
                continue;
            }

            const auto name = "oper " + std::to_string(index);

            value_t value{};
            const auto actual = trie.find(key, value);

            ::Standard::Algorithms::ert::are_equal(is_expected(oper), actual, name + " expected");

            if (actual)
            {
                ::Standard::Algorithms::ert::are_equal(oper.value, value, name + " value");
            }
        }
    }

    [[maybe_unused]] void trie_map_tests_load_tests()
    {
        constexpr auto word_count = 25'000;
        constexpr auto length = 4;
        constexpr auto min_char = '0';
        constexpr auto max_char = '9';

        std::vector<string_t> words(word_count);

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        constexpr auto max_attempts = 10;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            for (std::int32_t index{}; index < word_count; ++index)
            {
                words[index] = Standard::Algorithms::Utilities::random_string<decltype(rnd), min_char, max_char>(
                    rnd, length, length);
            }

            using node_t = Standard::Algorithms::Trees::simple_trie_map_node_t<key_t, value_t, min_char, max_char>;

            const Standard::Algorithms::elapsed_time_ns tim0;

            constexpr auto total_size = static_cast<std::size_t>(word_count) * static_cast<std::size_t>(length);

            Standard::Algorithms::Trees::trie_map<key_t, std::int32_t, node_t> trie_map(total_size);

            for (std::int32_t ind{}; const auto &word : words)
            {
                // Need to measure how fast.
                // cppcheck-suppress [unreadVariable]
                trie_map[word] = ++ind;
            }

            const auto elapsed0 = tim0.elapsed();

            const Standard::Algorithms::elapsed_time_ns tim1;
            std::unordered_map<string_t, std::int32_t> map1;
            map1.reserve(word_count);

            for (std::int32_t ind{}; const auto &word : words)
            {
                // Need to measure how fast.
                // cppcheck-suppress [unreadVariable]
                map1[word] = ++ind;
            }

            const auto elapsed1 = tim1.elapsed();
            const auto ratio = ::Standard::Algorithms::ratio_compute(elapsed0, elapsed1);

            const auto report = " att " + std::to_string(att) + ", t1 " + std::to_string(elapsed1) + " / t0 " +
                std::to_string(elapsed0) + " = " + std::to_string(ratio) + "\n";

            std::cout << report;
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::trie_map_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);

    if constexpr (trie_map_tests_load_flag)
    {
        trie_map_tests_load_tests();
    }
}
