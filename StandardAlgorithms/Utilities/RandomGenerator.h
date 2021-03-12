#pragma once
#include <cassert>
#include <random>
#include <limits>
#include <iomanip>
#include <sstream>
#include "StreamUtilities.h"

template<typename Number,
    typename Distribution = std::uniform_int_distribution<Number>,
    typename Engine = std::default_random_engine>
    class RandomGenerator final
{
    std::random_device _device;
    Distribution _distr;
    Engine _engine;

    unsigned int _count;

#ifdef _DEBUG
    Number _minInclusive, _maxInclusive;
#endif

    inline static void CheckLimits(
        const Number& minInclusive, const Number& maxInclusive);

#ifdef _DEBUG
    static void CheckWithin(
        const Number& minInclusive, const Number& maxInclusive,
        const Number& value);
#endif

public:
    explicit RandomGenerator(
        const Number& minInclusive = std::numeric_limits<Number>::min(),
        const Number& maxInclusive = std::numeric_limits<Number>::max());

    Number operator()();

    Number operator()(const Number& minInclusive, const Number& maxInclusive);

private:
    inline void TryReset();
};

template <typename Number, typename Distribution, typename Engine>
inline void RandomGenerator<Number, Distribution, Engine>::CheckLimits(
    const Number& minInclusive, const Number& maxInclusive)
{
    if (maxInclusive < minInclusive)
    {
        std::ostringstream ss;
        ss << "The maxInclusive (" << maxInclusive
            << ") must be not less than minInclusive (" << minInclusive << ").";
        StreamUtilities::throw_exception(ss);
    }
}

#ifdef _DEBUG

template <typename Number, typename Distribution, typename Engine>
void RandomGenerator<Number, Distribution, Engine>::CheckWithin(
    const Number& minInclusive, const Number& maxInclusive, const Number& value)
{
    const bool isOk = minInclusive <= value && value <= maxInclusive;
    if (!isOk)
    {
        std::ostringstream ss;
        const auto prec = StreamUtilities::GetMaxDoublePrecision();

        ss << std::setprecision(prec)
            << "The value (" << value
            << ") must belong to the range ["
            << minInclusive
            << ", " << maxInclusive
            << "].";
        StreamUtilities::throw_exception(ss);
    }
}

#endif

template <typename Number, typename Distribution, typename Engine>
RandomGenerator<Number, Distribution, Engine>::RandomGenerator(
    const Number& minInclusive, const Number& maxInclusive)
    :
    _device(),
    _distr(minInclusive, maxInclusive),
    _engine(_device()),
    _count(0)

#ifdef _DEBUG
    , _minInclusive(minInclusive), _maxInclusive(maxInclusive)
#endif
{
    CheckLimits(minInclusive, maxInclusive);
}

template <typename Number, typename Distribution, typename Engine>
Number RandomGenerator<Number, Distribution, Engine>::operator()()
{
    TryReset();

    const Number result = _distr(_engine);
#ifdef _DEBUG
    CheckWithin(_minInclusive, _maxInclusive, result);
#endif
    return result;
}

template <typename Number, typename Distribution, typename Engine>
Number RandomGenerator<Number, Distribution, Engine>::operator()(
    const Number& minInclusive, const Number& maxInclusive)
{
    CheckLimits(minInclusive, maxInclusive);

    TryReset();

    const Number result = _distr(_engine,
        Distribution::param_type(minInclusive, maxInclusive));
#ifdef _DEBUG
    CheckWithin(minInclusive, maxInclusive, result);
#endif
    return result;
}

template <typename Number, typename Distribution, typename Engine>
inline void RandomGenerator<Number, Distribution, Engine>::TryReset()
{
    const int maxCount =
#ifdef _DEBUG
        16
#else
        1024
#endif
        ;
    if (++_count == maxCount)
    {
        _count = 0;
        _engine.seed(_device());
    }
}

using DoubleRandom = RandomGenerator<double, std::uniform_real_distribution<double>>;
using SizeRandom = RandomGenerator<size_t, std::uniform_int_distribution<size_t>>;
using IntRandom = RandomGenerator<int, std::uniform_int_distribution<int>>;
using UnsignedIntRandom = RandomGenerator<unsigned, std::uniform_int_distribution<unsigned>>;
using ShortRandom = RandomGenerator<short, std::uniform_int_distribution<short>>;