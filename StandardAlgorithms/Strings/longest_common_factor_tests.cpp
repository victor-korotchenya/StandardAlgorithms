#include"longest_common_factor_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"longest_common_factor.h"
#include"ukkonen_suffix_tree_utilities.h"

namespace
{
    using p_t = std::pair<std::int32_t, std::int32_t>;
    using pp_t = std::pair<p_t, p_t>;

    constexpr auto unique_cha = '@';
    constexpr auto not_computed = -1;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::string &&source, std::string &&destination,
            std::int32_t start0 = not_computed, std::int32_t stop0 = not_computed, std::int32_t start1 = not_computed,
            std::int32_t stop1 = not_computed) noexcept
            : base_test_case(std::move(name))
            , Source(std::move(source))
            , Destination(std::move(destination))
            , Expected({ { start0, stop0 }, { start1, stop1 } })
        {
        }

        [[nodiscard]] constexpr auto source() const &noexcept -> const std::string &
        {
            return Source;
        }

        [[nodiscard]] constexpr auto destination() const &noexcept -> const std::string &
        {
            return Destination;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const pp_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto &[src, dst] = Expected;
            if (src.first == not_computed)
            {
                return;
            }

            Standard::Algorithms::Strings::test_check_pair("source", src, Source.size());
            Standard::Algorithms::Strings::test_check_pair("destination", dst, Destination.size());

            ::Standard::Algorithms::ert::are_equal(dst.second - dst.first, src.second - src.first, "lengths");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value("source", str, Source);
            ::Standard::Algorithms::print_value("destination", str, Destination);
            ::Standard::Algorithms::print_value("expected", str, Expected);
        }

private:
        std::string Source;
        std::string Destination;
        pp_t Expected;
    };

    void generate_test_cases(std::vector<test_case> &tests)
    {
        constexpr auto fiv = 5;
        constexpr auto six = 6;
        constexpr auto sev = 7;
        constexpr auto nin = 9;
        constexpr auto ten = 10;

        tests.emplace_back("8", "559874", "8", 3, 4, 0, 1);
        tests.emplace_back("ra", "Agra", "Tehran", 2, 4, 3, fiv);
        tests.emplace_back("ara", "Ararat", "Ankara", 2, fiv, 3, six);
        tests.emplace_back("Belf", "Belfast", "Belfort", 0, 4, 0, 4);
        tests.emplace_back("sh", "Bishkek", "Dushanbe", 2, 4, 2, 4);
        tests.emplace_back("Bres", "Brescia", "Brest", 0, 4, 0, 4);
        tests.emplace_back("as", "Caracas", "Wasilla", fiv, sev, 1, 3);
        tests.emplace_back("ama", "Damascus", "Samarkand", 1, 4, 1, 4);
        tests.emplace_back("za", "Giza", "Byzantium", 2, 4, 2, 4);
        tests.emplace_back("Ha", "Hamhung", "Hanoi", 0, 2, 0, 2);
        tests.emplace_back("li", "Kigali", "Tripoli", 4, six, fiv, sev);
        tests.emplace_back("en", "Leiden", "Athens", 4, six, 3, fiv);
        tests.emplace_back("dina", "Medina", "Mdina", 2, six, 1, fiv);
        tests.emplace_back("os", "Moscow", "Rostock", 1, 3, 1, 3);
        tests.emplace_back("airo", "Nairobi", "Cairo", 1, fiv, 1, fiv);
        tests.emplace_back("Rossl", "Rossland", "Rosslyn", 0, fiv, 0, fiv);
        tests.emplace_back("Rus", "Ruse", "Rustavi", 0, 3, 0, 3);
        tests.emplace_back("mara", "Samara", "Asmara", 2, six, 2, six);
        tests.emplace_back("Sha", "Shanghai", "Shakhtinsk", 0, 3, 0, 3);
        tests.emplace_back("grad", "Stalingrad", "Belgrade", six, ten, 3, sev);
        tests.emplace_back("ba", "Ulaanbaatar, ", "Dubai", fiv, sev, 2, 4);
        tests.emplace_back("ab ca", "ab", "ca", 0, 1, 1, 2);
        tests.emplace_back("bse1", "basep1ba", "epd9bsep1qd", 2, six, fiv, nin);
        tests.emplace_back("empty", "", "", 0, 0, 0, 0);
        tests.emplace_back("a e", "a", "", 0, 0, 0, 0);
        tests.emplace_back("e a", "", "a", 0, 0, 0, 0);
        tests.emplace_back("a", "a", "a", 0, 1, 0, 1);

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto length_max = 10;

            tests.emplace_back("Random" + std::to_string(att),
                Standard::Algorithms::Utilities::random_string(rnd, 1, length_max),
                Standard::Algorithms::Utilities::random_string(rnd, 1, length_max));
        }
    }

    [[nodiscard]] constexpr auto length(const p_t &par) noexcept -> std::int32_t
    {
        return par.second - par.first;
    }

    template<class f_t>
    constexpr void run_sub_test(const test_case &test, const pp_t &expected, const std::string &name, f_t func)
    {
        // No constant
        // NOLINTNEXTLINE
        pp_t actual{};

        func(test.source(), test.destination(), actual);
        if (expected == actual)
        {
            return;
        }

        const auto len = length(expected.first);
        auto a_len = length(actual.first);
        auto b_len = length(actual.second);

        auto start1 = actual.first.first;
        auto start2 = actual.second.first;

        if (!a_len && b_len)
        {// todo(p3): It can be implemented: find positions in both strings.
            assert(!start1);
            a_len = b_len;

            const auto sub = test.destination().substr(start2, b_len); // todo(p3): std::string_view
            start1 = static_cast<std::int32_t>(test.source().find(sub));

            assert(!(start1 < 0));
        }
        else if (a_len && !b_len)
        {
            assert(!start2);
            b_len = a_len;

            const auto sub = test.source().substr(start1, a_len); // todo(p3): std::string_view
            start2 = static_cast<std::int32_t>(test.destination().find(sub));

            assert(!(start2 < 0));
        }

        ::Standard::Algorithms::ert::are_equal(len, a_len, name + " f length");
        ::Standard::Algorithms::ert::are_equal(len, b_len, name + " s length");

        const auto src1 = test.source().substr(start1, a_len); // todo(p3): std::string_view
        const auto tgt1 = test.destination().substr(start2, b_len);

        ::Standard::Algorithms::ert::are_equal(src1, tgt1, name + " substr");
    }

    constexpr void run_test_case(const test_case &test)
    {
        pp_t expected{};

        Standard::Algorithms::Strings::longest_common_factor_slow_still2(test.source(), test.destination(), expected);

        if (test.expected().first.first != not_computed)
        {
            ::Standard::Algorithms::ert::are_equal(test.expected(), expected, "longest_common_factor_slow_still2");
        }

        run_sub_test(test, expected, "longest_common_factor_slow_still",
            Standard::Algorithms::Strings::longest_common_factor_slow_still<std::string, std::string, pp_t>);

        run_sub_test(test, expected, "longest_common_factor",
            Standard::Algorithms::Strings::longest_common_factor<std::int32_t, std::string, pp_t>);

        run_sub_test(test, expected, "longest_common_factor_slow",
            Standard::Algorithms::Strings::longest_common_factor_slow<std::string, std::string, pp_t>);

        run_sub_test(test, expected, "lcp_longest_common_factor",
            [](const auto &str1, const auto &str2, pp_t &actual1)
            {
                Standard::Algorithms::Strings::lcp_longest_common_factor<std::string, char, pp_t>(
                    str1, unique_cha, str2, actual1);
            });
    }
} // namespace

void Standard::Algorithms::Strings::Tests::longest_common_factor_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
