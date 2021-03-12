#include"skip_list_tests.h"
#include"../Utilities/ert.h"
#include"skip_list.h"
#include<cmath>
#include<numbers>

namespace
{
    using key_t = std::int32_t;
    using long_key_t = std::int64_t;
    using value_t = Standard::Algorithms::floating_point_type;

    constexpr auto min_key = std::numeric_limits<key_t>::min();
    constexpr auto max_key = std::numeric_limits<key_t>::max();
    constexpr key_t key0 = 1;
    constexpr key_t key1 = 101;
    constexpr key_t key2 = 10378;
    constexpr key_t miss_key = 80;
    constexpr value_t value1 = 204;
    constexpr value_t value2 = 2;
    constexpr value_t value3 = 204.01;
    constexpr value_t value4 = 6543221;
    constexpr value_t value5 = 86742;

    constexpr auto probability = 0.5;
    constexpr auto max_levels = 7;

    using list_t = Standard::Algorithms::Graphs::skip_list<key_t, value_t, max_levels>;

    struct skip_context final
    {
        // [[nodiscard]]  constexpr auto size() const noexcept -> long_key_t { return Size; }

        constexpr void reset() &noexcept
        {
            Size = {};
        }

        constexpr void reset_visit(const char *const name, const list_t &list1, const long_key_t &expect = 0) &
        {
            assert(name != nullptr);

            reset();

            list1.visit(std::ref(*this));

            ::Standard::Algorithms::ert::are_equal(expect, Size, name);
        }

        [[nodiscard]] constexpr auto operator() (const key_t &key, const value_t &value) & -> bool
        {
            // Pacify the compiler.
            // NOLINTNEXTLINE NOLINT
            if (value == value_t{})
            {
            }

            Size += key;

            return true;
        }

private:
        long_key_t Size{};
    };

    constexpr void one_run(skip_context &context, list_t &slist)
    {
        ::Standard::Algorithms::ert::are_equal(0U, slist.size(), "List size after creation.");

        {
            const auto *const fin1 = slist.find(key1);
            ::Standard::Algorithms::ert::is_null_ptr(fin1, "Find data before inserts.");
        }

        slist.add(key0, value2);
        slist.add(key1, value1);
        slist.add(key2, value3);

        constexpr auto size_after_inserts = 3U;
        ::Standard::Algorithms::ert::are_equal(size_after_inserts, slist.size(), "Size after inserts.");

        {
            const auto *const fin2 = slist.find(miss_key);
            ::Standard::Algorithms::ert::is_null_ptr(fin2, "Find not existing data.");
        }

        context.reset_visit("Visit_012.", slist, key0 + static_cast<long_key_t>(key1) + key2);

        {
            const auto *const fin3 = slist.find(key1);
            ::Standard::Algorithms::ert::are_equal_by_ptr(&value1, fin3, "Find existing key1.");
        }

        ::Standard::Algorithms::ert::are_equal(size_after_inserts, slist.size(), "Size after 2nd find.");

        ::Standard::Algorithms::ert::are_equal(true, slist.erase(key1), "is key1 deleted");

        constexpr auto size_after_delete = size_after_inserts - 1U;
        ::Standard::Algorithms::ert::are_equal(size_after_delete, slist.size(), "Size after delete key1.");

        {
            const auto *const already_deleted = slist.find(key1);
            ::Standard::Algorithms::ert::is_null_ptr(already_deleted, "Find the deleted key1.");
        }

        context.reset_visit("Visit_02.", slist, key0 + static_cast<long_key_t>(key2));

        slist.add(min_key, value4);
        ::Standard::Algorithms::ert::are_equal(size_after_delete + 1U, slist.size(), "Size after adding -inf.");

        slist.add(max_key, value5);
        ::Standard::Algorithms::ert::are_equal(size_after_delete + 2U, slist.size(), "Size after adding +inf.");

        context.reset_visit("Visit_02 +inf -inf.", slist, key0 + static_cast<long_key_t>(key2) + min_key + max_key);

        slist.erase(key0);
        slist.erase(key1);
        slist.erase(key2);
        context.reset_visit("Visit +inf -inf  after delete.", slist, -1);

        slist.erase(min_key);
        slist.erase(max_key);
        context.reset_visit("Visit empty after delete all keys.", slist);

        slist.clear();
        context.reset_visit("Visit empty after clear.", slist);

        Standard::Algorithms::Utilities::random_t<std::int32_t> rnd{};

        constexpr auto steps = 10;

        for (std::int32_t index{}; index < steps; ++index)
        {
            slist.erase(index);

            const auto random_key = rnd();
            constexpr auto same_value_wont_cause_add_problems = std::numbers::pi;

            slist.add(random_key, same_value_wont_cause_add_problems);
        }

        slist.clear();

        context.reset_visit("Visit after last clear.", slist);
        ::Standard::Algorithms::ert::are_equal(0U, slist.size(), "size after clear.");
    }
} // namespace

void Standard::Algorithms::Trees::Tests::skip_list_tests()
{
    constexpr auto max_attempts = 2;

    skip_context context{};

    for (std::int32_t attempt{}; attempt < max_attempts; ++attempt)
    {
        context.reset();

        list_t slist(min_key, probability, max_key);

        try
        {
            one_run(context, slist);
        }
        catch (const std::exception &exc)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "Attempt " << attempt << " error: " << exc.what();
            // todo(p3): log the details.

            throw_exception(str);
        }
    }
}
