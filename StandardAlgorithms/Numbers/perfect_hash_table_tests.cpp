#include"perfect_hash_table_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"perfect_hash_table.h"
#include<set>

namespace
{
    using rand_t = Standard::Algorithms::Utilities::random_t<std::int16_t>;
    using int_t = typename rand_t::int_t;

    // todo(p4): [[nodiscard]] constexpr auto simple_tests() -> bool
    constexpr void simple_tests()
    {
        constexpr int_t some = 30;

        const std::vector<int_t> data{// NOLINTNEXTLINE
            20, some, 40, 10
        };

        {
            const auto iter = std::find(data.cbegin(), data.cend(), some);
            ::Standard::Algorithms::ert::are_equal(false, iter == data.cend(), "some in source data");
        }

        constexpr int_t inexisting = some + 1;
        {
            const auto iter = std::find(data.cbegin(), data.cend(), inexisting);
            ::Standard::Algorithms::ert::are_equal(true, iter == data.cend(), "inexisting in source data");
        }

        const Standard::Algorithms::Numbers::perfect_hash_table<int_t> table(data);
        {
            const auto has = table.contains(some);
            ::Standard::Algorithms::ert::are_equal(true, has, "some in table.");
        }
        {
            const auto has = table.contains(inexisting);
            ::Standard::Algorithms::ert::are_equal(false, has, " inexisting in table.");
        }
    }

    constexpr int_t small_man_leap = 1'000;
    constexpr int_t steps = 14;
    constexpr int_t cutter = steps * small_man_leap;

    void build_random(rand_t &rnd, std::vector<int_t> &data, std::set<int_t> &key_set)
    {
        constexpr int_t min_size = 1U;
        constexpr int_t max_size = 15U;

        static_assert(min_size <= max_size && max_size < cutter);

        const auto size = rnd(min_size, max_size);
        Standard::Algorithms::Utilities::fill_random<int_t>(data, size, cutter, true);

        key_set.clear();
        key_set.insert(data.cbegin(), data.cend());

        ::Standard::Algorithms::ert::are_equal(key_set.size(), data.size(), "The random keys must be unique");
    }

    void test_random(rand_t &rnd, const std::vector<int_t> &data, const std::set<int_t> &key_set)
    {
        Standard::Algorithms::require_positive(data.size(), "size");
        assert(key_set.size() == data.size());

        const Standard::Algorithms::Numbers::perfect_hash_table<int_t> table(data);

        for (const auto &key : data)
        {
            const auto has = table.contains(key);
            ::Standard::Algorithms::ert::are_equal(true, has, "table contains an existing random " + std::to_string(key));
        }

        for (int_t index{}; index < steps; ++index)
        {
            const auto key = rnd();
            const auto expected = key_set.contains(key);
            const auto actual = table.contains(key);

            ::Standard::Algorithms::ert::are_equal(
                expected, actual, "table contains a mankind random " + std::to_string(key));
        }
    }

    void random_tests()
    {
        rand_t rnd{};
        std::vector<int_t> data{};
        std::set<int_t> key_set{};

        constexpr auto max_attempts = 1;

        for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
        {
            build_random(rnd, data, key_set);

            try
            {
                test_random(rnd, data, key_set);
            }
            catch (const std::exception &exc)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Error in test_random: " << exc.what();
                ::Standard::Algorithms::print("Data", data, str);

                throw std::runtime_error(str.str());
            }
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::perfect_hash_table_tests()
{
    simple_tests();
    random_tests();
}
