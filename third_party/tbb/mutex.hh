// clang-format off
/*
    Copyright (c) 2021-2023 Intel Corporation

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

#ifndef __TBB_mutex_H
#define __TBB_mutex_H

#include "third_party/tbb/detail/_namespace_injection.hh"
#include "third_party/tbb/detail/_utils.hh"
#include "third_party/tbb/detail/_scoped_lock.hh"
#include "third_party/tbb/detail/_waitable_atomic.hh"
#include "third_party/tbb/detail/_mutex_common.hh"
#include "third_party/tbb/profiling.hh"

namespace tbb {
namespace detail {
namespace d1 {

class mutex {
public:
    //! Constructors
    mutex() {
        create_itt_sync(this, "tbb::mutex", "");
    };

    //! Destructor
    ~mutex() = default;

    //! No Copy
    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    using scoped_lock = unique_scoped_lock<mutex>;

    //! Mutex traits
    static constexpr bool is_rw_mutex = false;
    static constexpr bool is_recursive_mutex = false;
    static constexpr bool is_fair_mutex = false;

    //! Acquire lock
    /** Spin if the lock is taken */
    void lock() {
        call_itt_notify(prepare, this);
        while (!try_lock()) {
            my_flag.wait(true, /* context = */ 0, std::memory_order_relaxed);
        }
    }

    //! Try acquiring lock (non-blocking)
    /** Return true if lock acquired; false otherwise. */
    bool try_lock() {
        bool result = !my_flag.load(std::memory_order_relaxed) && !my_flag.exchange(true);
        if (result) {
            call_itt_notify(acquired, this);
        }
        return result;
    }

    //! Release lock
    void unlock() {
        call_itt_notify(releasing, this);
        // We need Write Read memory barrier before notify that reads the waiter list.
        // In C++ only full fence covers this type of barrier.
        my_flag.exchange(false);
        my_flag.notify_one_relaxed();
    }

private:
    waitable_atomic<bool> my_flag{0};
}; // class mutex

} // namespace d1
} // namespace detail

inline namespace v1 {
using detail::d1::mutex;
} // namespace v1

} // namespace tbb

#endif // __TBB_mutex_H
