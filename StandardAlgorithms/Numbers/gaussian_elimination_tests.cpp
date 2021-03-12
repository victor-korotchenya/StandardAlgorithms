#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"
#include "gaussian_elimination.h"
#include "gaussian_elimination_tests.h"

namespace Privet::Algorithms::Numbers
{
    std::ostream& operator<<(std::ostream& s, const gaussian_elimination_result r)
    {
        switch (r)
        {
        case gaussian_elimination_result::none:
            s << "none";
            break;
        case gaussian_elimination_result::inf_many_solutions:
            s << "infinitely many solutions";
            break;
        case gaussian_elimination_result::one_solution:
            s << "one solution";
            break;
        case gaussian_elimination_result::singular:
            s << "singular";
            break;
        default:
            assert(0);
            break;
        }
        return s;
    }
}

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using float_t = double;
    using x_t = vector<float_t>;
    using sol_t = pair<gaussian_elimination_result, x_t>;
    using matrix_t = vector<vector<float_t>>;

    struct test_case final : base_test_case
    {
        matrix_t matrix;
        sol_t sol;

        test_case(string&& name,
            matrix_t&& matrix,
            sol_t&& sol = {})
            : base_test_case(forward<string>(name)),
            matrix(forward<matrix_t>(matrix)),
            sol(forward<sol_t>(sol))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(matrix.size(), "matrix size.");
            Assert::AreEqual(matrix.size() + 1u, matrix[0].size(), "matrix[0] size.");

            if (sol.first != gaussian_elimination_result::one_solution)
                return;

            Assert::AreEqual(matrix.size(), sol.second.size(), "sol size.");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("matrix", matrix, str);
            str << "sol.result" << sol.first;
            ::Print("sol.x", sol.second, str);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({
            "base2",
            matrix_t{{2, 4, -4},
                {3, 12, -15}},
            { gaussian_elimination_result::one_solution, {1, -3 / 2.0} } });

        test_cases.push_back({
            "singular2",
            matrix_t{{3, 7, 10},
                {3 / 7.0, 1, 5}},
            { gaussian_elimination_result::singular, {} } });

        test_cases.push_back({
            "base1",
            matrix_t{{5, 10}},
            { gaussian_elimination_result::one_solution, {2} } });

        test_cases.push_back({
            "base3",
            matrix_t{{3, 2, -4, 3},
                {2, 3, 3, 15},
                {5, -3, 1, 14}},
            { gaussian_elimination_result::one_solution, {3, 1, 2} } });

        matrix_t matrix;
        IntRandom r;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr auto n_max = 10, value_max = 1000;
            const auto n = r(1, n_max);

            matrix.clear();
            matrix.resize(n);

            for (auto i = 0; i < n; ++i)
            {
                auto& m = matrix[i];
                m.resize(n + 1llu);

                for (auto j = 0; j <= n; ++j)
                    m[j] = r(-value_max, value_max);
            }

            test_cases.push_back({ "random" + to_string(att), move(matrix) });
        }
    }

    void run_test_case(const test_case& test)
    {
        auto matr = test.matrix;
        const auto actual = gaussian_elimination<float_t>(matr, static_cast<int>(matr.size()));
        if (test.sol.first != gaussian_elimination_result::none)
        {
            Assert::AreEqual(test.sol.first, actual.first, "gaussian_elimination.sol");
            Assert::AreEqualWithEpsilon(test.sol.second, actual.second, string("gaussian_elimination x"));
        }
    }
}

void Privet::Algorithms::Numbers::Tests::gaussian_elimination_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}