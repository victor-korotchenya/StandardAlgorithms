#include"most_recent_used_cache_tests.h"
#include"../Utilities/make_optional2.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"most_recent_used_cache.h"

namespace
{
    using key_t = std::int16_t;
    using value_t = std::int64_t;
    using expected_t = std::int32_t;

    constexpr auto max_size = 3;
    constexpr auto not_computed = -1;

    // Only 1 union member can be initialized.
    // NOLINTNEXTLINE
    struct oper_t final
    {
        key_t key{};
        value_t value{};

        union
        {
            std::uint32_t size{};

            // Only 1 union member can be initialized.
            // NOLINTNEXTLINE
            expected_t expected;
        };

        bool is_add{};
    };

    [[nodiscard]] constexpr auto add_op(key_t key, value_t value, std::uint32_t size = {}) -> oper_t
    {
        return { .key = key, .value = value, .size = size, .is_add = true };
    }

    [[nodiscard]] constexpr auto move_op(key_t key, expected_t expected = {}, value_t value = {}) -> oper_t
    {
        return { .key = key, .value = value, .expected = expected, .is_add = false };
    }

    auto operator<< (std::ostream &str, const oper_t &opec) -> std::ostream &
    {
        if (opec.is_add)
        {
            str << "Add {" << opec.key << " " << opec.value << "} size " << opec.size;
        }
        else
        {
            str << "Find {" << opec.key << "} is " << opec.expected << " " << opec.value;
        }

        return str;
    }

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        constexpr test_case(std::string &&name, std::vector<oper_t> &&operations) noexcept
            : base_test_case(std::move(name))
            , Operations(std::move(operations))
        {
        }

        [[nodiscard]] constexpr auto operations() const &noexcept -> const std::vector<oper_t> &
        {
            return Operations;
        }

        constexpr void validate() const override
        {
            base_test_case::validate();

            Standard::Algorithms::require_positive(Operations.size(), "Operations size.");
        }

        void print(std::ostream &str) const override
        {
            ::Standard::Algorithms::print("Operations", Operations, str);
        }

private:
        std::vector<oper_t> Operations;
    };

    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        {
            constexpr key_t ke1 = 10;
            constexpr value_t va1 = 100;

            constexpr key_t ke2 = 20;
            constexpr value_t va2 = 2'000;

            constexpr key_t ke3 = 30;
            constexpr value_t va3 = 30'000;

            constexpr key_t ke4 = 40;
            constexpr value_t va4 = 400'000;
            constexpr value_t va42 = 5'000'000;

            test_cases.emplace_back("simple",
                std::vector<oper_t>{ move_op(ke1), move_op(ke2), add_op(ke1, va1, 1), move_op(ke1, 1, va1),
                    add_op(ke1, va1, 1), // 2nd attempt
                    move_op(ke1, 1, va1), add_op(ke2, va2, 2), add_op(ke3, va3, max_size), // max size reached
                    move_op(ke1, 1, va1), move_op(ke2, 1, va2), move_op(ke3, 1, va3),
                    add_op(ke4, va4, max_size), // evict 1
                    move_op(ke1), move_op(ke4, 1, va4), move_op(ke2, 1, va2), move_op(ke3, 1, va3),
                    add_op(ke4, va42, max_size), move_op(ke3, 1, va3), move_op(ke4, 1, va42), move_op(ke2, 1, va2) });
        }

        constexpr key_t min_key = 1;
        constexpr key_t max_key = 20;

        Standard::Algorithms::Utilities::random_t<key_t> rnd(min_key, max_key);

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            constexpr auto op_size = 15;

            std::vector<oper_t> operations(op_size + att);

            for (auto &operation : operations)
            {
                const auto key = rnd();

                if (rnd)
                {
                    operation = move_op(key, not_computed);
                }
                else
                {
                    constexpr value_t min_value = 1;
                    constexpr value_t max_value = 200;

                    const auto value = rnd(min_value, max_value);
                    operation = add_op(key, value);
                }
            }

            test_cases.emplace_back("Random" + std::to_string(att), std::move(operations));
        }
    }

    template<class cache_t>
    constexpr void test_add(const oper_t &operation, cache_t &cache, const char *const name, const std::size_t expected,
        const bool is_computed = true)
    {
        assert(name != nullptr);

        cache.add(operation.key, operation.value);

        if (!is_computed)
        {
            return;
        }

        const auto size = cache.size();
        ::Standard::Algorithms::ert::are_equal(expected, size, name);
    }

    template<class cache_t>
    [[maybe_unused]] constexpr auto test_move(const oper_t &operation, cache_t &cache, const char *const name,
        const std::optional<value_t> &expected, const bool is_computed = true) -> std::optional<value_t>
    {
        assert(name != nullptr);

        value_t value{};
        const auto has_value = cache.move_to_front(operation.key, value);

        if (is_computed)
        {
            ::Standard::Algorithms::ert::are_equal(expected.has_value(), has_value, name);

            if (expected.has_value())
            {
                ::Standard::Algorithms::ert::are_equal(expected.value(), value, std::string("value ") + name);
            }
        }

        return ::Standard::Algorithms::Utilities::make_optional2(has_value, value);
    }

    template<class cache_t, class cache_t2>
    constexpr void verify_last(const cache_t &cache, const cache_t2 &cache2)
    {
        const auto &front1 = cache.front();
        const auto &front2 = cache2.front();

        ::Standard::Algorithms::ert::are_equal(front1, front2, "verify last");
    }

    void run_test_case(const test_case &test)
    {
        Standard::Algorithms::Numbers::most_recent_used_cache<key_t, value_t> cache(max_size);

        ::Standard::Algorithms::ert::are_equal(0U, cache.size(), "initial cache size");

        Standard::Algorithms::Numbers::most_recent_used_cache_slow<key_t, value_t> cache_slow(max_size);

        ::Standard::Algorithms::ert::are_equal(0U, cache_slow.size(), "initial slow cache size");

        const auto &operations = test.operations();
        const auto size = Standard::Algorithms::require_positive(operations.size(), "operations size");

        for (std::size_t index{}; index < size; ++index)
        {
            const auto &operation = operations[index];

            try
            {
                if (operation.is_add)
                {
                    test_add(operation, cache, "cache add", operation.size, 0U < operation.size);

                    test_add(operation, cache_slow, "cache slow add", cache.size());
                }
                else
                {
                    const auto opt =
                        ::Standard::Algorithms::Utilities::make_optional2(0 < operation.expected, operation.value);

                    const auto fun = test_move(operation, cache, "cache move", opt, 0 <= operation.expected);

                    test_move(operation, cache_slow, "slow cache move", fun);
                }

                if (cache.size() != 0U)
                {
                    verify_last(cache, cache_slow);
                }
            }
            catch (const std::exception &exc)
            {
                auto err = "Error at operation " + std::to_string(index) + ": " + exc.what();

                throw std::runtime_error(err);
            }
        }
    }
} // namespace

void Standard::Algorithms::Numbers::Tests::most_recent_used_cache_tests()
{
    Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}
