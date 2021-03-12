#include "../test_utilities.h"
#include "number_mod.h"
#include "number_mod_tests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using integer_t = int;
    using long_integer_t = long long;
    constexpr auto mod = 1000 * 1000 * 1000 + 7;
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            namespace Tests
            {
                void number_mod_tests()
                {
                    using n_t = number_mod<integer_t, mod, long_integer_t>;
                    const n_t one = 1;

                    n_t e = 5;
                    e += static_cast<n_t>(n_t::modulo() - 3);
                    e = e + static_cast<n_t>(-1);
                    Assert::AreEqual(one, e, "equal");

                    auto b = one + static_cast<n_t>(5);
                    Assert::AreEqual(static_cast<n_t>(6), b, "sum");

                    auto c = static_cast<n_t>(8167) + one;
                    Assert::AreEqual(static_cast<n_t>(8168), c, "sum2");

                    auto d = static_cast<n_t>(10) * b;
                    Assert::AreEqual(static_cast<n_t>(60), d, "product");

                    {
                        const auto mp = modular_power<n_t::number_t,
                            n_t::modulo(), n_t::long_number_t>(one, 123456789);
                        Assert::AreEqual(one, mp, "power(1, x)");
                    }

                    const n_t two = 2;
                    for (int power = 1; power < 64; ++power)
                    {
                        const auto mp = modular_power<n_t::number_t,
                            n_t::modulo(), n_t::long_number_t>(two, power);

                        const n_t expected = (1ull << power) % n_t::modulo();
                        Assert::AreEqual(expected, mp, "power(2, " + to_string(power) + ')');
                    }
                }
            }
        }
    }
}