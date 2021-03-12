#include <vector>
#include "../test_utilities.h"
#include "DropFloor.h"
#include "DropFloorTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                class DropFloorTest final
                {
                    DropFloorTest() = delete;

                public:

                    static void Test();

                private:

                    using Number = long long int;

                    static constexpr Number MaxFloor{ 100000 };

                    struct TestCase final : base_test_case
                    {
                        Number Balls, Floors, Expected;

                        TestCase(string&& name,
                            const Number balls,
                            const Number floors,
                            const Number expected)
                            : base_test_case(forward<string>(name)),
                            Balls(balls), Floors(floors), Expected(expected)
                        {
                        }

                        TestCase(const Number balls,
                            const Number floors,
                            const Number expected)
                            : TestCase("", balls, floors, expected)
                        {
                        }

                        void Print(ostream& str) const override
                        {
                            base_test_case::Print(str);

                            str << ", Balls=" << Balls
                                << ", Floors=" << Floors
                                << ", Expected=" << Expected;
                        }

                        void fix_name();
                    };

                    static void RunTestCase(const TestCase& testCase);

                    static void GenerateTestCases(vector<TestCase>& testCases);
                };

                void DropFloorTest::GenerateTestCases(vector<TestCase>& testCases)
                {
                    testCases.insert(testCases.end(), {
                      { 2, 2, 2 },
                      { 2, 3, 2 },
                      { 2, 4, 3 },//edge
                      { 2, 5, 3 },
                      { 2, 6, 3 },
                      { 2, 7, 4 },//edge
                      { 2, 8, 4 },
                      { 2, 10, 4 },
                      { 2, 11, 5 },//edge
                      { 2, 12, 5 },
                      { 2, 15, 5 },
                      { 2, 16, 6 },//edge
                      { 2, 17, 6 },

                      { 2, 35, 8 },
                      { 2, 36, 8 },
                      { 2, 37, 9 },//edge
                      { 2, 38, 9 },

                      { 2, 45, 9 },
                      { 2, 46, 10 },//edge

                      { 2, 90, 13 },
                      { 2, 91, 13 },
                      { 2, 92, 14 },//edge
                      { 2, 100, 14 },

                      { 2, 703, 37 },
                      { 2, 704, 38 },//edge
                      { 2, 10011, 141 },
                      { 2, 10012, 142 },//edge

                      { 3, 2, 2 },
                      { 3, 4, 3 },
                      { 3, 7, 3 },
                      { 3, 8, 4 },//edge
                      { 3, 9, 4 },
                      { 3, 14, 4 },
                      { 3, 15, 5 },//edge
                      { 3, 16, 5 },
                      { 3, 25, 5 },
                      { 3, 26, 6 },//edge
                      { 3, 27, 6 },
                      { 3, 41, 6 },
                      { 3, 42, 7 },//edge
                      { 3, 63, 7 },
                      { 3, 64, 8 },//edge
                      { 3, 65, 8 },
                      { 3, 92, 8 },
                      { 3, 93, 9 },//edge

                      { 3, 100, 9 },
                      { 3, 1000, 19 },

                      { 4, 2, 2 },
                      { 4, 3, 2 },
                      { 4, 5, 3 },
                      { 4, 6, 3 },
                      { 4, 7, 3 },
                      { 4, 8, 4 },//edge
                      { 4, 9, 4 },
                      { 4, 15, 4 },
                      { 4, 16, 5 },//edge
                      { 4, 17, 5 },
                      { 4, 10000, 23 },

                      { 5, 5000, 16 },
                      { 7, 10000, 15 },

                      { 9, 10, 4 },
                      { 10, 5000, 13 },
                      { 10, 10000, 14 },

                      { 22, 8, 4 },
                      { 22, 9, 4 },
                      { 22, 10, 4 },

                      { 32, 3, 2 },

                      { 50, 5000, 13 },

                      { 100, 4, 3 },
                      { 100, 5, 3 },
                      { 102, 6, 3 },
                      { 102, 7, 3 },
                      { 100, 5000, 13 },

                      { 1000, 1000, 10 },
                      { 1000, 5000, 13 },

                      { 5000, 5000, 13 },
                        });

                    for (Number i = 1; i <
#ifdef _DEBUG
                        7
#else
                        MaxFloor / 5
#endif
                        ; ++i)
                    {
                        {
                            const Number addToAvoidRepetition = 1;
                                const Number sameValue = i + addToAvoidRepetition;
                                testCases.push_back({ 1, sameValue, sameValue });
                            testCases.push_back({ sameValue, 1, 1 });
                        }
                        {
                            const auto expected = static_cast<Number>(
                                1 + MostSignificantBit(static_cast<unsigned long>(i)));

                            testCases.push_back({ to_string(i), i, i, expected });
                        }
                    }

                    //Must be the last line.
                    fix_names(testCases);
                }

                void DropFloorTest::TestCase::fix_name()
                {
                    if (get_Name().empty())
                    {
                        set_Name(to_string(Balls) + "_" + to_string(Floors));
                    }
                }

                void DropFloorTest::RunTestCase(
                    const TestCase& testCase)
                {
                    const auto actual = drop_floor<Number>(
                        testCase.Balls, testCase.Floors, MaxFloor);
                    Assert::AreEqual(testCase.Expected, actual, "Expected");
                }

                void DropFloorTest::Test()
                {
                    test_utilities<TestCase>::run_tests<0>(RunTestCase, GenerateTestCases);
                }

                void DropFloorTests()
                {
                    DropFloorTest::Test();
                }
            }
        }
    }
}