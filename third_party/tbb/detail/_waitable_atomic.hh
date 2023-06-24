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

#ifndef __TBB_detail__address_waiters_H
#define __TBB_detail__address_waiters_H

#include "third_party/tbb/detail/_utils.hh"

namespace tbb {
namespace detail {

namespace r1 {
TBB_EXPORT void __TBB_EXPORTED_FUNC wait_on_address(void* address, d1::delegate_base& wakeup_condition, std::uintptr_t context);
TBB_EXPORT void __TBB_EXPORTED_FUNC notify_by_address(void* address, std::uintptr_t context);
TBB_EXPORT void __TBB_EXPORTED_FUNC notify_by_address_one(void* address);
TBB_EXPORT void __TBB_EXPORTED_FUNC notify_by_address_all(void* address);
} // namespace r1

namespace d1 {

template <typename Predicate>
void adaptive_wait_on_address(void* address, Predicate wakeup_condition, std::uintptr_t context) {
    if (!timed_spin_wait_until(wakeup_condition)) {
        d1::delegated_function<Predicate> pred(wakeup_condition);
        r1::wait_on_address(address, pred, context);
    }
}

template <typename T>
class waitable_atomic {
public:
    waitable_atomic() = default;

    explicit waitable_atomic(T value) : my_atomic(value) {}

    waitable_atomic(const waitable_atomic&) = delete;
    waitable_atomic& operator=(const waitable_atomic&) = delete;

    T load(std::memory_order order) const noexcept {
        return my_atomic.load(order);
    }

    T exchange(T desired) noexcept {
        return my_atomic.exchange(desired);
    }

    void wait(T old, std::uintptr_t context, std::memory_order order) {
        auto wakeup_condition = [&] { return my_atomic.load(order) != old; };
        if (!timed_spin_wait_until(wakeup_condition)) {
            // We need to use while here, because notify_all() will wake up all threads
            // But predicate for them might be false
            d1::delegated_function<decltype(wakeup_condition)> pred(wakeup_condition);
            do {
                r1::wait_on_address(this, pred, context);
            } while (!wakeup_condition());
        }
    }

    void wait_until(T expected, std::uintptr_t context, std::memory_order order) {
        auto wakeup_condition = [&] { return my_atomic.load(order) == expected; };
        if (!timed_spin_wait_until(wakeup_condition)) {
            // We need to use while here, because notify_all() will wake up all threads
            // But predicate for them might be false
            d1::delegated_function<decltype(wakeup_condition)> pred(wakeup_condition);
            do {
                r1::wait_on_address(this, pred, context);
            } while (!wakeup_condition());
        }
    }

    void notify_relaxed(std::uintptr_t context) {
        r1::notify_by_address(this, context);
    }

    void notify_one_relaxed() {
        r1::notify_by_address_one(this);
    }

    // TODO: consider adding following interfaces:
    // store(desired, memory_order)
    // notify_all_relaxed()

private:
    std::atomic<T> my_atomic{};
};

} // namespace d1
} // namespace detail
} // namespace tbb

#endif // __TBB_detail__address_waiters_H
