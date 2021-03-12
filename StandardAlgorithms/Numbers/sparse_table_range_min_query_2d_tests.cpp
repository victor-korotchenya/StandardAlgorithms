#include"sparse_table_range_min_query_2d_tests.h"
#include"../Numbers/standard_operations.h"
#include"../Utilities/random.h"
#include"../Utilities/simple_test_case.h"
#include"../Utilities/test_utilities.h"
#include"sparse_table_range_min_query_2d.h"
#include<vector>

namespace
{
    using int_t = std::int16_t;
    using long_int_t = std::int32_t;
    using data_t = std::vector<std::vector<int_t>>;
    using operation_t = int_t (*)(const int_t &, const int_t &);

    constexpr auto query_size = 5U;
    using query_t = std::array<int_t, query_size>;

    constexpr int_t log_nnn = 4;
    constexpr int_t nnn = 11;
    static_assert(Standard::Algorithms::Numbers::sparse_table_log_and_size_validate(log_nnn, nnn));

    constexpr int_t log_mmmmm = 5;
    constexpr int_t mmmmm = 23;
    static_assert(Standard::Algorithms::Numbers::sparse_table_log_and_size_validate(log_mmmmm, mmmmm));

    struct unique_tag final
    {
    };

    [[nodiscard]] constexpr auto gen_range(auto &rando, int_t exclusive) -> std::pair<int_t, int_t>
    {
        assert(int_t{} < exclusive);
        --exclusive;

        auto low = rando.operator() (0, exclusive);
        auto high = rando.operator() (0, exclusive);

        if (high < low)
        {
            std::swap(low, high);
        }

        return { low, high };
    };

    using test_case = Standard::Algorithms::Tests::simple_test_case<unique_tag, data_t, std::vector<query_t>>;

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        data_t data(nnn);

        for (long_int_t index{}; index < nnn; ++index)
        {
            Standard::Algorithms::Utilities::fill_random(data[index], mmmmm);
        }

        constexpr auto queries_size = 1;

        std::vector<query_t> queries(queries_size);
        Standard::Algorithms::Utilities::random_t<int_t> rnd{};

        for (auto &query : queries)
        {
            const auto [xx1, xx2] = gen_range(rnd, nnn);
            assert(0 <= xx1 && xx1 <= xx2 && xx2 < nnn);

            const auto [yy1, yy2] = gen_range(rnd, mmmmm);
            assert(0 <= yy1 && yy1 <= yy2 && yy2 < mmmmm);

            auto slow_answer = std::numeric_limits<int_t>::max();

            for (long_int_t row = xx1; row <= xx2; row++)
            {
                for (long_int_t col = yy1; col <= yy2; col++)
                {
                    slow_answer = std::min(slow_answer, data.at(row).at(col));
                }
            }

            query = { xx1, yy1, xx2, yy2, slow_answer };
        }

        test_cases.emplace_back("random test", std::move(data), std::move(queries));
    }

    constexpr void run_test_case(const test_case &test)
    {
        const Standard::Algorithms::Numbers::sparse_table_range_min_query_2d<int_t, log_nnn, nnn, log_mmmmm, mmmmm,
            operation_t>
            rmq(test.input(), &Standard::Algorithms::Numbers::min<int_t>);

        const auto &queries = test.output();

        for (std::size_t index{}; index < queries.size(); ++index)
        {
            const auto &query = queries[index];
            const auto actual = rmq.get(query[0], query[1], query[2], query[3]);

            ::Standard::Algorithms::ert::are_equal(query[4], actual, "Get at " + std::to_string(index));
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::sparse_table_range_min_query_2d_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
