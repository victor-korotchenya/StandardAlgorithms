#include <mutex>
#include "eratosthenes_sieve.h"
#include "eratosthenes_sieve_cache.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using value_t = pair<vector<int>, vector<int>>;
    shared_ptr<value_t> sieve_p;

    mutex mutex_factorials;
}

shared_ptr<value_t> Privet::Algorithms::Numbers::eratosthenes_sieve_cache(int n)
{
    constexpr auto n_min = 1009;
    if (n < n_min)
        n = n_min;

    {
        auto s_p = sieve_p;
        const auto p = s_p.get();
        if (p && p->first.back() >= n)
            return s_p;
    }

    lock_guard<mutex> lock(mutex_factorials);
    auto s_p = sieve_p;
    auto p = s_p.get();
    if (!p || p->first.back() < n)
    {
        s_p = make_shared<value_t>(eratosthenes_sieve<int64_t>(n));
        sieve_p = s_p;
    }

    return s_p;
}