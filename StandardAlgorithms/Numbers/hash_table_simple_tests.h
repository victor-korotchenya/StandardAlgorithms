#pragma once
#include"../Utilities/random.h"
#include"../Utilities/stringer.h"
#include"../Utilities/test_utilities.h"
#include<unordered_set>
#include<vector>

namespace Standard::Algorithms::Numbers::Tests::Inner
{
    template<class key_t, class table_t>
    constexpr void simple_insert(
        Standard::Algorithms::Utilities::stringer &stringer, const std::vector<key_t> &data, table_t &table)
    {
        ::Standard::Algorithms::ert::are_equal(0U, table.size(), stringer.reset_append(" size before insert"));

        std::size_t cur_size{};

        for (const auto &datum : data)
        {
            const auto name = std::to_string(datum);
            {
                const auto has = table.contains(datum);

                ::Standard::Algorithms::ert::are_equal(
                    false, has, stringer.reset_append(" before insert inexisting ").append(name));
            }
            {
                const auto has = table.insert(datum);

                ::Standard::Algorithms::ert::are_equal(true, has, stringer.reset_append(" first insert ").append(name));
            }
            {
                const auto has = table.insert(datum);

                ::Standard::Algorithms::ert::are_equal(false, has, stringer.reset_append(" second insert ").append(name));
            }
            {
                const auto has = table.contains(datum);

                ::Standard::Algorithms::ert::are_equal(
                    true, has, stringer.reset_append(" after insert existing ").append(name));
            }

            ++cur_size;

            ::Standard::Algorithms::ert::are_equal(
                cur_size, table.size(), stringer.reset_append(" size after insert ").append(name));
        }
    }

    template<class key_t, class table_t>
    constexpr void simple_erase(
        Standard::Algorithms::Utilities::stringer &stringer, const std::vector<key_t> &data, table_t &table)
    {
        auto cur_size = data.size();

        ::Standard::Algorithms::ert::are_equal(cur_size, table.size(), stringer.reset_append(" size before erase"));

        for (const auto &datum : data)
        {
            const auto name = std::to_string(datum);
            {
                const auto has = table.contains(datum);

                ::Standard::Algorithms::ert::are_equal(
                    true, has, stringer.reset_append(" before erase existing ").append(name));
            }
            {
                const auto has = table.erase(datum);

                ::Standard::Algorithms::ert::are_equal(true, has, stringer.reset_append(" first erase ").append(name));
            }
            {
                const auto has = table.erase(datum);

                ::Standard::Algorithms::ert::are_equal(false, has, stringer.reset_append(" second erase ").append(name));
            }
            {
                const auto has = table.contains(datum);

                ::Standard::Algorithms::ert::are_equal(
                    false, has, stringer.reset_append(" contains after second erase ").append(name));
            }

            --cur_size;

            ::Standard::Algorithms::ert::are_equal(
                cur_size, table.size(), stringer.reset_append(" size after erase ").append(name));
        }
    }

    template<class table_t>
    constexpr void simple_clear(Standard::Algorithms::Utilities::stringer &stringer, table_t &table)
    {
        table.clear();

        ::Standard::Algorithms::ert::are_equal(0U, table.size(), stringer.reset_append(" size after clear"));
    }

    template<std::signed_integral key_t, class table_t>
    requires(sizeof(std::int16_t) <= sizeof(key_t))
    constexpr void plain_hash_table_tests(const std::string &name)
    {
        assert(!name.empty());

        const std::vector<key_t> data{// NOLINTNEXTLINE
            20, 958, 30, 40, 10, 732, -101
        };

        constexpr auto small_value = 3U;

        const auto size1 = data.size() * small_value;

        Standard::Algorithms::Utilities::stringer stringer{};

        for (std::size_t initial_prime{}; initial_prime <= size1; ++initial_prime)
        {
            table_t table(initial_prime);

            constexpr auto total_steps = 2;

            for (std::int32_t step{}; step < total_steps; ++step)
            {
                stringer.initialize(
                    name + " initial " + std::to_string(initial_prime) + ", simple step " + std::to_string(step) + ".");

                Inner::simple_insert(stringer, data, table);
                Inner::simple_erase(stringer, data, table);
                Inner::simple_clear(stringer, table);
            }

            const auto capacity = table.capacity();

            ::Standard::Algorithms::ert::greater(capacity, data.size(), "simple capacity");
        }
    }

    template<class table_t, class random_t, class set_t>
    constexpr void random_hash_table_test_impl(table_t &table, random_t &rnd, set_t &set1, std::string &report)
    {
        assert(!report.empty());

        ::Standard::Algorithms::ert::are_equal(0U, table.size(), "empty table size");

        const auto *const yes = " yes";
        const auto *const nope = " no";

        constexpr auto total_steps = 20;

        for (std::int32_t step{}; step < total_steps; ++step)
        {
            ::Standard::Algorithms::ert::are_equal(set1.size(), table.size(), "table size");

            {
                const auto repka = rnd();
                report += "\nContains ";
                report += std::to_string(repka);

                const auto expected = set1.contains(repka);
                report += expected ? yes : nope;

                const auto actual = table.contains(repka);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "Contains key");
            }
            {
                const auto babka = rnd();
                report += ". Erase ";
                report += std::to_string(babka);

                const auto expected = 0U < set1.erase(babka);
                report += expected ? yes : nope;

                const auto actual = table.erase(babka);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "is erased");
            }
            {
                const auto dedka = rnd();
                report += ". Insert ";
                report += std::to_string(dedka);

                const auto expected = set1.insert(dedka).second;
                report += expected ? yes : nope;

                const auto actual = table.insert(dedka);
                ::Standard::Algorithms::ert::are_equal(expected, actual, "is inserted");
            }
        }

        ::Standard::Algorithms::ert::are_equal(set1.size(), table.size(), "table size");
    }

    template<std::integral key_t, class table_t>
    constexpr void random_hash_table_tests(const std::string &name)
    {
        assert(!name.empty());

        using random_t = Standard::Algorithms::Utilities::random_t<key_t>;
        random_t rnd{};

        std::string report{};
        std::unordered_set<key_t> set1{}; // Let's hope STL is bug free.

        constexpr auto max_attempts = 1;

        for (std::int32_t att{}; att < max_attempts; ++att)
        {
            set1.clear();
            report.clear();
            report += name;
            report += " Start attempt ";
            report += std::to_string(att);

            try
            {
                {
                    table_t table{};
                    report += " Created table.";

                    random_hash_table_test_impl<table_t, random_t>(table, rnd, set1, report);
                    report += "\n Before the destructor call.";
                }
            }
            catch (const std::exception &exc)
            {
                report += "\nGot en error: ";
                report += exc.what();

                throw std::runtime_error(report);
            }
        }
    }
} // namespace Standard::Algorithms::Numbers::Tests::Inner

namespace Standard::Algorithms::Numbers::Tests
{
    template<std::signed_integral key_t, class table_t>
    requires(sizeof(std::int16_t) <= sizeof(key_t))
    constexpr void hash_table_simple_tests(const std::string &name)
    {
        throw_if_empty("hash table test name", name);

        Inner::plain_hash_table_tests<key_t, table_t>(name);

        Inner::random_hash_table_tests<key_t, table_t>(name);
    }
} // namespace Standard::Algorithms::Numbers::Tests
