#pragma once
#include <vector>

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            template <typename BaseType,
                typename TypeWithId,
                typename BaseTypeContainer = std::vector< BaseType >,
                typename TypeWithIdContainer = std::vector< TypeWithId >
            >
                class WithIdUtilities final
            {
                WithIdUtilities() = delete;

            public:
                static void CopyAndAssignSequentialId(
                    const BaseTypeContainer& source,
                    TypeWithIdContainer& target)
                {
                    const size_t size = source.size();

                    target.clear();
                    target.reserve(size);
                    size_t index = 0;
                    do
                    {
                        //TypeWithId withId2;withId2 = std::move(source[index]);
                        TypeWithId withId = source[index];
                        withId.Id = index;

                        target.push_back(withId);
                    } while ((++index) < size);
                }
            };
        }
    }
}