// clang-format off
/*
    Copyright (c) 2005-2021 Intel Corporation

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

#ifndef __TBB_monitor_mutex_H
#define __TBB_monitor_mutex_H

#include "third_party/tbb/detail/_utils.hh"
#include "third_party/tbb/detail/_aligned_space.hh"
#include "third_party/tbb/semaphore.hh"

#include "third_party/libcxx/mutex"

namespace tbb {
namespace detail {
namespace r1 {

class concurrent_monitor_mutex {
public:
    using scoped_lock = std::lock_guard<concurrent_monitor_mutex>;

    constexpr concurrent_monitor_mutex() {}

    ~concurrent_monitor_mutex() = default;

    void destroy() {
#if !__TBB_USE_FUTEX
        if (my_init_flag.load(std::memory_order_relaxed)) {
            get_semaphore().~semaphore();
        }
#endif
    }

    void lock() {
        auto wakeup_condition = [&] {
            return my_flag.load(std::memory_order_relaxed) == 0;
        };

        while (my_flag.exchange(1)) {
            if (!timed_spin_wait_until(wakeup_condition)) {
                ++my_waiters;
                while (!wakeup_condition()) {
                    wait();
                }
                --my_waiters;
            }
        }
    }

    void unlock() {
        my_flag.exchange(0); // full fence, so the next load is relaxed
        if (my_waiters.load(std::memory_order_relaxed)) {
            wakeup();
        }
    }

private:
    void wait() {
#if __TBB_USE_FUTEX
        futex_wait(&my_flag, 1);
#else
        get_semaphore().P();
#endif
    }

    void wakeup() {
#if __TBB_USE_FUTEX
        futex_wakeup_one(&my_flag);
#else
        get_semaphore().V();
#endif
    }

    // The flag should be int for the futex operations
    std::atomic<int> my_flag{0};
    std::atomic<int> my_waiters{0};

#if !__TBB_USE_FUTEX
    semaphore& get_semaphore() {
        if (!my_init_flag.load(std::memory_order_acquire)) {
            std::lock_guard<std::mutex> lock(my_init_mutex);
            if (!my_init_flag.load(std::memory_order_relaxed)) {
                new (my_semaphore.begin()) semaphore();
                my_init_flag.store(true, std::memory_order_release);
            }
        }

        return *my_semaphore.begin();
    }

    static std::mutex my_init_mutex;
    std::atomic<bool> my_init_flag{false};
    aligned_space<semaphore> my_semaphore{};
#endif
};

} // namespace r1
} // namespace detail
} // namespace tbb

#endif // __TBB_monitor_mutex_H
