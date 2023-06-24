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

#ifndef __TBB_rw_mutex_H
#define __TBB_rw_mutex_H

#include "third_party/tbb/detail/_namespace_injection.h"
#include "third_party/tbb/detail/_utils.h"
#include "third_party/tbb/detail/_waitable_atomic.h"
#include "third_party/tbb/detail/_scoped_lock.h"
#include "third_party/tbb/detail/_mutex_common.h"
#include "third_party/tbb/profiling.h"

namespace tbb {
namespace detail {
namespace d1 {

class rw_mutex {
public:
    //! Constructors
    rw_mutex() noexcept : m_state(0) {
       create_itt_sync(this, "tbb::rw_mutex", "");
    }

    //! Destructor
    ~rw_mutex() {
        __TBB_ASSERT(!m_state.load(std::memory_order_relaxed), "destruction of an acquired mutex");
    }

    //! No Copy
    rw_mutex(const rw_mutex&) = delete;
    rw_mutex& operator=(const rw_mutex&) = delete;

    using scoped_lock = rw_scoped_lock<rw_mutex>;

    //! Mutex traits
    static constexpr bool is_rw_mutex = true;
    static constexpr bool is_recursive_mutex = false;
    static constexpr bool is_fair_mutex = false;

    //! Acquire lock
    void lock() {
        call_itt_notify(prepare, this);
        while (!try_lock()) {
            if (!(m_state.load(std::memory_order_relaxed) & WRITER_PENDING)) { // no pending writers
                m_state |= WRITER_PENDING;
            }

            auto wakeup_condition = [&] { return !(m_state.load(std::memory_order_relaxed) & BUSY); };
            adaptive_wait_on_address(this, wakeup_condition, WRITER_CONTEXT);
        }

        call_itt_notify(acquired, this);
    }

    //! Try acquiring lock (non-blocking)
    /** Return true if lock acquired; false otherwise. */
    bool try_lock() {
        // for a writer: only possible to acquire if no active readers or writers
        // Use relaxed memory fence is OK here because
        // Acquire memory fence guaranteed by compare_exchange_strong()
        state_type s = m_state.load(std::memory_order_relaxed);
        if (!(s & BUSY)) { // no readers, no writers; mask is 1..1101
            if (m_state.compare_exchange_strong(s, WRITER)) {
                call_itt_notify(acquired, this);
                return true; // successfully stored writer flag
            }
        }
        return false;
    }

    //! Release lock
    void unlock() {
        call_itt_notify(releasing, this);
        state_type curr_state = (m_state &= READERS | WRITER_PENDING); // Returns current state

        if (curr_state & WRITER_PENDING) {
            r1::notify_by_address(this, WRITER_CONTEXT);
        } else {
            // It's possible that WRITER sleeps without WRITER_PENDING,
            // because other thread might clear this bit at upgrade()
            r1::notify_by_address_all(this);
        }
    }

    //! Lock shared ownership mutex
    void lock_shared() {
        call_itt_notify(prepare, this);
        while (!try_lock_shared()) {
            state_type has_writer = WRITER | WRITER_PENDING;
            auto wakeup_condition = [&] { return !(m_state.load(std::memory_order_relaxed) & has_writer); };
            adaptive_wait_on_address(this, wakeup_condition, READER_CONTEXT);
        }
        __TBB_ASSERT(m_state.load(std::memory_order_relaxed) & READERS, "invalid state of a read lock: no readers");
    }

    //! Try lock shared ownership mutex
    bool try_lock_shared() {
        // for a reader: acquire if no active or waiting writers
        // Use relaxed memory fence is OK here because
        // Acquire memory fence guaranteed by fetch_add()
        state_type has_writer = WRITER | WRITER_PENDING;
        if (!(m_state.load(std::memory_order_relaxed) & has_writer)) {
            if (m_state.fetch_add(ONE_READER) & has_writer) {
                m_state -= ONE_READER;
                r1::notify_by_address(this, WRITER_CONTEXT);
            } else {
                call_itt_notify(acquired, this);
                return true; // successfully stored increased number of readers
            }
        }
        return false;
    }

    //! Unlock shared ownership mutex
    void unlock_shared() {
        __TBB_ASSERT(m_state.load(std::memory_order_relaxed) & READERS, "invalid state of a read lock: no readers");
        call_itt_notify(releasing, this);

        state_type curr_state = (m_state -= ONE_READER); // Returns current state

        if (curr_state & (WRITER_PENDING)) {
            r1::notify_by_address(this, WRITER_CONTEXT);
        } else {
            // It's possible that WRITER sleeps without WRITER_PENDING,
            // because other thread might clear this bit at upgrade()
            r1::notify_by_address_all(this);
        }
    }

private:
    /** Internal non ISO C++ standard API **/
    //! This API is used through the scoped_lock class

    //! Upgrade reader to become a writer.
    /** Returns whether the upgrade happened without releasing and re-acquiring the lock */
    bool upgrade() {
        state_type s = m_state.load(std::memory_order_relaxed);
        __TBB_ASSERT(s & READERS, "invalid state before upgrade: no readers ");
        // Check and set writer-pending flag.
        // Required conditions: either no pending writers, or we are the only reader
        // (with multiple readers and pending writer, another upgrade could have been requested)
        while ((s & READERS) == ONE_READER || !(s & WRITER_PENDING)) {
            if (m_state.compare_exchange_strong(s, s | WRITER | WRITER_PENDING)) {
                auto wakeup_condition = [&] { return (m_state.load(std::memory_order_relaxed) & READERS) == ONE_READER; };
                while ((m_state.load(std::memory_order_relaxed) & READERS) != ONE_READER) {
                    adaptive_wait_on_address(this, wakeup_condition, WRITER_CONTEXT);
                }

                __TBB_ASSERT((m_state.load(std::memory_order_relaxed) & (WRITER_PENDING|WRITER)) == (WRITER_PENDING | WRITER),
                             "invalid state when upgrading to writer");
                // Both new readers and writers are blocked at this time
                m_state -= (ONE_READER + WRITER_PENDING);
                return true; // successfully upgraded
            }
        }
        // Slow reacquire
        unlock_shared();
        lock();
        return false;
    }

    //! Downgrade writer to a reader
    void downgrade() {
        __TBB_ASSERT(m_state.load(std::memory_order_relaxed) & WRITER, nullptr),
        call_itt_notify(releasing, this);
        m_state += (ONE_READER - WRITER);

        if (!(m_state & WRITER_PENDING)) {
            r1::notify_by_address(this, READER_CONTEXT);
        }

        __TBB_ASSERT(m_state.load(std::memory_order_relaxed) & READERS, "invalid state after downgrade: no readers");
    }

    using state_type = std::intptr_t;
    static constexpr state_type WRITER = 1;
    static constexpr state_type WRITER_PENDING = 2;
    static constexpr state_type READERS = ~(WRITER | WRITER_PENDING);
    static constexpr state_type ONE_READER = 4;
    static constexpr state_type BUSY = WRITER | READERS;

    using context_type = std::uintptr_t;
    static constexpr context_type WRITER_CONTEXT = 0;
    static constexpr context_type READER_CONTEXT = 1;
    friend scoped_lock;
    //! State of lock
    /** Bit 0 = writer is holding lock
        Bit 1 = request by a writer to acquire lock (hint to readers to wait)
        Bit 2..N = number of readers holding lock */
    std::atomic<state_type> m_state;
}; // class rw_mutex

} // namespace d1
} // namespace detail

inline namespace v1 {
using detail::d1::rw_mutex;
} // namespace v1

} // namespace tbb

#endif // __TBB_rw_mutex_H
