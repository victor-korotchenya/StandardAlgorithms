#include"min_three_tests.h"
#include"../Utilities/random.h"
#include"sort.h"

void Standard::Algorithms::Numbers::Tests::min_three_tests()
{
    using int_t = std::int16_t;

    Standard::Algorithms::Utilities::random_t<int_t> rnd{};

    const auto aaa = rnd();
    const auto bbb = rnd();
    const auto ccc = rnd();

    const auto expected = std::min({ aaa, bbb, ccc });
    const auto actual = min3(aaa, bbb, ccc);

    if (expected == actual) [[likely]]
    {
        return;
    }

    const auto message =
        "Error in min3(" + std::to_string(aaa) + ", " + std::to_string(bbb) + ", " + std::to_string(ccc) + ")";

    throw std::runtime_error(message);
}
