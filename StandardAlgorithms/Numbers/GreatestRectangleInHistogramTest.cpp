#include "GreatestRectangleInHistogram.h"
#include "GreatestRectangleInHistogramTest.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

void Privet::Algorithms::Numbers::Tests::GreatestRectangleInHistogramTest::Test()
{
    using Number = int;
    using NumberSquared = long long int;
    using TAlgorithm = GreatestRectangleInHistogram< Number, NumberSquared >;

    const bool shallComputeStartIndex = true;

    {
        const Number bigHeight = 9;

        const Number heigths[] = { 5, 4, bigHeight, bigHeight + 1, 2, 7, 7 };
        const size_t heigthsSize = sizeof(heigths) / sizeof(Number);

        const size_t expectedStartIndex = 2;
        const Number expectedStartHeigth = bigHeight;
        const size_t expectedWidth = 2;

        const NumberSquared expectedArea = NumberSquared(expectedStartHeigth)
            * NumberSquared(expectedWidth);

        const vector< Number > heigthsVector(heigths, heigths + heigthsSize);

        {
            size_t startIndex = 0;
            Number startHeigth = 0;
            size_t startWidth = 0;

            const NumberSquared actualArea = TAlgorithm::Compute(
                heigthsVector, startIndex, startHeigth, startWidth, shallComputeStartIndex);

            Assert::AreEqual(expectedArea, actualArea, "area_Simple");
            Assert::AreEqual(expectedStartIndex, startIndex, "startIndex_Simple");
            Assert::AreEqual(expectedStartHeigth, startHeigth, "startHeigth_Simple");
            Assert::AreEqual(expectedWidth, startWidth, "expectedWidth_Simple");
        }
        {
            size_t startIndex = 0;
            Number startHeigth = 0;
            size_t startWidth = 0;

            const NumberSquared actualArea = TAlgorithm::ComputeFast(
                heigthsVector, startIndex, startHeigth, startWidth, shallComputeStartIndex);

            Assert::AreEqual(expectedArea, actualArea, "area_Fast");
            Assert::AreEqual(expectedStartIndex, startIndex, "startIndex_Fast");
            Assert::AreEqual(expectedStartHeigth, startHeigth, "startHeigth_Fast");
            Assert::AreEqual(expectedWidth, startWidth, "expectedWidth_Fast");
        }
    }

    {
        vector< Number > heigthsVector;
        const Number size = 10;
        for (Number i = 0; i < size - 1; ++i)
        {
            heigthsVector.push_back(size - i);
        }

        heigthsVector.push_back(size);

        const size_t expectedStartIndex = 0;
        const Number expectedStartHeigth = 6;
        const size_t expectedWidth = 5;

        const NumberSquared expectedArea = NumberSquared(expectedStartHeigth)
            * NumberSquared(expectedWidth);

        {
            size_t startIndex = 0;
            Number startHeigth = 0;
            size_t startWidth = 0;

            const NumberSquared actualAreaSimple = TAlgorithm::Compute(
                heigthsVector, startIndex, startHeigth, startWidth, shallComputeStartIndex);

            Assert::AreEqual(expectedArea, actualAreaSimple, "area_Simple2");
            Assert::AreEqual(expectedStartIndex, startIndex, "startIndex_Simple2");
            Assert::AreEqual(expectedStartHeigth, startHeigth, "startHeigth_Simple2");
            Assert::AreEqual(expectedWidth, startWidth, "expectedWidth_Simple2");
        }
        {
            size_t startIndex = 0;
            Number startHeigth = 0;
            size_t startWidth = 0;

            const NumberSquared actualAreaFast = TAlgorithm::ComputeFast(
                heigthsVector, startIndex, startHeigth, startWidth, shallComputeStartIndex);

            Assert::AreEqual(expectedArea, actualAreaFast, "area_Fast2");
            Assert::AreEqual(expectedStartIndex, startIndex, "startIndex_Fast2");
            Assert::AreEqual(expectedStartHeigth, startHeigth, "startHeigth_Fast2");
            Assert::AreEqual(expectedWidth, startWidth, "expectedWidth_Fast2");
        }
    }
}