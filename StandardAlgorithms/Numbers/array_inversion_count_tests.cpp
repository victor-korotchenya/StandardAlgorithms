#include"array_inversion_count_tests.h"
#include"../Graphs/avl_tree.h"
#include"../Utilities/ert.h"
#include"../Utilities/random.h"
#include"../Utilities/require_utilities.h"
#include"array_inversion_count.h"

namespace
{
    using int_t = std::int64_t;
    using vec_int_t = std::vector<int_t>;
    using bst_t = Standard::Algorithms::Trees::avl_tree<int_t>;

    static_assert(Standard::Algorithms::Trees::Inner::can_count_v<typename bst_t::node_t>);

    constexpr auto has_unique_data(auto data) // pass by copy
    {
        std::sort(data.begin(), data.end());

        const auto iter = std::adjacent_find(data.cbegin(), data.cend());
        auto is_unique = iter == data.cend();
        return is_unique;
    }

    constexpr void test_helper(
        const vec_int_t &data, const std::string &name, bool has_expected = false, std::uint64_t expected = 0)
    {
        using pair_t = std::pair<std::string, std::uint64_t (*)(vec_int_t &)>;

        std::vector<pair_t> tests{ pair_t("slow " + name,
                                       [] [[nodiscard]] (vec_int_t & dat)
                                       {
                                           return Standard::Algorithms::Numbers::array_inversion_count::slow<int_t>(dat);
                                       }),
            pair_t("Merge sort " + name,
                [] [[nodiscard]] (vec_int_t & dat)
                {
                    const auto old_size = dat.size();

                    auto result1 =
                        Standard::Algorithms::Numbers::array_inversion_count::fast_using_merge_sort<int_t>(dat);

                    Standard::Algorithms::require_sorted(dat, "fast data");
                    ::Standard::Algorithms::ert::are_equal(old_size, dat.size(), "fast data size after sort");

                    return result1;
                }) };

        if (const auto is_unique = has_unique_data(data); is_unique)
        {
            tests.emplace_back("Unique data via BST " + name,
                [] [[nodiscard]] (vec_int_t & dat)
                {
                    return Standard::Algorithms::Numbers::array_inversion_count::unique_data_via_bst<int_t, bst_t>(dat);
                });
        }

        for (const pair_t &test : tests)
        {
            std::uint64_t actual{};
            auto copy1 = data;

            try
            {
                actual = test.second(copy1);

                if (has_expected)
                {
                    ::Standard::Algorithms::ert::are_equal(expected, actual, test.first);
                }
                else
                {
                    expected = actual;
                    has_expected = true;
                }
            }
            catch (const std::exception &exc)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Error in '" << test.first << "': " << exc.what();
                ::Standard::Algorithms::print("data", data, str);

                Standard::Algorithms::throw_exception(str);
            }
        }
    }

    constexpr void simple_test()
    {
        {// NOLINTNEXTLINE
            const vec_int_t data3{ 10, 10, 20 };
            test_helper(data3, "trio", true, 0);
        }

        const vec_int_t data{// NOLINTNEXTLINE
            1297, 127, 41, 400, 1, 14, 4
        };

        // NOLINTNEXTLINE
        constexpr std::size_t expected = 6 + 4 + 3 + 3 + 1;
        {// NOLINTNEXTLINE
            constexpr std::size_t actual_bst = 1 + 2 + 1 + 4 + 4 + 5;

            static_assert(expected == actual_bst);
        }

        test_helper(data, "Simple", true, expected);
    }

    constexpr void inversed_test()
    {// NOLINTNEXTLINE
        const vec_int_t data{ 4, 3, 2, 1 };

        const std::size_t expected = data.size() * (data.size() - 1U) / 2U;

        test_helper(data, "Inversed", true, expected);
    }

    void random_test()
    {
        constexpr auto mi_size = 1U;

        // NOLINTNEXTLINE
        constexpr auto max_size = ::Standard::Algorithms::is_debug ? 9U : 99U;

        vec_int_t data{};

        constexpr auto cutter = static_cast<int_t>(max_size / 2);
        static_assert(int_t{} < cutter);

        const auto size = Standard::Algorithms::Utilities::random_t<std::uint32_t>(mi_size, max_size)();
        Standard::Algorithms::Utilities::fill_random(data, size, cutter);

        test_helper(data, "Random", false);
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::array_inversion_count_tests()
{
    simple_test();
    inversed_test();
    random_test();
}
