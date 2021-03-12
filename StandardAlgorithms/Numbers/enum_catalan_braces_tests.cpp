#include"enum_catalan_braces_tests.h"
#include"../Utilities/test_utilities.h"
#include"enum_catalan_braces.h"
#include"index_of_catalan_braces.h"

namespace
{
    using expected_t = std::vector<std::string>;

    [[nodiscard]] constexpr auto catalan_naive(const std::int32_t num) -> std::int64_t
    {
        if (constexpr auto max1 = 30; num < 0 || max1 < num) [[unlikely]]
        {
            throw std::runtime_error("catalan_naive bad num");
        }

        std::int64_t res = 1;

        for (auto index = 1; index <= num; ++index)
        {
            assert(res * (index + num) % index == 0);

            res = res * (index + num) / index;
        }

        if (const auto remainder = res % (num + 1); 0 != remainder) [[unlikely]]
        {
            throw std::runtime_error("catalan_naive bad num - non zero remainder.");
        }

        res /= num + 1;

        assert(num <= res);

        return res;
    }

    static_assert(catalan_naive(0) == 1 && catalan_naive(1) == 1 && catalan_naive(2) == 2);

    // NOLINTNEXTLINE
    static_assert(catalan_naive(3) == 5 && catalan_naive(4) == 14);

    // NOLINTNEXTLINE
    static_assert(catalan_naive(5) == 42 && catalan_naive(6) == 132);

    // NOLINTNEXTLINE
    static_assert(catalan_naive(30) == 3814986502092304LL);

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::int32_t num, expected_t &&expected) noexcept
            : base_test_case(std::move(name))
            , Expected(std::move(expected))
            , Num(num)
        {
        }

        [[nodiscard]] constexpr auto num() const noexcept -> std::int32_t
        {
            return Num;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const expected_t &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            ::Standard::Algorithms::ert::greater(Num, 0, "number");

            const auto size = static_cast<std::int32_t>(Expected.size());
            {
                const auto expected_size = catalan_naive(Num);

                ::Standard::Algorithms::ert::are_equal(expected_size, size, "expected size");
            }

            for (std::int32_t index{}; index < size; ++index)
            {
                const auto &str = Expected.at(index);
                const auto str_len = static_cast<std::int32_t>(str.size());
                const auto name = ::Standard::Algorithms::Utilities::zu_string(index);

                ::Standard::Algorithms::ert::are_equal(Num * 2LL, str_len, "expected size at " + name);

                {
                    const auto is_cat = Standard::Algorithms::Numbers::is_catalan_string(str);

                    ::Standard::Algorithms::ert::are_equal(true, is_cat, "is_catalan_string at " + name);
                }

                if (0 < index)
                {
                    const auto &pre = Expected[index - 1];

                    ::Standard::Algorithms::ert::greater(str, pre, "Cat strings must increase at " + name);
                }
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print_value(", number", str, Num);
            ::Standard::Algorithms::print("expected", Expected, str);
        }

private:
        expected_t Expected;
        std::int32_t Num;
    };

    constexpr void generate_test_cases(std::vector<test_case> &tests)
    {
        {
            constexpr auto num = 1;

            tests.emplace_back(::Standard::Algorithms::Utilities::zu_string(num), num, expected_t{ "[]" });
        }
        {
            constexpr auto num = 2;

            tests.emplace_back(::Standard::Algorithms::Utilities::zu_string(num), num, expected_t{ "[[]]", "[][]" });
        }
        {
            constexpr auto num = 3;

            tests.emplace_back(::Standard::Algorithms::Utilities::zu_string(num), num,
                expected_t{ "[[[]]]", "[[][]]", "[[]][]", "[][[]]", "[][][]" });
        }
        {
            constexpr auto num = 4;

            tests.emplace_back(::Standard::Algorithms::Utilities::zu_string(num), num,
                expected_t{ "[[[[]]]]", "[[[][]]]", "[[[]][]]", "[[[]]][]", "[[][[]]]", "[[][][]]", "[[][]][]",
                    "[[]][[]]", "[[]][][]", "[][[[]]]", "[][[][]]", "[][[]][]", "[][][[]]", "[][][][]" });
        }
    }

    constexpr void enum_catalan_braces_test(const test_case &test)
    {
        expected_t actual{};

        auto kind_carlson = [&actual](const std::string &str) -> void
        {
            actual.push_back(str);
        };

        Standard::Algorithms::Numbers::enum_catalan_braces(kind_carlson, test.num());

        ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "enum_catalan_braces");
    }

    constexpr void next_catalan_string_test(const test_case &test)
    {
        std::string str(test.num(), '[');
        str.resize(test.num() * 2LL, ']');

        ::Standard::Algorithms::ert::are_equal(test.expected().at(0), str, "Initial string");

        const auto size =
            Standard::Algorithms::require_positive(static_cast<std::int32_t>(test.expected().size()), "expected size");

        for (std::int32_t index{}; index <= size; ++index)
        {
            const auto cur_ind = (index + 1) % size;
            const auto &cur = test.expected()[cur_ind];

            const auto has = Standard::Algorithms::Numbers::next_catalan_string(str);

            const auto name = ::Standard::Algorithms::Utilities::zu_string(cur_ind);

            {
                const auto is_cat = Standard::Algorithms::Numbers::is_catalan_string(str);

                ::Standard::Algorithms::ert::are_equal(true, is_cat, "is_catalan_string at " + name);
            }

            ::Standard::Algorithms::ert::are_equal(cur, str, "next_catalan_string at " + name);

            const auto prev_ind = index % size;
            const auto has_expected = prev_ind != size - 1;

            ::Standard::Algorithms::ert::are_equal(has_expected, has, "next_catalan_string has at " + name);
        }
    }

    constexpr void index_of_catalan_braces_test(const test_case &test)
    {
        const auto &expected = test.expected();
        Standard::Algorithms::require_positive(expected.size(), "expected size");

        const auto size = static_cast<std::int32_t>(expected[0].size());
        const auto table = Standard::Algorithms::Numbers::compute_catalan_brace_table(size);

        std::int32_t index{};
        std::string temp(size + 2LL, 0);

        for (const auto &str : expected)
        {
            const auto actual = Standard::Algorithms::Numbers::index_of_catalan_braces(table, str);

            ::Standard::Algorithms::ert::are_equal(index, actual, "index_of_catalan_braces " + str);

            constexpr auto garbage = 'g';
            temp.assign(size + 2, garbage);

            Standard::Algorithms::Numbers::brace_string_by_index_of_catalan(table, temp, index);

            ::Standard::Algorithms::ert::are_equal(
                str, temp, "brace_string_by_index_of_catalan " + ::Standard::Algorithms::Utilities::zu_string(index));

            ++index;
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        enum_catalan_braces_test(test);
        next_catalan_string_test(test);
        index_of_catalan_braces_test(test);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::enum_catalan_braces_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
