// clang-format off
/*
    Copyright (c) 2005-2022 Intel Corporation

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

#include "third_party/tbb/detail/_assert.hh"
#include "third_party/tbb/detail/_rtm_rw_mutex.hh"
#include "third_party/tbb/itt_notify.hh"
#include "third_party/tbb/governor.hh"
#include "third_party/tbb/misc.hh"

#include "third_party/libcxx/atomic"

namespace tbb {
namespace detail {
namespace r1 {

struct rtm_rw_mutex_impl {
    // maximum number of times to retry
    // TODO: experiment on retry values.
    static constexpr int retry_threshold_read = 10;
    static constexpr int retry_threshold_write = 10;
    using transaction_result_type = decltype(begin_transaction());

    //! Release speculative mutex
    static void release(d1::rtm_rw_mutex::scoped_lock& s) {
        switch(s.m_transaction_state) {
        case d1::rtm_rw_mutex::rtm_type::rtm_transacting_writer:
        case d1::rtm_rw_mutex::rtm_type::rtm_transacting_reader:
            __TBB_ASSERT(is_in_transaction(), "m_transaction_state && not speculating");
            end_transaction();
            s.m_mutex = nullptr;
            break;
        case d1::rtm_rw_mutex::rtm_type::rtm_real_reader:
            __TBB_ASSERT(!s.m_mutex->write_flag.load(std::memory_order_relaxed), "write_flag set but read lock acquired");
            s.m_mutex->unlock_shared();
            s.m_mutex = nullptr;
            break;
        case d1::rtm_rw_mutex::rtm_type::rtm_real_writer:
            __TBB_ASSERT(s.m_mutex->write_flag.load(std::memory_order_relaxed), "write_flag unset but write lock acquired");
            s.m_mutex->write_flag.store(false, std::memory_order_relaxed);
            s.m_mutex->unlock();
            s.m_mutex = nullptr;
            break;
        case d1::rtm_rw_mutex::rtm_type::rtm_not_in_mutex:
            __TBB_ASSERT(false, "rtm_not_in_mutex, but in release");
            break;
        default:
            __TBB_ASSERT(false, "invalid m_transaction_state");
        }
        s.m_transaction_state = d1::rtm_rw_mutex::rtm_type::rtm_not_in_mutex;
    }

    //! Acquire write lock on the given mutex.
    static void acquire_writer(d1::rtm_rw_mutex& m, d1::rtm_rw_mutex::scoped_lock& s, bool only_speculate) {
        __TBB_ASSERT(s.m_transaction_state == d1::rtm_rw_mutex::rtm_type::rtm_not_in_mutex, "scoped_lock already in transaction");
        if(governor::speculation_enabled()) {
            int num_retries = 0;
            transaction_result_type abort_code = 0;
            do {
                if(m.m_state.load(std::memory_order_acquire)) {
                    if(only_speculate) return;
                    spin_wait_until_eq(m.m_state, d1::rtm_rw_mutex::state_type(0));
                }
                // _xbegin returns -1 on success or the abort code, so capture it
                if((abort_code = begin_transaction()) == transaction_result_type(speculation_successful_begin))
                {
                    // started speculation
                    if(m.m_state.load(std::memory_order_relaxed)) {  // add spin_rw_mutex to read-set.
                        // reader or writer grabbed the lock, so abort.
                        abort_transaction();
                    }
                    s.m_transaction_state = d1::rtm_rw_mutex::rtm_type::rtm_transacting_writer;
                    // Don not wrap the following assignment to a function,
                    // because it can abort the transaction in debug. Need mutex for release().
                    s.m_mutex = &m;
                    return;  // successfully started speculation
                }
                ++num_retries;
            } while((abort_code & speculation_retry) != 0 && (num_retries < retry_threshold_write));
        }

        if(only_speculate) return;
        s.m_mutex = &m;                                                          // should apply a real try_lock...
        s.m_mutex->lock();                                                       // kill transactional writers
        __TBB_ASSERT(!m.write_flag.load(std::memory_order_relaxed), "After acquire for write, write_flag already true");
        m.write_flag.store(true, std::memory_order_relaxed);                       // kill transactional readers
        s.m_transaction_state = d1::rtm_rw_mutex::rtm_type::rtm_real_writer;
    }

    //! Acquire read lock on given mutex.
    //  only_speculate : true if we are doing a try_acquire.  If true and we fail to speculate, don't
    //     really acquire the lock, return and do a try_acquire on the contained spin_rw_mutex.  If
    //     the lock is already held by a writer, just return.
    static void acquire_reader(d1::rtm_rw_mutex& m, d1::rtm_rw_mutex::scoped_lock& s, bool only_speculate) {
        __TBB_ASSERT(s.m_transaction_state == d1::rtm_rw_mutex::rtm_type::rtm_not_in_mutex, "scoped_lock already in transaction");
        if(governor::speculation_enabled()) {
            int num_retries = 0;
            transaction_result_type abort_code = 0;
            do {
                // if in try_acquire, and lock is held as writer, don't attempt to speculate.
                if(m.write_flag.load(std::memory_order_acquire)) {
                    if(only_speculate) return;
                    spin_wait_while_eq(m.write_flag, true);
                }
                // _xbegin returns -1 on success or the abort code, so capture it
                if((abort_code = begin_transaction()) == transaction_result_type(speculation_successful_begin))
                {
                    // started speculation
                    if(m.write_flag.load(std::memory_order_relaxed)) {  // add write_flag to read-set.
                        abort_transaction();  // writer grabbed the lock, so abort.
                    }
                    s.m_transaction_state = d1::rtm_rw_mutex::rtm_type::rtm_transacting_reader;
                    // Don not wrap the following assignment to a function,
                    // because it can abort the transaction in debug. Need mutex for release().
                    s.m_mutex = &m;
                    return;  // successfully started speculation
                }
                // fallback path
                // retry only if there is any hope of getting into a transaction soon
                // Retry in the following cases (from Section 8.3.5 of
                // Intel(R) Architecture Instruction Set Extensions Programming Reference):
                // 1. abort caused by XABORT instruction (bit 0 of EAX register is set)
                // 2. the transaction may succeed on a retry (bit 1 of EAX register is set)
                // 3. if another logical processor conflicted with a memory address
                //    that was part of the transaction that aborted (bit 2 of EAX register is set)
                // That is, retry if (abort_code & 0x7) is non-zero
                ++num_retries;
            } while((abort_code & speculation_retry) != 0 && (num_retries < retry_threshold_read));
        }

        if(only_speculate) return;
        s.m_mutex = &m;
        s.m_mutex->lock_shared();
        s.m_transaction_state = d1::rtm_rw_mutex::rtm_type::rtm_real_reader;
    }

    //! Upgrade reader to become a writer.
    /** Returns whether the upgrade happened without releasing and re-acquiring the lock */
    static bool upgrade(d1::rtm_rw_mutex::scoped_lock& s) {
        switch(s.m_transaction_state) {
        case d1::rtm_rw_mutex::rtm_type::rtm_real_reader: {
            s.m_transaction_state = d1::rtm_rw_mutex::rtm_type::rtm_real_writer;
            bool no_release = s.m_mutex->upgrade();
            __TBB_ASSERT(!s.m_mutex->write_flag.load(std::memory_order_relaxed), "After upgrade, write_flag already true");
            s.m_mutex->write_flag.store(true, std::memory_order_relaxed);
            return no_release;
        }
        case d1::rtm_rw_mutex::rtm_type::rtm_transacting_reader: {
            d1::rtm_rw_mutex& m = *s.m_mutex;
            if(m.m_state.load(std::memory_order_acquire)) {  // add spin_rw_mutex to read-set.
                // Real reader or writer holds the lock; so commit the read and re-acquire for write.
                release(s);
                acquire_writer(m, s, false);
                return false;
            } else
            {
                s.m_transaction_state = d1::rtm_rw_mutex::rtm_type::rtm_transacting_writer;
                return true;
            }
        }
        default:
            __TBB_ASSERT(false, "Invalid state for upgrade");
            return false;
        }
    }

    //! Downgrade writer to a reader.
    static bool downgrade(d1::rtm_rw_mutex::scoped_lock& s) {
        switch (s.m_transaction_state) {
        case d1::rtm_rw_mutex::rtm_type::rtm_real_writer:
            s.m_transaction_state = d1::rtm_rw_mutex::rtm_type::rtm_real_reader;
            __TBB_ASSERT(s.m_mutex->write_flag.load(std::memory_order_relaxed), "Before downgrade write_flag not true");
            s.m_mutex->write_flag.store(false, std::memory_order_relaxed);
            s.m_mutex->downgrade();
            return true;
        case d1::rtm_rw_mutex::rtm_type::rtm_transacting_writer:
            s.m_transaction_state = d1::rtm_rw_mutex::rtm_type::rtm_transacting_reader;
            return true;
        default:
            __TBB_ASSERT(false, "Invalid state for downgrade");
            return false;
        }
    }

    //! Try to acquire write lock on the given mutex.
    //  There may be reader(s) which acquired the spin_rw_mutex, as well as possibly
    //  transactional reader(s).  If this is the case, the acquire will fail, and assigning
    //  write_flag will kill the transactors.  So we only assign write_flag if we have successfully
    //  acquired the lock.
    static bool try_acquire_writer(d1::rtm_rw_mutex& m, d1::rtm_rw_mutex::scoped_lock& s) {
        acquire_writer(m, s, /*only_speculate=*/true);
        if (s.m_transaction_state == d1::rtm_rw_mutex::rtm_type::rtm_transacting_writer) {
            return true;
        }
        __TBB_ASSERT(s.m_transaction_state == d1::rtm_rw_mutex::rtm_type::rtm_not_in_mutex, nullptr);
        // transacting write acquire failed. try_lock the real mutex
        if (m.try_lock()) {
            s.m_mutex = &m;
            // only shoot down readers if we're not transacting ourselves
            __TBB_ASSERT(!m.write_flag.load(std::memory_order_relaxed), "After try_acquire_writer, write_flag already true");
            m.write_flag.store(true, std::memory_order_relaxed);
            s.m_transaction_state = d1::rtm_rw_mutex::rtm_type::rtm_real_writer;
            return true;
        }
        return false;
    }

    //! Try to acquire read lock on the given mutex.
    static bool try_acquire_reader(d1::rtm_rw_mutex& m, d1::rtm_rw_mutex::scoped_lock& s) {
        // speculatively acquire the lock. If this fails, do try_lock_shared on the spin_rw_mutex.
        acquire_reader(m, s, /*only_speculate=*/true);
        if (s.m_transaction_state == d1::rtm_rw_mutex::rtm_type::rtm_transacting_reader) {
            return true;
        }
        __TBB_ASSERT(s.m_transaction_state == d1::rtm_rw_mutex::rtm_type::rtm_not_in_mutex, nullptr);
        // transacting read acquire failed. try_lock_shared the real mutex
        if (m.try_lock_shared()) {
            s.m_mutex = &m;
            s.m_transaction_state = d1::rtm_rw_mutex::rtm_type::rtm_real_reader;
            return true;
        }
        return false;
    }
};

void __TBB_EXPORTED_FUNC acquire_writer(d1::rtm_rw_mutex& m, d1::rtm_rw_mutex::scoped_lock& s, bool only_speculate) {
    rtm_rw_mutex_impl::acquire_writer(m, s, only_speculate);
}
//! Internal acquire read lock.
// only_speculate == true if we're doing a try_lock, else false.
void __TBB_EXPORTED_FUNC acquire_reader(d1::rtm_rw_mutex& m, d1::rtm_rw_mutex::scoped_lock& s, bool only_speculate) {
    rtm_rw_mutex_impl::acquire_reader(m, s, only_speculate);
}
//! Internal upgrade reader to become a writer.
bool __TBB_EXPORTED_FUNC upgrade(d1::rtm_rw_mutex::scoped_lock& s) {
    return rtm_rw_mutex_impl::upgrade(s);
}
//! Internal downgrade writer to become a reader.
bool __TBB_EXPORTED_FUNC downgrade(d1::rtm_rw_mutex::scoped_lock& s) {
    return rtm_rw_mutex_impl::downgrade(s);
}
//! Internal try_acquire write lock.
bool __TBB_EXPORTED_FUNC try_acquire_writer(d1::rtm_rw_mutex& m, d1::rtm_rw_mutex::scoped_lock& s) {
    return rtm_rw_mutex_impl::try_acquire_writer(m, s);
}
//! Internal try_acquire read lock.
bool __TBB_EXPORTED_FUNC try_acquire_reader(d1::rtm_rw_mutex& m, d1::rtm_rw_mutex::scoped_lock& s) {
    return rtm_rw_mutex_impl::try_acquire_reader(m, s);
}
//! Internal release lock.
void __TBB_EXPORTED_FUNC release(d1::rtm_rw_mutex::scoped_lock& s) {
    rtm_rw_mutex_impl::release(s);
}

} // namespace r1
} // namespace detail
} // namespace tbb


