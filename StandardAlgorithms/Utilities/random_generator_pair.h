#pragma once
// "random_generator_pair.h"
#include"random_generator.h"

namespace Standard::Algorithms::Utilities
{
    template<class int_t1, class distribution_t1, bool is_sorted1 = true,
        class pair_t1 = std::pair<int_t1, int_t1>, class engine_t1 = std::default_random_engine>
    struct random_generator_pair final
    {
        using int_t = int_t1;
        using distribution_t = distribution_t1;
        using pair_t = pair_t1;
        using engine_t = engine_t1;

public:
        constexpr explicit random_generator_pair(const int_t &min_inclusive1 = std::numeric_limits<int_t>::min(),
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

        constexpr random_generator_pair(const pair_t &min_inclusive1, const pair_t &max_inclusive2)
            : random_generator_pair(std::min(min_inclusive1.first, max_inclusive2.first),
                  std::max(min_inclusive1.second, max_inclusive2.second))
        {
        }

        [[nodiscard]] constexpr auto operator() () -> pair_t
        {
            Inner::try_reset(rand_dev, engine, count);

            int_t first = distr(engine);
            int_t second = distr(engine);

            return build_pair(first, second
#ifdef _DEBUG
                ,
                min_inclusive, max_inclusive
#endif
            );
        }

        [[nodiscard]] constexpr auto operator() (const int_t &min_inclusive1, const int_t &max_inclusive2)
            -> int_t
        {
            Inner::check_limits(min_inclusive1, max_inclusive2);

            Inner::try_reset(rand_dev, engine, count);

            int_t first = distr(engine, typename distribution_t::param_type(min_inclusive1, max_inclusive2));

            int_t second = distr(engine, typename distribution_t::param_type(min_inclusive1, max_inclusive2));

            return build_pair(first, second
#ifdef _DEBUG
                ,
                min_inclusive1, max_inclusive2
#endif
            );
        }

        template<class t = int_t>
        requires(std::is_integral_v<t>)
        [[nodiscard]] constexpr explicit operator bool ()
        {
            auto bul = Inner::random_boolean(this->operator() ());
            return bul;
        }

private:
        [[nodiscard]] static constexpr auto build_pair(int_t &first, int_t &second
#ifdef _DEBUG
            ,
            const int_t &min_inclusive1, const int_t &max_inclusive2
#endif
            ) -> pair_t
        {
            if constexpr (is_sorted1)
            {
                if (second < first)
                {
                    std::swap(first, second);
                }
            }

#if _DEBUG
            Inner::check_within(min_inclusive1, max_inclusive2, first);
            Inner::check_within(min_inclusive1, max_inclusive2, second);
#endif

            return pair_t(first, second);
        }

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
    using random_pair_t =
        std::conditional_t<std::is_integral_v<t>, random_generator_pair<t, std::uniform_int_distribution<t>>,
            // case
            std::conditional_t<std::is_floating_point_v<t>, random_generator_pair<t, std::uniform_real_distribution<t>>,
                // default
                std::void_t<t>>>;
} // namespace Standard::Algorithms::Utilities
