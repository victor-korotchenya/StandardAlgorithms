#include"edit_distance_vintsiuk_tests.h"
#include"../Utilities/test_utilities.h"
#include"edit_distance_vintsiuk.h"
#include<tuple>

namespace
{
    using costs_t = std::tuple<std::size_t, std::size_t, std::size_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, costs_t &&costs, std::string &&source, std::size_t expected,
            std::string &&destination) noexcept
            : base_test_case(std::move(name))
            , Source(std::move(source))
            , Destination(std::move(destination))
            , Add_cost(std::get<0>(costs))
            , Change_cost(std::get<1>(costs))
            , Delete_cost(std::get<2>(costs))
            , Expected_cost(expected)
        {
        }

        [[nodiscard]] constexpr auto source() const &noexcept -> const std::string &
        {
            return Source;
        }

        [[nodiscard]] constexpr auto destination() const &noexcept -> const std::string &
        {
            return Destination;
        }

        [[nodiscard]] constexpr auto add_cost() const noexcept -> std::size_t
        {
            return Add_cost;
        }

        [[nodiscard]] constexpr auto change_cost() const noexcept -> std::size_t
        {
            return Change_cost;
        }

        [[nodiscard]] constexpr auto delete_cost() const noexcept -> std::size_t
        {
            return Delete_cost;
        }

        [[nodiscard]] constexpr auto expected_cost() const noexcept -> std::size_t
        {
            return Expected_cost;
        }

        void print(std::ostream &str) const override
        {
            str << ", Add cost " << Add_cost << ", change cost " << Change_cost << ", delete cost " << Delete_cost
                << ", source " << Source << ", destination " << Destination << ", expected " << Expected_cost;
        }

private:
        std::string Source;
        std::string Destination;
        std::size_t Add_cost;
        std::size_t Change_cost;
        std::size_t Delete_cost;
        std::size_t Expected_cost;
    };

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr auto add = 1;
        constexpr auto change = 2;
        constexpr auto deletec = 4;

        test_cases.emplace_back("Simple", costs_t(add, change, deletec), "abcfNM", change * 2 + add, "acdffNM");

        test_cases.emplace_back("Same", costs_t(add, change, deletec), "abcf", 0, "abcf");

        test_cases.emplace_back("Delete", costs_t(add, change, deletec), "abcf", deletec, "abf");

        test_cases.emplace_back("Delete2", costs_t(add, change, deletec),
            // "ab c eee c f";
            // "ab   eee   f";
            "abceeecf", deletec * 2, "abeeef");

        test_cases.emplace_back("Delete too expensive", costs_t(add, change, deletec),
            // "sd   f s mj dhh";
            // "lj e f a mj    ";//1 add, 3 deletes, 3 changes = 1 + 12 + 6 = 19.
            // vs
            // "sdfsmjd hh";
            // "ljefamj   ";//2 deletes, 7 changes = 8 + 14 = 22.
            // vs
            // "  sd f s mj dhh";
            // "l je f a mj   ";//1 add, 3 deletes, 3 changes = 1 + 12 + 6 = 19.
            "AK sdfsmjdhh FSD", add + change * 3 + deletec * 3, "AK ljefamj FSD");

        test_cases.emplace_back("All same cost.", costs_t(add, add, add),
            // "sd   f s mj dhh";
            // "lj e f a mj    ";//1 add, 3 deletes, 3 changes = 7.
            // vs
            // "sdfsmjd hh";
            // "ljefamj   ";//2 deletes, 7 changes = 9.
            // vs
            // "  sd f s mj dhh";
            // "l je f a mj   ";//1 add, 3 deletes, 3 changes = 7.
            "AK sdfsmjdhh FSD", add * (1 + 3 + 3), "AK ljefamj FSD");

        {
            constexpr auto expensive = 10;
            constexpr auto add_count = 7;

            test_cases.emplace_back("Delete, Add expensive", costs_t(expensive, add, expensive), "AK sdfsmjdhh FSD",
                add * add_count + expensive * 2,
                "AK ljefamj FSD"); // 7 changes, 2 deletes.
        }

        test_cases.emplace_back("Add", costs_t(add, change, deletec), "acf", add, "abcf");

        test_cases.emplace_back("Simple2", costs_t(add, change, deletec),
            // "sdh f s kj dh";
            // "lje f a kj   ";
            "AK_sdhfskjdh FSD", change * 4 + deletec * 2, "AK_ljefakj FSD");

        test_cases.emplace_back("Simple2 Reversed", costs_t(add, change, deletec), "AK_ljefakj FSD",
            add * 2 + change * 4, "AK_sdhfskjdh FSD");
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto actual = Standard::Algorithms::Strings::edit_distance_vintsiuk(
            test.add_cost(), test.source(), test.change_cost(), test.destination(), test.delete_cost());

        ::Standard::Algorithms::ert::are_equal(test.expected_cost(), actual, "edit_distance_vintsiuk");
    }
} // namespace

void Standard::Algorithms::Strings::Tests::edit_distance_vintsiuk_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
