#include"fractional_cascading_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"fractional_cascading.h"
#include<array>
#include<initializer_list>
#include<set>
#include<span>

namespace
{
    using key_t = std::int16_t;
    using pos_t = std::size_t;
    using random_t = Standard::Algorithms::Utilities::random_t<std::uint32_t>;

    const auto size_mask = 15U;
    const auto key_mask = 31U;

    constexpr void append_sorted_data(const std::span<const key_t> sorted_arr, std::vector<std::vector<key_t>> &data)
    {
        auto &last_row = data.emplace_back();

        const auto size = sorted_arr.size();
        if (size == 0U)
        {
            return;
        }

        auto pre = sorted_arr[0];

        for (const auto &index : sorted_arr)
        {
            ::Standard::Algorithms::ert::greater_or_equal(index, pre, "index vs pre in sorted array");

            last_row.push_back(index);
            pre = index;
        }
    }

    constexpr void run_step(const std::vector<std::vector<key_t>> &data,
        const Standard::Algorithms::Numbers::lower_bound_fractional_cascading<key_t> &frac,
        const std::vector<pos_t> &expected, const key_t &key, std::vector<pos_t> &actual,
        const bool shall_check_expected = true)
    {
        const auto name = ::Standard::Algorithms::Utilities::zu_string(key);

        if (shall_check_expected)
        {
            Standard::Algorithms::Numbers::lower_bound_many_slow(data, key, actual);

            ::Standard::Algorithms::ert::are_equal(expected, actual, "lower_bound_many_slow " + name);
        }

        actual.clear();

        frac.lower_bound_many(key, actual);

        ::Standard::Algorithms::ert::are_equal(expected, actual, "lower_bound_fractional_cascading " + name);
    }

    [[nodiscard]] constexpr auto build_predefined_data() -> std::vector<std::vector<key_t>>
    {
        std::vector<std::vector<key_t>> data;

        append_sorted_data(std::array<key_t, 0>{}, data);

        append_sorted_data(std::array<key_t, // NOLINTNEXTLINE
                               2>{ 6, 206 },
            data);

        append_sorted_data(std::array<key_t, // NOLINTNEXTLINE
                               3>{ 7, 57, 345 },
            data);

        append_sorted_data(std::array<key_t, // NOLINTNEXTLINE
                               1>{ 7 },
            data);

        append_sorted_data(std::array<key_t, // NOLINTNEXTLINE
                               5>{ -3, 7, 10, 1'234, 31'567 },
            data);

        const std::array<key_t, // NOLINTNEXTLINE
            6>
            arr = { -32'768, -32'767, 2'001, 23'456, 32'766, 32'767 };

        append_sorted_data(arr, data);

        append_sorted_data(std::array<key_t, // NOLINTNEXTLINE
                               4>{ -234, -67, 0, 8 },
            data);

        append_sorted_data(arr, data);

        return data;
    }

    constexpr void predefined_data_test()
    {
        const auto data = build_predefined_data();

        const Standard::Algorithms::Numbers::lower_bound_fractional_cascading<key_t> frac(data);

        std::vector<pos_t> actual;
        {
            constexpr key_t key = -32'768;

            const std::vector<pos_t> expected(data.size());

            run_step(data, frac, expected, key, actual);
        }
        {
            constexpr key_t key = 5;

            const std::vector<pos_t> expected{// NOLINTNEXTLINE
                0, 0, 0, 0, 1, 2, 3, 2
            };

            run_step(data, frac, expected, key, actual);
        }
        {
            constexpr key_t key = 6;

            const std::vector<pos_t> expected{// NOLINTNEXTLINE
                0, 0, 0, 0, 1, 2, 3, 2
            };

            run_step(data, frac, expected, key, actual);
        }
        {
            constexpr key_t key = 7;

            const std::vector<pos_t> expected{// NOLINTNEXTLINE
                0, 1, 0, 0, 1, 2, 3, 2
            };

            run_step(data, frac, expected, key, actual);
        }
        {
            constexpr key_t key = 8;

            const std::vector<pos_t> expected{// NOLINTNEXTLINE
                0, 1, 1, 1, 2, 2, 3, 2
            };

            run_step(data, frac, expected, key, actual);
        }
        {
            constexpr key_t key = 20;

            const std::vector<pos_t> expected{// NOLINTNEXTLINE
                0, 1, 1, 1, 3, 2, 4, 2
            };

            run_step(data, frac, expected, key, actual);
        }
        {
            constexpr key_t key = 32'766;

            const std::vector<pos_t> expected{// NOLINTNEXTLINE
                0, 2, 3, 1, 5, 4, 4, 4
            };

            run_step(data, frac, expected, key, actual);
        }
        {
            constexpr key_t key = 32'767;

            const std::vector<pos_t> expected{// NOLINTNEXTLINE
                0, 2, 3, 1, 5, 5, 4, 5
            };

            run_step(data, frac, expected, key, actual);
        }
    }

    void random_data_set(random_t &rnd, std::set<key_t> &set1)
    {
        constexpr auto shift = 16U;
        {
            constexpr auto maxi = 12'000U;

            static_assert(size_mask < key_mask && key_mask <= maxi);
        }

        const auto size1 = rnd() & size_mask;
        assert(size1 <= size_mask);

        set1.clear();

        while (set1.size() < size1)
        {
            const auto rando = rnd();
            const auto one = static_cast<key_t>(rando & key_mask);

            const auto two = static_cast<key_t>((rando >> shift) & key_mask);

            set1.insert(std::initializer_list<key_t>{ one, two });
        }
    }

    void random_data(random_t &rnd, const std::set<key_t> &set1, std::vector<std::vector<key_t>> &data)
    {
        const auto size = Standard::Algorithms::require_positive((rnd() & size_mask) + 1U, "size");

        data.clear();
        data.resize(size);

        for (const auto &index : set1)
        {
            const auto line = rnd() % size;
            assert(line < size);

            auto &datu = data[line];
            datu.push_back(index);
        }
    }

    void random_data_sub(random_t &rnd, std::set<key_t> &set1, std::vector<pos_t> &expected,
        std::vector<std::vector<key_t>> &data, std::vector<pos_t> &actual)
    {
        // todo(p3): any non-unique data.
        random_data_set(rnd, set1);
        random_data(rnd, set1, data);

        const Standard::Algorithms::Numbers::lower_bound_fractional_cascading<key_t> frac(data);

        constexpr auto max_attempts = 1;

        for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
        {
            const auto key = static_cast<key_t>(rnd() & key_mask);

            expected.clear();

            Standard::Algorithms::Numbers::lower_bound_many_slow(data, key, expected);

            run_step(data, frac, expected, key, actual, false);
        }
    }

    void random_data_test()
    {
        random_t rnd{};
        std::set<key_t> set1{};
        std::vector<std::vector<key_t>> data{};
        std::vector<pos_t> actual{};
        std::vector<pos_t> expected{};

        constexpr auto max_attemps = 1;

        for (std::int32_t step{}; step < max_attemps; ++step)
        {
            try
            {
                data.clear();

                random_data_sub(rnd, set1, expected, data, actual);
            }
            catch (const std::exception &exc)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Error in random_data_sub: " << exc.what() << "\n";

                ::Standard::Algorithms::print("data", data, str);

                throw std::runtime_error(str.str());
            }
        }
    }

    void empty_lines_test()
    {
        constexpr auto size = 9;

        const std::vector<std::vector<key_t>> data(size);

        const Standard::Algorithms::Numbers::lower_bound_fractional_cascading<key_t> frac(data);
        const std::vector<pos_t> expected(size);

        std::vector<pos_t> actual;
        random_t rnd{};

        constexpr auto max_attemps = 1;

        for (std::int32_t step{}; step < max_attemps; ++step)
        {
            const auto key = static_cast<key_t>(rnd());

            run_step(data, frac, expected, key, actual);
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::fractional_cascading_tests()
{
    random_data_test();
    predefined_data_test();
    empty_lines_test();
}
