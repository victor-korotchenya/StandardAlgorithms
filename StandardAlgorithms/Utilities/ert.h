#pragma once
// "ert.h"
#include"clang_constexpr.h"
#include"epsilon_default.h"
#include"is_char_ptr.h"
#include"max_double_precision.h"
#include"mess_c_str.h"
#include"pair_like.h"
#include"require_utilities.h"
#include"throw_exception.h"
#include<cmath>
#include<iomanip> // setprecision
#include<set>
#include<span>
#include<string>
#include<vector>

namespace Standard::Algorithms
{
    template<class c, class v = typename c::value_type, class size_type = typename c::size_type>
    concept container = std::unsigned_integral<size_type> && std::default_initializable<c> &&
        std::copy_constructible<c> && requires(c one) {
                                          {
                                              one.size()
                                              } -> std::same_as<size_type>;
                                          {
                                              one.empty()
                                              } -> std::convertible_to<bool>;
                                      };

    template<class t1, class t2>
    [[nodiscard]] constexpr auto compare_ptr(const t1 *const one, const t2 *const two) noexcept -> bool
    {
        if (one == two)
        {
            return true;
        }

        if (nullptr == one || nullptr == two)
        {
            return false;
        }

        auto result = *one == *two;
        return result;
    }

    // todo(p3): remove printing from here.
    template<class t>
    [[maybe_unused]] auto output_value(std::ostream &str, const t &value) -> std::ostream &
    {
        str << value;
        return str;
    }

    // Print "std::string", surrounded with quotes (').
    template<>
    inline auto output_value<std::string>(std::ostream &str, const std::string &value) -> std::ostream &
    {
        str << "'" << value << "'";
        return str;
    }

    // Print "unsigned char" as "unsigned" so that
    // you could read it.
    template<>
    inline auto output_value<unsigned char>(std::ostream &str, const unsigned char &value) -> std::ostream &
    {
        str << static_cast<std::uint32_t>(value);
        return str;
    }

    // Print "char" as "std::int32_t" so that
    // people could read it.
    template<>
    inline auto output_value<char>(std::ostream &str, const char &value) -> std::ostream &
    {
        str << static_cast<std::int32_t>(value);
        return str;
    }

    template<>
    inline auto output_value<double>(std::ostream &str, const double &value) -> std::ostream &
    {
        str << std::setprecision(max_double_precision) << value;
        return str;
    }

    template<>
    inline auto output_value<long double>(std::ostream &str, const long double &value) -> std::ostream &
    {
        str << std::setprecision(max_double_precision) << value;
        return str;
    }

    template<class t1, class t2>
    [[noreturn]] void not_equal_throw_exception(const t1 &one, const t2 &two, const std::string &message
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
    )
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        output_value(str, two) << " must be equal to \n";
        output_value(str, one) << ".";

#ifndef __clang__
        Standard::Algorithms::Utilities::print(str, loc);
#endif

        throw_exception(str, message);
    }

    template<class t1, class t2>
    [[noreturn]] void equal_throw_exception(const t1 &one, const t2 &two, const std::string &message
#ifndef __clang__
        ,
        const std::source_location &loc = std::source_location::current()
#endif
    )
    {
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        output_value(str, two) << " must Not be equal to \n";
        output_value(str, one) << ".";

#ifndef __clang__
        Standard::Algorithms::Utilities::print(str, loc);
#endif

        throw_exception(str, message);
    }

    struct ert final
    {
        ert() = delete;

        using floating_t = floating_point_type;

        template<class t>
        static constexpr void is_not_null_ptr(const t *const one, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            if (nullptr != one)
            {
                return;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Null returned, not null is expected.";

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw_exception(str, message);
        }

        template<class t>
        static constexpr void is_null_ptr(const t *const one, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            if (nullptr == one)
            {
                return;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Null is expected, but returned not null:\n";
            output_value(str, *one) << ".";

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw_exception(str, message);
        }

        template<class t_exception, class func_t = void (*)(void)>
        static constexpr void expect_exception(const std::string &expected_message, const func_t function1,
            const std::string &message, const bool partial_match = true
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            try
            {
                function1();
            }
            catch (const t_exception &exc)
            {
                const auto *const actual = exc.what();
                const std::string_view actual_mesage = actual;
                if (expected_message == actual_mesage || (partial_match && actual_mesage.contains(expected_message)))
                {
                    return;
                }

                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Expected\n" << expected_message << "\n but got\n" << actual_mesage;

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }

            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Did not get the expected message '" << expected_message << "'.";

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }
        }

        // The "t" must implement both: operator<< AND operator==.
        template<class t1, class t2>
        static constexpr void are_equal_by_ptr(const t1 *const one, const t2 *const two, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            if (one == two)
            {
                return;
            }

            if (nullptr == one)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                output_value(str, *two) << "\n must be equal to \nnullptr.";

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }

            if (nullptr == two)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The nullptr must be equal to \n";
                output_value(str, *one);

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }

            are_equal<t1, t2>(*one, *two, message
#ifndef __clang__
                ,
                loc
#endif
            );
        }

        template<class t1, class t2>
        static constexpr void are_equal(const t1 &one, const t2 &two, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            are_equal<t1, t2>(one, two, mess_c_str(message)
#ifndef __clang__
                                            ,
                loc
#endif
            );
        }

        template<class t1, class t2>
        static constexpr void are_equal(const t1 &one, const t2 &two, const char *message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            if (one == two)
            {
                return;
            }

            if constexpr (constexpr auto is_cptr =
                              Standard::Algorithms::is_char_ptr<t1> && Standard::Algorithms::is_char_ptr<t2>;
                          is_cptr)
            {
                if (one != nullptr && two != nullptr && 0 == std::strcmp(one, two))
                {
                    return;
                }
            }

            not_equal_throw_exception<t1, t2>(one, two, message
#ifndef __clang__
                ,
                loc
#endif
            );
        }

        template<class t1, class t2>
        static constexpr void greater(const t1 &great, const t2 &smaller, const char *message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            if (smaller < great)
            {
                return;
            }

            auto str = ::Standard::Algorithms::Utilities::w_stream();
            output_value(str, great) << "\n must be greater than \n";
            output_value(str, smaller);

#ifndef __clang__
            Standard::Algorithms::Utilities::print(str, loc);
#endif

            throw_exception(str, message);
        }

        template<class t1, class t2>
        static constexpr void greater(const t1 &great, const t2 &smaller, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            greater<t1, t2>(great, smaller, mess_c_str(message)
#ifndef __clang__
                                                ,
                loc
#endif
            );
        }

        template<class t1, class t2>
        static constexpr void greater_or_equal(const t1 &greateq, const t2 &smaller, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            greater_or_equal<t1, t2>(greateq, smaller, mess_c_str(message)
#ifndef __clang__
                                                           ,
                loc
#endif
            );
        }

        template<class t1, class t2>
        static constexpr void greater_or_equal(const t1 &greateq, const t2 &smaller, const char *message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            if (greateq < smaller)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                output_value(str, greateq) << "\n must be greater than or equal to \n";
                output_value(str, smaller);

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }
        }

        template<class number1, class number2>
        static constexpr void are_equal_with_epsilon(const number1 &one, const number1 &two,
            bool (*compare_function)(const number1 &val_x, const number2 &val_y), const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            assert(!message.empty());

            if constexpr (::Standard::Algorithms::is_debug)
            {
                if (nullptr == compare_function) [[unlikely]]
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "nullptr == compareFunction in are_equal_with_epsilon";

#ifndef __clang__
                    Standard::Algorithms::Utilities::print(str, loc);
#endif

                    throw std::runtime_error(str.str());
                }
            }

            if (!(compare_function(one, two)))
            {
                not_equal_throw_exception<number1, number2>(one, two, message
#ifndef __clang__
                    ,
                    loc
#endif
                );
            }
        }

        [[nodiscard]] inline static constexpr auto are_epsilon_equal(
            const std::pair<floating_t, floating_t> &values, const floating_t epsilon = epsilon_def
#ifndef __clang__
            ,
            [[maybe_unused]] const std::source_location &loc = std::source_location::current()
#endif
                ) -> bool
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                Standard::Algorithms::require_positive(epsilon, "epsilon"
#ifndef __clang__
                    ,
                    loc
#endif
                );
            }

            const auto &one = values.first;
            const auto &two = values.second;

            if (one == two)
            {
                return true;
            }

            const auto absolute_a = ::Standard::Algorithms::fabs(one);
            const auto absolute_b = ::Standard::Algorithms::fabs(two);

            if (const auto result1 = absolute_a <= epsilon && absolute_b <= epsilon; result1)
            {
                return true;
            }

            // const auto infA = is_infinite_ptr(one);
            // const auto infB = is_infinite_ptr(two);
            // if (infA)
            //{
            //   return infB;
            // }

            // It does not work to compare 0 and small number, but it must!
            //
            // const auto maxValue = max(absoluteA, absoluteB);
            // auto result = delta <= maxValue * Epsilon;
            // return result;

            const auto sum = absolute_a + absolute_b;
            if (0 == sum)
            {// todo(p1): check div by 0, overflow.
                return false;
            }

            const auto delta = ::Standard::Algorithms::fabs(one - two);
            const auto div = delta / sum;
            auto result = div <= epsilon;
            return result;
        }

        inline static constexpr void are_equal_with_epsilon(
            floating_t one, floating_t two, const std::string &message, floating_t epsilon = epsilon_def
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            assert(!message.empty());

            if (are_epsilon_equal(std::make_pair(one, two), epsilon
#ifndef __clang__
                    ,
                    loc
#endif
                    ))
            {
                return;
            }

            not_equal_throw_exception<floating_t, floating_t>(one, two, message
#ifndef __clang__
                ,
                loc
#endif
            );
        }

        static constexpr void are_equal_with_epsilon(const pair_like auto &one, const pair_like auto &two,
            const std::string &message, const floating_t epsilon = epsilon_def
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            if (const auto para = std::make_pair(one.first, two.first); !are_epsilon_equal(para, epsilon
#ifndef __clang__
                    ,
                    loc
#endif
                    )) [[unlikely]]
            {
                not_equal_throw_exception<floating_t, floating_t>(one.first, two.first, message + " at first"
#ifndef __clang__
                    ,
                    loc
#endif
                );
            }

            if (const auto para = std::make_pair(one.second, two.second); !are_epsilon_equal(para, epsilon
#ifndef __clang__
                    ,
                    loc
#endif
                    )) [[unlikely]]
            {
                not_equal_throw_exception<floating_t, floating_t>(one.second, two.second, message + " at second"
#ifndef __clang__
                    ,
                    loc
#endif
                );
            }
        }

        template<class a_t, class b_t>
        requires container<a_t> && container<b_t>
        static constexpr void are_equal_with_epsilon(
            const a_t &one, const b_t &two, const std::string &message, const floating_t epsilon = epsilon_def
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            const auto size = one.size();
            are_equal(size, two.size(), message + " size"
#ifndef __clang__
                ,
                loc
#endif
            );

            const auto *const message_c = mess_c_str(message);

            std::size_t index{};

            try
            {
                for (; index < size; ++index)
                {
                    are_equal_with_epsilon(one[index], two[index], message_c, epsilon
#ifndef __clang__
                        ,
                        loc
#endif
                    );
                }
            }
            catch (const std::exception &exc)
            {
                const auto err = "Error at index " + std::to_string(index) + ". " + exc.what();
                throw std::runtime_error(err);
            }
        }

        template<class t1, class t2>
        static constexpr void not_equal(const t1 &one, const t2 &two, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            not_equal<t1, t2>(one, two, mess_c_str(message)
#ifndef __clang__
                                            ,
                loc
#endif
            );
        }

        template<class t1, class t2>
        static constexpr void not_equal(const t1 &one, const t2 &two, const char *message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            if (one == two)
            {
                equal_throw_exception<t1, t2>(one, two, message
#ifndef __clang__
                    ,
                    loc
#endif
                );
            }
        }

        template<class t_item>
        static constexpr void empty(const std::vector<t_item> &data, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            if (message.empty()) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The 'message' must be not empty string.";

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw std::runtime_error(str.str());
            }

            if (!data.empty())
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The actual result must be empty. First item:\n";
                output_value(str, data[0]);

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }
        }

        template<class t_item>
        static constexpr void not_empty(const std::vector<t_item> &data, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            if (message.empty()) [[unlikely]]
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The 'message' must be not empty string.";
#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw std::runtime_error(str.str());
            }

            if (data.empty())
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "The actual result must be not empty.";

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }
        }

        template<class t1, class t2>
        static constexpr void are_equal(const std::vector<std::vector<t1>> &expected_vector,
            const std::vector<std::vector<t2>> &actual_vector, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            are_equal<t1, t2>(expected_vector, actual_vector, mess_c_str(message)
#ifndef __clang__
                                                                  ,
                loc
#endif
            );
        }

        template<class t1, class t2>
        static constexpr void are_equal(const std::vector<std::vector<t1>> &expected_vector,
            const std::vector<std::vector<t2>> &actual_vector, const char *const message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            const auto expected_size = expected_vector.size();
            const auto actual_size = actual_vector.size();

            if (expected_size != actual_size)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Expect \n" << expected_size << " item(s), but got \n" << actual_size << " item(s).";

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }

            std::size_t index{};
            try
            {
                for (; index < expected_size; ++index)
                {
                    const auto &expected_line = expected_vector[index];
                    const auto &actual_line = actual_vector[index];

                    are_equal<t1, t2>(expected_line, actual_line, message
#ifndef __clang__
                        ,
                        loc
#endif
                    );
                }
            }
            catch (const std::exception &exc)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Vectors of vectors mismatch at index " << index << ". Error: " << exc.what();

                throw_exception(str);
            }
        }

        inline static constexpr void are_equal(
            const std::vector<bool> &expected_vector, const std::vector<bool> &actual_vector, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            are_equal(expected_vector, actual_vector, mess_c_str(message)
#ifndef __clang__
                                                          ,
                loc
#endif
            );
        }

        template<class t1, class t2>
        static constexpr void are_equal(
            const std::vector<t1> &expected_vector, const std::vector<t2> &actual_vector, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            are_equal<t1, t2>(expected_vector, actual_vector, mess_c_str(message)
#ifndef __clang__
                                                                  ,
                loc
#endif
            );
        }

        template<class t1, class t2>
        static constexpr void are_equal(
            const std::vector<t1> &expected_vector, const std::vector<t2> &actual_vector, const char *const message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            const t1 *expected = expected_vector.data();
            const auto expected_size = expected_vector.size();

            const t2 *actual = actual_vector.data();
            const auto actual_size = actual_vector.size();

            are_equal<t1, t2>(expected, expected_size, actual, actual_size, message
#ifndef __clang__
                ,
                loc
#endif
            );
        }

        template<class t1, class t2>
        static constexpr void are_equal(const t1 *expected, const std::size_t expected_size,
            const std::vector<t2> &actual_vector, const char *const message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            const t2 *actual = actual_vector.data();
            const auto actual_size = actual_vector.size();

            are_equal<t1, t2>(expected, expected_size, actual, actual_size, message
#ifndef __clang__
                ,
                loc
#endif
            );
        }

        template<class t1, class t2>
        static constexpr void are_equal(
            const t1 *expected, const std::size_t expected_size, const t2 *actual, const std::size_t actual_size,
            // Can be nullptr, but not recommended.
            const char *const message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            are_equal<t1, t2>(expected, expected_size, actual, actual_size, message, compare_ptr<t1, t2>
#ifndef __clang__
                ,
                loc
#endif
            );
        }

        template<class t1, class t2, class comparer_t>
        static constexpr void are_equal(const std::vector<t1> &expected_vector, const std::vector<t2> &actual_vector,
            // Can be nullptr, but not recommended.
            const char *const message, comparer_t comparer
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            const t1 *expected = expected_vector.data();
            const auto expected_size = expected_vector.size();

            const t2 *actual = actual_vector.data();
            const auto actual_size = actual_vector.size();

            are_equal<t1, t2, comparer_t>(expected, expected_size, actual, actual_size, message, comparer
#ifndef __clang__
                ,
                loc
#endif
            );
        }

        // The original one for arrays.
        //
        // To solve the error C2679: binary '<<' :
        //  no operator found which takes one right-hand operand of type
        //  please implement the "operator<<".
        template<class t1, class t2, class comparer_t>
        static constexpr void are_equal(
            const t1 *expected, const std::size_t expected_size, const t2 *actual, const std::size_t actual_size,
            // Can be nullptr, but not recommended.
            const char *const message, comparer_t comparer
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            throw_if_null("comparer", comparer
#ifndef __clang__
                ,
                loc
#endif
            );

            if (expected_size == actual_size &&
                (0U == expected_size || static_cast<const void *>(expected) == static_cast<const void *>(actual)))
            {
                return;
            }

            if (nullptr == expected || 0U == expected_size)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();

                str << "Empty result is 'expected', but got " << (nullptr == actual ? 0U : actual_size)
                    << " item(s). actualSize " << actual_size << ".";

                if (0U < actual_size && nullptr != actual)
                {
                    str << " The first actual item is:\n";
                    const auto &item2 = *actual;
                    output_value(str, item2);
                }

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }

            if (expected_size != actual_size || nullptr == actual)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();

                str << "Expect \n"
                    << expected_size << " item(s), but got \n"
                    << (nullptr == actual ? 0U : actual_size) << " item(s). actualSize " << actual_size << ".";

                if (0U == actual_size)
                {
                    str << " The first item must be:\n";
                    const auto &item1 = *expected;
                    output_value(str, item1);
                }

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }

            assert(nullptr != expected && nullptr != actual && 0U < expected_size && expected_size == actual_size);

            const auto expected_span = std::span(expected, expected_size);
            const auto actual_span = std::span(actual, actual_size);

            are_equal_details<comparer_t>(expected_span, actual_span, message, comparer
#ifndef __clang__
                ,
                loc
#endif
            );
        }

        inline static constexpr void are_equal(
            const std::vector<bool> &expected, const std::vector<bool> &actual, const char *const message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            const auto expected_size = expected.size();
            const auto actual_size = actual.size();

            if (0U == expected_size)
            {
                if (expected_size == actual_size)
                {
                    return;
                }

                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Empty result is 'expected', but actually got " << actual_size << " item(s).";

                str << " The first actual item is:\n";
                output_value(str, actual[0]);

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }

            if (expected_size != actual_size)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Expect \n" << expected_size << " item(s), but actually got \n" << actual_size << " item(s).";

                if (0U == actual_size)
                {
                    str << " The first item must be:\n";
                    output_value(str, expected[0]);
                }

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }

            for (std::size_t index{}; index < expected_size; ++index)
            {
                // todo. p2. make eq faster by 64 bit compares.
                if (expected[index] != actual[index])
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();

                    output_value(str, expected[index]) << "\n is expected, but actual is \n";
                    output_value(str, actual[index]) << "\n at position " << index << ".";

#ifndef __clang__
                    Standard::Algorithms::Utilities::print(str, loc);
#endif

                    throw_exception(str, message);
                }
            }
        }

        template<class c1, class c2>
        static constexpr void are_equals(
            const std::set<c1> &expected, const std::set<c2> &actual, const std::string &message
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            const auto expected_size = expected.size();
            const auto actual_size = actual.size();

            if (0U == expected_size)
            {
                if (expected_size == actual_size)
                {
                    return;
                }

                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "Empty result is 'expected', but actually got " << actual_size << " item(s).";

                str << " The first actual item is:\n";
                output_value(str, *actual.cbegin());

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }

            if (expected_size != actual_size)
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();

                str << "Expect \n" << expected_size << " item(s), but actually got \n" << actual_size << " item(s).";

                if (0U == actual_size)
                {
                    str << " The first item must be:\n";
                    output_value(str, *expected.cbegin());
                }

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }

            auto it_expected = expected.cbegin();
            auto it_actual = actual.cbegin();
            std::size_t index{};

            for (;;)
            {
                are_equal(*it_expected, *it_actual, message + " at " + std::to_string(index)
#ifndef __clang__
                                                        ,
                    loc
#endif
                );

                if (++it_expected == expected.cend())
                {
                    break;
                }

                ++it_actual, ++index;
            }
        }

private:
        template<class comparer_t, class t1, class t2>
        static constexpr void are_equal_details(
            const std::span<t1> expected, const std::span<t2> actual, const char *const message, comparer_t comparer
#ifndef __clang__
            ,
            const std::source_location &loc = std::source_location::current()
#endif
        )
        {
            assert(comparer && !expected.empty() && expected.size() == actual.size() && nullptr != expected.data() &&
                nullptr != actual.data());

            assert(static_cast<const void *>(expected.data()) != static_cast<const void *>(actual.data()));

            for (std::size_t index{}; index < expected.size(); ++index)
            {
                const auto &one = expected[index];
                const auto &two = actual[index];
                if (comparer(&one, &two))
                {
                    continue;
                }

                auto str = ::Standard::Algorithms::Utilities::w_stream();

                output_value(str, one) << "\n is expected, but actual is \n";
                output_value(str, two) << "\n at position " << index << ".";

#ifndef __clang__
                Standard::Algorithms::Utilities::print(str, loc);
#endif

                throw_exception(str, message);
            }
        }

        static constexpr const auto epsilon_def = Standard::Algorithms::epsilon_default();

        static_assert(0.0 < epsilon_def && epsilon_def < 1.0);
    };
} // namespace Standard::Algorithms
