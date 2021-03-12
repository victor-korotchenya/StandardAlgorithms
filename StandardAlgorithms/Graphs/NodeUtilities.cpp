#include "NodeUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

size_t NodeUtilities::LeftChild(const size_t parentIndex) noexcept
{
    const size_t result = (parentIndex << 1) + 1;
    return result;
}

size_t NodeUtilities::RightChild(const size_t parentIndex) noexcept
{
    const size_t result = (parentIndex << 1) + 2;
    return result;
}

size_t NodeUtilities::ParentNode(const size_t childIndex)
{
#ifdef _DEBUG
    if (0 == childIndex)
    {
        throw std::runtime_error("0 == childIndex in ParentNode.");
    }
#endif

    const size_t result = (childIndex - 1) >> 1;

#ifdef _DEBUG
    if (childIndex <= result)
    {
        throw std::runtime_error("childIndex <= result in Parent.");
    }
#endif

    return result;
}