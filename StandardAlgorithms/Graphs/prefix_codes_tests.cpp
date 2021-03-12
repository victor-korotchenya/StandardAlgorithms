#include"prefix_codes_tests.h"
#include"../Utilities/test_utilities.h"
#include"prefix_codes.h"
#include<map>
#include<set>

namespace
{
    using code_t = std::string; // todo(p3): using code_t = std::vector<bool>;
    using letter_t = typename code_t::value_type;
    using frequency_t = std::uint64_t;
    using node_t = Standard::Algorithms::Trees::prefix_code_node<letter_t, frequency_t>;

    template<class key_t, class val_t>
    using map_t = std::map<key_t, val_t>;

    constexpr letter_t zero = 'a';
    constexpr letter_t one = 'B';
    static_assert(zero != one);

    constexpr void check_letter(const letter_t &letter)
    {
        if (letter == zero || letter == one) [[likely]]
        {
            return;
        }

        auto err = "Found a banned letter " + std::to_string(static_cast<std::int32_t>(letter));

        throw std::runtime_error(err);
    }

    constexpr void check_code(const code_t &code)
    {
        Standard::Algorithms::throw_if_empty("code", code);

        for (const auto &letter : code)
        {
            check_letter(letter);
        }
    }

    [[nodiscard]] constexpr auto extract_keys(const auto &mapp)
    {
        using key_t = std::remove_cvref_t<decltype(mapp.begin()->first)>;

        std::set<key_t> keys;

        for (const auto &par : mapp)
        {
            keys.insert(par.first);
        }

        return keys;
    }

    struct prefix_code_test_case final : Standard::Algorithms::Tests::base_test_case
    {
        prefix_code_test_case(std::string &&name, map_t<letter_t, frequency_t> &&frequencies,
            map_t<letter_t, code_t> &&expected_maps, code_t &&expected_seri) noexcept
            : base_test_case(std::move(name))
            , Frequencies(std::move(frequencies))
            , Expected_maps(std::move(expected_maps))
            , Expected_seri(std::move(expected_seri))
        {
        }

        [[nodiscard]] constexpr auto frequencies() const &noexcept -> const map_t<letter_t, frequency_t> &
        {
            return Frequencies;
        }

        [[nodiscard]] constexpr auto expected_maps() const &noexcept -> const map_t<letter_t, code_t> &
        {
            return Expected_maps;
        }

        [[nodiscard]] constexpr auto expected_seri() const &noexcept -> const code_t &
        {
            return Expected_seri;
        }

        void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::throw_if_empty("Frequencies", Frequencies);
            {
                const auto expected_letters = extract_keys(Frequencies);
                const auto actual_letters = extract_keys(Expected_maps);
                ::Standard::Algorithms::ert::are_equal(expected_letters, actual_letters, "letters from maps");
            }

            for (const auto &par : Expected_maps)
            {
                check_code(par.second);
            }

            Standard::Algorithms::require_less_equal(Frequencies.size(), Expected_seri.size(), "Expected seri size");

            check_code(Expected_seri);
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("frequencies", Frequencies.cbegin(), Frequencies.cend(), str);
            ::Standard::Algorithms::print("expected maps", Expected_maps.cbegin(), Expected_maps.cend(), str);

            str << " expected seri '" << Expected_seri << "'";
        }

private:
        map_t<letter_t, frequency_t> Frequencies;
        map_t<letter_t, code_t> Expected_maps;
        code_t Expected_seri;
    };

    void generate_test_cases(std::vector<prefix_code_test_case> &tests)
    {
        /*
        // todo(p3): fix the test so that it did not depend on the heap e.g. provide all possible valid answers.

                tests.push_back({"G5",
                    // NOLINTNEXTLINE
                    { {10,1}, {20,2}, {30,4}, {40,5}, {50, 6'000} },
                    // NOLINTNEXTLINE
                    { {50, "a" },{40, "Ba" }, {30, "BBa" }, {10, "BBBa" }, {20, "BBBB" }}});

                tests.push_back({"Five",
                    // NOLINTNEXTLINE
                    { {10,1}, {20,2}, {30,4}, {40,5}, {50,6} },
                    // NOLINTNEXTLINE
                    { {50, "BB" },{40, "Ba" }, {30, "aa" }, {10, "aBa" }, {20, "aBB" }}});
        */

        tests.push_back({ "Four",
            // NOLINTNEXTLINE
            { { 10, 1 }, { 20, 2 }, { 30, 14 }, { 40, 200 } },
            // NOLINTNEXTLINE
            { { 40, code_t(1, zero) }, { 30, "Ba" }, { 10, "BBa" }, { 20, "BBB" } },
            //               7 ; The root has id = 7, position = 6.
            // 4(40, "a")                         6
            //                   3(30, "Ba")                         5
            //                                         1(10, "BBa")      2(20, "BBB")
            // Pre-order: 7, 4, 6, 3, 5, 1, 2.
            // Let 'a' signify an inner node, 'B' - a leaf.
            // Serialized tree structure: {a, B, a, B, a, B, B}
            "aBaBaBB" });

        tests.push_back({ "Third large",
            // NOLINTNEXTLINE
            { { 10, 2 }, { 20, 30 }, { 50, 500 } },
            // NOLINTNEXTLINE
            { { 50, code_t(1, zero) }, { 20, "BB" }, { 10, "Ba" } },
            //                    5 ; root
            // 3(50, "a")                       4
            //                             1(10)             2(20)
            // Pre-order: 5, 3, 4, 1, 2.
            // Serialized tree structure: {a, B, a, B, B}
            "aBaBB" });

        tests.push_back({ "Two codes",
            // NOLINTNEXTLINE
            { { 10, 20 }, { 30, 500 } },
            // NOLINTNEXTLINE
            { { 10, code_t(1, zero) }, { 30, code_t(1, one) } },
            //            root
            // 1(10)             2(20)
            "aBB" });

        tests.push_back({ "Trivial",
            // NOLINTNEXTLINE
            { { 10, 200 } }, { { 10, code_t(1, zero) } }, "B" });
    }

    void run_test_case(const prefix_code_test_case &test)
    {
        map_t<letter_t, code_t> maps;
        std::vector<node_t> nodes;
        std::size_t root_position{};

        Standard::Algorithms::Trees::prefix_codes<code_t, frequency_t, letter_t>(
            zero, test.frequencies(), one, maps, root_position, nodes);

        ::Standard::Algorithms::ert::are_equal(test.expected_maps(), maps, "prefix code maps");

        Standard::Algorithms::require_greater(nodes.size(), root_position, "root position");

        code_t seri{};
        Standard::Algorithms::Trees::serialize_preorder_tree_structure_only<letter_t, node_t>(
            zero, nodes, one, root_position, seri);

        ::Standard::Algorithms::ert::are_equal(test.expected_seri(), seri, "seri");
    }
} // namespace

void Standard::Algorithms::Trees::Tests::prefix_codes_tests()
{
    Standard::Algorithms::Tests::test_utilities<prefix_code_test_case>::run_tests(run_test_case, generate_test_cases);
}
