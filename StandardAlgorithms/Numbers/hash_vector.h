#pragma once
#include<algorithm>
#include<array>
#include<cassert>
#include<concepts>
#include<cstddef>
#include<functional>
#include<limits>
#include<stdexcept>
#include<type_traits>
#include<utility>
#include<vector>

namespace Standard::Algorithms::Numbers
{
    template<class random_t, std::int32_t function_count>
    requires(2 <= function_count)
    struct hash_vector final
    {
        constexpr hash_vector()
        {
            regenerate();
        }

        [[nodiscard]] constexpr auto func_size() const noexcept -> std::int32_t
        {
            return function_count;
        }

        template<class key_t>
        [[nodiscard]] constexpr auto hash(const key_t &key, const std::int32_t func_index) const noexcept -> std::size_t
        {
            assert(0 <= func_index && func_index < function_count);

            const auto &coef = Coefficients.at(func_index);
            auto hash1 = coef * static_cast<std::size_t>(key);
            return hash1;
        }

        constexpr void regenerate()
        {
            for (auto &coef : Coefficients)
            {
                constexpr auto max_attempts = 1024;

                for (std::int32_t attempt{};;)
                {
                    coef = static_cast<std::size_t>(Rnd());

                    if (coef != 0U)
                    {
                        break;
                    }

                    if (max_attempts < ++attempt) [[unlikely]]
                    {
                        throw std::runtime_error("The hash_vector has failed to generate a non-zero coefficient. "
                                                 "Please check your random number generator.");
                    }
                }
            }
        }

private:
        random_t Rnd{};

        std::array<std::size_t, function_count> Coefficients
            // Had better be left initially uninitialized.
            // todo(p3): del the initialization?
            {};
    };

    template<std::integral hash_t, class iter_t, class iter_t2 = iter_t>
    [[nodiscard]] constexpr auto rolling_hash(const hash_t &coef, iter_t begin1, iter_t2 end1, const hash_t &mod)
        -> hash_t
    {
        assert(static_cast<hash_t>(2) <= coef && static_cast<hash_t>(2) <= mod && coef != mod);

        hash_t hash1{};

        while (begin1 != end1)
        {
            const auto &val = *begin1;
            hash1 = (hash1 * coef + val) % mod;
            ++begin1;
        }

        return hash1;
    }

    /*template<class random_t, std::int32_t function_count>
    requires(2 <= function_count)
    struct poly_hash final
    {
        constexpr poly_hash()
        {
            regenerate();
        }

        [[nodiscard]] constexpr auto func_size() const noexcept { return function_count; }

        template<class key_t>
        [[nodiscard]]  constexpr auto hash(const key_t &key) const noexcept -> std::size_t
        {
            std::size_t hash1{};
            std::int32_t index{};

            for (const auto &c : key)
            {
                assert(index < function_count);

                const auto &coef = Coefficients[index];
                hash1 = hash1 * coef + static_cast<std::size_t>(c);

                if (!(++index < function_count))
                {
                    index = 0;
                }
            }

            return hash1;
        }

         constexpr void regenerate()
        {
            for (auto &coef : Coefficients)
            {
                constexpr auto max_attempts = 1024;

                std::int32_t attempt{};

                for (; ;)
                {
                    coef = static_cast<std::size_t>(Rnd());
                    if (coef != 0U)
                    {
                        break;
                    }

                    if (max_attempts < ++attempt) [[unlikely]]
                    {
                        throw std::runtime_error("The poly_hash has failed to generate a non-zero coefficient. Please
check your random number generator.");
                    }
                }
            }
        }

private:
        random_t Rnd{};
        std::array<std::size_t, function_count> Coefficients;
    };

    template<class random_t, std::int32_t rows = sizeof(std::size_t)>
    requires(2 <= rows && rows <= 8);
    struct tabulation_hash final
    {
         constexpr tabulation_hash()
        {
            regenerate();
        }

        [[nodiscard]] constexpr auto func_size() const noexcept { return rows; }

        template<class key_t>
        [[nodiscard]]  constexpr auto hash(const key_t &key) const noexcept -> std::size_t
        {
            std::size_t hash1{};

            for (const auto &cha : key)
            {
                const auto number = static_cast<std::size_t>(cha);

                for (std::int32_t index{}; index < rows; ++index)
                {
                    const auto pos = (number >> ((CHAR_BIT)*index)) & (Columns - 1U);
                    assert(pos < Columns);

                    const auto &coef = Coefficients[index][pos];
                    hash1 ^= coef;
                }
            }

            return hash1;
        }

         constexpr void regenerate()
        {
            for (auto &row_coeffs : Coefficients)
            {
                for (auto &coef : row_coeffs)
                {
                    constexpr auto max_attempts = 1024;

                    std::int32_t attempt{};

                    for (; ;)
                    {
                        coef = static_cast<std::size_t>(Rnd());
                        if (coef != 0U)
                        {
                            break;
                        }

                        if (max_attempts < ++attempt) [[unlikely]]
                        {
                            throw std::runtime_error("The tabulation_hash has failed to generate a non-zero coefficient.
Please check your random number generator.");
                        }
                    }
                }
            }
        }

private:
        static constexpr auto Columns = 256U;

        random_t Rnd{};
        std::array<std::array<std::size_t, Columns>, rows> Coefficients{};
    }; */
} // namespace Standard::Algorithms::Numbers
