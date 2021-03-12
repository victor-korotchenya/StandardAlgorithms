#include <random>
#include "WeightedHousesOn2DTests.h"
#include "../Assert.h"

using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Geometry;
using namespace Privet::Algorithms;
using namespace std;

using Distance = int;
using Weight = unsigned int;
using TPoint = point2d < Distance >;
using House = PointAndWeight < TPoint, Weight >;

struct TestCase_WeightedHouse2D final
{
    vector< House > Houses;
    Distance Radius{};
    size_t MaximumStationCount{};
    //Weight MaximumAllowedTotalWeight;

    Weight ExpectedWeight{};
    vector< TPoint > ExpectedPlacements;
};

void WeightedHousesOn2DTests::TestRandom(
    const int maxX, const int maxY)
{
    TestCase_WeightedHouse2D test;
    {
        const Weight maxWeight = 0x7;

        random_device rd;
        for (int x = 0; x < maxX; ++x)
        {
            for (int y = 0; y < maxY; ++y)
            {
                House house;
                house.Point.X = x;
                house.Point.Y = y;
                unsigned int randomNumber = rd();
                house.Weight = (randomNumber & maxWeight) + 1;

                test.Houses.push_back(house);
            }
        }

        test.Radius = 1;
        test.MaximumStationCount = 3;
        test.ExpectedWeight = 1;
    }

    vector< TPoint > chosenPlacementsExpected;
    const Weight weightExpected = SolveByPermutation(
        test.Houses,
        test.Radius,
        test.MaximumStationCount,
        //test.MaximumAllowedTotalWeight,
        maxX,
        maxY,
        chosenPlacementsExpected);

    vector< TPoint > chosenPlacements;
    const Weight weight = WeightedHousesOn2D< Distance, Weight, TPoint, House >
        ::Solve(
            test.Houses,
            test.Radius,
            test.MaximumStationCount,
            //test.MaximumAllowedTotalWeight,
            chosenPlacements);

    Assert::AreEqual(weightExpected, weight, "Weight in Random test.");
}

unsigned int WeightedHousesOn2DTests::SolveByPermutation(
    const vector< PointAndWeight< point2d < int >, unsigned int > >& houses,
    const int radius,
    const size_t maximumStationCount,
    const int maxX,
    const int maxY,
    vector< point2d < int > >& chosenPlacements)
{
    chosenPlacements.clear();

    if (0 == maximumStationCount)
    {
        throw exception("maximumStationCount must be > 0.");
    }

    if (radius <= 0)
    {
        throw exception("radius must be > 0.");
    }

    if (maxX <= 0)
    {
        throw exception("maxX must be > 0.");
    }

    if (maxY <= 0)
    {
        throw exception("maxY must be > 0.");
    }

    unsigned int result = 0;

    //vector <size_t> chosenIndexes;
    //chosenIndexes.resize(maximumStationCount, 0);

    const size_t product = (maxX + 1) * (maxY + 1);

    for (int i0 = 0; i0 < product - 2; ++i0)
    {
        const int x0 = i0 / maxY;
        const int y0 = i0 % maxY;

        for (int i1 = 1 + i0; i1 < product - 1; ++i1)
        {
            const int x1 = i1 / maxY;
            const int y1 = i1 % maxY;

            for (int i2 = 1 + i1; i2 < product; ++i2)
            {
                const int x2 = i2 / maxY;
                const int y2 = i2 % maxY;

                Weight w = 0;

                //TODO: p2. Iterate through houses once only.
                auto it = houses.cbegin();
                const auto end = houses.cend();
                do
                {
                    const PointAndWeight< point2d < int >, unsigned int >& pw = *it;

                    bool isWithin;

                    const bool isWithinX = x0 <= (pw.Point.X + radius) && pw.Point.X <= (x0 + radius);
                    const bool isWithinY = y0 <= (pw.Point.Y + radius) && pw.Point.Y <= (y0 + radius);
                    isWithin = isWithinX && isWithinY;

                    if (!isWithin)
                    {
                        const bool isWithinX2 = x1 <= (pw.Point.X + radius) && pw.Point.X <= (x1 + radius);
                        const bool isWithinY2 = y1 <= (pw.Point.Y + radius) && pw.Point.Y <= (y1 + radius);
                        isWithin = isWithinX2 && isWithinY2;
                    }

                    if (!isWithin)
                    {
                        const bool isWithinX2 = x2 <= (pw.Point.X + radius) && pw.Point.X <= (x2 + radius);
                        const bool isWithinY2 = y2 <= (pw.Point.Y + radius) && pw.Point.Y <= (y2 + radius);
                        isWithin = isWithinX2 && isWithinY2;
                    }

                    if (isWithin)
                    {
                        w += pw.Weight;
                    }
                } while (end != (++it));

                if (w > result)
                {
                    result = w;

                    chosenPlacements.clear();

                    point2d < int > p0;
                    point2d < int > p1;
                    point2d < int > p2;

                    p0.X = x0; p0.Y = y0;
                    p1.X = x1; p1.Y = y1;
                    p2.X = x2; p2.Y = y2;

                    chosenPlacements.push_back(p0);
                    chosenPlacements.push_back(p1);
                    chosenPlacements.push_back(p2);
                }
            }
        }
    }

    return result;
}

void WeightedHousesOn2DTests::Test4()
{
    const int maxX = 4;
    const int maxY = 4;

    TestCase_WeightedHouse2D test;
    {
        {
            const House house = { TPoint(0, 0), 2 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(0, 1), 6 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(0, 2), 8 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(0, 3), 8 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(1, 0), 2 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(1, 1), 2 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(1, 2), 7 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(1, 3), 4 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(2, 0), 1 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(2, 1), 7 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(2, 2), 4 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(2, 3), 6 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(3, 0), 8 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(3, 1), 4 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(3, 2), 6 }; test.Houses.push_back(house);
        }
        {
            const House house = { TPoint(3, 3), 8 }; test.Houses.push_back(house);
        }

        test.Radius = 1;
        test.MaximumStationCount = 3;
        test.ExpectedWeight = 81;
    }

    vector< TPoint > chosenPlacementsExpected;
    const Weight weightExpected = SolveByPermutation(
        test.Houses,
        test.Radius,
        test.MaximumStationCount,
        //test.MaximumAllowedTotalWeight,
        maxX,
        maxY,
        chosenPlacementsExpected);
    Assert::AreEqual(Weight(81), weightExpected, "weightExpected in test4.");

    vector< TPoint > chosenPlacements;
    const Weight weight = WeightedHousesOn2D< Distance, Weight, TPoint, House >
        ::Solve(
            test.Houses,
            test.Radius,
            test.MaximumStationCount,
            //test.MaximumAllowedTotalWeight,
            chosenPlacements);

    Assert::AreEqual(weightExpected, weight, "Weight in test4.");
}

void WeightedHousesOn2DTests::Test()
{
    /*
    TestCase_WeightedHouse2D test_AllWithinDiameter;
    {
      test_AllWithinDiameter.Radius = 5;

      const Distance x0 = 10 * 0;
      const Distance y0 = 5000 * 0;
      const House houses[] = {
        {x0, y0, 78125},
        {x0 + (test_AllWithinDiameter.Radius >> 1), y0 + 1, 3125},
        {x0 + test_AllWithinDiameter.Radius, y0 - 1, 625},
        {x0 - test_AllWithinDiameter.Radius, y0 - 2, 25},
        {x0 + 2, y0, 5},
      };
      const size_t housesCount = (sizeof houses)/sizeof(House);

      test_AllWithinDiameter.Houses.insert(test_AllWithinDiameter.Houses.begin(),
        houses, houses + housesCount);

      test_AllWithinDiameter.MaximumStationCount = 3;

      test_AllWithinDiameter.ExpectedWeight = 0;
      for (size_t j = 0; j < housesCount; ++j)
      {
        test_AllWithinDiameter.ExpectedWeight += houses[j].Weight;
      }

      //test_AllWithinDiameter.MaximumAllowedTotalWeight = test_AllWithinDiameter.ExpectedWeight;

      const TPoint p = {x0, y0 - 1};
      test_AllWithinDiameter.ExpectedPlacements.push_back(p);
    }

    TestCase_WeightedHouse2D test_2Close;
    {// 3 centers at: 10 [1,4], 13 [2,1], 28 [4,4].
      test_2Close.Radius = 1;

      const Distance x0 = 10 * 0;
      const Distance y0 = 5000 * 0;

      const Weight w0 = 1;

      const House houses[] = {
        {x0, y0 + 3, w0},
        {x0, y0 + 4, w0},
        {x0, y0 + 5, w0},

        {x0 + 1, y0 + 0, w0},
        {x0 + 1, y0 + 1, w0},
        {x0 + 1, y0 + 2, w0},
        {x0 + 1, y0 + 3, w0},
        {x0 + 1, y0 + 4, w0},
        {x0 + 1, y0 + 5, w0},

        {x0 + 2, y0 + 0, w0},
        {x0 + 2, y0 + 1, w0},
        {x0 + 2, y0 + 2, w0},
        {x0 + 2, y0 + 3, w0},
        {x0 + 2, y0 + 4, w0},
        {x0 + 2, y0 + 5, w0},

        {x0 + 3, y0 + 0, w0},
        {x0 + 3, y0 + 1, w0},
        {x0 + 3, y0 + 2, w0},
        {x0 + 3, y0 + 3, w0},
        {x0 + 3, y0 + 4, w0},
        {x0 + 3, y0 + 5, w0},

        {x0 + 4, y0 + 3, w0},
        {x0 + 4, y0 + 4, w0},
        {x0 + 4, y0 + 5, w0},

        {x0 + 5, y0 + 3, w0},
        {x0 + 5, y0 + 4, w0},
        {x0 + 5, y0 + 5, w0},
      };
      const size_t housesCount = (sizeof houses)/sizeof(House);

      test_2Close.Houses.insert(test_2Close.Houses.begin(),
        houses, houses + housesCount);

      test_2Close.MaximumStationCount = 3;

      test_2Close.ExpectedWeight = 3 * 9 * w0;

      //test_2Close.MaximumAllowedTotalWeight = test_2Close.ExpectedWeight;

      {
        const TPoint p = {x0 + 1, y0 + 4};
        test_2Close.ExpectedPlacements.push_back(p);
      }

      {
        const TPoint p = {x0 + 2, y0 + 1};
        test_2Close.ExpectedPlacements.push_back(p);
      }

      {
        const TPoint p = {x0 + 4, y0 + 4};
        test_2Close.ExpectedPlacements.push_back(p);
      }
    }

    TestCase_WeightedHouse2D test_3Close;
    {// 3 centers at linear indexes: 10 [1,4], 13 [2,1], 28 [4,4].
      test_3Close.Radius = 1;

      const Distance x0 = 10 * 0;
      const Distance y0 = 5000 * 0;

      const Weight w0 = 20;
      const Weight w1 = 3;

      const House houses[] = {
        {x0, y0 + 3, w0},
        {x0, y0 + 4, w0},
        {x0, y0 + 5, w0},

        {x0 + 1, y0 + 0, w0},
        {x0 + 1, y0 + 1, w0},
        {x0 + 1, y0 + 2, w0},
        {x0 + 1, y0 + 3, w0},
        {x0 + 1, y0 + 4, w0},
        {x0 + 1, y0 + 5, w0},

        {x0 + 2, y0 + 0, w0},
        {x0 + 2, y0 + 1, w0},
        {x0 + 2, y0 + 2, w0},
        {x0 + 2, y0 + 3, w0},
        {x0 + 2, y0 + 4, w0},
        {x0 + 2, y0 + 5, w0},

        {x0 + 3, y0 + 0, w0},
        {x0 + 3, y0 + 1, w0},
        {x0 + 3, y0 + 2, w0},
        {x0 + 3, y0 + 3, w0},
        {x0 + 3, y0 + 4, w0},
        {x0 + 3, y0 + 5, w0},

        {x0 + 4, y0 + 3, w0},
        {x0 + 4, y0 + 4, w0},
        {x0 + 4, y0 + 5, w0},

        {x0 + 5, y0 + 3, w0},
        {x0 + 5, y0 + 4, w0},
        {x0 + 5, y0 + 5, w0},

        //These do not matter.
        {x0, y0, w1},
        {x0 + 5, y0 + 1, w1},
      };
      const size_t housesCount = (sizeof houses)/sizeof(House);

      test_3Close.Houses.insert(test_3Close.Houses.begin(),
        houses, houses + housesCount);

      test_3Close.MaximumStationCount = 3;

      test_3Close.ExpectedWeight = 3 * 9 * w0;

      //test_3Close.MaximumAllowedTotalWeight = test_3Close.ExpectedWeight;

      {
        const TPoint p = {x0 + 1, y0 + 4};
        test_3Close.ExpectedPlacements.push_back(p);
      }

      {
        const TPoint p = {x0 + 2, y0 + 1};
        test_3Close.ExpectedPlacements.push_back(p);
      }

      {
        const TPoint p = {x0 + 4, y0 + 4};
        test_3Close.ExpectedPlacements.push_back(p);
      }
    }
    */
    /*
    TestCase_WeightedHouse2D test_simple1;
    {
      test_simple1.Radius = 10;
      const House houses[] = {
        {9, 197},
      };
      const size_t housesCount = (sizeof houses)/sizeof(House);

      test_simple1.Houses.insert(test_simple1.Houses.begin(),
        houses, houses + housesCount);

      test_simple1.MaximumStationCount = 500;
      test_simple1.ExpectedWeight = 197;
      //test_simple1.MaximumAllowedTotalWeight = test_simple1.ExpectedWeight;

      test_simple1.ExpectedPlacements.push_back(9);
    }

    TestCase_WeightedHouse2D test_simple2;
    {
      test_simple2.Radius = 10;
      const House houses[] = {
        {9, 197},
        {100, 1},
      };
      const size_t housesCount = (sizeof houses)/sizeof(House);

      test_simple2.Houses.insert(test_simple2.Houses.begin(),
        houses, houses + housesCount);

      test_simple2.MaximumStationCount = 10;
      test_simple2.ExpectedWeight = 198;
      //test_simple2.MaximumAllowedTotalWeight = test_simple2.ExpectedWeight;

      test_simple2.ExpectedPlacements.push_back(19);
      test_simple2.ExpectedPlacements.push_back(90);
    }

    TestCase_WeightedHouse2D test_simple3;
    {
      test_simple3.Radius = 10;
      const House house0 = {50, 197};
      const House house1 = {house0.Distance + (2 * test_simple3.Radius) + 1, 1000};

      const House houses[] = {house0, house1, };
      const size_t housesCount = (sizeof houses)/sizeof(House);

      test_simple3.Houses.insert(test_simple3.Houses.begin(),
        houses, houses + housesCount);

      test_simple3.MaximumStationCount = 2;
      test_simple3.ExpectedWeight = house0.Weight + house1.Weight;
      //test_simple3.MaximumAllowedTotalWeight = test_simple3.ExpectedWeight;

      test_simple3.ExpectedPlacements.push_back(house0.Distance + test_simple3.Radius);
      test_simple3.ExpectedPlacements.push_back(house0.Distance + test_simple3.Radius *3 + 1);
    }

    TestCase_WeightedHouse2D test_simple4 = test_simple3;
    {
      test_simple4.Houses[1].Distance += ((test_simple4.Radius << 1) + 1);

      test_simple4.ExpectedPlacements[1]++;
    }

    TestCase_WeightedHouse2D test_GeneralCase;
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
      const size_t housesCount = (sizeof houses)/sizeof(House);

      test_GeneralCase.Houses.insert(test_GeneralCase.Houses.begin(),
        houses, houses + housesCount);

      test_GeneralCase.MaximumStationCount = 2;
      test_GeneralCase.ExpectedWeight = 470;
      //test_GeneralCase.MaximumAllowedTotalWeight = test_GeneralCase.ExpectedWeight;

      test_GeneralCase.ExpectedPlacements.push_back(bestDistance1);
      test_GeneralCase.ExpectedPlacements.push_back(lastDistance - test_GeneralCase.Radius);
    }

    TestCase_WeightedHouse2D test_Feng;
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
      const size_t housesCount = (sizeof houses)/sizeof(House);

      test_Feng.Houses.insert(test_Feng.Houses.begin(),
        houses, houses + housesCount);

      test_Feng.MaximumStationCount = 2;
      test_Feng.ExpectedWeight = 14;
      //test_Feng.MaximumAllowedTotalWeight = test_Feng.ExpectedWeight;

      test_Feng.ExpectedPlacements.push_back(4);
      test_Feng.ExpectedPlacements.push_back(16);
    }

    TestCase_WeightedHouse2D test_Feng2;
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
      const size_t housesCount = (sizeof houses)/sizeof(House);

      test_Feng2.Houses.insert(test_Feng2.Houses.begin(),
        houses, houses + housesCount);

      test_Feng2.MaximumStationCount = 2;
      test_Feng2.ExpectedWeight = 21;
      //test_Feng2.MaximumAllowedTotalWeight = test_Feng2.ExpectedWeight;

      test_Feng2.ExpectedPlacements.push_back(4);
      test_Feng2.ExpectedPlacements.push_back(16);
    }

    TestCase_WeightedHouse2D test_Feng3;
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
      const size_t housesCount = (sizeof houses)/sizeof(House);

      test_Feng3.Houses.insert(test_Feng3.Houses.begin(),
        houses, houses + housesCount);

      test_Feng3.MaximumStationCount = 2;
      test_Feng3.ExpectedWeight = 26;
      //test_Feng3.MaximumAllowedTotalWeight = test_Feng3.ExpectedWeight;

      test_Feng3.ExpectedPlacements.push_back(4);
      test_Feng3.ExpectedPlacements.push_back(16);
    }

    TestCase_WeightedHouse2D test_ManySmallHouses;
    {
      test_ManySmallHouses.Radius = 4;
      for (Distance i = 5; i < 1000; ++i)
      {
        const House house = {TPoint(i, 1};
        test_ManySmallHouses.Houses.push_back(house);
      }

      for (Distance i = 6; i < 13; ++i)
      {
        const House house = {TPoint(i, 10};
        test_ManySmallHouses.Houses.push_back(house);
      }

      test_ManySmallHouses.MaximumStationCount = 2;
      test_ManySmallHouses.ExpectedWeight = 88;
      //test_ManySmallHouses.MaximumAllowedTotalWeight = test_ManySmallHouses.ExpectedWeight;

      test_ManySmallHouses.ExpectedPlacements.push_back(9);
      test_ManySmallHouses.ExpectedPlacements.push_back(18);
    }

    TestCase_WeightedHouse2D test_ManySmallHouses2 = test_ManySmallHouses;
    {
      {
        const House house = {TPoint(16, 100};
        test_ManySmallHouses2.Houses.push_back(house);
      }

      {
        const House house = {TPoint(2021, 200};
        test_ManySmallHouses2.Houses.push_back(house);
      }

      test_ManySmallHouses2.ExpectedWeight = 359;//50 + 100 + 9 + 200
      test_ManySmallHouses2.ExpectedPlacements[0] = 12;
      test_ManySmallHouses2.ExpectedPlacements[1] = 2017;
    }

    TestCase_WeightedHouse2D test_Rhombus;
    {//To f. the "greedy" approach.
      test_Rhombus.Radius = 4;

      //10..19.
      for (Distance i = 10; i < 20; ++i)
      {
        const House house = {TPoint(i, 10};
        test_Rhombus.Houses.push_back(house);
      }

      //5..24
      for (Distance i = 5; i < 25; ++i)
      {
        const House house = {TPoint(i, 1};
        test_Rhombus.Houses.push_back(house);
      }

      test_Rhombus.MaximumStationCount = 2;
      test_Rhombus.ExpectedWeight = 118;
      //test_Rhombus.MaximumAllowedTotalWeight = test_Rhombus.ExpectedWeight;

      test_Rhombus.ExpectedPlacements.push_back(9);
      test_Rhombus.ExpectedPlacements.push_back(18);
    }

    TestCase_WeightedHouse2D test_ManyStations;
    {
      test_ManyStations.Radius = 4;

      const House house0 = {7, 1};
      test_ManyStations.Houses.push_back(house0);

      const House house1 = {test_ManyStations.Radius * 1024, 1000};
      test_ManyStations.Houses.push_back(house1);

      test_ManyStations.MaximumStationCount = 50000;
      test_ManyStations.ExpectedWeight = house0.Weight + house1.Weight;
      //test_ManyStations.MaximumAllowedTotalWeight = test_ManyStations.ExpectedWeight;

      test_ManyStations.ExpectedPlacements.push_back(house0.Distance + test_ManyStations.Radius);
      test_ManyStations.ExpectedPlacements.push_back(house1.Distance - test_ManyStations.Radius);
    }
  */
  /*
  const TestCase_WeightedHouse2D tests[] = {
    //test_simple4,
    //test_simple1,
    //test_simple2,
    //test_simple3,
    //test_GeneralCase,
    //test_Feng,
    //test_Feng2,
    //test_Feng3,
    test_2Close,
    //test_3Close,
    test_AllWithinDiameter,
    //  test_ManySmallHouses,
    //  test_ManySmallHouses2,
    //  test_Rhombus,
    //  test_ManyStations,
    };

  const size_t testsCount = (sizeof tests)/sizeof(TestCase_WeightedHouse2D);
  vector< TPoint > chosenPlacements;

   Test4();
  for (size_t i = 0; i < testsCount; ++i)
  {
    const TestCase_WeightedHouse2D &test = tests[i];

    const Weight weight = WeightedHousesOn2D< Distance, Weight, TPoint, House >
      ::Solve(
      test.Houses,
      test.Radius,
      test.MaximumStationCount,
      //test.MaximumAllowedTotalWeight,
      chosenPlacements);

    vector< TPoint > chosenPlacementsExpected;
    const int maxX = 10;
    const int maxY = 10;

    const Weight weightExpected = SolveByPermutation(
      test.Houses,
      test.Radius,
      test.MaximumStationCount,
      //test.MaximumAllowedTotalWeight,
      maxX,
      maxY,
      chosenPlacementsExpected);

    Assert::AreEqual(weightExpected, test.ExpectedWeight, "weightExpected Weight");
    Assert::AreEqual(test.ExpectedWeight, weight, "Weight");

    //TODO: do it. Assert::AreEqual(test.ExpectedPlacements, chosenPlacements, "chosenPlacements");
  }

  const int attempts = 10;
  for (int attempt =

#ifdef _DEBUG
    2;
#else
    0;
#endif

    attempt < attempts; ++attempt)
  {
    TestRandom(2 + attempt, 2 + attempt);
  }*/
}