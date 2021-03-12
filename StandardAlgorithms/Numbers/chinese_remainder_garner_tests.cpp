#include"chinese_remainder_garner_tests.h"
#include"../Utilities/ert.h"
#include"chinese_remainder_garner.h"

void Standard::Algorithms::Numbers::Tests::chinese_remainder_garner_tests()
{
    using int_t = std::int64_t;

    const std::vector<int_t> mods{// NOLINTNEXTLINE
        2, 5, 11, 7
    };

    const std::vector<int_t> remainders{// NOLINTNEXTLINE
        1, 4, 6, 0
    };

    int_t xxx = -1;
    chinese_remainder_garner(mods, xxx, remainders);

    constexpr auto expected = 259;

    ::Standard::Algorithms::ert::are_equal(expected, xxx, "chinese_remainder_garner");
}
