// clang-format off
/*
    Copyright (c) 2022-2023 Intel Corporation

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

#ifndef _TBB_cancellation_disseminator_H
#define _TBB_cancellation_disseminator_H

#include "third_party/tbb/mutex.h"
#include "third_party/tbb/task_group.h"

#include "third_party/tbb/intrusive_list.h"
#include "third_party/tbb/thread_data.h"

namespace tbb {
namespace detail {
namespace r1 {

class cancellation_disseminator {
public:
    //! Finds all contexts affected by the state change and propagates the new state to them.
    /*  The propagation is relayed to the cancellation_disseminator because tasks created by one
        external thread can be passed to and executed by other external threads. This means
        that context trees can span several arenas at once and thus state change
        propagation cannot be generally localized to one arena only.
    */
    bool propagate_task_group_state(std::atomic<uint32_t> d1::task_group_context::*mptr_state, d1::task_group_context& src, uint32_t new_state) {
        if (src.my_may_have_children.load(std::memory_order_relaxed) != d1::task_group_context::may_have_children) {
            return true;
        }

        // The whole propagation algorithm is under the lock in order to ensure correctness
        // in case of concurrent state changes at the different levels of the context tree.
        threads_list_mutex_type::scoped_lock lock(my_threads_list_mutex);
        // TODO: consider to use double-check idiom
        if ((src.*mptr_state).load(std::memory_order_relaxed) != new_state) {
            // Another thread has concurrently changed the state. Back down.
            return false;
        }

        // Advance global state propagation epoch
        ++the_context_state_propagation_epoch;
        // Propagate to all workers and external threads and sync up their local epochs with the global one
        // The whole propagation sequence is locked, thus no contention is expected
        for (auto& thr_data : my_threads_list) {
            thr_data.propagate_task_group_state(mptr_state, src, new_state);
        }

        return true;
    }

    void register_thread(thread_data& td) {
        threads_list_mutex_type::scoped_lock lock(my_threads_list_mutex);
        my_threads_list.push_front(td);
    }

    void unregister_thread(thread_data& td) {
        threads_list_mutex_type::scoped_lock lock(my_threads_list_mutex);
        my_threads_list.remove(td);
    }

private:
    using thread_data_list_type = intrusive_list<thread_data>;
    using threads_list_mutex_type = d1::mutex;

    threads_list_mutex_type my_threads_list_mutex;
    thread_data_list_type my_threads_list;
};

} // namespace r1
} // namespace detail
} // namespace tbb

#endif // _TBB_cancellation_disseminator_H
