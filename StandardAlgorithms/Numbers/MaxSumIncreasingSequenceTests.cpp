#include <algorithm>
#include <tuple>
#include <vector>
#include "../Utilities/PrintUtilities.h"
#include "MaxSumIncreasingSequence.h"
#include "MaxSumIncreasingSequenceTests.h"
#include "../Assert.h"
#include "../Utilities/Random.h"
#include "../Utilities/RandomGenerator.h"
#include "../Utilities/VectorUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void MaxSumIncreasingSequenceTests()
                {
                    using Number = unsigned int;
                    using LongNumber = unsigned long long int;
                    using Size = unsigned;

                    using alg = MaxSumIncreasingSequence<Number, LongNumber, Size>;

                    //Input, sum, chosen indexes.
                    using TestCase = tuple<vector<Number>, LongNumber, vector<Size>>;

                    TestCase randomTestCase;
                    {
                        UnsignedIntRandom random1;
                        const auto size = random1(1, 10);

                        vector<Number> randomData;
                        const Number cutter = 1000 * 1000 * 1000 + 7;
                        FillRandom<Number>(randomData, size, cutter);

                        transform(randomData.begin(), randomData.end(), randomData.begin(),
                            [](Number n) {return n
                            //Make positive.
                            + 1; });

                        const auto actual = alg::compute_fast(randomData);
                        randomTestCase = TestCase(randomData, actual.first, actual.second);
                    }

                    const vector<TestCase> testCases{
                      TestCase({ 738037162, 149507639, 345537762, 367063963, 608683613,
                      261236996, 653617450, 617585322, 243701855, 621195149 },
                        2709573448, { 1, 2, 3, 4, 7, 9 }),
                      TestCase({ 15, 7, 2, 15, 8 }, 22,{ 1, 3 }),
                      TestCase({ 2, 4, 6, 4, 13, 3, 3, 16 }, 41,{ 0,1,2,4,7 }),
                      randomTestCase,
                      TestCase({ 3, 2, 5 }, 8,{ 0,2 }),
                      TestCase({ 17 }, 17,{ 0 }),
                      TestCase({ 1, 2 }, 3,{ 0, 1 }),
                      TestCase({ 1, 612, 70005 }, 70618,{ 0, 1, 2 }),
                      TestCase({ 1, 3, 10, 83 }, 97,{ 0, 1, 2, 3 })
                    };
                    Assert::NotEmpty(testCases, "testCases");

                    Size index = 0;
                    for (const auto& testCase : testCases)
                    {
                        const auto indexAsString = to_string(index);

                        Assert::NotEmpty(get<0>(testCase), "input_" + indexAsString);
                        Assert::NotEmpty(get<2>(testCase), "output_" + indexAsString);

                        try
                        {
                            VectorUtilities::RequireArrayIsSorted(
                                get<2>(testCase), indexAsString + "_sorted_chosen_indexes", true);

                            const auto last_index = get<2>(testCase).size() - 1;
                            const auto& last_chosen_index = get<2>(testCase)[last_index];
                            Assert::Greater(get<0>(testCase).size(), last_chosen_index,
                                indexAsString + "_last_chosen_index");

                            {
                                LongNumber expected_sum{};
                                vector<Number> chosen_values(get<2>(testCase).size());
                                Size ind{};
                                for (const auto& i : get<2>(testCase))
                                {
                                    chosen_values[ind] = get<0>(testCase)[i];
                                    expected_sum += get<0>(testCase)[i];

                                    ++ind;
                                }
                                Assert::AreEqual(get<1>(testCase), expected_sum, indexAsString + "_expected_sum");
                                VectorUtilities::RequireArrayIsSorted(
                                    chosen_values, indexAsString + "_sorted_chosen_values", true);
                            }

                            const auto actual = alg::compute_fast(get<0>(testCase));
                            Assert::AreEqual(get<1>(testCase), actual.first, indexAsString + "_sum");
                            Assert::AreEqual(get<2>(testCase), actual.second, indexAsString + "_chosen_indexes");

                            const auto actual_slow = alg::compute_slow(get<0>(testCase));

                            Assert::AreEqual(get<1>(testCase), actual_slow.first, indexAsString + "_sum_slow");
                            Assert::AreEqual(get<2>(testCase), actual_slow.second, indexAsString + "_chosen_indexes_slow");
                        }
                        catch (const std::exception& ex)
                        {
                            std::ostringstream ss;
                            ss << ex.what() << "\n:";

                            Privet::Algorithms::Print("Input", get<0>(testCase), ss);
                            Privet::Algorithms::Print("chosen_indexes_slow", get<2>(testCase), ss);

                            StreamUtilities::throw_exception(ss);
                        }

                        ++index;
                    }
                }
            }
        }
    }
}