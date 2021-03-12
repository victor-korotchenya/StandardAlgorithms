#include"enumerate_partitions_tests.h"
#include"../Utilities/test_utilities.h"
#include"enumerate_partitions.h"
#include<iostream>
#include<set>

namespace
{
    constexpr auto print_items = false;

    template<class item_t>
    constexpr void verify_subsets(
        const std::vector<item_t> &elements, const std::set<std::vector<std::vector<item_t>>> &all_sub_sets)
    {
        const std::set<item_t> sorted(elements.cbegin(), elements.cend());

        std::vector<item_t> buf(elements.size());
        std::set<item_t> uni;

        for (const auto &subset : all_sub_sets)
        {
            buf.clear();
            uni.clear();

            ::Standard::Algorithms::ert::greater(subset.size(), 0U, "Set size");

            for (const auto &sub_1 : subset)
            {
                ::Standard::Algorithms::ert::greater(sub_1.size(), 0U, "Subset size");

                for (const auto &item : sub_1)
                {
                    const auto added = uni.insert(item);

                    ::Standard::Algorithms::ert::are_equal(true, added.second, "Subset items must be unique.");
                }
            }

            ::Standard::Algorithms::ert::are_equals(sorted, uni, "The subsets must be complete.");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::enumerate_partitions_tests()
{
    auto lambda0 = [](const std::vector<std::vector<std::int32_t>> &items0, std::int32_t &cnt0,
                       std::set<std::vector<std::vector<std::int32_t>>> &set0) -> void
    {
        ::Standard::Algorithms::ert::greater(items0.size(), 0U, "Partition cannot be empty.");

        auto items_copy = items0;

        for (auto &subs : items_copy)
        {
            std::sort(subs.begin(), subs.end());
        }

        std::sort(items_copy.begin(), items_copy.end());

        set0.insert(items_copy);
        ++cnt0;

        ::Standard::Algorithms::ert::are_equal(static_cast<std::size_t>(cnt0), set0.size(), "A partition cannot repeat.");

        if constexpr (print_items)
        {
            auto has0 = false;

            for (const auto &list0 : items0)
            {
                assert(!list0.empty());

                std::cout << (has0 ? " (" : "(");
                has0 = true;

                auto has1 = false;

                for (const auto &ele : list0)
                {
                    std::cout << (has1 ? " " : "") << ele;
                    has1 = true;
                }

                std::cout << ")";
            }

            std::cout << '\n';
        }
    };

    std::int32_t cnt1{};
    std::set<std::vector<std::vector<std::int32_t>>> set1;

    auto lam1 = [&lambda0, &cnt1, &set1](const std::vector<std::vector<std::int32_t>> &elems)
    {
        lambda0(elems, cnt1, set1);
    };

    const std::vector<std::int32_t> elements{ 1, 2, 3, 4 };
    {
        auto elements_copy = elements;

        enumerate_partitions_slow(elements_copy, lam1);

        {
            constexpr auto expected = 15;

            ::Standard::Algorithms::ert::are_equal(expected, cnt1, "enumerate_partitions_slow");
        }

        verify_subsets(elements, set1);
    }

    std::int32_t cnt2{};
    std::set<std::vector<std::vector<std::int32_t>>> set2;

    auto lam2 = [&lambda0, &cnt2, &set2](const std::vector<std::vector<std::int32_t>> &elems)
    {
        lambda0(elems, cnt2, set2);
    };

    enumerate_partitions_by_one(elements, lam2);

    ::Standard::Algorithms::ert::are_equal(cnt1, cnt2, "enumerate_partitions_by_one");

    ::Standard::Algorithms::ert::are_equals(set1, set2, "sets");
}
