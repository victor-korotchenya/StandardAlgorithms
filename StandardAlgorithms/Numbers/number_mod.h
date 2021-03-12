#pragma once
#include <ostream>
#include <type_traits>

namespace Privet::Algorithms::Numbers
{
    template<typename int_t, int_t mod, typename long_t>
    class number_mod final
    {
        static_assert(2 <= mod && sizeof(int_t) <= sizeof(long_t));
        int_t _val;

    public:
        using number_t = int_t;
        using long_number_t = long_t;

        template<typename int_t2 = int_t>
        number_mod(int_t2 n = {})
        {
            _val = static_cast<int_t>(n % mod);
            if (_val < 0)
                _val += mod;
        }

        const int_t& val() const noexcept
        {
            return _val;
        }

        inline int_t operator()() const
        {
            return _val;
        }

        static constexpr inline int_t modulo()
        {
            return mod;
        }

        inline friend std::ostream& operator <<
            (std::ostream& str, const number_mod& b)
        {
            str << b._val;
            return str;
        }

        inline number_mod& operator+= (const number_mod& b)
        {
            _val += b._val;
            if (mod <= _val)
                _val -= mod;
            return *this;
        }

        inline number_mod& operator*= (const number_mod& b)
        {
            _val = static_cast<int_t>((static_cast<long_t>(_val)
                * static_cast<long_t>(b._val)) % mod);
            return *this;
        }
    };

    template<typename int_t, int_t mod, typename long_t>
    inline constexpr bool operator==(
        const number_mod<int_t, mod, long_t> a,
        const number_mod<int_t, mod, long_t> b)
    {
        return a.val() == b.val();
    }

    template<typename int_t, int_t mod, typename long_t>
    inline constexpr bool operator<(
        const number_mod<int_t, mod, long_t> a,
        const number_mod<int_t, mod, long_t> b)
    {
        return a.val() < b.val();
    }

    template<typename int_t, int_t mod, typename long_t>
    inline number_mod<int_t, mod, long_t>
        operator+(
            const number_mod<int_t, mod, long_t> a,
            const number_mod<int_t, mod, long_t> b)
    {
        auto result = a;
        result += b;
        return result;
    }

    template<typename int_t, int_t mod, typename long_t>
    inline number_mod<int_t, mod, long_t>
        operator*(
            const number_mod<int_t, mod, long_t> a,
            const number_mod<int_t, mod, long_t> b)
    {
        auto result = a;
        result *= b;
        return result;
    }

    // modular exponentiation
    template<typename int_t, int_t mod, typename long_t, typename int_t2>
    inline number_mod<int_t, mod, long_t>
        modular_power(
            number_mod<int_t, mod, long_t> base,
            int_t2 exponent)
    {
        if (0 == base.val())
        {
            return 0;
        }

        number_mod<int_t, mod, long_t> result = 1;
        while (exponent)
        {
            if (exponent & 1)
            {
                result *= base;
            }
            base *= base;
            exponent >>= 1;
        }
        return result;
    }
}