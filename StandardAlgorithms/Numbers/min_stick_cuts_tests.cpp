#include"min_stick_cuts_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/remove_duplicates.h"
#include"../Utilities/test_utilities.h"
#include"min_stick_cuts.h"
#include<set>

namespace
{
    using random_t = Standard::Algorithms::Utilities::random_t<std::int16_t>;
    using int_t = typename random_t::int_t;
    using long_int_t = std::int32_t;

    static_assert(sizeof(int_t) <= sizeof(long_int_t));

    constexpr long_int_t not_computed = -static_cast<long_int_t>(1);

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&cut_coordinates, int_t stick_length,
            std::vector<int_t> &&sequence_expected = {}, long_int_t sum_expected = not_computed)
            : base_test_case(std::move(name))
            , Cut_coordinates(std::move(cut_coordinates))
            , Stick_length(stick_length)
            , Expected({ std::move(sequence_expected), sum_expected })
        {
            Standard::Algorithms::sort_remove_duplicates(Cut_coordinates);
        }

        [[nodiscard]] constexpr auto cut_coordinates() const &noexcept -> const std::vector<int_t> &
        {
            return Cut_coordinates;
        }

        [[nodiscard]] constexpr auto stick_length() const noexcept -> int_t
        {
            return Stick_length;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::pair<std::vector<int_t>, long_int_t> &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Stick_length, "stick length");

            const auto size = Cut_coordinates.size();
            const auto &[sequence, sum] = Expected;

            if (size == 0U)
            {
                ::Standard::Algorithms::ert::are_equal(long_int_t{}, sum, "expected sum");

                ::Standard::Algorithms::ert::are_equal(0U, sequence.size(), "expected sequence size");

                return;
            }

            Standard::Algorithms::require_positive(Cut_coordinates.at(0), "first cut coordinate");

            for (std::size_t index = 1; index < size; ++index)
            {
                const auto &prev = Cut_coordinates[index - 1U];

                const auto &cur = Cut_coordinates[index];

                ::Standard::Algorithms::ert::greater(
                    cur, prev, "cut coordinates must increase at " + std::to_string(index));
            }

            Standard::Algorithms::require_greater(
                Stick_length, Cut_coordinates.back(), "stick length vs last cut coordinate");

            if (Expected.second < long_int_t{})
            {
                return;
            }

            ::Standard::Algorithms::ert::greater_or_equal(sum, Stick_length, "expected sum");

            ::Standard::Algorithms::ert::are_equal(size, sequence.size(), "expected sequence size");

            {
                auto cop = sequence;

                Standard::Algorithms::sort_remove_duplicates(cop);

                ::Standard::Algorithms::ert::are_equal(Cut_coordinates, cop, "expected sequence with duplicates removed");
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("cut coordinates", Cut_coordinates, str);
            ::Standard::Algorithms::print_value("stick length", str, Stick_length);

            ::Standard::Algorithms::print("expected sequence", Expected.first, str);
            ::Standard::Algorithms::print_value("expected sum", str, Expected.second);
        }

private:
        std::vector<int_t> Cut_coordinates;
        int_t Stick_length;
        std::pair<std::vector<int_t>, long_int_t> Expected;
    };

    void generate_random_tests(std::vector<test_case> &test_cases)
    {
        constexpr int_t min_stick = 2;
        constexpr int_t max_stick = 100;
        static_assert(min_stick < max_stick);

        random_t rnd(min_stick, max_stick);
        std::set<int_t> uniq{};

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto stick_length = rnd();

            constexpr int_t min_size = 1;
            constexpr int_t max_size = 10;
            static_assert(min_size < max_size);

            const auto size = rnd(min_size, max_size);

            constexpr int_t min_value = 1;
            const auto max_value = static_cast<int_t>(stick_length - 1);

            uniq.clear();

            for (int_t index{}; index < size; ++index)
            {
                auto cut = rnd(min_value, max_value);
                uniq.insert(cut);
            }

            std::vector<int_t> cut_coordinates(uniq.cbegin(), uniq.cend()); // set<> sorts.

            test_cases.emplace_back("Random" + std::to_string(att), std::move(cut_coordinates), stick_length);
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        generate_random_tests(test_cases);

        constexpr int_t cut1 = 1;
        constexpr int_t cut3 = 3;
        constexpr int_t cut6 = 6;
        constexpr int_t stick_length = 10;

        test_cases.push_back({ "base3", std::vector<int_t>{ cut1, cut3, cut6 }, stick_length,
            std::vector<int_t>{ cut6, cut3, cut1 }, stick_length + cut6 + cut3 });
        //  \ to  0      1      3      6      10
        // from-----------------------------------
        //  0             0      3      9(3) 19(6)
        //  1                     0      5      14(6)
        //  3                             0      7
        //  6                                     0

        test_cases.push_back({ "base2", std::vector<int_t>{ cut1, cut3 }, stick_length,
            std::vector<int_t>{ cut3, cut1 }, stick_length + cut3 });

        test_cases.push_back(
            { "base 1", std::vector<int_t>{ cut3 }, stick_length, std::vector<int_t>{ cut3 }, stick_length });

        test_cases.push_back({ "size-one only parts 9", // NOLINTNEXTLINE
            std::vector<int_t>{ 1, 2, 3, 4, 5, 6, 7, 8 }, 9,
            // [0*9] -> {0, 0} ; {current, total}
            // [0*4] [4 9] -> {+9, 9}
            // [0 2] [2 4] [4*9] -> {+4, 13}
            // [0*2] [2 4] [4 6] [6 9] -> {+5, 18}
            // [0 1] [1 2] [2*4] [4 6] [6 9] -> {+2, 20}
            // [0 1] [1 2] [2 3] [3 4] [4*6] [6 9] -> {+2, 22}
            // [0 1] [1 2] [2 3] [3 4] [4 5] [5 6] [6*9] -> {+2, 24}
            // [0 1] [1 2] [2 3] [3 4] [4 5] [5 6] [6 7] [7*9] -> {+3, 27}
            // [0 1] [1 2] [2 3] [3 4] [4 5] [5 6] [6 7] [7 8] [8 9] -> {+2, 29}
            // NOLINTNEXTLINE
            std::vector<int_t>{ 4, 2, 6, 1, 3, 5, 7, 8 }, 29 });

        test_cases.push_back({ "size-one only parts 8", // NOLINTNEXTLINE
            std::vector<int_t>{ 1, 2, 3, 4, 5, 6, 7 }, 8,
            // [0*8] -> {0, 0} ; {current, total}
            // [0*4] [4 8] -> {+8, 8}
            // [0 2] [2 4] [4*8] -> {+4, 12}
            // [0*2] [2 4] [4 6] [6 8] -> {+4, 16}
            // [0 1] [1 2] [2*4] [4 6] [6 8] -> {+2, 18}
            // [0 1] [1 2] [2 3] [3 4] [4*6] [6 8] -> {+2, 20}
            // [0 1] [1 2] [2 3] [3 4] [4 5] [5 6] [6*8] -> {+2, 22}
            // [0 1] [1 2] [2 3] [3 4] [4 5] [5 6] [6 7] [7*8] -> {+2, 24}
            // NOLINTNEXTLINE
            std::vector<int_t>{ 4, 2, 6, 1, 3, 5, 7 }, 24 });

        test_cases.push_back({ "base 0", {}, stick_length, {}, 0 });
    }

    constexpr void verify_cuts(const std::vector<int_t> &source_cuts, const int_t &stick_length,
        const std::pair<std::vector<int_t>, long_int_t> &expected, const std::string &name)
    {
        Standard::Algorithms::require_positive(stick_length, name + " stick length");

        const auto &[sequence, sum] = expected;
        const auto size = sequence.size();

        ::Standard::Algorithms::ert::are_equal(source_cuts.size(), size, name + " sequence size");

        const auto name_sum = name + " sum";

        if (source_cuts.empty())
        {
            ::Standard::Algorithms::ert::are_equal(long_int_t{}, sum, name_sum);

            return;
        }

        Standard::Algorithms::require_positive(sum, name_sum);

        const auto cuts = Standard::Algorithms::Numbers::Inner::pend_zero_and_length<int_t>(source_cuts, stick_length);

        using cut_t = std::pair<int_t, int_t>;
        std::set<cut_t> pieces{ { int_t{}, stick_length } };

        long_int_t actual_sum{};

        for (std::size_t index{}; index < size; ++index)
        {
            const auto isn = std::to_string(index);
            Standard::Algorithms::require_positive(pieces.size(), "pieces at index " + isn);

            const auto &cut = sequence[index];
            auto iter = pieces.lower_bound({ cut, int_t{} });
            if (iter == pieces.begin()) [[unlikely]]
            {
                throw std::runtime_error("Cannot split a piece " + std::to_string(cut) + " at " + isn);
            }

            --iter;
            const auto &[piece_begin, piece_end] = *iter;

            // NOLINTNEXTLINE
            if (!(piece_begin < cut && cut < piece_end)) [[unlikely]]
            {
                auto err = "Wrong split (" + std::to_string(piece_begin) + ", " + std::to_string(piece_end) +
                    ") of a piece " + std::to_string(cut) + " at " + isn;

                throw std::runtime_error(err);
            }

            const auto diff = static_cast<long_int_t>(piece_end - piece_begin);
            actual_sum += diff;
            Standard::Algorithms::require_positive(actual_sum, "actual sum at " + isn);

            const std::array<cut_t, 2> new_cuts{ { { piece_begin, cut }, { cut, piece_end } } };

            pieces.erase(iter);

            for (const auto &[piece_start, piece_stop] : new_cuts)
            {
                Standard::Algorithms::require_greater(piece_stop, piece_start, "cut at " + isn);

                if (const auto end1 = cuts.cend(), fin = std::lower_bound(cuts.cbegin(), end1, piece_start);
                    fin != end1 && *fin == piece_start)
                {
                    if (const auto nex = std::next(fin); nex != end1 && *nex == piece_stop)
                    {
                        continue;
                    }
                }

                const auto inserted = pieces.emplace(piece_start, piece_stop);

                ::Standard::Algorithms::ert::are_equal(
                    true, inserted.second, "Intervals cannot be duplicated, cut at " + isn);
            }
        }

        ::Standard::Algorithms::ert::are_equal(0U, pieces.size(), name + " leftover size");

        ::Standard::Algorithms::ert::are_equal(sum, actual_sum, name_sum);
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto fast =
            Standard::Algorithms::Numbers::min_stick_cuts<long_int_t, int_t>(test.cut_coordinates(), test.stick_length());

        if (int_t{} <= test.expected().second)
        {// todo(p3): print a pair properly.
            ::Standard::Algorithms::ert::are_equal(test.expected().first, fast.first, "min_stick_cuts.first");

            ::Standard::Algorithms::ert::are_equal(test.expected().second, fast.second, "min_stick_cuts.second");
        }

        verify_cuts(test.cut_coordinates(), test.stick_length(), fast, "min_stick_cuts");

        const auto slow =
            Standard::Algorithms::Numbers::min_stick_cuts_slow<int_t>(test.cut_coordinates(), test.stick_length());

        ::Standard::Algorithms::ert::are_equal(fast.second, slow.second, "min_stick_cuts_slow");

        verify_cuts(test.cut_coordinates(), test.stick_length(), slow, "min_stick_cuts_slow");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::min_stick_cuts_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
