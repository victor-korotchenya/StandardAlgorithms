#include"tests.h"
#include"../Geometry/add_tests.h"
#include"../Graphs/add_tests.h"
#include"../Numbers/add_tests.h"
#include"../Strings/add_tests.h"
#include"floating_point_type.h"
#include<cassert>
#include<functional>
#include<iostream>
#include<numeric>
#include<stdexcept>
#include<tuple>
#include<utility>
#include<vector>

// todo(p4): #include<format>, print, println

namespace
{
    using name_tests_algorithms_t = std::tuple<std::string, std::size_t, std::size_t>;
    using floating_t = Standard::Algorithms::floating_point_type;

    struct count_report final
    {
        std::size_t test_count{};
        std::size_t algorithm_count{};
        std::vector<name_tests_algorithms_t> name_counts{};
    };

    using tests_t = std::vector<Standard::Algorithms::test_function>;

    using add_func_t = std::function<void(tests_t &, const std::string &)>;

    void add_tests_and_report(const std::string &name, const add_func_t &func,
        const std::string &long_time_running_prefix, tests_t &tests, count_report &report)
    {
        assert(!long_time_running_prefix.empty() && !name.empty());

        func(tests, long_time_running_prefix);

        const auto new_tests = tests.size();
        if (new_tests <= report.test_count) [[unlikely]]
        {
            throw std::runtime_error("The tests of '" + name + "' must exist.");
        }

        {
            constexpr auto max_algorithms = 100U;

            if (const auto ite = std::find_if(tests.cbegin(), tests.cend(),
                    [] [[nodiscard]] (const auto &test)
                    {
                        return 0U == test.algorithm_count || max_algorithms < test.algorithm_count;
                    });
                ite != tests.cend()) [[unlikely]]
            {
                throw std::runtime_error("The algorithms of '" + ite->name +
                    "' must be positive and not greater than " + std::to_string(max_algorithms) + ".");
            }
        }

        const auto new_algorithms = std::accumulate(tests.cbegin(), tests.cend(), std::size_t{},
            [] [[nodiscard]] (const std::size_t sum1, const auto &test)
            {
                return sum1 + test.algorithm_count;
            });

        if (new_algorithms <= report.algorithm_count) [[unlikely]]
        {
            throw std::runtime_error("The algorithms of '" + name + "' must exist.");
        }

        report.name_counts.emplace_back(name, new_tests - report.test_count, new_algorithms - report.algorithm_count);

        report.test_count = new_tests;
        report.algorithm_count = new_algorithms;
    }

    void print(const count_report &report)
    {
        if (report.test_count == 0U) [[unlikely]]
        {
            throw std::runtime_error("There must be tests.");
        }

        if (report.algorithm_count == 0U) [[unlikely]]
        {
            throw std::runtime_error("There must be algorithms.");
        }

        constexpr floating_t hundred{ 100.0 };

        const auto test_mult = hundred / static_cast<floating_t>(report.test_count);

        const auto algorithm_mult = hundred / static_cast<floating_t>(report.algorithm_count);

        for (const auto &[name, tests, algorithms] : report.name_counts)
        {
            const auto test_perc = static_cast<floating_t>(tests) * test_mult;
            const auto algorithm_perc = static_cast<floating_t>(algorithms) * algorithm_mult;

            std::cout << name << " " << tests << " tests " << test_perc << " % of " << report.test_count << ", "
                      << algorithms << " algorithms " << algorithm_perc << " % of " << report.algorithm_count << ".\n";
        }

        // std::cout << std::format(std::string("Total {} tests.\n\n"), report.test_count);

        std::cout << "Total " << report.test_count << " tests, " << report.algorithm_count << " algorithms.\n\n";
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::gather_all_tests() -> std::vector<Standard::Algorithms::test_function>
{
    std::vector<test_function> tests;

    const std::string long_time_running_prefix = "#";
    count_report report{};

    add_tests_and_report(
        "Geometry", &Standard::Algorithms::Geometry::Tests::add_tests, long_time_running_prefix, tests, report);

    add_tests_and_report(
        "Graphs", &Standard::Algorithms::Graphs::Tests::add_tests, long_time_running_prefix, tests, report);

    add_tests_and_report(
        "Numbers", &Standard::Algorithms::Numbers::Tests::add_tests, long_time_running_prefix, tests, report);

    add_tests_and_report(
        "Strings", &Standard::Algorithms::Strings::Tests::add_tests, long_time_running_prefix, tests, report);

    print(report);

    return tests;
}
