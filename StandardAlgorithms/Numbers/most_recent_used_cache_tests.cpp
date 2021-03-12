#include "../Utilities/Random.h"
#include "../Utilities/PrintUtilities.h"
#include "../test_utilities.h"
#include "most_recent_used_cache.h"
#include "most_recent_used_cache_tests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms::Tests;
using namespace Privet::Algorithms;

namespace
{
    using key_t = int16_t;
    using value_t = int64_t;
    constexpr auto size_max = 3, not_computed = -1;

    struct oper_t final
    {
        value_t val{};
        key_t k{};

        union
        {
            unsigned size{};
            signed char expected;
        };

        bool is_add{};
    };

    oper_t add_op(key_t k, value_t val, unsigned size = {})
    {
        oper_t op;
        op.k = k;
        op.val = val;
        op.size = size;
        op.is_add = true;
        return op;
    }

    oper_t find_op(key_t k, signed expected = {}, value_t val = {})
    {
        oper_t op;
        op.k = k;
        op.val = val;
        op.expected = static_cast<signed char>(expected);
        op.is_add = false;
        return op;
    }

    ostream& operator <<(ostream& str, oper_t const& op)
    {
        if (op.is_add)
            str << "Add {" << op.k << " " << op.val << "} size " << op.size;
        else
            str << "Find {" << op.k << "} is " << op.expected << " " << op.val;

        return str;
    }

    struct test_case final : base_test_case
    {
        vector<oper_t> data;

        test_case(string&& name,
            vector<oper_t>&& data)
            : base_test_case(forward<string>(name)),
            data(forward<vector<oper_t>>(data))
        {
        }

        void Validate() const override
        {
            base_test_case::Validate();

            RequirePositive(data.size(), "data size.");
        }

        void Print(ostream& str) const override
        {
            base_test_case::Print(str);
            Privet::Algorithms::Print("data", data, str);
        }
    };

    void generate_test_cases(vector<test_case>& test_cases)
    {
        constexpr auto k1 = 10, v1 = 100,
            k2 = 20, v2 = 2000,
            k3 = 30, v3 = 30000,
            k4 = 40, v4 = 400000, v42 = 5000000;

        test_cases.push_back({
            "simple",
            vector<oper_t>{
                find_op(k1),
                find_op(k2),
                add_op(k1, v1, 1),
                find_op(k1, true, v1),
                add_op(k1, v1, 1),//2nd attempt
                find_op(k1, true, v1),
                add_op(k2, v2, 2),
                add_op(k3, v3, size_max),//max size reached
                find_op(k1, true, v1),
                find_op(k2, true, v2),
                find_op(k3, true, v3),
                add_op(k4, v4, size_max), // evict 1
                find_op(k1),
                find_op(k4, true, v4),
                find_op(k2, true, v2),
                find_op(k3, true, v3),
                add_op(k4, v42, size_max),
                find_op(k3, true, v3),
                find_op(k4, true, v42),
                find_op(k2, true, v2),
            }
            });

        vector<oper_t> ops;
        ShortRandom r;
        for (auto att = 0, att_max = 1; att < att_max; ++att)
        {
            ops.resize(15);

            for (auto& op : ops)
            {
                const auto key = r(1, 20);
                if (r() & 1)
                    op = find_op(key, not_computed);
                else
                {
                    const auto value = r(1, 200);
                    op = add_op(key, value);
                }
            }

            test_cases.push_back({ "random" + to_string(att), move(ops) });
        }
    }

    template<class cache_t>
    void run_add(const oper_t& op, cache_t& cache, const char* const name,
        const size_t expected, const bool is_computed = true)
    {
        cache.add(op.k, op.val);
        if (!is_computed)
            return;

        const auto size = cache.size();
        Assert::AreEqual(expected, size, name);
    }

    template<class cache_t>
    pair<bool, value_t> run_find(const oper_t& op, cache_t& cache, const char* const name,
        const pair<bool, value_t>& expected, const bool is_computed = true)
    {
        value_t val{};
        const auto actual = cache.find(op.k, val);
        if (is_computed)
        {
            Assert::AreEqual(expected.first, actual, name);
            if (actual)
                Assert::AreEqual(expected.second, val, string("value ") + name);
        }

        return { actual, val };
    }

    template<class cache_t, class cache_t2>
    void verify_last(const cache_t& cache, const cache_t2& cache2)
    {
        const auto f = cache.front();
        const auto f2 = cache2.front();
        Assert::AreEqual(f, f2, "verify_last");
    }

    void run_test_case(const test_case& test)
    {
        most_recent_used_cache<key_t, value_t> cache(size_max);
        Assert::AreEqual(0, cache.size(), "cache.size");

        most_recent_used_cache_slow<key_t, value_t> cache_slow(size_max);
        Assert::AreEqual(0, cache_slow.size(), "cache_slow.size");

        for (auto i = 0u; i < test.data.size(); ++i)
        {
            const auto& op = test.data[i];
            try
            {
                if (op.is_add)
                {
                    run_add(op, cache, "cache add", op.size, op.size > 0);
                    run_add(op, cache_slow, "cache_slow add", cache.size());
                }
                else
                {
                    const auto f = run_find(op, cache, "cache find", { op.expected > 0, op.val }, op.expected >= 0);
                    run_find(op, cache_slow, "cache_slow find", f);
                }

                if (cache.size())
                    verify_last(cache, cache_slow);
            }
            catch (const exception& ex)
            {
                throw runtime_error(ex.what() + string(" Error at op ") + to_string(i));
            }
        }
    }
}

void Privet::Algorithms::Numbers::Tests::most_recent_used_cache_tests()
{
    test_utilities<test_case>::run_tests(run_test_case, generate_test_cases);
}