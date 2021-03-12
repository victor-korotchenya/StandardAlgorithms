#include "stirling_numbers_modulo.h"
#include "stirling_numbers_modulo_tests.h"
#include "../test_utilities.h"
using namespace std;

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void stirling_1()
                {
                    const auto actual = stirling_numbers_1_permut_cycle_cnt_modulo<1000000007>(10);

                    const vector<vector<int>> expected = {
                        {1,0},
                        {0,1,0},//1
                        {0,1,1,0},
                        { 0,2,3,1,0},//3
                        { 0,6,11,6,1,0},
                        { 0,24,50,35,10,1,0},//5
                        { 0,120,274,225,85,15,1,0},
                        { 0,720,1764,1624,735,175,21,1,0},//7
                        { 0,5040,13068,13132,6769,1960,322,28,1,0},
                        { 0,40320,109584,118124,67284,22449,4536,546,36,1,0},//9
                        { 0,362880,1026576,1172700,723680,269325,63273,9450,870,45,1,0,
                        },
                    };
                    Assert::AreEqual(expected, actual, "stirling_numbers_1_permut_cycle_cnt_modulo");
                }

                void stirling_2()
                {
                    const auto actual = stirling_numbers_2_partition_modulo<999979>(10);

                    const vector<vector<int>> expected = {
                        { 1,0 },
                        { 0,1,0 },//1
                        { 0,1,1,0 },
                        { 0,1,3,1,0 },//3
                        { 0,1,7,6,1,0 },
                        { 0,1,15,25,10,1,0 },//5
                        { 0,1,31,90,65,15,1,0 },
                        { 0,1,63,301,350,140,21,1,0 },//7
                        { 0,1,127,966,1701,1050,266,28,1,0 },
                        { 0,1,255,3025,7770,6951,2646,462,36,1,0 },//9
                        { 0,1,511,9330,34105,42525,22827,5880,750,45,1,0 },
                    };
                    Assert::AreEqual(expected, actual, "stirling_numbers_2_partition_modulo");
                }

                void stirling_numbers_modulo_tests()
                {
                    stirling_1();
                    stirling_2();
                }
            }
        }
    }
}