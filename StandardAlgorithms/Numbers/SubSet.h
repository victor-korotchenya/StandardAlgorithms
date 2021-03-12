#pragma once

#include <set>
#include <ostream>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            //TODO: p3. vector could be faster than set.

            template <typename Item>
            struct SubSet final
            {
                static const char BeginSymbol = '{';
                static const char EndSymbol = '}';

                using TSubSequence = std::set<Item>;
                using TSubSets = std::set<SubSet>;

                TSubSequence SubSequence;
                TSubSets SubSets;

                explicit SubSet(
                    const TSubSequence& subSequence = {},
                    const TSubSets& subSets = {})
                    : SubSequence(subSequence), SubSets(subSets)
                {
                }

                explicit SubSet(
                    const TSubSets& subSets,
                    const TSubSequence& subSequence = {})
                    : SubSequence(subSequence), SubSets(subSets)
                {
                }

                inline bool empty() const
                {
                    const bool result = SubSequence.empty() && SubSets.empty();
                    return result;
                }

                inline bool operator ==(const SubSet& b) const
                {
                    const bool result = SubSets == b.SubSets
                        && SubSequence == b.SubSequence;
                    return result;
                }

                inline bool operator < (const SubSet& b) const
                {
                    if (SubSequence < b.SubSequence)
                    {
                        return true;
                    }

                    if (b.SubSequence < SubSequence)
                    {
                        return false;
                    }

                    const bool result = SubSets < b.SubSets;
                    return result;
                }

                friend std::ostream& operator << (
                    std::ostream& str, const SubSet& b)
                {
                    const bool isEmptySubSequence = b.SubSequence.empty();
                    const bool isEmptySubSets = b.SubSets.empty();

                    if (isEmptySubSequence && isEmptySubSets)
                    {
                        str << BeginSymbol << EndSymbol;
                        return str;
                    }

                    if (!isEmptySubSets)
                    {
                        str << BeginSymbol;
                    }

                    if (!isEmptySubSequence)
                    {
                        str << BeginSymbol;
                        PrintCollection(str, b.SubSequence);
                        str << EndSymbol;

                        if (!isEmptySubSets)
                        {
                            PrintSeparator(str);
                        }
                    }

                    if (!isEmptySubSets)
                    {
                        PrintCollection(str, b.SubSets);
                        str << EndSymbol;
                    }

                    return str;
                }

                template<typename Collection>
                static void PrintCollection(std::ostream& str, const Collection& collection)
                {
                    bool wasBefore = false;
                    for (const auto& item : collection)
                    {
                        if (wasBefore)
                        {
                            PrintSeparator(str);
                        }
                        else
                        {
                            wasBefore = true;
                        }

                        str << item;
                    }
                }

                inline static void PrintSeparator(std::ostream& str)
                {
                    str << ", ";
                }
            };
        }
    }
}