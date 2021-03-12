#include "WeightedHousesOnRoad.h"
#include "WeightedHousesOnRoadTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

using Distance = int;
using Weight = unsigned int;
using House = DistanceAndWeight<Distance, Weight>;

//TODO: p1. Finish it.
struct TestCase_WeightedHouse final
{
    vector< House > Houses;
    Distance Radius{};
    size_t MaximumStationCount{};
    //Weight MaximumAllowedTotalWeight;

    Weight ExpectedWeight{};
    vector< Distance > ExpectedPlacements;
};

void WeightedHousesOnRoadTests::Test()
{
    TestCase_WeightedHouse test_AllWithinDiameter;
    {
        test_AllWithinDiameter.Radius = 40;

        const Distance firstDistance = 10;
        const House houses[] = {
          {firstDistance, 78125},
          {firstDistance + (test_AllWithinDiameter.Radius << 1), 3125},
          {firstDistance + test_AllWithinDiameter.Radius, 625},
          {firstDistance + ((3 * test_AllWithinDiameter.Radius) >> 1), 25},
          {firstDistance + 2, 5},
        };
        const size_t housesCount = (sizeof houses) / sizeof(House);

        test_AllWithinDiameter.Houses.insert(test_AllWithinDiameter.Houses.begin(),
            houses, houses + housesCount);

        test_AllWithinDiameter.MaximumStationCount = 3;

        test_AllWithinDiameter.ExpectedWeight = 0;
        for (size_t j = 0; j < housesCount; ++j)
        {
            test_AllWithinDiameter.ExpectedWeight += houses[j].Weight;
        }

        //test_AllWithinDiameter.MaximumAllowedTotalWeight = test_AllWithinDiameter.ExpectedWeight;

        test_AllWithinDiameter.ExpectedPlacements.push_back(firstDistance
            + test_AllWithinDiameter.Radius);
    }

    TestCase_WeightedHouse test_simple1;
    {
        test_simple1.Radius = 10;
        const House houses[] = {
          {9, 197},
        };
        const size_t housesCount = (sizeof houses) / sizeof(House);

        test_simple1.Houses.insert(test_simple1.Houses.begin(),
            houses, houses + housesCount);

        test_simple1.MaximumStationCount = 500;
        test_simple1.ExpectedWeight = 197;
        //test_simple1.MaximumAllowedTotalWeight = test_simple1.ExpectedWeight;

        test_simple1.ExpectedPlacements.push_back(9);
    }

    TestCase_WeightedHouse test_simple2;
    {
        test_simple2.Radius = 10;
        const House houses[] = {
          {9, 197},
          {100, 1},
        };
        const size_t housesCount = (sizeof houses) / sizeof(House);

        test_simple2.Houses.insert(test_simple2.Houses.begin(),
            houses, houses + housesCount);

        test_simple2.MaximumStationCount = 10;
        test_simple2.ExpectedWeight = 198;
        //test_simple2.MaximumAllowedTotalWeight = test_simple2.ExpectedWeight;

        test_simple2.ExpectedPlacements.push_back(19);
        test_simple2.ExpectedPlacements.push_back(90);
    }

    TestCase_WeightedHouse test_simple3;
    {
        test_simple3.Radius = 10;
        const House house0 = { 50, 197 };
        const House house1 = { house0.Distance + (2 * test_simple3.Radius) + 1, 1000 };

        const House houses[] = { house0, house1, };
        const size_t housesCount = (sizeof houses) / sizeof(House);

        test_simple3.Houses.insert(test_simple3.Houses.begin(),
            houses, houses + housesCount);

        test_simple3.MaximumStationCount = 2;
        test_simple3.ExpectedWeight = house0.Weight + house1.Weight;
        //test_simple3.MaximumAllowedTotalWeight = test_simple3.ExpectedWeight;

        test_simple3.ExpectedPlacements.push_back(house0.Distance + test_simple3.Radius);
        test_simple3.ExpectedPlacements.push_back(house0.Distance + test_simple3.Radius * 3 + 1);
    }

    TestCase_WeightedHouse test_simple4 = test_simple3;
    {
        test_simple4.Houses[1].Distance += ((test_simple4.Radius << 1) + 1);

        test_simple4.ExpectedPlacements[1]++;
    }

    TestCase_WeightedHouse test_GeneralCase;
    {
        test_GeneralCase.Radius = 10;
        const Distance bestDistance1 = 55;
        const Distance lastDistance = 92;
        const Distance farAfterLastDistance = lastDistance + (test_GeneralCase.Radius << 2);

        const House houses[] = {
          {9, 197},
          {10, 1},
          {15, 1},//199.

          {40, 15},
          {45, 100},
          {50, 25},//140 + 36 = 176.
          {bestDistance1, 5},//130 + 70= 200 * *.
          {60, 30},
          {65, 40},

          {69, 40},

          {40, 1},

          {90, 150},//270.
          {90, 119},
          {lastDistance, 1},

          {farAfterLastDistance, 1},
        };
        const size_t housesCount = (sizeof houses) / sizeof(House);

        test_GeneralCase.Houses.insert(test_GeneralCase.Houses.begin(),
            houses, houses + housesCount);

        test_GeneralCase.MaximumStationCount = 2;
        test_GeneralCase.ExpectedWeight = 470;
        //test_GeneralCase.MaximumAllowedTotalWeight = test_GeneralCase.ExpectedWeight;

        test_GeneralCase.ExpectedPlacements.push_back(bestDistance1);
        test_GeneralCase.ExpectedPlacements.push_back(lastDistance - test_GeneralCase.Radius);
    }

    TestCase_WeightedHouse test_Feng;
    {
        test_Feng.Radius = 4;
        const House houses[] = {
          {0, 1},
          {4, 2},
          {8, 4},
          {12, 4},
          {16, 2},
          {20, 1},
        };
        const size_t housesCount = (sizeof houses) / sizeof(House);

        test_Feng.Houses.insert(test_Feng.Houses.begin(),
            houses, houses + housesCount);

        test_Feng.MaximumStationCount = 2;
        test_Feng.ExpectedWeight = 14;
        //test_Feng.MaximumAllowedTotalWeight = test_Feng.ExpectedWeight;

        test_Feng.ExpectedPlacements.push_back(4);
        test_Feng.ExpectedPlacements.push_back(16);
    }

    TestCase_WeightedHouse test_Feng2;
    {
        test_Feng2.Radius = 4;
        const House houses[] = {
          {0, 1},
          {4, 2},
          {8, 4},
          {12, 8},
          {16, 4},
          {20, 2},
          {24, 1},
        };
        const size_t housesCount = (sizeof houses) / sizeof(House);

        test_Feng2.Houses.insert(test_Feng2.Houses.begin(),
            houses, houses + housesCount);

        test_Feng2.MaximumStationCount = 2;
        test_Feng2.ExpectedWeight = 21;
        //test_Feng2.MaximumAllowedTotalWeight = test_Feng2.ExpectedWeight;

        test_Feng2.ExpectedPlacements.push_back(4);
        test_Feng2.ExpectedPlacements.push_back(16);
    }

    TestCase_WeightedHouse test_Feng3;
    {
        test_Feng3.Radius = 4;
        const House houses[] = {
          {0, 1},
          {4, 2},
          {7, 5},
          {8, 4},
          {12, 8},
          {16, 4},
          {20, 2},
          {24, 1},
        };
        const size_t housesCount = (sizeof houses) / sizeof(House);

        test_Feng3.Houses.insert(test_Feng3.Houses.begin(),
            houses, houses + housesCount);

        test_Feng3.MaximumStationCount = 2;
        test_Feng3.ExpectedWeight = 26;
        //test_Feng3.MaximumAllowedTotalWeight = test_Feng3.ExpectedWeight;

        test_Feng3.ExpectedPlacements.push_back(4);
        test_Feng3.ExpectedPlacements.push_back(16);
    }
#ifndef _DEBUG
    TestCase_WeightedHouse test_ManySmallHouses;
    {
        test_ManySmallHouses.Radius = 4;
        for (Distance i = 5; i < 100; ++i)
        {
            const House house = { i, 1 };
            test_ManySmallHouses.Houses.push_back(house);
        }

        for (Distance i = 6; i < 13; ++i)
        {
            const House house = { i, 10 };
            test_ManySmallHouses.Houses.push_back(house);
        }

        test_ManySmallHouses.MaximumStationCount = 2;
        test_ManySmallHouses.ExpectedWeight = 88;
        //test_ManySmallHouses.MaximumAllowedTotalWeight = test_ManySmallHouses.ExpectedWeight;

        test_ManySmallHouses.ExpectedPlacements.push_back(9);
        test_ManySmallHouses.ExpectedPlacements.push_back(18);
    }

    TestCase_WeightedHouse test_ManySmallHouses2 = test_ManySmallHouses;
    {
        {
            const House house = { 16, 100 };
            test_ManySmallHouses2.Houses.push_back(house);
        }

        {
            const House house = { 2021, 200 };
            test_ManySmallHouses2.Houses.push_back(house);
        }

        test_ManySmallHouses2.ExpectedWeight = 359;//50 + 100 + 9 + 200
        test_ManySmallHouses2.ExpectedPlacements[0] = 12;
        test_ManySmallHouses2.ExpectedPlacements[1] = 2017;
    }

    TestCase_WeightedHouse test_Rhombus;
    {//To f. the "greedy" approach.
        test_Rhombus.Radius = 4;

        //10..19.
        for (Distance i = 10; i < 20; ++i)
        {
            const House house = { i, 10 };
            test_Rhombus.Houses.push_back(house);
        }

        //5..24
        for (Distance i = 5; i < 25; ++i)
        {
            const House house = { i, 1 };
            test_Rhombus.Houses.push_back(house);
        }

        test_Rhombus.MaximumStationCount = 2;
        test_Rhombus.ExpectedWeight = 118;
        //test_Rhombus.MaximumAllowedTotalWeight = test_Rhombus.ExpectedWeight;

        test_Rhombus.ExpectedPlacements.push_back(9);
        test_Rhombus.ExpectedPlacements.push_back(18);
    }

    TestCase_WeightedHouse test_ManyStations;
    {
        test_ManyStations.Radius = 4;

        const House house0 = { 7, 1 };
        test_ManyStations.Houses.push_back(house0);

        const House house1 = { test_ManyStations.Radius * 1024, 1000 };
        test_ManyStations.Houses.push_back(house1);

        test_ManyStations.MaximumStationCount = 50000;
        test_ManyStations.ExpectedWeight = house0.Weight + house1.Weight;
        //test_ManyStations.MaximumAllowedTotalWeight = test_ManyStations.ExpectedWeight;

        test_ManyStations.ExpectedPlacements.push_back(house0.Distance + test_ManyStations.Radius);
        test_ManyStations.ExpectedPlacements.push_back(house1.Distance - test_ManyStations.Radius);
    }
#endif
    const TestCase_WeightedHouse tests[] = {
      test_simple4,
      test_simple1,
      test_simple2,
      test_simple3,
      test_GeneralCase,
      test_Feng,
      test_Feng2,
      test_Feng3,
      test_AllWithinDiameter,
  #ifndef _DEBUG
      test_ManySmallHouses,
      test_ManySmallHouses2,
      test_Rhombus,
      test_ManyStations,
  #endif
    };

    const size_t testsCount = (sizeof tests) / sizeof(TestCase_WeightedHouse);
    vector< Distance > chosenPlacements;
    for (size_t i = 0; i < testsCount; ++i)
    {
        const TestCase_WeightedHouse& test = tests[i];

        const Weight weight = WeightedHousesOnRoad<Distance, Weight>::Solve(
            test.Houses,
            test.Radius,
            test.MaximumStationCount,
            //test.MaximumAllowedTotalWeight,
            chosenPlacements);

        Assert::AreEqual(test.ExpectedWeight, weight, "Weight");

        Assert::AreEqual(test.ExpectedPlacements, chosenPlacements, "chosenPlacements");
    }
}