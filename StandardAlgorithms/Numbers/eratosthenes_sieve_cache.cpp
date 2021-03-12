#include"eratosthenes_sieve_cache.h"
#include"eratosthenes_sieve.h"
#include"prime_number_utility.h"
#include<algorithm>
#include<mutex>

namespace
{
    using value_t = std::pair<std::vector<std::int32_t>, std::vector<std::int32_t>>;

    // In an application, it'd be passed in as an argument. todo(p4): Add another test kind: void(*)(shared_res&).
    // And place all needed shared_ptrs into shared_res.
    // NOLINTNEXTLINE
    std::shared_ptr<const value_t> sieve_shar{};

    // todo(p4): Recheck in C++26. std::atomic_shared_ptr<const value_t> sieve_shar;

    // Needed for the sieve_shar.
    // NOLINTNEXTLINE
    std::mutex mutex_factorials{};

    [[nodiscard]] auto try_sieve(const std::int32_t num) -> std::shared_ptr<const value_t>
    {
        assert(0 < num);

        auto old = sieve_shar;

        const auto *const ptr = old.get();

        if (ptr != nullptr && !ptr->first.empty() && num <= ptr->first.back()) [[likely]]
        {
            return old;
        }

        return {};
    }
} // namespace

[[nodiscard]] auto Standard::Algorithms::Numbers::eratosthenes_sieve_cache(std::int32_t num)
    -> std::shared_ptr<const value_t>
{
    {
        constexpr std::int32_t min_value = 1009;

        static_assert(is_prime_simple(min_value));

        num = std::max(num, min_value);

        if (const auto is_even1 = 0 == num % 2; is_even1)
        {
            ++num;
        }
    }

    // todo(p4): finish in C++26
    //    if (auto old = try_sieve(num);
    //        old)
    //    {
    //        return old;
    //    }

    const std::lock_guard lock(mutex_factorials);

    if (auto old = try_sieve(num); old)
    {
        return old;
    }

    auto new1 = std::make_shared<const value_t>(eratosthenes_sieve<std::int64_t>(num));

    sieve_shar = new1;

    return new1;
}
