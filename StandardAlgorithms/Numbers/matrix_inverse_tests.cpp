#include"matrix_inverse_tests.h"
#include"../Utilities/iota_vector.h"
#include"../Utilities/random_matrix.h"
#include"../Utilities/test_utilities.h"
#include"matrix_inverse.h"
#include<cstdlib> // std::exit
#include<functional> // std::reference_wrapper
#include<optional>

namespace
{
    using floating_t = Standard::Algorithms::floating_point_type;
    using matrix_t = std::vector<std::vector<floating_t>>;
    using wrapper_t = std::reference_wrapper<const matrix_t>; // Avoid a copy.

    constexpr auto shall_print_gauss_elim = Standard::Algorithms::Numbers::Inner::shall_print_gauss_elim;

    constexpr floating_t zero{};
    constexpr floating_t one{ 1 };
    constexpr floating_t two{ 2 };
    constexpr floating_t three{ 3 };
    constexpr floating_t four{ 4 };

    constexpr auto baranka_test(const matrix_t &source_matrix, const std::string &name,
        const std::optional<wrapper_t> &expected_matrix = {}, const bool is_computed = true) -> bool
    {
        assert(!name.empty());

        const auto size = source_matrix.size();

        const auto [actual_flag, inverse] = [&source_matrix, size]()
        {// Use an immediately invoked lambda to make the variables such as inverse constant.
            auto src = source_matrix;
            matrix_t inverse_temp(size);

            auto flag = Standard::Algorithms::Numbers::matrix_inverse_gauss<floating_t>(src, inverse_temp);

            return std::make_pair(flag, std::move(inverse_temp));
        }();

        if constexpr (shall_print_gauss_elim)
        {
            ::Standard::Algorithms::print(name + " actual flag", actual_flag, std::cout);
            ::Standard::Algorithms::print(name + " inverse", inverse, std::cout);
            // std::exit(0);
        }

        if (actual_flag)
        {
            const auto ident = Standard::Algorithms::Numbers::matrix_identity<floating_t>(size);

            const auto matr_prod =
                Standard::Algorithms::Numbers::matrix_product_slow<floating_t>(size, source_matrix, size, inverse, size);

            ::Standard::Algorithms::ert::are_equal_with_epsilon(
                ident, matr_prod, name + " source_matrix * inverse ~== I");
        }

        if (is_computed)
        {
            const auto has_inverse = expected_matrix.has_value();
            ::Standard::Algorithms::ert::are_equal(has_inverse, actual_flag, name + " matrix_inverse_gauss has inverse");

            if (has_inverse)
            {
                const auto &exp_inv = expected_matrix.value().get();

                ::Standard::Algorithms::ert::are_equal_with_epsilon(
                    exp_inv, inverse, name + " matrix_inverse_gauss inverse matrix");
            }
        }
        {
            auto src_2 = source_matrix;
            matrix_t inverse_2(actual_flag ? size : 0ZU);

            const auto actual_2 = Standard::Algorithms::Numbers::matrix_inverse_via_lup<floating_t>(src_2, inverse_2);

            if constexpr (shall_print_gauss_elim)
            {
                ::Standard::Algorithms::print(name + " actual flag via LUP", actual_2, std::cout);
                ::Standard::Algorithms::print(name + " inverse via LUP", inverse_2, std::cout);
            }

            ::Standard::Algorithms::ert::are_equal(actual_flag, actual_2, name + " matrix_inverse_via_lup has inverse");

            if (actual_flag)
            {
                ::Standard::Algorithms::ert::are_equal_with_epsilon(
                    inverse, inverse_2, name + " matrix_inverse_via_lup inverse matrix");
            }
        }

        return true;
    }
} // namespace

namespace // No inverse.
{
    constexpr auto no_inverse_test(const matrix_t &source_matrix, const std::string &name) -> bool
    {
        auto res = baranka_test(source_matrix, name);
        return res;
    }

    [[nodiscard]] consteval auto no_inverse_tests() -> bool
    {
        static_assert(no_inverse_test(matrix_t{ { zero } }, "0"));

        static_assert(no_inverse_test(matrix_t{ { zero, zero }, { zero, zero } }, "0000"));

        static_assert(no_inverse_test(matrix_t{ { zero, zero }, { zero, one / three } }, "000t"));

        static_assert(no_inverse_test(matrix_t{ { zero, zero }, { zero, one / two } }, "000h"));
        static_assert(no_inverse_test(matrix_t{ { zero, zero }, { zero, one } }, "0001"));

        static_assert(no_inverse_test(matrix_t{ { zero, zero }, { one, one } }, "0011"));

        static_assert(no_inverse_test(matrix_t{ { one, zero }, { zero, zero } }, "1000"));

        static_assert(no_inverse_test(matrix_t{ { one, one }, { zero, zero } }, "1100"));

        static_assert(no_inverse_test(matrix_t{ { two, zero }, { zero, zero } }, "2000"));
        static_assert(no_inverse_test(matrix_t{ { two, two }, { zero, zero } }, "2200"));

        return true;
    }

    static_assert(no_inverse_tests());
} // namespace

namespace // There is an inverse matrix.
{
    constexpr auto has_inverse_test(
        const matrix_t &source_matrix, const std::string &name, const matrix_t &expected_matrix) -> bool
    {
        const std::optional<wrapper_t> expected_opt(std::cref(expected_matrix));

        auto res = baranka_test(source_matrix, name, expected_opt);
        return res;
    }

    [[nodiscard]] consteval auto have_inverse_tests() -> bool
    {
        static_assert(has_inverse_test(matrix_t{ { one / three } }, "1/3.0", matrix_t{ { three } }));
        static_assert(has_inverse_test(matrix_t{ { three } }, "3", matrix_t{ { one / three } }));

        static_assert(has_inverse_test(matrix_t{ { one } }, "1", matrix_t{ { one } }));
        static_assert(has_inverse_test(matrix_t{ { -one } }, "-1", matrix_t{ { -one } }));

        static_assert(has_inverse_test(matrix_t{ { two } }, "2", matrix_t{ { one / two } }));
        static_assert(has_inverse_test(matrix_t{ { -two } }, "-2", matrix_t{ { -one / two } }));

        static_assert(has_inverse_test(matrix_t{ { one / two } }, "0.5", matrix_t{ { two } }));
        static_assert(has_inverse_test(matrix_t{ { -one / two } }, "-0.5", matrix_t{ { -two } }));

        static_assert(has_inverse_test(matrix_t{ { zero, two }, { one, one } }, "stasi", //
            matrix_t{ { -one / two, one }, { one / two, zero } }));

        static_assert(has_inverse_test(matrix_t{ { two, -one / two }, { -four, four + one } }, "specific two",
            matrix_t{ { (four + one) / two / four, one / four / four }, // === {0.625, 0.0625}
                { one / two, one / four } }));

        static_assert(
            has_inverse_test(matrix_t{ { (four + one) / two / four, one / four / four }, // === {0.625, 0.0625}
                                 { one / two, one / four } },
                "specific two swapped", matrix_t{ { two, -one / two }, { -four, four + one } }));

        return true;
    }

    static_assert(have_inverse_tests());
} // namespace

namespace
{
    void random_inv_tests_many()
    {
        const std::string name = "random inverse test";
        matrix_t source_matrix;

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            source_matrix = Standard::Algorithms::Utilities ::random_matrix<floating_t>();

            try
            {
                constexpr auto is_computed = false;

                baranka_test(source_matrix, name, {}, is_computed);
            }
            catch (const std::exception &exc)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                ::Standard::Algorithms::print("Random source matrix", source_matrix, str);
                str << "\nError: " << exc.what();

                throw std::runtime_error(str.str());
            }
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::matrix_inverse_tests()
{
    random_inv_tests_many();
}
