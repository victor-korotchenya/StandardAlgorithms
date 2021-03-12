#include"offline_min_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"offline_min.h"

namespace
{
    constexpr auto min_size = 1;
    constexpr auto max_size = 63;

    template<std::signed_integral int_t>
    constexpr void run_subtest(const std::string &name, const std::vector<int_t> &data, const int_t &perm_size,
        const std::vector<int_t> &raw_expected)
    {
        assert(!name.empty());

        try
        {
            const auto slow = Standard::Algorithms::Numbers::offline_min_slow(data, perm_size);

            if (const auto has_expected = !raw_expected.empty(); has_expected)
            {
                ::Standard::Algorithms::ert::are_equal(raw_expected, slow, "offline_min_slow");
            }

            std::vector<int_t> actual;
            Standard::Algorithms::Numbers::offline_min(data, perm_size, actual);
            ::Standard::Algorithms::ert::are_equal(slow, actual, "offline_min");
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << exc.what() << ", " << name << ", permutation size " << perm_size << ", ";
            ::Standard::Algorithms::print("data", data, str);
            ::Standard::Algorithms::print("raw expected", raw_expected, str);

            throw std::runtime_error(str.str());
        }
    }

    constexpr void simple_tests()
    {
        run_subtest("simply nine", // NOLINTNEXTLINE
            { 8, 4, 7, -1, -1, 6, 5, 3, -1, -1, -1, -1, 1, 2, 0, -1, -1, -1 }, // NOLINTNEXTLINE
            9, { 4, 7, 3, 5, 6, 8, 0, 1, 2 });

        run_subtest("simple reversed", // NOLINTNEXTLINE
            { 9, -1, 8, -1, 7, -1, 6, -1, 5, -1, 4, -1, 3, -1, 2, -1, 1, -1, 0, -1 }, // NOLINTNEXTLINE
            10, { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 });

        run_subtest("simple direct", // NOLINTNEXTLINE
            { 0, -1, 1, -1, 2, -1, 3, -1, 4, -1, 5, -1, 6, -1, 7, -1 }, // NOLINTNEXTLINE
            8, { 0, 1, 2, 3, 4, 5, 6, 7 });

        run_subtest("extract some", // NOLINTNEXTLINE
            { 4, 3, -1, 2, 0, 1, -1 }, 5, { 3, 0 });

        run_subtest("extract some b", // NOLINTNEXTLINE
            { 4, 3, -1, 2, 0, 1, -1, -1 }, 5, { 3, 0, 1 });

        run_subtest("extract some c", // NOLINTNEXTLINE
            { 4, 3, -1, 2, -1, 0, 1, -1, -1 }, 5, { 3, 2, 0, 1 });

        run_subtest("size 2", { 0, -1, 1, -1 }, 2, { 0, 1 });
        run_subtest("size 2 rev", { 1, -1, 0, -1 }, 2, { 1, 0 });

        run_subtest("partial size 2", { 0, 1, -1 }, 2, { 0 });
        run_subtest("partial size 2 rev", { 1, 0, -1 }, 2, { 0 });

        run_subtest("trivial", { 0, -1 }, 1, { 0 });
    }

    template<std::signed_integral int_t, class rand_t>
    constexpr auto build_random(std::vector<int_t> &buffer, rand_t &rnd, std::vector<int_t> &data) -> int_t
    {
        data.clear();

        const auto perm_size = rnd();
        Standard::Algorithms::require_positive(perm_size, "permutation size");
        data.reserve(perm_size * 2LL);

        constexpr int_t zero{};
        constexpr int_t one = 1;

        buffer.resize(perm_size);
        std::iota(buffer.begin(), buffer.end(), zero);
        Standard::Algorithms::Utilities::shuffle_data(buffer);

        for (const auto &beaf : buffer)
        {
            data.push_back(beaf);

            if (const auto to_be = static_cast<bool>(rnd()); to_be)
            {
                data.push_back(-one);
            }
        }

        if (!(data.back() < zero))
        {
            data.push_back(-one);
        }

        return perm_size;
    }

    void random_tests()
    {
        using rand_t = Standard::Algorithms::Utilities::random_t<std::int16_t>;
        using int_t = typename rand_t::int_t;

        const std::string name = "random test";
        std::vector<int_t> buffer;
        std::vector<int_t> data;
        rand_t rnd(min_size, max_size);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            const auto perm_size = build_random(buffer, rnd, data);

            run_subtest(name, data, perm_size, {});
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::offline_min_tests()
{
    simple_tests();
    random_tests();
}
