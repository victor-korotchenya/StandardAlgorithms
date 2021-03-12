#include"weighted_interval_scheduling_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"weighted_interval_scheduling.h"

namespace // are_compatible tests.
{
    using distance_t = std::uint32_t;
    using rad_t = Standard::Algorithms::Utilities::random_t<distance_t>;
    using weight_t = std::uint64_t;
    using weighted_segment_t = Standard::Algorithms::Numbers::weighted_interval<distance_t, weight_t>;

    // Compile-time tests could be better than runtime-ones.
    [[nodiscard]] consteval auto compa(const weighted_segment_t &one, const weighted_segment_t &two) -> bool
    {
        Standard::Algorithms::require_less_equal(one.Start, one.Stop, "first interval");
        Standard::Algorithms::require_less_equal(two.Start, two.Stop, "second interval");

        auto compati = Standard::Algorithms::Numbers::Inner::are_compatible<weighted_segment_t>(one, two);

        const auto rev = Standard::Algorithms::Numbers::Inner::are_compatible<weighted_segment_t>(two, one);

        if (compati == rev) [[likely]]
        {
            return compati;
        }

        throw std::invalid_argument("are_compatible has an error.");
    }

    // Far away, at least 1 unit.
    static_assert(compa(weighted_segment_t{ 0, 0 }, weighted_segment_t{ 1, 1 }));

    static_assert(compa(weighted_segment_t{ 0, 0 }, weighted_segment_t{ 2, 2 }));

    static_assert(compa(weighted_segment_t{ 1, 1 }, weighted_segment_t{ 2, 2 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 1, 1 }, weighted_segment_t{ 2, 20 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 1, 1 }, weighted_segment_t{ 3, 20 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 1, 2 }, weighted_segment_t{ 10, 20 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 21, 21 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 21, 22 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 100, 120 }));

    // One touching at either end point.
    static_assert(compa(weighted_segment_t{ 0, 1 }, weighted_segment_t{ 1, 1 }));

    static_assert(compa(weighted_segment_t{ 1, 1 }, weighted_segment_t{ 1, 1 }));

    static_assert(compa(weighted_segment_t{ 1, 1 }, weighted_segment_t{ 1, 2 }));

    static_assert(compa(weighted_segment_t{ 2, 2 }, weighted_segment_t{ 1, 2 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 1, 10 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 9, 10 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 10, 10 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 20, 20 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 20, 21 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 20, 22 }));

    // NOLINTNEXTLINE
    static_assert(compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 20, 122 }));

    // Either interval length is at least 3.
    // Another interval length is 1,
    // and it crosses the first one somewhere in the middle excluding the end points.
    static_assert(!compa(weighted_segment_t{ 2, 2 }, weighted_segment_t{ 1, 3 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 3, 3 }, weighted_segment_t{ 1, 4 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 3, 3 }, weighted_segment_t{ 1, 5 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 11, 11 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 15, 15 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 18, 18 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 19, 19 }));

    // Touching/crossing in 2 or more points.
    static_assert(!compa(weighted_segment_t{ 1, 2 }, weighted_segment_t{ 1, 2 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 1, 2 }, weighted_segment_t{ 1, 3 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 2, 3 }, weighted_segment_t{ 1, 3 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 1, 3 }, weighted_segment_t{ 1, 3 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 1, 120 }, weighted_segment_t{ 10, 20 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 1, 11 }, weighted_segment_t{ 10, 20 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 1, 15 }, weighted_segment_t{ 10, 20 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 10, 19 }, weighted_segment_t{ 10, 20 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 15, 16 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 15, 19 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 15, 20 }));

    // NOLINTNEXTLINE
    static_assert(!compa(weighted_segment_t{ 10, 20 }, weighted_segment_t{ 15, 110 }));
} // namespace

namespace
{
    [[nodiscard]] constexpr auto chosen_weight_sum(
        const std::vector<weighted_segment_t> &segments, const std::vector<std::size_t> &chosen_indexes) -> weight_t
    {
        weight_t total_sum{};

        for (const auto &index : chosen_indexes)
        {
            const auto &weight =
                Standard::Algorithms::require_positive(segments.at(index).weight, "chosen segment weight");

            const auto old = total_sum;
            total_sum += weight;

            Standard::Algorithms::require_greater(total_sum, old, "chosen segments partial sum");
        }

        return total_sum;
    }

    constexpr void repair_ids(std::vector<weighted_segment_t> &segments) noexcept
    {
        std::size_t index{};

        for (auto &segment : segments)
        {
            segment.Id = index++;
        }
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<weighted_segment_t> &&segments,
            weight_t expected_weight = {}, std::vector<std::size_t> &&expected_segment_indexes = {}) noexcept
            : base_test_case(std::move(name))
            , Segments(std::move(segments))
            , Expected_weight(expected_weight)
            , Expected_segment_indexes(std::move(expected_segment_indexes))
        {
            repair_ids(Segments);
        }

        [[nodiscard]] constexpr auto segments() const &noexcept -> const std::vector<weighted_segment_t> &
        {
            return Segments;
        }

        [[nodiscard]] constexpr auto expected_weight() const &noexcept -> const weight_t &
        {
            return Expected_weight;
        }

        [[nodiscard]] constexpr auto is_computed() const noexcept -> bool
        {
            return weight_t{} < Expected_weight;
        }

        [[nodiscard]] constexpr auto expected_segment_indexes() const &noexcept -> const std::vector<std::size_t> &
        {
            return Expected_segment_indexes;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::throw_if_empty("segments", Segments);

            for (const auto &segment : Segments)
            {
                segment.validate();
            }

            const auto *const indexes_name = "Expected segment indexes";

            if (!is_computed())
            {
                ::Standard::Algorithms::ert::are_equal(0U, Expected_segment_indexes.size(), indexes_name);

                return;
            }

            Standard::Algorithms::throw_if_empty(indexes_name, Expected_segment_indexes);
            {
                constexpr auto require_unique = true;

                Standard::Algorithms::require_sorted(Expected_segment_indexes, indexes_name, require_unique);
            }
            {
                const auto sum = chosen_weight_sum(Segments, Expected_segment_indexes);

                ::Standard::Algorithms::ert::are_equal(Expected_weight, sum, "Computed chosen weight sum");
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Segments", Segments, str);
            ::Standard::Algorithms::print_value("Expected weight", str, Expected_weight);
            ::Standard::Algorithms::print("Expected segment indexes", Expected_segment_indexes, str);
        }

private:
        std::vector<weighted_segment_t> Segments;
        weight_t Expected_weight;
        std::vector<std::size_t> Expected_segment_indexes;
    };

    void add_random(std::vector<test_case> &test_cases, rad_t &rnd, const auto attempt)
    {
        constexpr distance_t min_size = 1U;
        constexpr distance_t max_size = 10U;

        const auto size = rnd(min_size, max_size);
        std::vector<weighted_segment_t> segments(size);

        for (auto &segment : segments)
        {
            constexpr distance_t min_start = 0U;
            constexpr distance_t maxim = 500U;
            static_assert(min_start < maxim);

            segment.Start = rnd(min_start, maxim);
            segment.Stop = rnd(min_start, maxim);

            if (segment.Stop < segment.Start)
            {
                std::swap(segment.Stop, segment.Start);
            }

            constexpr distance_t min_weight = 1U;
            static_assert(min_weight < maxim);

            segment.weight = rnd(min_weight, maxim);
        }

        test_cases.emplace_back("random" + std::to_string(attempt), std::move(segments));
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        {
            rad_t rnd{};

            constexpr auto max_attempts = 1;

            for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
            {
                add_random(test_cases, rnd, attempt);
            }
        }

        test_cases.emplace_back("First 3 of 4",
            std::vector<weighted_segment_t>{// NOLINTNEXTLINE
                { 232, 468, 630 }, { 806, 967, 276 }, // NOLINTNEXTLINE
                { 967, 967, 78 }, { 87, 402, 468 } }, // NOLINTNEXTLINE
            630 + 276 + 78, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 1, 2 });

        test_cases.emplace_back("Edges",
            std::vector<weighted_segment_t>{// NOLINTNEXTLINE
                { 9, 11, 1 }, // NOLINTNEXTLINE
                { 1, 7, 2 }, { 3, 6, 4 } }, // NOLINTNEXTLINE
            5, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 2 });

        test_cases.emplace_back("Some",
            std::vector<weighted_segment_t>{// NOLINTNEXTLINE
                { 9, 11, 1 }, // NOLINTNEXTLINE
                { 1, 6, 2 }, { 3, 7, 4 }, { 5, 8, 4 }, // NOLINTNEXTLINE
                { 4, 9, 7 }, { 4, 9, 6 }, { 8, 10, 2 }, // NOLINTNEXTLINE
                { 1, 12, 1 } }, // NOLINTNEXTLINE
            8, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 4 });

        test_cases.emplace_back("Some 2",
            std::vector<weighted_segment_t>{// NOLINTNEXTLINE
                { 9, 11, 1 }, // NOLINTNEXTLINE
                { 1, 4, 2 }, { 3, 7, 4 } }, // NOLINTNEXTLINE
            5, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 2 });

        test_cases.emplace_back("Some 3",
            std::vector<weighted_segment_t>{// NOLINTNEXTLINE
                { 9, 11, 1 }, // NOLINTNEXTLINE
                { 1, 7, 2 }, { 3, 6, 4 } }, // NOLINTNEXTLINE
            5, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 2 });

        test_cases.emplace_back("Some 4",
            std::vector<weighted_segment_t>{// NOLINTNEXTLINE
                { 9, 11, 1 }, // NOLINTNEXTLINE
                { 1, 7, 2 }, { 3, 7, 4 } }, // NOLINTNEXTLINE
            5, // NOLINTNEXTLINE
            std::vector<std::size_t>{ 0, 2 });

        {
            constexpr distance_t expected_weight = 1025U;

            test_cases.emplace_back("One segment", std::vector<weighted_segment_t>{ { 1, 3, expected_weight } },
                expected_weight, std::vector<std::size_t>{ 0 });
        }
        {
            constexpr auto lower_weight = 45U;
            constexpr auto expected_weight = 123U;

            static_assert(lower_weight < expected_weight);

            test_cases.emplace_back("Two overlap",
                std::vector<weighted_segment_t>{ { 0, 2, lower_weight }, { 1, 3, expected_weight } }, expected_weight,
                std::vector<std::size_t>{ 1 });

            test_cases.emplace_back("Two overlap rev",
                std::vector<weighted_segment_t>{ { 0, 2, expected_weight }, { 1, 3, lower_weight } }, expected_weight,
                std::vector<std::size_t>{ 0 });
        }
        {
            constexpr auto weight0 = 874100U;
            constexpr auto weight1 = 100U;

            test_cases.emplace_back("Two close",
                std::vector<weighted_segment_t>{ { 0, 1, weight0 }, { 2, 3, weight1 } }, weight0 + weight1,
                std::vector<std::size_t>{ 0, 1 });
        }
        {
            constexpr auto coord_0 = 10U;
            constexpr auto coord_1 = 31U;
            constexpr auto coord_2 = 449U;

            constexpr auto weight0 = 8721U;
            constexpr auto weight1 = 458U;

            test_cases.emplace_back("Two touching",
                std::vector<weighted_segment_t>{ { coord_0, coord_1, weight0 }, { coord_1, coord_2, weight1 } },
                weight0 + weight1, std::vector<std::size_t>{ 0, 1 });
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        // No constant
        // NOLINTNEXTLINE
        std::vector<std::size_t> chosenSegmentIndexes;

        const auto fast = ([&test, &chosenSegmentIndexes]() -> weight_t
        {
            chosenSegmentIndexes.clear();

            auto segments = test.segments();
            auto best =
                Standard::Algorithms::Numbers::weighted_interval_scheduling<distance_t, weight_t>::find_min_weight_schedule(
                    segments, chosenSegmentIndexes);

         Standard::Algorithms::require_positive(best, "best weight");

            if (!test.is_computed())
            {
                return best;
            }

            ::Standard::Algorithms::ert::are_equal(test.expected_weight(), best, "Fast weight");

            ::Standard::Algorithms::ert::are_equal(
                test.expected_segment_indexes(), chosenSegmentIndexes,
                // A long name.
                "Fast chosen segment indexes");

            return best;
        })();

        chosenSegmentIndexes.clear();

        const auto slow = Standard::Algorithms::Numbers::weighted_interval_scheduling<distance_t,
            weight_t>::find_min_weight_schedule_slow(test.segments(), chosenSegmentIndexes);

        ::Standard::Algorithms::ert::are_equal(fast, slow, "Slow weight");

        Standard::Algorithms::require_positive(chosenSegmentIndexes.size(), "Slow chosenSegmentIndexes size");

        const auto sum = chosen_weight_sum(test.segments(), chosenSegmentIndexes);

        ::Standard::Algorithms::ert::are_equal(fast, sum, "Slow chosen segment weight sum");
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::weighted_interval_scheduling_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
