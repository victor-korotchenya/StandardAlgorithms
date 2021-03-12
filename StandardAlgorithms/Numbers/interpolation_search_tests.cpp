#include"interpolation_search_tests.h"
#include"../Utilities/ert.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/random.h"
#include"binary_search.h"
#include"interpolation_search.h"

namespace
{
    using int_t = std::int16_t;
    using long_int_t = std::int64_t;
    static_assert(sizeof(int_t) < sizeof(long_int_t), "avoid an overflow");

    constexpr int_t min_size = 0;
    constexpr int_t max_size = 138;
    static_assert(min_size < max_size);

    constexpr auto min_value = std::numeric_limits<int_t>::min();
    constexpr auto max_value = std::numeric_limits<int_t>::max();
    static_assert(min_value < max_value);

    [[nodiscard]] constexpr auto build_source(Standard::Algorithms::Utilities::random_t<int_t> &rnd) -> std::vector<int_t>
    {
        auto vect = Standard::Algorithms::Utilities::random_vector(rnd, min_size, max_size, min_value, max_value);

        std::sort(vect.begin(), vect.end());

        return vect;
    }

    constexpr void subtest(const auto &source, const int_t &atm)
    {
        const std::span<const int_t> spa = source;

        const auto *const bina = Standard::Algorithms::Numbers::binary_search(spa, atm);

        {
            const auto *const nameb = "binary_search";

            const auto stl = std::lower_bound(source.cbegin(), source.cend(), atm);

            if (const auto found = stl != source.cend() && (*stl) == atm; found)
            {
                Standard::Algorithms::throw_if_null(nameb, bina);

                const auto dif = bina - spa.data();
                Standard::Algorithms::require_less_equal(0, dif, nameb);
                Standard::Algorithms::require_greater(source.size(), static_cast<std::size_t>(dif), nameb);

                ::Standard::Algorithms::ert::are_equal(atm, *bina, nameb);
            }
            else
            {
                ::Standard::Algorithms::ert::are_equal(nullptr, bina, "binary_search non-existing element");
            }
        }
        {
            const auto *const interp = Standard::Algorithms::Numbers::interpolation_search<long_int_t>(spa, atm);

            ::Standard::Algorithms::ert::are_equal_by_ptr(bina, interp, "interpolation_search");
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::interpolation_search_tests()
{
    Standard::Algorithms::Utilities::random_t<int_t> rnd{};
    const auto source = build_source(rnd);

    constexpr auto max_attempts = 1;

    for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
    {
        const auto number = rnd();

        try
        {
            subtest(source, number);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            ::Standard::Algorithms::print("source", source, str);
            str << "\nNumber: " << number << "\nError: " << exc.what();

            throw std::runtime_error(str.str());
        }
    }
}
