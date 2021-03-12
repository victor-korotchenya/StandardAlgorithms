#pragma once
#include"../Numbers/to_unsigned.h"
#include"is_debug.h"
#include"throw_exception.h"
#include<concepts>
#include<limits>
// flawfinder: ignore suppress
#include<random>
#include<type_traits>
#include<utility>

namespace Standard::Algorithms::Utilities
{
    namespace Inner
    {
        template<class int_t>
        constexpr void check_within(const int_t &min_inclusive, const int_t &max_inclusive, const int_t &value)
        {
            if (const auto is_ok = !(value < min_inclusive) && !(max_inclusive < value); is_ok) [[likely]]
            {
                return;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "The value (" << value << ") must belong to the range [" << min_inclusive << ", " << max_inclusive
                << "].";

            throw_exception(str);
        }

        template<class int_t>
        constexpr void check_limits(const int_t &min_inclusive, const int_t &max_inclusive)
        {
            if (max_inclusive < min_inclusive)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The maxInclusive (" << max_inclusive << ") must be not less than minInclusive ("
                    << min_inclusive << ").";

                throw_exception(str);
            }
        }

        constexpr void try_reset( // Cppcheck must be on a separate line.
                                  // cppcheck-suppress [constParameter]
            std::random_device &rand_dev, auto &engine, std::uint32_t &count)
        {
            // NOLINTNEXTLINE
            constexpr auto maxCount = ::Standard::Algorithms::is_debug ? 16U : 1024U;

            if (!(++count < maxCount))
            {
                count = 0U;
                engine.seed(rand_dev());
            }
        }

        [[nodiscard]] constexpr auto random_boolean(const std::integral auto &number) -> bool
        {
            const auto value = ::Standard::Algorithms::Numbers::to_unsigned(number);

            auto bul = 0U != (value & 1U);
            return bul;
        }
    } // namespace Inner

    template<class int_t1, class distribution_t1, class engine_t1 = std::default_random_engine>
    struct random_generator final
    {
        using int_t = int_t1;
        using distribution_t = distribution_t1;
        using engine_t = engine_t1;

public:
        constexpr explicit random_generator(const int_t &min_inclusive1 = std::numeric_limits<int_t>::min(),
            const int_t &max_inclusive2 = std::numeric_limits<int_t>::max())
            : distr(min_inclusive1, max_inclusive2)
            , engine(rand_dev())
#ifdef _DEBUG
            , min_inclusive(min_inclusive1)
            , max_inclusive(max_inclusive2)
#endif
        {
            Inner::check_limits(min_inclusive1, max_inclusive2);
        }

        random_generator(const random_generator &) = delete;
        auto operator= (const random_generator &) & -> random_generator & = delete;
        random_generator(random_generator &&) noexcept = delete;
        auto operator= (random_generator &&) &noexcept -> random_generator & = delete;

        // Mark dtor constexpr.
        constexpr ~random_generator() noexcept = default;

        [[nodiscard]] constexpr auto operator() () -> int_t
        {
            Inner::try_reset(rand_dev, engine, count);
            int_t result = distr(engine);

#if _DEBUG
            Inner::check_within(min_inclusive, max_inclusive, result);
#endif

            return result;
        }

        // Prefer to use the other operator without arguments which is faster.
        [[nodiscard]] constexpr auto operator() (const int_t &min_inclusive1, const int_t &max_inclusive2) -> int_t
        {
            Inner::check_limits(min_inclusive1, max_inclusive2);
            Inner::try_reset(rand_dev, engine, count);

            int_t result = distr(engine, typename distribution_t::param_type(min_inclusive1, max_inclusive2));

            if constexpr (::Standard::Algorithms::is_debug)
            {
                Inner::check_within(min_inclusive1, max_inclusive2, result);
            }

            return result;
        }

        template<class t = int_t>
        requires(std::is_integral_v<t>)
        [[nodiscard]] constexpr explicit operator bool ()
        {
            auto bul = Inner::random_boolean(this->operator() ());
            return bul;
        }

        constexpr void limits(const int_t &min_inclusive1 = std::numeric_limits<int_t>::min(),
            const int_t &max_inclusive2 = std::numeric_limits<int_t>::max())
        {
            Inner::check_limits(min_inclusive1, max_inclusive2);
            distr = distribution_t(min_inclusive1, max_inclusive2);
            count = {};

#ifdef _DEBUG
            min_inclusive = min_inclusive1;
            max_inclusive = max_inclusive2;
#endif
        }

private:
        std::random_device rand_dev{};
        distribution_t distr{};
        engine_t engine{};

        std::uint32_t count{};

#ifdef _DEBUG
        int_t min_inclusive;
        int_t max_inclusive;
#endif
    };

    template<class t>
    using random_t = std::conditional_t<std::is_integral_v<t>, random_generator<t, std::uniform_int_distribution<t>>,
        // case
        std::conditional_t<std::is_floating_point_v<t>, random_generator<t, std::uniform_real_distribution<t>>,
            // default
            std::void_t<t>>>;
} // namespace Standard::Algorithms::Utilities
