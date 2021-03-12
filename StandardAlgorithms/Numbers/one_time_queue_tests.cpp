#include "../Assert.h"
#include "one_time_queue.h"
#include "one_time_queue_tests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers;
using namespace Privet::Algorithms;

namespace
{
    using q_t = one_time_queue<int, size_t>;

    void simple_q_test(q_t& q, const string message)
    {
        constexpr auto val = 10;
        q.push(val);

        const auto a = q.top();
        q.pop();

        Assert::AreEqual(val, a, "dequeued item, " + message);
        Assert::AreEqual(0u, q.size(), "size, " + message);

        auto lambda = [&]() -> void {
            q.pop();
        };
        Assert::ExpectException<runtime_error>(lambda,
            "The one_time_queue is empty at pop.",
            "Dequeuing an empty queue, " + message);
    }
}

void Privet::Algorithms::Numbers::Tests::one_time_queue_tests()
{
    q_t q(1);
    simple_q_test(q, "initial");

    q.reset();
    simple_q_test(q, "after reset");
}