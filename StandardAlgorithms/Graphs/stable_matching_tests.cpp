#include"stable_matching_tests.h"
#include"../Utilities/ert.h"
#include"stable_matching.h"

namespace
{
    using vertex_t = std::int32_t;
    using preference_t = std::vector<vertex_t>;
    using preference_list_t = std::vector<preference_t>;

    struct sm_test_case final
    {
        std::string name{};
        preference_list_t a{};
        preference_list_t b{};
        preference_t expected{};
    };

    [[nodiscard]] constexpr auto build_test_cases() -> std::vector<sm_test_case>
    {
        std::vector<sm_test_case> tests;

        {
            sm_test_case test{ "specific 4 men and 4 women" };

            test.a.push_back(preference_t({ 2, 1, 0, 3 }));
            test.a.push_back(preference_t({ 1, 3, 0, 2 }));
            test.a.push_back(preference_t({ 2, 0, 3, 1 }));
            test.a.push_back(preference_t({ 0, 1, 2, 3 }));

            test.b.push_back(preference_t({ 0, 2, 1, 3 }));
            test.b.push_back(preference_t({ 2, 3, 1, 0 }));
            test.b.push_back(preference_t({ 1, 2, 3, 0 }));
            test.b.push_back(preference_t({ 3, 1, 0, 2 }));

            test.expected = preference_t({ 0, 3, 2, 1 });

            tests.push_back(std::move(test));
        }
        {
            sm_test_case test{ "repeatetive 4 men, 4 women" };

            {
                preference_t preference3{ 0, 1, 2, 3 };

                test.a = preference_list_t({ preference3, preference3, preference3, preference3 });

                test.b.push_back(preference_t({ 3, 1, 2, 0 }));
                test.b.push_back(preference_t({ 1, 0, 2, 3 }));
                test.b.push_back(preference3);
                test.b.push_back(std::move(preference3));
            }

            test.expected = preference_t({ 2, 1, 3, 0 });

            tests.push_back(std::move(test));
        }
        {
            sm_test_case test{ "Symmetric 2 men, 2 women" };
            // 0: (0, 1)
            // 1: (1, 0)

            preference_t preference{ 0, 1 };

            test.a.push_back(preference);
            test.a.push_back(preference_t({ 1, 0 }));

            test.b = test.a;

            test.expected = std::move(preference);

            tests.push_back(std::move(test));
        }
        {
            sm_test_case test{ "Trivial" };

            preference_t preference(1);
            test.a.push_back(preference);
            test.b.push_back(preference);

            test.expected = std::move(preference);

            tests.push_back(std::move(test));
        }

        return tests;
    }
} // namespace

void Standard::Algorithms::Graphs::Tests::stable_matching_tests()
{
    const auto tests = build_test_cases();

    std::vector<vertex_t> result{};

    for (const auto &test : tests)
    {
        Standard::Algorithms::Graphs::stable_matching<vertex_t>::find(test.a, result, test.b);

        ::Standard::Algorithms::ert::are_equal(test.expected, result, test.name);
    }
}
