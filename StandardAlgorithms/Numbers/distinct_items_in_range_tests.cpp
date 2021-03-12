#include"distinct_items_in_range_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"distinct_items_in_range.h"

namespace
{
    template<std::integral int_t, std::unsigned_integral size_t1>
    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        using range_t = std::pair<size_t1, size_t1>;

        constexpr test_case(std::string &&name, std::vector<int_t> &&data, std::vector<range_t> &&queries,
            std::vector<size_t1> &&expected = {}) noexcept
            : base_test_case(std::move(name))
            , Data(std::move(data))
            , Queries(std::move(queries))
            , Expected(std::move(expected))
        {
        }

        [[nodiscard]] constexpr auto data() const &noexcept -> const std::vector<int_t> &
        {
            return Data;
        }

        [[nodiscard]] constexpr auto queries() const &noexcept -> const std::vector<range_t> &
        {
            return Queries;
        }

        [[nodiscard]] constexpr auto expected() const &noexcept -> const std::vector<size_t1> &
        {
            return Expected;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            const auto data_size = Standard::Algorithms::require_positive(Data.size(), "Data size.");

            Standard::Algorithms::require_positive(Queries.size(), "Queries size.");

            ::Standard::Algorithms::ert::are_equal(Queries.size(), Expected.size(), "Expected size.");

            {
                const auto &max1 = *std::max_element(Expected.cbegin(), Expected.cend());

                ::Standard::Algorithms::ert::greater_or_equal(data_size, max1, "query maximum result");
            }

            for (const auto &query : Queries)
            {
                ::Standard::Algorithms::ert::greater(data_size, query.second, "query.right");

                ::Standard::Algorithms::ert::greater_or_equal(query.second, query.first, "query.left");
            }
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Data ", Data, str);
            ::Standard::Algorithms::print("Queries ", Queries, str);
            ::Standard::Algorithms::print("Expected ", Expected, str);
        }

private:
        std::vector<int_t> Data;
        std::vector<range_t> Queries;
        std::vector<size_t1> Expected;
    };

    template<class t>
    [[nodiscard]] constexpr auto append_type_name(const std::string &prefix) -> std::string
    {
        const auto *const type_name = typeid(t).name();

        auto result = prefix + "_" + std::string(type_name);
        return result;
    }

    template<std::integral int_t, std::unsigned_integral size_t1>
    [[nodiscard]] constexpr auto append_type_names(const std::string &prefix) -> std::string
    {
        auto result = append_type_name<size_t1>(append_type_name<int_t>(prefix));

        return result;
    }

    template<std::integral int_t, std::unsigned_integral size_t1>
    requires(sizeof(std::int16_t) <= sizeof(int_t))
    constexpr void add_random_test_case(std::vector<test_case<int_t, size_t1>> &test_cases)
    {
        constexpr size_t1 min_size = 1;
        constexpr size_t1 max_size = ::Standard::Algorithms::is_debug ? 10 : 20;

        Standard::Algorithms::Utilities::random_t<std::uint32_t> size_random;

        const auto size =
            Standard::Algorithms::require_positive(static_cast<size_t1>(size_random(min_size, max_size)), "random size");

        const auto queries_size = size << 1U;

        std::vector<typename test_case<int_t, size_t1>::range_t> range_queries(queries_size);

        for (size_t1 index{}; index < queries_size; ++index)
        {
            const auto dat_1 = static_cast<size_t1>(size_random() % size);
            const auto dat_2 = static_cast<size_t1>(size_random() % size);

            auto &quer = range_queries[index];

            quer.first = std::min(dat_1, dat_2);
            quer.second = std::max(dat_1, dat_2);
        }

        constexpr int_t max_value{ 32'000 };
        // todo(p4): use hash_map instead of max_element.

        std::vector<int_t> data;
        Standard::Algorithms::Utilities::fill_random<int_t, std::uint32_t>(data, size, max_value);

        auto expected = Standard::Algorithms::Numbers::distinct_items_in_range<int_t, size_t1>(data, range_queries);

        test_cases.emplace_back(append_type_names<int_t, size_t1>("Random"), std::move(data), std::move(range_queries),
            std::move(expected));
    }

    template<std::integral int_t, std::unsigned_integral size_t1>
    constexpr void generate_test_cases(std::vector<test_case<int_t, size_t1>> &test_cases)
    {
        test_cases.push_back({ append_type_names<int_t, size_t1>("Simple"),
            // NOLINTNEXTLINE
            { 4, 6, 5, 4, 5, 4, 6, 5, 4, 3, 5, 1 },
            // NOLINTNEXTLINE
            { { 0, 0 }, { 0, 1 }, { 2, 4 }, { 0, 10 }, { 0, 11 } },
            // NOLINTNEXTLINE
            { 1, 2, 2, 4, 5 } });

        add_random_test_case<int_t, size_t1>(test_cases);
    }

    template<std::integral int_t, std::unsigned_integral size_t1>
    constexpr void run_test_case(const test_case<int_t, size_t1> &test)
    {
        {
            const auto actual =
                Standard::Algorithms::Numbers::distinct_items_in_range<int_t, size_t1>(test.data(), test.queries());

            ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "distinct_items_in_range");
        }
        {
            const auto actual =
                Standard::Algorithms::Numbers::distinct_items_in_range_slow<int_t, size_t1>(test.data(), test.queries());

            ::Standard::Algorithms::ert::are_equal(test.expected(), actual, "distinct_items_in_range_slow");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::distinct_items_in_range_tests()
{
    {
        using test1 = test_case<std::uint64_t, std::uint32_t>;

        Standard::Algorithms::Tests::test_utilities<test1>::run_tests(run_test_case, generate_test_cases);
    }
    {
        using test2 = test_case<std::int16_t, std::uint64_t>;

        Standard::Algorithms::Tests::test_utilities<test2>::run_tests(run_test_case, generate_test_cases);
    }
}
