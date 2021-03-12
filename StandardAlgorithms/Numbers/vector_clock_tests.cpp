#include <iostream>
#include <memory>
#include <numeric>
#include <thread>
#include "../Assert.h"
#include "../Utilities/Random.h"
#include "vector_clock.h"
#include "vector_clock_tests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using vc_t = vector_clock<int>;

    void check_clock(const vc_t& clock, int n)
    {
        const auto id = clock.get_id();
        const auto times = clock.get_times();

        assert(n == static_cast<int>(times.size()) && n);

        const auto prefix = "times[" + to_string(id) + "][";
        const auto max_time = (n - 1) << 1;
        for (auto i = 0; i < n; ++i)
        {
            const auto name = prefix + to_string(i) + "]";
            if (i == id)
            {
                Assert::AreEqual(times[i], max_time, name);
            }
            else
            {
                Assert::Greater(times[i], 0, name);
                Assert::GreaterOrEqual(max_time, times[i], name);
            }
        }
    }

    void check_clocks(const vector<unique_ptr<vc_t>>& clocks)
    {
        const auto n = static_cast<int>(clocks.size());
        assert(n);

        for (auto i = 0; i < n; ++i)
        {
            const auto clock = clocks[i].get();
            assert(clock && clock->get_id() == i);

            check_clock(*clock, n);
        }
    }

    void launch_clock_thread(vector<unique_ptr<vc_t>>& clocks,
        vector<thread>& threads, const int k, bool& has_error)
    {
        assert(clocks.size() == threads.size());

        auto func = [k, &clocks, &has_error]()-> void
        {
            try
            {
                vector<int> ids(clocks.size());
                iota(ids.begin(), ids.end(), 0);
                ShuffleArray(ids);

                auto clock_source = clocks[k].get();
                assert(clock_source);
                for (const auto& id : ids)
                {
                    if (k == id)
                        continue;

                    const auto sent_times = clock_source->send(id);
                    auto clock_target = clocks[id].get();
                    assert(clock_target);
                    clock_target->receive(sent_times, k);
                }
            }
            catch (const exception& e)
            {
                has_error = true;
                try
                {
                    const auto err = "Error in launch_clock_thread("s + to_string(k) + "): "s + e.what();
                    cout << err;
                }
                catch (...)
                {//Nothing can be done.
                }
            }
            catch (...)
            {
                has_error = true;
            }
        };

        threads[k] = std::thread(func);
    }
}

void Privet::Algorithms::Numbers::Tests::vector_clock_tests()
{
    IntRandom r;
    const auto n = r(2, 16);

    vector<unique_ptr<vc_t>> clocks(n);
    for (auto i = 0; i < n; ++i)
        clocks[i].reset(new vc_t(n, i));

    auto has_error = false;
    vector<thread> threads(n);
    for (auto i = 0; i < n; ++i)
        launch_clock_thread(clocks, threads, i, has_error);

    for (auto i = 0; i < n; ++i)
        threads[i].join();

    threads.clear();

    check_clocks(clocks);
    Assert::AreEqual(false, has_error, "has_error");
}