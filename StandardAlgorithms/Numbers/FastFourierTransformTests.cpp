#include <vector>
#include "FastFourierTransform.h"
#include "FastFourierTransformOld.h"
#include "PolynomialMultiplier.h"
#include "NumberComparer.h"
#include "NumberUtilities.h"
#include "../Assert.h"
#include "FastFourierTransformTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

using Number = double;
using Complex = complex< Number >;

static NumberComparer< Number > Comparer;

inline static bool AreEqualNumbersByPtr(const Number* a, const Number* b)
{
    const bool result = Comparer.AreEqual(*a, *b);
    return result;
}

inline static bool AreEqualComplexNumbers(
    const Complex* const aPtr, const Complex* const bPtr)
{
    const Complex& a = *aPtr;
    const Complex& b = *bPtr;

    const bool result =
        Comparer.AreEqual(a.real(), b.real())
        && Comparer.AreEqual(a.imag(), b.imag());
    return result;
}

void FFTTest(const Number& pi)
{
    const Complex dataArray[] = { 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0 };
    const size_t size = sizeof(dataArray) / sizeof(Complex);
    valarray< Complex > data(dataArray, size);

    FastFourierTransformOld< Number >::Compute(pi, data);

    const Complex expectedArray[] = {
      Complex(4),
      Complex(1, -2.4142135623730949),
      Complex(0),
      Complex(1, -0.41421356237309492),
      Complex(0),
      Complex(1, 0.41421356237309492),
      Complex(0),
      Complex(1, 2.4142135623730949)
    };
    const size_t sizeExpected = sizeof(expectedArray) / sizeof(Complex);

    vector< Complex > actual;
    actual.assign(begin(data), end(data));

    Assert::AreEqual< Complex >(
        const_cast<Complex*>(expectedArray), sizeExpected,
        actual.data(), data.size(),
        "FFT",
        AreEqualComplexNumbers);

    FastFourierTransformOld< Number >::ComputeInverse(pi, data);

    actual.assign(begin(data), end(data));

    Assert::AreEqual< Complex >(
        const_cast<Complex*>(dataArray), size,
        actual.data(), data.size(),
        "InverseFFT",
        AreEqualComplexNumbers);
}

void PolyMultiplyTest(const Number& pi)
{
    //2 + 4*x + 0*x*x + 16*x*x*x + 32*x*x*x*x
    const unsigned int a1Array[] = { 2, 4, 0, 16, 32 };
    const size_t a1ArraySize = sizeof(a1Array) / sizeof(unsigned int);
    vector< unsigned int > a1(a1Array, a1Array + a1ArraySize);

    //3 + 9*x + 27*x*x
    const unsigned int a2Array[] = { 3, 9, 27 };
    const size_t a2ArraySize = sizeof(a2Array) / sizeof(unsigned int);
    vector< unsigned int > a2(a2Array, a2Array + a2ArraySize);

    // (2 + 4*x + 0*x*x + 16*x*x*x + 32*x*x*x*x) * (3 + 9*x + 27*x*x)
    // == 6 + 30*x + 90*x^x + 156*x^3 + 240 * x^4 + 720*x^5 + 864*x^6
    const Complex expectedArray[] = { 6, 30, 90, 156, 240, 720, 864, 0 };
    const size_t expectedArraySize = sizeof(expectedArray) / sizeof(Complex);
    vector< Complex > expected(expectedArray, expectedArray + expectedArraySize);

    vector< Complex > actual;

    PolynomialMultiplier< unsigned int, Number >::Multiply(pi, a1, a2, actual);

    Assert::AreEqual< Complex >(
        expected,
        actual,
        "Multiply poly",
        AreEqualComplexNumbers);
}

void FastFourierTransformTests::Test()
{
    const Number epsilon = 1.0e-10;

    Comparer.Epsilon = epsilon;
    Comparer.AbsoluteValuePtr = AbsoluteValue;

    const Number pi = 3.1415926535897932384626433832795028841971693993751;

    FFTTest(pi);
    PolyMultiplyTest(pi);
}