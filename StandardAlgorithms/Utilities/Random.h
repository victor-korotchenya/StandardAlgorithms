#pragma once
#include <limits>
#include <vector>
#include <unordered_set>
#include "StreamUtilities.h"
#include "ExceptionUtilities.h"
#include "../Numbers/Arithmetic.h"
#include "RandomGenerator.h"

template <typename t>
void ShuffleArray(t* data, size_t size)
{
    ThrowIfNull(data, "data");
    if (size-- <= 1)
        return;

    SizeRandom sizeRandom;
    size_t index = 0;
    do
    {
        const auto randomIndex = sizeRandom(index, size);
        std::swap(data[index], data[randomIndex]);
    } while (++index < size);
}

template <typename t>
void ShuffleArray(std::vector<t>& dataVector)
{
    ShuffleArray(dataVector.data(), dataVector.size());
}

void FillRandom(std::vector<bool>& data, const size_t size);

template <typename T,
    //It can be used to create negative values
    //e.g. when T is "long long int", the RandomType must be "int",
    //or otherwise the numbers will be non-negative.
    typename RandomType = T>
    void FillRandom(std::vector<T>& data, const size_t size,
        //The cutter can be used to create datasets of smaller range.
        //If (0 != cutter), the each added item will be taken modulo "cutter".
        const T cutter = {},
        //Whether to generate only unique data.
        const bool isUniqueOnly = {})
{
    const bool hasCutter = T(0) != cutter;
    if (hasCutter)
    {
        if (IsNegative<T>(cutter))
        {
            std::ostringstream ss;
            ss << "The cutter(" << cutter << ") must be positive.";
            StreamUtilities::throw_exception(ss);
        }

        if (isUniqueOnly && size_t(cutter) < size)
        {
            std::ostringstream ss;
            ss << "Cannot make unique data of size=" << size
                << " having cutter=" << cutter << ".";
            StreamUtilities::throw_exception(ss);
        }
    }

    const RandomType minInclusive = hasCutter ?
        (std::numeric_limits<RandomType>::is_signed ? RandomType(1) - RandomType(cutter) : RandomType(0))
        : std::numeric_limits<RandomType>::min();

    const RandomType maxInclusive = hasCutter
        ? RandomType(cutter) - RandomType(1) : std::numeric_limits<RandomType>::max();

    RandomGenerator<RandomType> randomGenerator(minInclusive, maxInclusive);

    data.clear();
    data.resize(size);

    std::unordered_set<T> uniqueItems;
    size_t index = 0;
    do
    {
    Label_ReStart:
        const RandomType randomValue = randomGenerator();
        const T item = static_cast<T>(randomValue);

        if (isUniqueOnly)
        {
            const auto added = uniqueItems.insert(item);
            if (!added.second)
            {//Skip duplicates.
                goto Label_ReStart;
            }
        }

        data[index] = item;
    } while (++index < size);
}

template<class random_t, char min_char = '0', char max_char = '9', class string_t = std::string>
[[nodiscard]] string_t random_string(random_t& r, const int min_size = 1, const int max_size = 10)
{
    static_assert(min_char <= max_char);
    assert(0 <= min_size && min_size <= max_size);

    const auto size = r(min_size, max_size);
    string_t s(size, 0);
    for (auto i = 0; i < size; ++i)
        s[i] = static_cast<char>(r(min_char, max_char));

    return s;
}

template<class int_t, class random_t>
[[nodiscard]] std::vector<int_t> random_vector(random_t& r, const int min_size = 1, const int max_size = 10,
    const int_t min_value = {}, const int_t max_value = 10)
{
    assert(0 <= min_size && min_size <= max_size && min_value <= max_value);

    const auto size = r(min_size, max_size);
    std::vector<int_t> v(size);

    for (auto& a : v)
        a = static_cast<int_t>(r(min_value, max_value));

    return v;
}

// In case of failure, return an empty string.
template<class random_t, class set_t, char min_char = '0', char max_char = '9', class string_t = std::string>
[[nodiscard]] string_t random_string_unique(random_t& r, set_t& uniq, const int min_size = 1, const int max_size = 10, const int attempts = 10)
{
    assert(min_size > 0 && attempts > 0);

    for (auto att = 0; att < attempts; ++att)
    {
        const auto s = random_string<random_t, min_char, max_char, string_t>(r, min_size, max_size);
        if (uniq.insert(s).second)
            return s;
    }

    return {};
}