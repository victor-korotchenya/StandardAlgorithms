#pragma once
#include"../Utilities/require_utilities.h"
#include"gcd.h"
#include<vector>

namespace Standard::Algorithms::Numbers
{
    namespace Inner
    {
        template<std::signed_integral int_t>
        constexpr void mod_cleaner(const int_t &mod, int_t &value)
        {
            constexpr int_t zero{};
            require_positive(mod, "mod");

            value = static_cast<int_t>(value % mod);
            if (value < zero)
            {
                value += mod;
            }

            assert(!(value < zero) && value < mod);
        }
    } // namespace Inner

    // Modular linear equation solutions, could be none.
    // a * x == b modulo mod.
    template<std::signed_integral int_t> // Signed as ext. GCD may return a negative multiplier.
    constexpr void modulo_linear_equation(const int_t &mod, int_t aaa, std::vector<int_t> &xxxs, int_t bbb)
    {
        require_positive(mod, "mod");

        Inner::mod_cleaner<int_t>(mod, aaa);
        Inner::mod_cleaner<int_t>(mod, bbb);

        xxxs.clear();

        constexpr int_t zero{};

        int_t solut{};
        int_t qqq{};
        const auto gcd1 = gcd_extended_simple<int_t>(aaa, mod, solut, qqq);

        assert(zero < gcd1 && gcd1 <= mod);
        assert(gcd1 == solut * aaa + mod * qqq);

        if (const auto remainder = bbb % gcd1; zero != remainder)
        {// No solutions.
            return;
        }

        const auto delta = static_cast<int_t>(mod / gcd1);
        assert(zero == mod % gcd1 && zero < delta);

        // The "solut" could be negative - make it non-negative.
        solut = static_cast<int_t>((solut % mod + mod) * bbb / gcd1 % mod);

        assert(zero <= solut && solut < mod);
        {// Ensure minimal solut.
            const auto temp_q = static_cast<int_t>(solut / delta);

            solut -= static_cast<int_t>(temp_q * delta);
            assert(zero <= solut && solut < mod);
        }

        xxxs.reserve(static_cast<std::size_t>(gcd1));

        for (auto index = zero; index < gcd1; ++index)
        {
            assert(zero <= solut && solut < mod);

            xxxs.push_back(solut);
            solut += delta;
        }
    }

    // Slow.
    template<std::signed_integral int_t>
    constexpr void modulo_linear_equation_slow(const int_t &mod, int_t aaa, std::vector<int_t> &xxxs, int_t bbb)
    {
        require_positive(mod, "mod");

        Inner::mod_cleaner<int_t>(mod, aaa);
        Inner::mod_cleaner<int_t>(mod, bbb);

        xxxs.clear();

        constexpr int_t zero{};

        for (int_t solut{}; solut < mod; ++solut)
        {
            const auto lefts = (aaa * solut - bbb) % mod;
            if (zero == lefts)
            {
                xxxs.push_back(solut);
            }
        }
    }
} // namespace Standard::Algorithms::Numbers
