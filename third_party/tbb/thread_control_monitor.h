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

#ifndef __TBB_thread_control_monitor_H
#define __TBB_thread_control_monitor_H

#include "third_party/tbb/concurrent_monitor.h"
#include "third_party/tbb/scheduler_common.h"

#include "third_party/libcxx/atomic"

namespace tbb {
namespace detail {
namespace r1 {

struct market_context {
    market_context() = default;

    market_context(std::uintptr_t first_addr, arena* a) :
        my_uniq_addr(first_addr), my_arena_addr(a)
    {}

    std::uintptr_t my_uniq_addr{0};
    arena* my_arena_addr{nullptr};
};

#if __TBB_RESUMABLE_TASKS
class resume_node : public wait_node<market_context> {
    using base_type = wait_node<market_context>;
public:
    resume_node(market_context ctx, execution_data_ext& ed_ext, task_dispatcher& target)
        : base_type(ctx), my_curr_dispatcher(ed_ext.task_disp), my_target_dispatcher(&target)
        , my_suspend_point(my_curr_dispatcher->get_suspend_point())
    {}

    ~resume_node() override {
        if (this->my_skipped_wakeup) {
            spin_wait_until_eq(this->my_notify_calls, 1);
        }

        poison_pointer(my_curr_dispatcher);
        poison_pointer(my_target_dispatcher);
        poison_pointer(my_suspend_point);
    }

    void init() override {
        base_type::init();
    }

    void wait() override {
        my_curr_dispatcher->resume(*my_target_dispatcher);
        __TBB_ASSERT(!this->my_is_in_list.load(std::memory_order_relaxed), "Still in the queue?");
    }

    void reset() override {
        base_type::reset();
        spin_wait_until_eq(this->my_notify_calls, 1);
        my_notify_calls.store(0, std::memory_order_relaxed);
    }

    // notify is called (perhaps, concurrently) twice from:
    //   - concurrent_monitor::notify
    //   - post_resume_action::register_waiter
    // The second notify is called after thread switches the stack
    // (Because we can not call resume while the stack is occupied)
    // We need calling resume only when both notifications are performed.
    void notify() override {
        if (++my_notify_calls == 2) {
            r1::resume(my_suspend_point);
        }
    }

private:
    friend class thread_data;
    friend struct suspend_point_type::resume_task;
    task_dispatcher* my_curr_dispatcher;
    task_dispatcher* my_target_dispatcher;
    suspend_point_type* my_suspend_point;
    std::atomic<int> my_notify_calls{0};
};
#endif // __TBB_RESUMABLE_TASKS

class thread_control_monitor : public concurrent_monitor_base<market_context> {
    using base_type = concurrent_monitor_base<market_context>;
public:
    using base_type::base_type;

    ~thread_control_monitor() {
        destroy();
    }

    /** per-thread descriptor for concurrent_monitor */
    using thread_context = sleep_node<market_context>;
#if __TBB_RESUMABLE_TASKS
    using resume_context = resume_node;
#endif
};

} // namespace r1
} // namespace detail
} // namespace tbb

#endif // __TBB_thread_control_monitor_H
