#include <set>
#include "../Utilities/Random.h"
#include "min_stick_cuts_tests.h"
#include "min_stick_cuts.h"
#include "../test_utilities.h"
#include "../Utilities/VectorUtilities.h"
#include "../Utilities/PrintUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using int_t = short;

    struct test_case final : base_test_case
    {
        vector<int_t> cut_coordinates;
        int_t stick_length;

        pair<vector<int_t>, int_t> expected;

        test_case(string&& name,
            vector<int_t>&& cut_coordinates,
            int_t stick_length,
            vector<int_t>&& sequence_expected = {},
            int_t sum_expected = -1)
            : base_test_case(forward<string>(name)),
            cut_coordinates(forward<vector<int_t>>(cut_coordinates)),
            stick_length(stick_length),
            expected({ forward<vector<int_t>>(sequence_expected), sum_expected })
        {
            VectorUtilities::sort_remove_duplicates(this->cut_coordinates);
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(stick_length, "stick_length");

            const auto size = static_cast<int_t>(cut_coordinates.size());
            const auto& sequence_expected = expected.first;
            if (!size)
            {
                Assert::AreEqual(int_t(), expected.second, "sum_expected");
                Assert::AreEqual(size, sequence_expected.size(), "sequence_expected.size");
                return;
            }

            RequirePositive(size, "cut_coordinates.size");

            for (int_t i = 0; i < size; ++i)
            {
                const auto& c = cut_coordinates[i];
                RequirePositive(c, "cut_coordinates[i]");
                if (i)
                    Assert::Greater(c, cut_coordinates[i - 1], "cut_coordinates[i]");
            }

            RequireGreater(stick_length, cut_coordinates.back(), "stick_length vs cut_coordinates.back");

            if (expected.second < 0)
                return;

            Assert::GreaterOrEqual(expected.second, stick_length, "sum_expected");
            Assert::AreEqual(size, sequence_expected.size(), "sequence_expected.size");

            {
                auto cut_coord_cop = cut_coordinates;
                VectorUtilities::sort_remove_duplicates(cut_coord_cop);
                Assert::AreEqual(cut_coordinates, cut_coord_cop, "cut_coordinates dups removed");
            }

            auto seq_cop = sequence_expected;
            VectorUtilities::sort_remove_duplicates(seq_cop);
            Assert::AreEqual(cut_coordinates, seq_cop, "sequence_expected dups removed");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            ::Print("cut_coordinates", cut_coordinates, str);
            PrintValue(str, "stick_length", stick_length);

            ::Print("sequence_expected", expected.first, str);
            PrintValue(str, "sum_expected", expected.second);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        test_cases.push_back({ "base0", {}, 10, {}, 0 });
        test_cases.push_back({ "base1", vector<int_t>{ 4u }, 10,
            vector<int_t>{ 4u }, 10 });
        test_cases.push_back({ "base2", vector<int_t>{ 1u,4u }, 10,
            vector<int_t>{ 4u,1u }, 14 });

        ShortRandom r;
        set<int_t> uniq;
        vector<int_t> cut_coordinates;

        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            constexpr int_t size_max = 6, stick_length_max = size_max * 5;
            const auto n = r(1, size_max), stick_length = r(static_cast<int_t>(n + 2), stick_length_max);

            uniq.clear();
            for (int_t i = 0; i < n; ++i)
            {
                const auto c = r(1, static_cast<int_t>(stick_length - 1));
                uniq.insert(c);
            }

            cut_coordinates.resize(uniq.size());
            copy(uniq.begin(), uniq.end(), cut_coordinates.begin());
            // set sorts.

            test_cases.emplace_back("random" + to_string(att), move(cut_coordinates), stick_length);
        }
    }

    void verify_cuts(const pair<vector<int_t>, int_t>& expected,
        vector<int_t> cut_coordinates, const int_t stick_length, const string& name)
    {
        const auto& sequence = expected.first;
        const auto size = sequence.size();

        const auto& sum = expected.second;
        const auto name1 = name + " sequence";
        const auto name2 = name + " sum";

        RequirePositive(stick_length, name + " stick_length");
        if (cut_coordinates.empty())
        {
            Assert::AreEqual(0u, size, name1);
            Assert::AreEqual(int_t(), sum, name2);
            return;
        }

        RequirePositive(size, name1);
        RequirePositive(sum, name2);
        RequirePositive(cut_coordinates[0], "cut_coordinates[0]");
        RequireGreater(stick_length, cut_coordinates.back(), "stick_length vs cut_coordinates.back");

        // Make search easier
        cut_coordinates.reserve(cut_coordinates.size() + 2);
        cut_coordinates.insert(cut_coordinates.begin(), int_t());
        cut_coordinates.push_back(stick_length);

        using cut_t = pair<int_t, int_t>;
        set<cut_t> pieces{ { int_t(), stick_length} };

        int_t actual{};
        for (auto i = 0u; i < size; ++i)
        {
            RequirePositive(pieces.size(), "pieces at i " + to_string(i));

            const auto& cut = sequence[i];

            auto it = pieces.lower_bound({ cut, int_t() });
            if (it == pieces.begin())
                throw runtime_error("Cannot split a piece " + to_string(cut) + " at " + to_string(i));

            --it;
            const auto& piece = *it;
            if (!(piece.first < cut && cut < piece.second))
                throw runtime_error("Wrong split (" + to_string(piece.first) + ", " + to_string(piece.second) + ") of a piece " + to_string(cut) + " at " + to_string(i));

            const auto ad = piece.second - piece.first;

            actual = static_cast<int_t>(actual + ad);
            assert(ad > 0 && actual > 0);

            const array<cut_t, 2> new_cuts{ cut_t(piece.first, cut), cut_t(cut, piece.second) };

            pieces.erase(it);
            for (const auto& cut2 : new_cuts)
            {
                RequireGreater(cut2.second, cut2.first, "cut " + to_string(cut));

                const auto end = cut_coordinates.end();
                const auto fi = lower_bound(cut_coordinates.begin(), end, cut2.first);

                if (fi != end && *fi == cut2.first)
                {
                    const auto fi2 = next(fi);
                    if (fi2 != end && *fi2 == cut2.second)
                        continue;
                }

                const auto inserted = pieces.insert(cut2);
                Assert::AreEqual(true, inserted.second, "Intervals cannot be duplicated, cut " + to_string(cut));
            }
        }

        Assert::AreEqual(0u, pieces.size(), name + " leftover size");
        Assert::AreEqual(actual, sum, name2);
    }

    void run_test_case(const test_case& test_case)
    {
        const auto actual = min_stick_cuts_slow<int_t>(test_case.cut_coordinates, test_case.stick_length);
        if (test_case.expected.second >= 0)
        {// todo: p2. print << properly.
            Assert::AreEqual(test_case.expected.first, actual.first, "min_stick_cuts.first");
            Assert::AreEqual(test_case.expected.second, actual.second, "min_stick_cuts.second");
        }

        verify_cuts(actual, test_case.cut_coordinates, test_case.stick_length, "min_stick_cuts");

        const auto slow = min_stick_cuts_slow<int_t>(test_case.cut_coordinates, test_case.stick_length);
        Assert::AreEqual(actual.second, slow.second, "min_stick_cuts_slow");
        verify_cuts(slow, test_case.cut_coordinates, test_case.stick_length, "min_stick_cuts_slow");
    }
}

void Privet::Algorithms::Numbers::Tests::min_stick_cuts_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}