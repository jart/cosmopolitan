// clang-format off
/*
    Copyright (c) 2005-2023 Intel Corporation

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

#ifndef _TBB_waiters_H
#define _TBB_waiters_H

#include "third_party/tbb/detail/_task.hh"
#include "third_party/tbb/scheduler_common.hh"
#include "third_party/tbb/arena.hh"
#include "third_party/tbb/threading_control.hh"

namespace tbb {
namespace detail {
namespace r1 {

inline d1::task* get_self_recall_task(arena_slot& slot);

class waiter_base {
public:
    waiter_base(arena& a, int yields_multiplier = 1) : my_arena(a), my_backoff(int(a.my_num_slots), yields_multiplier) {}

    bool pause() {
        if (my_backoff.pause()) {
            my_arena.out_of_work();
            return true;
        }

        return false;
    }

    void reset_wait() {
        my_backoff.reset_wait();
    }

protected:
    arena& my_arena;
    stealing_loop_backoff my_backoff;
};

class outermost_worker_waiter : public waiter_base {
public:
    using waiter_base::waiter_base;

    bool continue_execution(arena_slot& slot, d1::task*& t) const {
        __TBB_ASSERT(t == nullptr, nullptr);

        if (is_worker_should_leave(slot)) {
            // Leave dispatch loop
            return false;
        }

        t = get_self_recall_task(slot);
        return true;
    }

    void pause(arena_slot&) {
        waiter_base::pause();
    }


    d1::wait_context* wait_ctx() {
        return nullptr;
    }

    static bool postpone_execution(d1::task&) {
        return false;
    }

private:
    using base_type = waiter_base;

    bool is_worker_should_leave(arena_slot& slot) const {
        bool is_top_priority_arena = my_arena.is_top_priority();
        bool is_task_pool_empty = slot.task_pool.load(std::memory_order_relaxed) == EmptyTaskPool;

        if (is_top_priority_arena) {
            // Worker in most priority arena do not leave arena, until all work in task_pool is done
            if (is_task_pool_empty && my_arena.is_recall_requested()) {
                return true;
            }
        } else {
            if (my_arena.is_recall_requested()) {
                // If worker has work in task pool, we must notify other threads,
                // because can appear missed wake up of other threads
                if (!is_task_pool_empty) {
                    my_arena.advertise_new_work<arena::wakeup>();
                }
                return true;
            }
        }

        return false;
    }
};

class sleep_waiter : public waiter_base {
protected:
    using waiter_base::waiter_base;

    template <typename Pred>
    void sleep(std::uintptr_t uniq_tag, Pred wakeup_condition) {
        my_arena.get_waiting_threads_monitor().wait<thread_control_monitor::thread_context>(wakeup_condition,
            market_context{uniq_tag, &my_arena});
    }
};

class external_waiter : public sleep_waiter {
public:
    external_waiter(arena& a, d1::wait_context& wo)
        : sleep_waiter(a, /*yields_multiplier*/10), my_wait_ctx(wo)
        {}

    bool continue_execution(arena_slot& slot, d1::task*& t) const {
        __TBB_ASSERT(t == nullptr, nullptr);
        if (!my_wait_ctx.continue_execution())
            return false;
        t = get_self_recall_task(slot);
        return true;
    }

    void pause(arena_slot&) {
        if (!sleep_waiter::pause()) {
            return;
        }

        auto wakeup_condition = [&] { return !my_arena.is_empty() || !my_wait_ctx.continue_execution(); };

        sleep(std::uintptr_t(&my_wait_ctx), wakeup_condition);
        my_backoff.reset_wait();
    }

    d1::wait_context* wait_ctx() {
        return &my_wait_ctx;
    }

    static bool postpone_execution(d1::task&) {
        return false;
    }

private:
    d1::wait_context& my_wait_ctx;
};

#if __TBB_RESUMABLE_TASKS

class coroutine_waiter : public sleep_waiter {
public:
    using sleep_waiter::sleep_waiter;

    bool continue_execution(arena_slot& slot, d1::task*& t) const {
        __TBB_ASSERT(t == nullptr, nullptr);
        t = get_self_recall_task(slot);
        return true;
    }

    void pause(arena_slot& slot) {
        if (!sleep_waiter::pause()) {
            return;
        }

        suspend_point_type* sp = slot.default_task_dispatcher().m_suspend_point;

        auto wakeup_condition = [&] { return !my_arena.is_empty() || sp->m_is_owner_recalled.load(std::memory_order_relaxed); };

        sleep(std::uintptr_t(sp), wakeup_condition);
        my_backoff.reset_wait();
    }

    void reset_wait() {
        my_backoff.reset_wait();
    }

    d1::wait_context* wait_ctx() {
        return nullptr;
    }

    static bool postpone_execution(d1::task& t) {
        return task_accessor::is_resume_task(t);
    }
};

#endif // __TBB_RESUMABLE_TASKS

} // namespace r1
} // namespace detail
} // namespace tbb

#endif // _TBB_waiters_H
