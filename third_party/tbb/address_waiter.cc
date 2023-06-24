// clang-format off
/*
    Copyright (c) 2021 Intel Corporation

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "third_party/tbb/detail/_utils.h"
#include "third_party/tbb/governor.h"
#include "third_party/tbb/concurrent_monitor.h"
#include "third_party/tbb/detail/_waitable_atomic.h"

#include "third_party/libcxx/type_traits"

namespace tbb {
namespace detail {
namespace r1 {

struct address_context {
    address_context() = default;

    address_context(void* address, std::uintptr_t context) :
        my_address(address), my_context(context)
    {}

    void* my_address{nullptr};
    std::uintptr_t my_context{0};
};

class address_waiter : public concurrent_monitor_base<address_context> {
    using base_type = concurrent_monitor_base<address_context>;
public:
    using base_type::base_type;
    /** per-thread descriptor for concurrent_monitor */
    using thread_context = sleep_node<address_context>;
};

// 1024 is a rough estimate based on two assumptions:
//   1) there are no more than 1000 threads in the application;
//   2) the mutexes are optimized for short critical sections less than a couple of microseconds,
//      which is less than 1/1000 of a time slice.
// In the worst case, we have single mutex that is locked and its thread is preempted.
// Therefore, the probability of a collision while taking unrelated mutex is about 1/size of a table.
static constexpr std::size_t num_address_waiters = 2 << 10;
static_assert(std::is_standard_layout<address_waiter>::value,
              "address_waiter must be with standard layout");
static address_waiter address_waiter_table[num_address_waiters];

void clear_address_waiter_table() {
    for (std::size_t i = 0; i < num_address_waiters; ++i) {
        address_waiter_table[i].destroy();
    }
}

static address_waiter& get_address_waiter(void* address) {
    std::uintptr_t tag = std::uintptr_t(address);
    return address_waiter_table[((tag >> 5) ^ tag) % num_address_waiters];
}

void wait_on_address(void* address, d1::delegate_base& predicate, std::uintptr_t context) {
    address_waiter& waiter = get_address_waiter(address);
    waiter.wait<address_waiter::thread_context>(predicate, address_context{address, context});
}

void notify_by_address(void* address, std::uintptr_t target_context) {
    address_waiter& waiter = get_address_waiter(address);

    auto predicate = [address, target_context] (address_context ctx) {
        return ctx.my_address == address && ctx.my_context == target_context;
    };

    waiter.notify_relaxed(predicate);
}

void notify_by_address_one(void* address) {
    address_waiter& waiter = get_address_waiter(address);

    auto predicate = [address] (address_context ctx) {
        return ctx.my_address == address;
    };

    waiter.notify_one_relaxed(predicate);
}

void notify_by_address_all(void* address) {
    address_waiter& waiter = get_address_waiter(address);

    auto predicate = [address] (address_context ctx) {
        return ctx.my_address == address;
    };

    waiter.notify_relaxed(predicate);
}

} // namespace r1
} // namespace detail
} // namespace tbb
