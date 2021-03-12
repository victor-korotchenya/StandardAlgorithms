#include "SystemFunctions.h"

bool IsLittleEndian() noexcept
{
    const unsigned short one = 1;
    const unsigned short* const pOne = &one;

    const unsigned char* const pOneAs1Byte =
        reinterpret_cast<const unsigned char* const>(pOne);

    const unsigned char oneAs1Byte = *pOneAs1Byte;

    const bool result = 0 != oneAs1Byte;
    return result;
}