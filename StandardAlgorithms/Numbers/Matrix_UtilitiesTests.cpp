#include "number_mod.h"
#include "matrix.h"
#include "Matrix_UtilitiesTests.h"
#include "../Assert.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

void  Privet::Algorithms::Numbers::Tests::Matrix_UtilitiesTests()
{
    using Number = unsigned int;
    using Long_Number = unsigned long long int;
    constexpr Number modulo = 1000 * 1000 * 1000 + 7;
    using n_t = number_mod<Number, modulo, Long_Number>;

    constexpr unsigned size = 4;
    using matrix_t = matrix<n_t, size, size>;

    const n_t data_src[size][size] = {
      { 159, 16, 4, 3 },
      { 1, 15, 153, 985625 },
      { 235, 15362, 657, 32645 },
      { 2467, 565, 2347, 956804 }
    };

    const n_t expected[size][size] = {
      { 284253588, 226920766, 817067932, 102261027 },
      { 440236870, 10749691, 182101956, 931651544 },
      { 668022195, 713435927, 647142552, 13969634 },
      { 266652219, 473563980, 753381942, 450274607 }
    };
    constexpr unsigned power = 1000 * 1000 * 1000 + 123;

    matrix_t actual;
    {
        matrix_t source(data_src);
        matrix_power<n_t, size>(source, power, actual);
    }

    string row_name, message;
    for (unsigned i = 0; i < size; i++)
    {
        row_name = "[" + to_string(i) + ", ";
        for (unsigned j = 0; j < size; j++)
        {
            message = row_name + to_string(j) + "]";

            const auto& value = actual[make_pair(i, j)];
            Assert::AreEqual(expected[i][j], value, message);
            Assert::Greater(modulo, value.val(), message);
            Assert::GreaterOrEqual(value.val(), 0u, message);
        }
    }
}