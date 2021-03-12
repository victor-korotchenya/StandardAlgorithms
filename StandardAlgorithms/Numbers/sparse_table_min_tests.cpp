#include"sparse_table_min_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"sparse_table_min.h"

namespace
{
    constexpr auto nnn = 100U;
    constexpr auto log_nnn = 7U; // rounded up

    static_assert(Standard::Algorithms::Numbers::sparse_table_log_and_size_validate(log_nnn, nnn));

    using value_t = std::int32_t;

    constexpr value_t value_max = 10 * nnn;

    static_assert(value_t{} < value_max);
} // namespace

void Standard::Algorithms::Numbers::Tests::sparse_table_min_tests()
{
    static_assert(log_base_2_down(1) == 0); // NOLINTNEXTLINE
    static_assert(log_base_2_down(2) == 1); // NOLINTNEXTLINE
    static_assert(log_base_2_down(3) == 1); // NOLINTNEXTLINE
    static_assert(log_base_2_down(4) == 2); // NOLINTNEXTLINE
    static_assert(log_base_2_down(5) == 2); // NOLINTNEXTLINE
    static_assert(log_base_2_down(6) == 2); // NOLINTNEXTLINE
    static_assert(log_base_2_down(7) == 2); // NOLINTNEXTLINE
    static_assert(log_base_2_down(8) == 3); // NOLINTNEXTLINE
    static_assert(log_base_2_down(15) == 3); // NOLINTNEXTLINE
    static_assert(log_base_2_down(16) == 4); // NOLINTNEXTLINE

    Standard::Algorithms::Utilities::random_t<value_t> rnd(-value_max, value_max);
    Standard::Algorithms::Numbers::sparse_table_min<value_t, log_nnn, nnn> table;

    auto &data = table.data();
    std::generate(data.begin(), data.end(),
        [&rnd]
        {
            return rnd();
        });

    const auto random_bit = 0 < (data.at(0) % 2);
    const auto size = static_cast<value_t>(nnn - (random_bit ? 0U : 1U));
    table.init(size);

    std::int32_t start{};
    std::int32_t stop{};

    try
    {
        for (std::size_t index{}; index < nnn; ++index)
        {
            start = rnd(0, size - 1);
            stop = rnd(start + 1, size);

            const auto actual = table.range_minimum_query_index(start, stop);
            const auto &fast_data = data.at(actual);

            const auto *const slow_iter = std::min_element(data.cbegin() + start, data.cbegin() + stop);

            ::Standard::Algorithms::ert::not_equal(data.cend(), slow_iter, "slow iterator");

            ::Standard::Algorithms::ert::are_equal(fast_data, *slow_iter, "range_minimum_query_index");
        }
    }
    catch (const std::exception &exc)
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "Start " << start << ", stop " << stop << ", size " << size << ", data " << data << ", error:\n"
            << exc.what();

        throw std::runtime_error(str.str());
    }
}
