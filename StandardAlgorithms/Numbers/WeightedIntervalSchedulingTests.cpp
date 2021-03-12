#include "WeightedIntervalScheduling.h"
#include "WeightedIntervalSchedulingTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

using Distance = unsigned int;
using Weight = size_t;
using TWeightedSegment = WeightedInterval< Distance, Weight >;

struct TestCase_WeightedIntervalScheduling final
{
    vector< TWeightedSegment > Segments;

    Weight ExpectedWeight{};
    vector< size_t > ExpectedSegmentIndexes;
};

void WeightedIntervalSchedulingTests::Test()
{
    TestCase_WeightedIntervalScheduling testCase_ClassExample;
    {
        TWeightedSegment segments0[] = {
          {9, 11, 1},

          {1, 6, 2},
          {3, 7, 4},
          {5, 8, 4},

          {4, 9, 7},//4
          {8, 10, 2},
          {1, 12, 1},
        };
        const size_t expectedSegmentIndexes0[] = { 0, 4, };

        testCase_ClassExample.Segments = vector< TWeightedSegment >(
            segments0, segments0 + (sizeof segments0) / sizeof(TWeightedSegment));

        testCase_ClassExample.ExpectedWeight = 8;

        testCase_ClassExample.ExpectedSegmentIndexes = vector< size_t >(
            expectedSegmentIndexes0, expectedSegmentIndexes0
            + (sizeof expectedSegmentIndexes0) / sizeof(size_t));
    }

    TestCase_WeightedIntervalScheduling testCase_One;
    {
        testCase_One.ExpectedWeight = 1025;

        const TWeightedSegment segment = { 1, 4, testCase_One.ExpectedWeight };
        testCase_One.Segments.push_back(segment);

        testCase_One.ExpectedSegmentIndexes.push_back(0);
    }

    TestCase_WeightedIntervalScheduling testCase_TwoElementsOverlap;
    {
        const TWeightedSegment segment0 = { 3, 5, 123 };
        const TWeightedSegment segment1 = { 1, 4, 45 };

        testCase_TwoElementsOverlap.Segments.push_back(segment1);
        testCase_TwoElementsOverlap.Segments.push_back(segment0);

        testCase_TwoElementsOverlap.ExpectedWeight = segment0.Weight;
        testCase_TwoElementsOverlap.ExpectedSegmentIndexes.push_back(1);
    }

    TestCase_WeightedIntervalScheduling testCase_TwoElementsOverlap2
        = testCase_TwoElementsOverlap;
    {
        swap(testCase_TwoElementsOverlap2.Segments[0],
            testCase_TwoElementsOverlap2.Segments[1]);

        testCase_TwoElementsOverlap2.Segments[0].Weight--;
        testCase_TwoElementsOverlap2.Segments[1].Weight--;

        testCase_TwoElementsOverlap2.ExpectedWeight--;

        testCase_TwoElementsOverlap2.ExpectedSegmentIndexes[0] = 0;
    }

    TestCase_WeightedIntervalScheduling testCase_TwoElementsOverlap3
        = testCase_TwoElementsOverlap;
    {
        swap(testCase_TwoElementsOverlap3.Segments[0].Weight,
            testCase_TwoElementsOverlap3.Segments[1].Weight);

        testCase_TwoElementsOverlap3.ExpectedSegmentIndexes[0] = 0;
    }

    TestCase_WeightedIntervalScheduling testCase_TwoElementsClose;
    {
        const TWeightedSegment segment0 = { 31, 50, 874100 };
        const TWeightedSegment segment1 = { 10, 30, 100 };

        testCase_TwoElementsClose.Segments.push_back(segment1);
        testCase_TwoElementsClose.Segments.push_back(segment0);

        testCase_TwoElementsClose.ExpectedWeight = segment0.Weight + segment1.Weight;

        testCase_TwoElementsClose.ExpectedSegmentIndexes.push_back(0);
        testCase_TwoElementsClose.ExpectedSegmentIndexes.push_back(1);
    }

    TestCase_WeightedIntervalScheduling testCase_TwoElementsTogether;
    {
        const TWeightedSegment segment0 = { 30, 50, 458 };
        const TWeightedSegment segment1 = { 10, 30, 8721 };

        testCase_TwoElementsTogether.Segments.push_back(segment1);
        testCase_TwoElementsTogether.Segments.push_back(segment0);

        testCase_TwoElementsTogether.ExpectedWeight = segment0.Weight + segment1.Weight;

        testCase_TwoElementsTogether.ExpectedSegmentIndexes.push_back(0);
        testCase_TwoElementsTogether.ExpectedSegmentIndexes.push_back(1);
    }

    const TestCase_WeightedIntervalScheduling testCases[] = {
      testCase_ClassExample,
      testCase_One,
      testCase_TwoElementsOverlap,
      testCase_TwoElementsOverlap2,
      testCase_TwoElementsOverlap3,
      testCase_TwoElementsClose,
      testCase_TwoElementsTogether,
    };

    const size_t size = (sizeof testCases) / sizeof(TestCase_WeightedIntervalScheduling);

    vector< size_t > chosenSegmentIndexes;
    for (size_t i = 0; i < size; ++i)
    {
        const TestCase_WeightedIntervalScheduling& testCase = testCases[i];
        const vector< TWeightedSegment >& segments = testCase.Segments;

        const Weight actualWeight = WeightedIntervalScheduling<Distance, Weight>
            ::FindMinWeightSchedule(
                segments,
                chosenSegmentIndexes);

        Assert::AreEqual(testCase.ExpectedWeight, actualWeight, "Weight");

        Assert::AreEqual(testCase.ExpectedSegmentIndexes, chosenSegmentIndexes, "chosenSegmentIndexes");
    }
}