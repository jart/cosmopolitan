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
#include "third_party/tbb/detail/_rtm_mutex.hh"
#include "third_party/tbb/itt_notify.hh"
#include "third_party/tbb/governor.hh"
#include "third_party/tbb/misc.hh"

#include "third_party/libcxx/atomic"

namespace tbb {
namespace detail {
namespace r1 {


struct rtm_mutex_impl {
    // maximum number of times to retry
    // TODO: experiment on retry values.
    static constexpr int retry_threshold = 10;
    using transaction_result_type = decltype(begin_transaction());

    //! Release speculative mutex
    static void release(d1::rtm_mutex::scoped_lock& s) {
        switch(s.m_transaction_state) {
        case d1::rtm_mutex::rtm_state::rtm_transacting:
            __TBB_ASSERT(is_in_transaction(), "m_transaction_state && not speculating");
            end_transaction();
            s.m_mutex = nullptr;
            break;
        case d1::rtm_mutex::rtm_state::rtm_real:
            s.m_mutex->unlock();
            s.m_mutex = nullptr;
            break;
        case d1::rtm_mutex::rtm_state::rtm_none:
            __TBB_ASSERT(false, "mutex is not locked, but in release");
            break;
        default:
            __TBB_ASSERT(false, "invalid m_transaction_state");
        }
        s.m_transaction_state = d1::rtm_mutex::rtm_state::rtm_none;
    }

    //! Acquire lock on the given mutex.
    static void acquire(d1::rtm_mutex& m, d1::rtm_mutex::scoped_lock& s, bool only_speculate) {
        __TBB_ASSERT(s.m_transaction_state == d1::rtm_mutex::rtm_state::rtm_none, "scoped_lock already in transaction");
        if(governor::speculation_enabled()) {
            int num_retries = 0;
            transaction_result_type abort_code = 0;
            do {
                if(m.m_flag.load(std::memory_order_acquire)) {
                    if(only_speculate) return;
                    spin_wait_while_eq(m.m_flag, true);
                }
                // _xbegin returns -1 on success or the abort code, so capture it
                if((abort_code = begin_transaction()) == transaction_result_type(speculation_successful_begin))
                {
                    // started speculation
                    if(m.m_flag.load(std::memory_order_relaxed)) {
                        abort_transaction();
                    }
                    s.m_transaction_state = d1::rtm_mutex::rtm_state::rtm_transacting;
                    // Don not wrap the following assignment to a function,
                    // because it can abort the transaction in debug. Need mutex for release().
                    s.m_mutex = &m;
                    return;  // successfully started speculation
                }
                ++num_retries;
            } while((abort_code & speculation_retry) != 0 && (num_retries < retry_threshold));
        }

        if(only_speculate) return;
        s.m_mutex = &m;
        s.m_mutex->lock();
        s.m_transaction_state = d1::rtm_mutex::rtm_state::rtm_real;
    }

    //! Try to acquire lock on the given mutex.
    static bool try_acquire(d1::rtm_mutex& m, d1::rtm_mutex::scoped_lock& s) {
        acquire(m, s, /*only_speculate=*/true);
        if (s.m_transaction_state == d1::rtm_mutex::rtm_state::rtm_transacting) {
            return true;
        }
        __TBB_ASSERT(s.m_transaction_state == d1::rtm_mutex::rtm_state::rtm_none, nullptr);
        // transacting acquire failed. try_lock the real mutex
        if (m.try_lock()) {
            s.m_mutex = &m;
            s.m_transaction_state = d1::rtm_mutex::rtm_state::rtm_real;
            return true;
        }
        return false;
    }
};

void __TBB_EXPORTED_FUNC acquire(d1::rtm_mutex& m, d1::rtm_mutex::scoped_lock& s, bool only_speculate) {
    rtm_mutex_impl::acquire(m, s, only_speculate);
}
bool __TBB_EXPORTED_FUNC try_acquire(d1::rtm_mutex& m, d1::rtm_mutex::scoped_lock& s) {
    return rtm_mutex_impl::try_acquire(m, s);
}
void __TBB_EXPORTED_FUNC release(d1::rtm_mutex::scoped_lock& s) {
    rtm_mutex_impl::release(s);
}

} // namespace r1
} // namespace detail
} // namespace tbb

