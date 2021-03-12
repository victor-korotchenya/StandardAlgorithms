#pragma once
#include <ostream>

namespace Privet::Algorithms::Strings
{
    template<typename TSize>
    class WordPosition final
    {
        TSize _Offset, _Length;

    public:

        WordPosition(TSize offset = {}, TSize length = {});

        inline TSize get_Offset() const noexcept
        {
            return _Offset;
        }

        inline TSize get_Length() const noexcept
        {
            return _Length;
        }

        inline bool operator == (WordPosition const& other) const noexcept
        {
            const bool result = _Offset == other._Offset
                && _Length == other._Length;
            return result;
        }

        inline friend std::ostream& operator <<
            (std::ostream& str, WordPosition const& b)
        {
            str
                << "(" << b.get_Offset() << ", " << b.get_Length() << ")";
            return str;
        }
    };

    template <typename TSize>
    WordPosition<TSize>::WordPosition(TSize offset, TSize length)
        : _Offset(offset), _Length(length)
    {
    }
}