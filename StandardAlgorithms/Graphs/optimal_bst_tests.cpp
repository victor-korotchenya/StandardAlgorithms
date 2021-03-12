#include"optimal_bst_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"optimal_bst.h"

namespace
{
    using random_t = Standard::Algorithms::Utilities::random_t<std::int32_t>;
    using int_t = std::int32_t;
    using long_int_t = std::int64_t;
    using node_t = ::Standard::Algorithms::Trees::optimal_bst_node<int_t>;

    constexpr long_int_t inf = std::numeric_limits<long_int_t>::max() / 8;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<int_t> &&hit_probabilities,
            std::vector<int_t> &&miss_probabilities, // std::vector<node_t> &&nodes,
            long_int_t expected_cost //, node_t expected_root
            ) noexcept
            : base_test_case(std::move(name))
            , Hit_probabilities(std::move(hit_probabilities))
            , Miss_probabilities(std::move(miss_probabilities)) // Nodes(std::move(nodes)),
            , Expected_cost(expected_cost) // , Expected_root(expected_root)
        {
        }

        [[nodiscard]] constexpr auto hit_probabilities() const &noexcept -> const std::vector<int_t> &
        {
            return Hit_probabilities;
        }

        [[nodiscard]] constexpr auto miss_probabilities() const &noexcept -> const std::vector<int_t> &
        {
            return Miss_probabilities;
        }

        [[nodiscard]] constexpr auto expected_cost() const &noexcept -> const long_int_t &
        {
            return Expected_cost;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Hit_probabilities.size(), "hit probabilities size");
            ::Standard::Algorithms::ert::are_equal(
                Hit_probabilities.size() + 1U, Miss_probabilities.size(), "miss probabilities size");

            Standard::Algorithms::require_all_positive(
                Hit_probabilities.cbegin(), Hit_probabilities.cend(), "hit probabilities");

            Standard::Algorithms::require_all_positive(
                Miss_probabilities.cbegin(), Miss_probabilities.cend(), "miss probabilities");

            Standard::Algorithms::require_positive(Expected_cost, "expected cost");

            // todo(p3): ::Standard::Algorithms::ert::are_equal(true, is_tree(Tree, true), "is tree");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("hit probabilities", Hit_probabilities, str);
            ::Standard::Algorithms::print("miss probabilities", Miss_probabilities, str);
            ::Standard::Algorithms::print_value("expected cost", str, Expected_cost);
        }

private:
        std::vector<int_t> Hit_probabilities;
        std::vector<int_t> Miss_probabilities;

        // std::vector<node_t> Nodes;

        long_int_t Expected_cost;
        // node_t Expected_root;
    };

    void generate_random(std::vector<test_case> &test_cases)
    {
        constexpr auto max_attempts = 1;

        const std::string prefix = "random_";
        std::vector<node_t> nodes;

        constexpr int_t min_value = 1;
        constexpr int_t max_value = 1'000'000'000;
        static_assert(min_value < max_value);

        random_t rnd(min_value, max_value);

        for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
        {
            constexpr auto min_size = 1;
            constexpr auto max_size = 40;

            auto hits = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);

            const auto size1 = static_cast<std::int32_t>(hits.size() + 1U);
            assert(min_size < size1 && size1 <= max_size + 1);

            auto misses = Standard::Algorithms::Utilities::random_vector(rnd, size1, size1, min_value, max_value);

            assert(misses.size() == static_cast<std::size_t>(size1));

            const auto actual = Standard::Algorithms::Trees::optimal_bst<int_t, long_int_t>(hits, inf, misses, nodes);

            const auto &cost = actual.first;
            assert(long_int_t{} < cost);

            test_cases.emplace_back(prefix + std::to_string(attempt), std::move(hits), std::move(misses), cost);
        }
    }

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        generate_random(test_cases);

        constexpr long_int_t dod1 = 1;
        constexpr long_int_t dod2 = 2;
        constexpr long_int_t dod3 = 3;
        constexpr long_int_t dod4 = 4;
        constexpr long_int_t dod5 = 5;

        constexpr int_t hit0 = 1'000;
        constexpr int_t miss0 = 3;
        constexpr int_t miss1 = 5;
        {
            constexpr long_int_t tc1 = dod1 * hit0 + dod2 * (miss0 + miss1);

            test_cases.push_back({ "trivial", { hit0 }, { miss0, miss1 }, tc1 });
            test_cases.push_back({ "trivial rev", { hit0 }, { miss1, miss0 }, tc1 });
        }

        constexpr int_t hit1 = 10'000;
        constexpr int_t miss2 = 7;
        {
            constexpr long_int_t tc2 = dod1 * hit1 + dod2 * hit0 + dod3 * miss0 + dod3 * miss1 + dod2 * miss2;

            //             hit1
            //           /        \;
            //       hit0      miss2
            //     /       \;
            // miss0  miss1
            test_cases.push_back({ "two right", { hit0, hit1 }, { miss0, miss1, miss2 }, tc2 });

            test_cases.push_back({ "two left", { hit1, hit0 }, { miss2, miss1, miss0 }, tc2 });
        }

        constexpr int_t hit2 = 100'000;
        constexpr int_t miss3 = 9;
        {
            //         hit2
            //         /;
            //     hit1
            //     /;
            // hit0
            constexpr long_int_t side_tc =
                dod1 * hit2 + dod2 * hit1 + dod3 * hit0 + dod4 * miss0 + dod4 * miss1 + dod3 * miss2 + dod2 * miss3;

            test_cases.push_back({ "012", { hit0, hit1, hit2 }, { miss0, miss1, miss2, miss3 }, side_tc });

            //     hit2
            //     /;
            // hit1
            //     \;
            //     hit0
            test_cases.push_back({ "102", { hit1, hit0, hit2 }, { miss2, miss0, miss1, miss3 }, side_tc });

            // hit2
            //      \;
            //       hit1
            //        /;
            //      hit0
            test_cases.push_back({ "201", { hit2, hit0, hit1 }, { miss3, miss1, miss0, miss2 }, side_tc });

            // hit2
            //     \;
            //     hit1
            //        \;
            //       hit0
            test_cases.push_back({ "210", { hit2, hit1, hit0 }, { miss3, miss2, miss1, miss0 }, side_tc });
        }
        {
            constexpr long_int_t center_tc =
                dod1 * hit2 + dod2 * (hit1 + hit0) + dod3 * (miss0 + miss1 + miss2 + miss3);

            //      hit2
            //     /      \;
            // hit0    hit1
            test_cases.push_back({ "021", { hit0, hit2, hit1 }, { miss0, miss1, miss2, miss3 }, center_tc });

            test_cases.push_back({ "120", { hit1, hit2, hit0 }, { miss0, miss1, miss2, miss3 }, center_tc });
        }
        constexpr int_t hit3 = 1'000'000;
        constexpr int_t miss4 = 11;
        {
            constexpr long_int_t tc4 = dod1 * hit3 + dod2 * hit2 + dod3 * hit1 + dod4 * hit0 + dod5 * miss0 +
                dod5 * miss1 + dod4 * miss2 + dod3 * miss3 + dod2 * miss4;

            {
                constexpr long_int_t expected_tc4 = 1234117;

                static_assert(expected_tc4 == tc4);
            }

            test_cases.push_back({ "four", { hit0, hit1, hit2, hit3 }, { miss0, miss1, miss2, miss3, miss4 }, tc4 });
            // The best tree:
            //                                     1e6 * 1
            //                                    /           \;
            //                            1e5 * 2        11 * 2
            //                            /         \;
            //                   1e4 * 3        9 * 3
            //                /              \;
            //     1e3 * 4             7 * 4
            //      /        \;
            // 3 * 5      5 * 5
            //
            // TC = 1234e3 + 11 * 2 + 9 * 3 + 7 * 4 + 5 * 5 + 3 * 5 =
            // = 1234000 + 22 + 27 + 28 + 40 =
            // = 1234117
            //
            //
            // range_min_costs
            // 3       1016    12038   123071  1234117
            //        5       10024   120054  1230097
            //                7       100032  1200070
            //                        9       1000040
            //                                11
            //
            // sums
            // 3       1008    11015   111024  1111035
            //        5       10012   110021  1110032
            //                7       100016  1100027
            //                        9       1000020
            //                                11
            //
            // roots
            // 0       1       2       3
            //        1       2       3
            //                2       3
            //                        3
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        std::vector<node_t> nodes;

        const auto slow = Standard::Algorithms::Trees::optimal_bst_slow<int_t, long_int_t>(
            test.hit_probabilities(), inf, test.miss_probabilities(), nodes);

        ::Standard::Algorithms::ert::are_equal(test.expected_cost(), slow.first, "optimal_bst_slow");
        // todo(p3): check the nodes.

        const auto actual = Standard::Algorithms::Trees::optimal_bst<int_t, long_int_t>(
            test.hit_probabilities(), inf, test.miss_probabilities(), nodes);

        ::Standard::Algorithms::ert::are_equal(test.expected_cost(), actual.first, "optimal_bst");
        // todo(p3): check the nodes.
    }
} // namespace

void Standard::Algorithms::Trees::Tests::optimal_bst_tests()
{
    ::Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
