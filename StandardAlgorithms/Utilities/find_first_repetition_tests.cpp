#include"find_first_repetition_tests.h"
#include"ert.h"
#include"find_first_repetition.h"

namespace
{
    using id_t = std::uint32_t;

    template<class item_t>
    constexpr void append_many(const std::size_t size, std::vector<item_t> &result, const std::size_t added_value = {})
    {
        result.reserve(result.size() + size);

        for (std::size_t index{}; index < size; ++index)
        {
            result.push_back(static_cast<item_t>(index + added_value));
        }
    }

    template<class full_type_t, class type_t = full_type_t,
        class get_value_func_t = decltype([] [[nodiscard]] (const full_type_t &val) -> type_t
        {
            return static_cast<type_t>(val);
        })>
#ifndef __clang__
    [[nodiscard]]
#endif
   constexpr auto test_inner(const std::string &name, get_value_func_t get_value_func = {}) -> bool
    {
        constexpr std::size_t size = 10U;
        constexpr auto throw_on_exception = false;

        std::vector<full_type_t> data;
        append_many<full_type_t>(size, data);

        auto first_repeat_index = size << 1U;
        {
            using iterator_t = typename std::vector<full_type_t>::const_iterator;

            const auto actual =
                Standard::Algorithms::Utilities::find_first_repetition<type_t, iterator_t, get_value_func_t>(
                    data.cbegin(), first_repeat_index, data.cend(), name, throw_on_exception, get_value_func);

            ::Standard::Algorithms::ert::are_equal(false, actual, name + " none");
        }

        {
            constexpr std::size_t item_index = 3;
            data.push_back(data.at(item_index));
        }

        constexpr std::size_t size_2 = 7;
        constexpr std::size_t added_value = 305;

        const auto expected_index = data.size() - 1ZU;

        append_many<full_type_t>(size_2, data, added_value);
        {
            using iterator_t = const full_type_t *;

            iterator_t data_ptr = data.data();

            const auto actual =
                Standard::Algorithms::Utilities::find_first_repetition<type_t, iterator_t, get_value_func_t>(
                    data_ptr, first_repeat_index, data_ptr + data.size(), name, throw_on_exception, get_value_func);

            ::Standard::Algorithms::ert::are_equal(true, actual, name + " repeat result");
            ::Standard::Algorithms::ert::are_equal(expected_index, first_repeat_index, name + " repeat index");
        }

        return true;
    }

    struct some_struct final
    {
        constexpr explicit some_struct(id_t ide = {}) noexcept
            : Ide(ide)
        {
        }

        [[nodiscard]] constexpr auto id() const &noexcept -> const id_t &
        {
            return Ide;
        }

        [[nodiscard]] inline constexpr auto operator() (const some_struct &some) const noexcept -> std::size_t
        {
            return static_cast<std::size_t>(some.Ide);
        }

private:
        id_t Ide;
    };

    [[nodiscard]] constexpr auto get_value(const some_struct &instance) noexcept -> id_t
    {
        return instance.id();
    }
} // namespace

void Standard::Algorithms::Utilities::Tests::find_first_repetition_tests()
{
#ifndef __clang__
    static_assert
#endif
        (test_inner<std::int16_t>("Short signed"));

    {
        using get_value_func_t = id_t (*)(const some_struct &);

#ifndef __clang__
        static_assert
#endif
            (test_inner<some_struct, id_t, get_value_func_t>("Some structure", get_value));
    }
}
