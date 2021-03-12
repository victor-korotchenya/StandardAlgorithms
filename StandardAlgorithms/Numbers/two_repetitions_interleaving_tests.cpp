#include"two_repetitions_interleaving_tests.h"
#include"../Utilities/require_utilities.h"
#include"../Utilities/test_utilities.h"
#include"two_repetitions_interleaving.h"

namespace
{
    using item_t = std::int16_t;
    using data_t = std::vector<item_t>;

    constexpr void ensure_valid_interleaving(const data_t &source, const std::vector<std::size_t> &indexes_0,
        const data_t &sequence0, const data_t &sequence1)
    {
        Standard::Algorithms::throw_if_empty("source", source);
        Standard::Algorithms::throw_if_empty("sequence 0", sequence0);
        Standard::Algorithms::throw_if_empty("sequence 1", sequence1);

        auto itera = indexes_0.cbegin();
        const auto ende = indexes_0.cend();

        for (std::size_t index{}, sin0{}, sin1{}; index < source.size(); ++index)
        {
            Standard::Algorithms::require_greater(sequence0.size(), sin0, "sin 0");
            Standard::Algorithms::require_greater(sequence1.size(), sin1, "sin 1");

            const auto &socha = source[index];

            if (const auto match_0 = itera != ende && index == *itera; match_0)
            {
                const auto &cha0 = sequence0[sin0];

                ::Standard::Algorithms::ert::are_equal(socha, cha0, "match 0 at " + std::to_string(index));

                if (++sin0 == sequence0.size())
                {
                    sin0 = 0U;
                }

                ++itera;
                continue;
            }

            const auto &cha1 = sequence1[sin1];

            ::Standard::Algorithms::ert::are_equal(socha, cha1, "match 1 at " + std::to_string(index));

            if (++sin1 == sequence1.size())
            {
                sin1 = 0U;
            }
        }
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, data_t &&source_signal, data_t &&sequence0, data_t &&sequence1,
            bool expected_result = {}, std::vector<std::size_t> &&expected_indexes = {}) noexcept
            : base_test_case(std::move(name))
            , Source_signal(std::move(source_signal))
            , Sequence0(std::move(sequence0))
            , Sequence1(std::move(sequence1))
            , Expected_indexes(std::move(expected_indexes))
            , Expected_result(expected_result)
        {
        }

        [[nodiscard]] constexpr auto source_signal() const &noexcept -> const data_t &
        {
            return Source_signal;
        }

        [[nodiscard]] constexpr auto sequence0() const &noexcept -> const data_t &
        {
            return Sequence0;
        }

        [[nodiscard]] constexpr auto sequence1() const &noexcept -> const data_t &
        {
            return Sequence1;
        }

        [[nodiscard]] constexpr auto expected_result() const noexcept -> bool
        {
            return Expected_result;
        }

        [[nodiscard]] constexpr auto expected_indexes() const &noexcept -> const std::vector<std::size_t> &
        {
            return Expected_indexes;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto size = Standard::Algorithms::require_positive(Source_signal.size(), "source signal size.");
            Standard::Algorithms::require_positive(Sequence0.size(), "sequence 0 size.");
            Standard::Algorithms::require_positive(Sequence1.size(), "sequence 1 size.");

            if (Expected_result)
            {
                ensure_valid_interleaving(Source_signal, Expected_indexes, Sequence0, Sequence1);
            }

            if (Expected_indexes.empty())
            {
                return;
            }

            ::Standard::Algorithms::ert::are_equal(
                true, Expected_result, "Expected result for not empty expected indexes");

            Standard::Algorithms::require_greater(size, Expected_indexes.back(), "last expected index");

            {
                constexpr auto require_unique = true;

                Standard::Algorithms::require_sorted(Expected_indexes, "expected indexes", require_unique);
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("source signal", Source_signal, str);
            ::Standard::Algorithms::print("sequence 0", Sequence0, str);
            ::Standard::Algorithms::print("sequence 1", Sequence1, str);
            ::Standard::Algorithms::print_value("expected result", str, Expected_result);
            ::Standard::Algorithms::print("expected indexes", Expected_indexes, str);
        }

private:
        data_t Source_signal;
        data_t Sequence0;
        data_t Sequence1;

        std::vector<std::size_t> Expected_indexes;
        bool Expected_result;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        test_cases.emplace_back("Simple", data_t{ 1, 0, 0, 0, 1, 0, 1, 0, 1 }, data_t{ 1, 0, 1 }, data_t{ 0, 0 },
            // 100010101 =
            // 10 001   0  101 =
            // 10     1       101
            //     00      0
            true, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 1, 4, 6, 7, 8 });

        test_cases.emplace_back("Same 1", data_t{ 1, 2, 3, 4 }, data_t{ 1, 2, 3, 4 }, data_t{ 1, 2, 3, 1 },
            true, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 1, 2, 3 });

        test_cases.emplace_back("Same 2", data_t{ 1, 2, 3, 4 }, data_t{ 1, 2, 1 }, data_t{ 1, 2, 3, 4 }, true);

        test_cases.emplace_back("First mismatch", data_t{ 0, 1 }, data_t{ 1, 0 }, data_t{ 1 });

        test_cases.emplace_back("Too short", data_t{ 1, 1, 0 }, data_t{ 1, 1, 1 }, data_t{ 1, 1, 1, 0 });

        test_cases.emplace_back("Trivial 3 0", data_t{ 2, 2, 2 }, data_t{ 2 }, data_t{ 0 },
            true, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 1, 2 });

        test_cases.emplace_back("Trivial 3 1", data_t{ 2, 2, 2 }, data_t{ 2 }, data_t{ 2 }, true);

        test_cases.emplace_back("Trivial 3 2", data_t{ 2, 2, 2 }, data_t{ 0 }, data_t{ 2 }, true);

        test_cases.emplace_back("Trivial 1 no", data_t{ 2 }, data_t{ 0 }, data_t{ 1 });

        test_cases.emplace_back("Trivial 4 no", data_t{ 2, 2, 2, 2 }, data_t{ 1 }, data_t{ 0 });

        test_cases.emplace_back("Trivial rnd", data_t{ 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 }, data_t{ 1 },
            data_t{ 0 }, true, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 1, 2, 5, 6, 7, 8, 9 });

        test_cases.emplace_back("Trivial rnd rev", data_t{ 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
            data_t{ 0, 0 }, data_t{ 1, 1, 1 }, true, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 3, 4, 10, 11, 12, 13, 14 });
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<std::size_t> actual_indexes;

        const auto actual = Standard::Algorithms::Numbers::two_repetitions_interleaving(
            test.source_signal(), actual_indexes, test.sequence0(), test.sequence1());

        ::Standard::Algorithms::ert::are_equal(test.expected_result(), actual, "two_repetitions_interleaving");

        ::Standard::Algorithms::ert::are_equal(
            test.expected_indexes(), actual_indexes, "two_repetitions_interleaving indexes");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::two_repetitions_interleaving_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
