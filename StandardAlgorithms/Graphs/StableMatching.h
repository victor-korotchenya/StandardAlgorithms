#pragma once

//#define ShowIntermediateResults

#ifdef ShowIntermediateResults
#include <iostream>
#endif

#include <vector>
#include <sstream>
#include <queue>
#include "../Utilities/QueueUtilities.h"
#include "../Utilities/StreamUtilities.h"

namespace Privet::Algorithms::Graphs
{
    //Finds the stable matching.
    template <typename Vertex >
    class StableMatching final
    {
        StableMatching() = delete;

    public:
        using VertexType = Vertex;
        using Preference = std::vector< Vertex >;
        using PreferenceList = std::vector< Preference >;

        static void Find(
            const PreferenceList& aPreferenceList,
            const PreferenceList& bPreferenceList,
            //The result for other part is symmetrical and
            // can be built from the result.
            Preference& result);

    private:
        static void CheckPreferenceList(
            const int number,
            const size_t size,
            const PreferenceList& preferenceList,
            std::vector< bool >& bitSet);

        static void CheckPreference(
            const int number,
            const size_t size,
            const size_t position,
            const Preference& preference,
            std::vector< bool >& bitSet);

        static void Run(
            const PreferenceList& aPreferenceList,
            const PreferenceList& backPreferenceList,
            const size_t size,
            Preference& other,
            Preference& result);

        static void RestoreValuesFromIndexes(
            const PreferenceList& aPreferenceList,
            const size_t size,
            Preference& result);

        static void BuildBackPreferenceList(
            const PreferenceList& preferenceList,
            PreferenceList& backPreferenceList);
    };

    template <typename Vertex >
    void StableMatching< Vertex >::Find(
        const PreferenceList& aPreferenceList,
        const PreferenceList& bPreferenceList,
        Preference& result)
    {
        const size_t size = aPreferenceList.size();
        if (0 == size)
        {
            throw std::runtime_error("The input A must be not empty.");
        }

        const size_t bSize = bPreferenceList.size();
        if (size != bSize)
        {
            std::ostringstream ss;
            ss << "The size of A (" << size
                << ") must be equal to that of B(" << bSize << ").";
            StreamUtilities::throw_exception(ss);
        }

        std::vector< bool > bitSet;
        CheckPreferenceList(0, size, aPreferenceList, bitSet);
        CheckPreferenceList(1, size, bPreferenceList, bitSet);

        //To determine others' preferences quickly.
        PreferenceList backPreferenceList;
        BuildBackPreferenceList(bPreferenceList, backPreferenceList);

        Preference other;
        Run(aPreferenceList, backPreferenceList, size, other, result);

#ifdef _DEBUG
        //Check the result.
        CheckPreference(-1, size, 0, result, bitSet);
#endif
    }

    template <typename Vertex >
    void StableMatching< Vertex >::Run(
        const PreferenceList& aPreferenceList,
        const PreferenceList& backPreferenceList,
        const size_t size,
        Preference& other,
        Preference& result)
    {
        const Vertex initialPreference = static_cast<Vertex>(0);

        //Store the indexes to enable fast move to the next other instance.
        result.assign(size, initialPreference);

        const Vertex freeAssignment = initialPreference - static_cast<Vertex>(1);
        other.assign(size, freeAssignment);

        std::queue<Vertex> freeQueue;
        iota(freeQueue, size);

#ifdef ShowIntermediateResults
        std::cout << "\nStart stable matching..\n";
#endif
        while (!freeQueue.empty())
        {
            const Vertex a = freeQueue.front();

            const Vertex index = result[a];
#ifdef _DEBUG
            if (size <= index)
            {
                std::ostringstream ss;
                ss << "Error1. The size=" << size << " <= index=" << index
                    << ".";
                StreamUtilities::throw_exception(ss);
            }
#endif
            const Vertex b = aPreferenceList[a][index];
            Vertex bPreference = other[b];

            if (freeAssignment == bPreference)
            {
#ifdef ShowIntermediateResults
                std::cout << "Free " << a << " to w=" << b << '\n';
#endif
                other[b] = a;
                freeQueue.pop();
            }
            else
            {
                const Preference& backPreference = backPreferenceList[b];

                const Vertex indexA = backPreference[a];
                const Vertex indexB = backPreference[bPreference];

                const bool isBetter = indexA < indexB;
                if (isBetter)
                {
#ifdef ShowIntermediateResults
                    std::cout << " " << a << " isBetter than " << bPreference
                        << " for w= " << b << '\n';
#endif
                    other[b] = a;
                    freeQueue.pop();
                    freeQueue.push(bPreference);
                }
                else
                {
#ifdef ShowIntermediateResults
                    std::cout << " " << a << " is not better than " << bPreference
                        << " for w= " << b << '\n';
#endif
                    bPreference = a;
                }

                ++result[bPreference];
            }
        }

        RestoreValuesFromIndexes(aPreferenceList, size, result);
    }

    template <typename Vertex>
    void StableMatching<Vertex>::RestoreValuesFromIndexes(
        const PreferenceList& aPreferenceList,
        const size_t size,
        Preference& result)
    {
#ifdef ShowIntermediateResults
        std::cout << "\n\nFinal results:\n";
#endif

        for (size_t i = 0; i < size; ++i)
        {
            const Vertex index = result[i];
#ifdef _DEBUG
            if (size <= index)
            {
                std::ostringstream ss;
                ss << "Error2. The size=" << size << " <= index=" << index
                    << ".";
                StreamUtilities::throw_exception(ss);
            }
#endif
            result[i] = aPreferenceList[i][index];

#ifdef ShowIntermediateResults
            std::cout << "M[ " << i << " ] = " << result[i] << '\n';
#endif
        }
    }

    template <typename Vertex >
    void StableMatching< Vertex >::CheckPreferenceList(
        const int number,
        const size_t size,
        const PreferenceList& preferenceList,
        std::vector< bool >& bitSet)
    {
        for (size_t position = 0; position < size; ++position)
        {
            const Preference& preference = preferenceList[position];
            CheckPreference(number, size, position, preference, bitSet);
        }
    }

    template <typename Vertex >
    void StableMatching< Vertex >::CheckPreference(
        const int number,
        const size_t size,
        const size_t position,
        const Preference& preference,
        std::vector< bool >& bitSet)
    {
        const size_t preferenceSize = preference.size();
        if (size != preferenceSize)
        {
            std::ostringstream ss;
            ss << "The " << number << "_" << position
                << " preference size (" << preferenceSize
                << ") must be equal to common size (" << size << ").";
            StreamUtilities::throw_exception(ss);
        }

        bitSet.assign(size, false);

        for (size_t j = 0; j < size; ++j)
        {
            const Vertex value = preference[j];

            const bool isOk = 0 <= value && value < size;
            if (!isOk)
            {
                std::ostringstream ss;
                ss << "The " << number << "_" << position
                    << " preference at [" << j
                    << "] has invalid value (" << value
                    << "). Please check the preferences.";
                StreamUtilities::throw_exception(ss);
            }

            if (bitSet[value])
            {
                std::ostringstream ss;
                ss << "The " << number << "_" << position
                    << " preference at [" << value
                    << "] has already been seen before."
                    << " Please check the preferences.";
                StreamUtilities::throw_exception(ss);
            }

            bitSet[value] = true;
        }
    }

    template <typename Vertex>
    void StableMatching<Vertex>::BuildBackPreferenceList(
        const PreferenceList& preferenceList,
        PreferenceList& backPreferenceList)
    {
        const size_t size = preferenceList.size();

        backPreferenceList.clear();
        backPreferenceList.resize(size);

        for (size_t i = 0; i < size; ++i)
        {
            const Preference& preference = preferenceList[i];
            Preference& backPreference = backPreferenceList[i];
            backPreference.resize(size);

            for (size_t j = 0; j < size; ++j)
            {
                const Vertex v = preference[j];
                backPreference[v] = static_cast<Vertex>(j);
            }
        }

#ifdef _DEBUG
        std::vector< bool > bitSet;
        CheckPreferenceList(-3, size, backPreferenceList, bitSet);
#endif
    }
}