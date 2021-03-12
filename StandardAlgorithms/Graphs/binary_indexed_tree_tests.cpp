#include"binary_indexed_tree_tests.h"
#include"../Numbers/standard_operations.h"
#include"../Utilities/test_utilities.h"
#include"../Utilities/w_stream.h"
#include"binary_indexed_tree.h"
#include<algorithm>
#include<tuple>
#include<vector>

namespace
{
    using int_t = std::int64_t;
    using operation_t = int_t (*)(const int_t &, const int_t &);
    using tree_t = Standard::Algorithms::Trees::binary_indexed_tree<int_t, operation_t>;

    // leftIndex, rightIndex, before, after.
    using left_right_before_after_t = std::tuple<std::size_t, std::size_t, int_t, int_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr test_case(std::string &&name, operation_t operation,
            std::vector<left_right_before_after_t> &&requests, operation_t reverse_operation, int_t zero = {}) noexcept
            : base_test_case(std::move(name))
            , Requests(std::move(requests))
            , Operation(operation)
            , Reverse_operation(reverse_operation)
            , Zero(zero)
        {
        }

        [[nodiscard]] constexpr auto requests() const &noexcept -> const std::vector<left_right_before_after_t> &
        {
            return Requests;
        }

        [[nodiscard]] constexpr auto operation() const &noexcept -> const operation_t &
        {
            return Operation;
        }

        [[nodiscard]] constexpr auto reverse_operation() const &noexcept -> const operation_t &
        {
            return Reverse_operation;
        }

        [[nodiscard]] constexpr auto zero() const &noexcept -> const int_t &
        {
            return Zero;
        }

        void print(std::ostream &str) const override
        {
            str << ", Zero " << Zero;
        }

private:
        std::vector<left_right_before_after_t> Requests;
        operation_t Operation;
        operation_t Reverse_operation;
        int_t Zero;
    };

    constexpr void make_changes(tree_t &tree)
    {
        // NOLINTNEXTLINE
        tree.set(1, 31);

        // NOLINTNEXTLINE
        tree.set(3, 522);

        // NOLINTNEXTLINE
        tree.set(6, 7803);
    }

    constexpr void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr int_t modulo_sum = 5;
        constexpr int_t modulo_prod = 23;

        // Indexes: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12.
        //     Sum: 2, 1, 1, 1,  ,  , 1,  ,  ,   ,   ,  1.
        //     Xor: 0, 1, 1, 1,  ,  , 1,  ,  ,   ,   ,  1.
        //  SumMod: 2, 1, 1, 1,  ,  , 1,  ,  ,   ,   ,  1.
        // ProdMod: 1, 1, 1, 1,  ,  , 1,  ,  ,   ,   ,  1.

        // Indexes: 1, 2,  3, 4, 5,   6, 7, 8, 9, 10, 11, 12.
        //    Sum: 33, 1,523, 1,  ,7803, 1,  ,  ,   ,   ,  1.
        //    Xor: 31, 1,523, 1,  ,7803, 1,  ,  ,   ,   ,  1.
        // ProdMod: 8, 1, 16, 1,  ,   6, 1,  ,  ,   ,   ,  1.

        test_cases.emplace_back("Sum", Standard::Algorithms::Numbers::sum_unchecked<int_t>,
            std::vector<left_right_before_after_t>({// NOLINTNEXTLINE
                { 0, 1, 2, 33 }, { 1, 1, 2, 33 }, { 2, 3, 2, 524 }, { 2, 6, 3, 8328 },
                // NOLINTNEXTLINE
                { 5, 5, 0, 0 }, { 5, 6, 0, 7803 }, { 6, 6, 0, 7803 }, { 7, 7, 1, 1 },
                // NOLINTNEXTLINE
                { 1, 12, 7, 8363 } }),
            Standard::Algorithms::Numbers::subtract_unchecked<int_t>);

        test_cases.emplace_back("Xor", Standard::Algorithms::Numbers::xorr<int_t>,
            std::vector<left_right_before_after_t>({// NOLINTNEXTLINE
                { 0, 1, 0, 31 }, { 1, 1, 0, 31 }, { 2, 3, 0, 522 }, { 2, 6, 1, 7280 },
                // NOLINTNEXTLINE
                { 5, 5, 0, 0 }, { 5, 6, 0, 7803 }, { 6, 6, 0, 7803 }, { 7, 7, 1, 1 },
                // NOLINTNEXTLINE
                { 1, 12, 1, 7279 } }),
            Standard::Algorithms::Numbers::xorr<int_t>);

        test_cases.emplace_back("SumModulo", Standard::Algorithms::Numbers::sum_modulo_unchecked<int_t, modulo_sum>,
            std::vector<left_right_before_after_t>({// NOLINTNEXTLINE
                { 0, 1, 2, 3 }, { 1, 1, 2, 3 }, { 2, 3, 2, 4 }, { 2, 6, 3, 3 },
                // NOLINTNEXTLINE
                { 5, 5, 0, 0 }, { 5, 6, 0, 3 }, { 6, 6, 0, 3 }, { 7, 7, 1, 1 },
                // NOLINTNEXTLINE
                { 1, 12, 2, 3 } }),
            Standard::Algorithms::Numbers::subtract_modulo_unchecked<int_t, modulo_sum>);

        test_cases.emplace_back("ProductModulo",
            Standard::Algorithms::Numbers::product_modulo_unchecked<int_t, modulo_prod>,
            std::vector<left_right_before_after_t>({// NOLINTNEXTLINE
                { 0, 1, 1, 8 }, { 1, 1, 1, 8 }, { 2, 3, 1, 16 }, { 2, 6, 1, 4 },
                // NOLINTNEXTLINE
                { 5, 5, 1, 1 }, { 5, 6, 1, 6 }, { 6, 6, 1, 6 }, { 7, 7, 1, 1 },
                // NOLINTNEXTLINE
                { 1, 12, 1, 9 } }),
            Standard::Algorithms::Numbers::divide_modulo_unchecked<int_t, modulo_prod>, 1);
    }

    [[nodiscard]] constexpr auto create_tree(const test_case &test) -> tree_t
    {
        // NOLINTNEXTLINE
        const std::vector<std::size_t> indexesToAdd{ 1Z, 2, 4, 1, 7, 3, 12 };
        const auto max_index = *std::max_element(indexesToAdd.cbegin(), indexesToAdd.cend());

        tree_t result(max_index, test.operation(), test.reverse_operation(), test.zero());

        for (const auto &index : indexesToAdd)
        {
            result.set(index);
        }

        return result;
    }

    void check_index_out_of_range(const tree_t &tree, const std::string &name)
    {
        constexpr auto too_large_index = 123456789;

        const auto max_index = tree.max_index();
        auto str = ::Standard::Algorithms::Utilities::w_stream();
        str << "The index (" << too_large_index << ") must be between 1 and " << std::to_string(max_index) << ".";
        const auto expected_message = str.str();

        const auto *const func_name = static_cast<const char *>(__FUNCTION__);
        const auto message = std::string(func_name) + "_" + name;

        ::Standard::Algorithms::ert::expect_exception<std::out_of_range>(
            expected_message,
            [&]() -> void
            {
                // Ignore the returned value.
                const auto ignor = tree.get({ too_large_index, too_large_index });

                // NOLINTNEXTLINE NOLINT
                if (ignor == 0)
                {
                }
            },
            message);
    }

    constexpr void test_value_at(const tree_t &tree, const std::string &step_name)
    {
        const auto max_index = tree.max_index();
        ::Standard::Algorithms::ert::greater(max_index, tree_t::initial_index, "max_index");

        const auto prefix = step_name + "_at_";

        for (auto index = tree_t::initial_index; index <= max_index; ++index)
        {
            const auto expected = tree.value_at(index);
            const auto actual = tree.get({ index, index });
            const auto name = prefix + std::to_string(index);
            ::Standard::Algorithms::ert::are_equal(expected, actual, name);
        }
    }

    constexpr void check_request(const tree_t &tree, const std::int32_t ide, const left_right_before_after_t &request,
        const std::string &step_name)
    {
        const auto &left = std::get<0>(request);
        const auto &right = std::get<1>(request);
        const auto &expected = ide != 0U ? std::get<3>(request) : std::get<2>(request);

        const std::string separator = ", ";

        const auto name = std::string()
                              .append(::Standard::Algorithms::Utilities::zu_string(left))
                              .append(separator)
                              .append(::Standard::Algorithms::Utilities::zu_string(right))
                              .append(separator)
                              .append(step_name);

        const auto actual = tree.get({ left, right });
        ::Standard::Algorithms::ert::are_equal(expected, actual, name);
    }

    constexpr void check_requests(const tree_t &tree, const std::vector<left_right_before_after_t> &requests,
        const std::int32_t ide, const std::string &step_name)
    {
        ::Standard::Algorithms::ert::not_empty(requests, step_name);

        for (const auto &request : requests)
        {
            check_request(tree, ide, request, step_name);
        }

        test_value_at(tree, step_name);
    }

    void run_test_case(const test_case &test)
    {
        auto tree = create_tree(test);
        check_index_out_of_range(tree, "initial tree");
        check_requests(tree, test.requests(), 0, "before changes");

        make_changes(tree);
        check_requests(tree, test.requests(), 1, "after changes");
    }
} // namespace

void Standard::Algorithms::Trees::Tests::binary_indexed_tree_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
