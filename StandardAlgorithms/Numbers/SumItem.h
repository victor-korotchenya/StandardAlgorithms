#pragma once

#include <sstream>

#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            template <typename Integer = unsigned long long int>
            struct SumItem final
            {
                Integer Count;

                Integer Value;

                //Sort by Value.
                inline bool operator < (
                    SumItem const& b) const
                {
                    return Value < b.Value;
                }

                void Validate() const
                {
                    if (Count <= 0 || Value <= 0)
                    {
                        std::ostringstream ss;
                        ss << "The both count " << Count
                            << " and value " << Value
                            << " must be strictly positive.";
                        StreamUtilities::throw_exception(ss);
                    }
                }
            };
        }
    }
}