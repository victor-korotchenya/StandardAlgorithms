#include"merge_tests.h"
#include"../Utilities/elapsed_time_ns.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"inplace_merge.h"
#include"merge.h"
#include<functional>
#include<iostream>
#include<tuple>

namespace // Test shall_run_in_parallel()
{
    constexpr auto lim = Standard::Algorithms::Numbers::Inner::profitable_merge;
    constexpr auto wow = Standard::Algorithms::Numbers::Inner::compute_double_limit(lim);

    static_assert(lim < wow);

    constexpr auto nanny = 0U;
    constexpr auto lefty = 1U;
    constexpr auto righty = 2U;
    constexpr auto booty = lefty | righty;

    [[nodiscard]] constexpr auto zyp(const std::pair<std::size_t, std::size_t> &left_eq, const std::size_t left_size,
        const std::pair<std::size_t, std::size_t> &right_eq, const std::size_t right_size,
        const std::uint32_t &expected) noexcept -> bool
    {
        const auto [hop, ala] =
            Standard::Algorithms::Numbers::Inner::shall_run_in_parallel<lim>(left_eq, left_size, right_eq, right_size);

        const auto actual = (hop ? lefty : nanny) | (ala ? righty : nanny);
        auto res = expected == actual;
        return res;
    }

    // Bad input tests.
    static_assert(zyp({ 0, 0 }, 0, { 0, 0 }, 0, nanny));
    static_assert(zyp({ 0, 1 }, 0, { 0, 1 }, 0, nanny));
    static_assert(zyp({ 0, 1 }, 0, { 0, 1 }, 1, nanny));
    static_assert(zyp({ 0, 1 }, 1, { 0, 1 }, 0, nanny));
    static_assert(zyp({ 0, lim }, lim + 1, { 0, 2 }, 1, nanny));
    static_assert(zyp({ 0, lim + 1 }, lim + 2, { 1, 3 }, 2, nanny));
    static_assert(zyp({ lim + 1, lim * 2 }, lim * 2, { lim + 1, lim }, lim * 2, nanny));

    // Both too small.
    static_assert(zyp({ 0, 1 }, 1, { 0, 1 }, 1, nanny));
    static_assert(zyp({ 0, lim }, lim, { 0, lim }, lim, nanny));
    static_assert(zyp({ 0, lim }, lim + 1, { 0, lim }, lim + 1, nanny));
    static_assert(zyp({ lim - 1, lim }, lim, { lim - 1, lim }, lim, nanny));
    static_assert(zyp({ lim, lim }, lim, { lim - 1, lim }, lim, nanny));

    // Left only.
    static_assert(zyp({ lim, lim }, lim,
        // No median is here, but this test is not about the median.
        { lim, lim }, lim, lefty));

    static_assert(zyp({ lim, lim }, lim + 1, { lim, lim + 1 }, lim + 1, lefty));
    static_assert(zyp({ lim, lim }, lim * 2, { lim, lim }, lim * 2, lefty));
    static_assert(zyp({ lim, lim }, lim * 2, { lim, lim + 1 }, lim * 2 + 2, lefty));
    static_assert(zyp({ lim, lim * 2 }, lim * 2, { lim, lim }, lim * 2, lefty));
    static_assert(zyp({ lim + 1, lim * 2 }, lim * 2, { lim, lim }, lim * 2, lefty));
    static_assert(zyp({ lim + 1, lim * 2 }, lim * 2, { lim + 1, lim + 1 }, lim * 2, lefty));
    static_assert(zyp({ lim + 1, lim * 2 }, lim * 2, { lim + 2, lim + 3 }, lim * 2, lefty));
    static_assert(zyp({ lim + 1, lim * 2 }, lim * 3 + 2, { lim + 2, lim + 3 }, lim * 2 + 4, lefty));

    // Right only.
    static_assert(zyp({ lim, lim }, lim * 2 + 1, { lim, lim + 1 }, lim * 2 + 2, righty));
    static_assert(zyp({ lim + 1, lim + 1 }, lim * 2 + 2, { lim, lim + 1 }, lim * 2 + 2, righty)); // NOLINTNEXTLINE
    static_assert(zyp({ lim + 1, lim * 2 }, lim * 3 + 2, { lim + 2, lim + 3 }, lim * 2 + 5, righty));

    static_assert(zyp({ wow - 2, wow }, wow * 2, { wow - 1, wow }, wow * 2, righty));
    static_assert(zyp({ wow - 1, wow }, wow * 2, { wow, wow }, wow * 2, righty));
    static_assert(zyp({ wow, wow }, wow * 2, { wow - 1, wow }, wow * 2, righty));
    static_assert(zyp({ wow, wow }, wow * 2 + 1, { wow - 1, wow }, wow * 2 + 1, righty)); // NOLINTNEXTLINE
    static_assert(zyp({ wow, wow }, wow * 20 + 1, { wow - 1, wow }, wow * 20 + 1, righty));

    // Both
    static_assert(zyp({ wow, wow }, wow * 2, { wow, wow }, wow * 2, booty)); // NOLINTNEXTLINE
    static_assert(zyp({ wow, wow }, wow * 20LLU, { wow, wow }, wow * 3LLU, booty)); // NOLINTNEXTLINE
    static_assert(zyp({ wow, wow }, wow * 20LLU, { wow, wow }, wow * 5000LLU, booty)); // NOLINTNEXTLINE
    static_assert(zyp({ wow, wow }, wow * 2000LLU, { wow, wow }, wow * 80000LLU, booty));
    static_assert(zyp({ wow, wow + 1 }, wow * 2 + 1, { wow, wow }, wow * 2, booty));
    static_assert(zyp({ wow, wow + 1 }, wow * 2 + 1, { wow, wow }, wow * 2 + 1, booty));
    static_assert(zyp({ wow * 2, wow * 2 }, wow * 3 + 1, { wow * 2, wow * 3 }, wow * 4 + 1, booty));
    static_assert(zyp({ wow * 2, wow * 2 }, wow * 4 + 1, { wow, wow }, wow * 4 + 1, booty));
} // namespace

namespace
{
    using int_t = std::int64_t;
    using random_t = Standard::Algorithms::Utilities::random_t<int_t>;

    constexpr auto shall_print_merge = false;
    constexpr int_t something = 0xDeadBeaf;

    [[nodiscard]] auto prepare_buffer_test_first(
        const std::vector<int_t> &aaa, const std::vector<int_t> &bbb, std::vector<int_t> &buff)
    {
        assert(&aaa != &buff && &bbb != &buff);

        {
            constexpr auto mess = 2;
            buff.assign(aaa.size() + bbb.size() + mess, something);
        }

        const Standard::Algorithms::elapsed_time_ns tim;
        Standard::Algorithms::Numbers::merge(aaa, bbb, buff);
        auto elapsed = tim.elapsed();

        ::Standard::Algorithms::ert::are_equal(aaa.size() + bbb.size(), buff.size(), "merge buffer size");
        {
            const auto good = std::is_sorted(buff.begin(), buff.end());
            ::Standard::Algorithms::ert::are_equal(true, good, "merge is sorted");
        }

        return elapsed;
    }

    constexpr void test_others(const std::vector<int_t> &aaa, const std::vector<int_t> &bbb,
        const std::vector<int_t> &buff, [[maybe_unused]] const auto &base_merge_time, std::vector<int_t> &buff_2)
    {
        assert(&aaa != &buff_2 && &bbb != &buff_2 && &buff != &buff_2);

        if constexpr (shall_print_merge)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << " " << buff.size() << " items, base time " << base_merge_time << "\n";

            std::cout << str.str();
        }

        using func_t = std::function<void()>;

        auto runner = [&aaa, &bbb, &buff, &base_merge_time, &buff_2](const std::string &name, const func_t &func)
        {
            assert(!name.empty());

            {
                constexpr auto extra_mess = 3;
                buff_2.assign(aaa.size() + bbb.size() + extra_mess, something);
            }

            const Standard::Algorithms::elapsed_time_ns tim;
            func();
            [[maybe_unused]] const auto elapsed = tim.elapsed();

            ::Standard::Algorithms::ert::are_equal(buff, buff_2, name);

            if constexpr (shall_print_merge)
            {
                const auto ratio = ::Standard::Algorithms::ratio_compute(base_merge_time, elapsed);

                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "  " << name << ", time " << elapsed << ", cur/base ratio " << ratio << "\n";

                std::cout << str.str();
            }
            else
            {
                // NOLINTNEXTLINE NOLINT
                if (0 < base_merge_time) // Pacify the compiler.
                {
                }
            }
        };

        runner("std::merge",
            [&aaa, &bbb, &buff_2]()
            {
                buff_2.clear();
                std::merge(aaa.begin(), aaa.end(), bbb.begin(), bbb.end(), std::back_inserter(buff_2));
            });

#ifndef __clang__
        runner("std::merge par_unseq",
            [&aaa, &bbb, &buff_2]()
            {
                buff_2.clear();
                std::merge(std::execution::par_unseq, aaa.begin(), aaa.end(), bbb.begin(), bbb.end(),
                    std::back_inserter(buff_2));
            });
#endif

        runner("inplace_merge",
            [&aaa, &bbb, &buff_2]()
            {
                buff_2 = bbb;
                Standard::Algorithms::Numbers::inplace_merge(aaa, buff_2);
            });

        runner("parallel_merge",
            [&aaa, &bbb, &buff_2]()
            {
                Standard::Algorithms::Numbers::parallel_merge(aaa, bbb, buff_2);
            });
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::merge_tests()
{
    random_t rnd{};

    auto gena = [&rnd](auto &data)
    {
        constexpr auto min_size = 0U;
        constexpr auto merge_shift = 6U;
        constexpr auto max_size = lim << merge_shift;

        static_assert(lim < max_size);

        constexpr auto min_value = std::numeric_limits<int_t>::min();
        constexpr auto max_value = std::numeric_limits<int_t>::max();

        data = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);
        std::sort(data.begin(), data.end());
    };

    std::vector<int_t> aaa;
    std::vector<int_t> bbb;
    std::vector<int_t> buff;
    std::vector<int_t> buff_2;

    constexpr auto max_attempts = 1;

    for (std::int32_t att{}; att < max_attempts; ++att)
    {
        gena(aaa);
        gena(bbb);

        try
        {
            const auto base_merge_time = prepare_buffer_test_first(aaa, bbb, buff);
            test_others(aaa, bbb, buff, base_merge_time, buff_2);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            ::Standard::Algorithms::print("Aaa", aaa, str);
            ::Standard::Algorithms::print("Bbb", bbb, str);

            str << "\nError: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }
}
