// clang-format off
/*
    Copyright (c) 2020-2023 Intel Corporation

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

#ifndef _TBB_task_dispatcher_H
#define _TBB_task_dispatcher_H

#include "third_party/tbb/detail/_utils.hh"
#include "third_party/tbb/detail/_task.hh"
#include "third_party/tbb/global_control.hh"

#include "third_party/tbb/scheduler_common.hh"
#include "third_party/tbb/waiters.hh"
#include "third_party/tbb/arena_slot.hh"
#include "third_party/tbb/arena.hh"
#include "third_party/tbb/thread_data.hh"
#include "third_party/tbb/mailbox.hh"
#include "third_party/tbb/itt_notify.hh"
#include "third_party/tbb/concurrent_monitor.hh"
#include "third_party/tbb/threading_control.hh"

#include "third_party/libcxx/atomic"

#if !__TBB_CPU_CTL_ENV_PRESENT
#include "libc/runtime/fenv.h" //
#endif

namespace tbb {
namespace detail {
namespace r1 {

inline d1::task* get_self_recall_task(arena_slot& slot) {
    suppress_unused_warning(slot);
    d1::task* t = nullptr;
#if __TBB_RESUMABLE_TASKS
    suspend_point_type* sp = slot.default_task_dispatcher().m_suspend_point;
    if (sp && sp->m_is_owner_recalled.load(std::memory_order_acquire)) {
        t = &sp->m_resume_task;
        __TBB_ASSERT(sp->m_resume_task.m_target.m_thread_data == nullptr, nullptr);
    }
#endif /* __TBB_RESUMABLE_TASKS */
    return t;
}

// Defined in exception.cpp
/*[[noreturn]]*/void do_throw_noexcept(void (*throw_exception)()) noexcept;

//------------------------------------------------------------------------
// Suspend point
//------------------------------------------------------------------------
#if __TBB_RESUMABLE_TASKS

inline d1::task* suspend_point_type::resume_task::execute(d1::execution_data& ed) {
    execution_data_ext& ed_ext = static_cast<execution_data_ext&>(ed);

    if (ed_ext.wait_ctx) {
        thread_control_monitor::resume_context monitor_node{{std::uintptr_t(ed_ext.wait_ctx), nullptr}, ed_ext, m_target};
        // The wait_ctx is present only in external_waiter. In that case we leave the current stack
        // in the abandoned state to resume when waiting completes.
        thread_data* td = ed_ext.task_disp->m_thread_data;
        td->set_post_resume_action(task_dispatcher::post_resume_action::register_waiter, &monitor_node);

        thread_control_monitor& wait_list = td->my_arena->get_waiting_threads_monitor();

        if (wait_list.wait([&] { return !ed_ext.wait_ctx->continue_execution(); }, monitor_node)) {
            return nullptr;
        }

        td->clear_post_resume_action();
        r1::resume(ed_ext.task_disp->get_suspend_point());
    } else {
        // If wait_ctx is null, it can be only a worker thread on outermost level because
        // coroutine_waiter interrupts bypass loop before the resume_task execution.
        ed_ext.task_disp->m_thread_data->set_post_resume_action(task_dispatcher::post_resume_action::notify,
            ed_ext.task_disp->get_suspend_point());
    }
    // Do not access this task because it might be destroyed
    ed_ext.task_disp->resume(m_target);
    return nullptr;
}

inline suspend_point_type::suspend_point_type(arena* a, size_t stack_size, task_dispatcher& task_disp)
    : m_arena(a)
    , m_random(this)
    , m_co_context(stack_size, &task_disp)
    , m_resume_task(task_disp)
{
    assert_pointer_valid(m_arena);
    assert_pointer_valid(m_arena->my_default_ctx);
    task_accessor::context(m_resume_task) = m_arena->my_default_ctx;
    task_accessor::isolation(m_resume_task) = no_isolation;
    // Initialize the itt_caller for the context of the resume task.
    // It will be bound to the stack of the first suspend call.
    task_group_context_impl::bind_to(*task_accessor::context(m_resume_task), task_disp.m_thread_data);
}

#endif /* __TBB_RESUMABLE_TASKS */

//------------------------------------------------------------------------
// Task Dispatcher
//------------------------------------------------------------------------
inline task_dispatcher::task_dispatcher(arena* a) {
    m_execute_data_ext.context = a->my_default_ctx;
    m_execute_data_ext.task_disp = this;
}

inline bool task_dispatcher::can_steal() {
    __TBB_ASSERT(m_stealing_threshold != 0, nullptr);
    stack_anchor_type anchor{};
    return reinterpret_cast<std::uintptr_t>(&anchor) > m_stealing_threshold;
}

inline d1::task* task_dispatcher::get_inbox_or_critical_task(
    execution_data_ext& ed, mail_inbox& inbox, isolation_type isolation, bool critical_allowed)
{
    if (inbox.empty())
        return nullptr;
    d1::task* result = get_critical_task(nullptr, ed, isolation, critical_allowed);
    if (result)
        return result;
    // Check if there are tasks mailed to this thread via task-to-thread affinity mechanism.
    result = get_mailbox_task(inbox, ed, isolation);
    // There is a race with a thread adding a new task (possibly with suitable isolation)
    // to our mailbox, so the below conditions might result in a false positive.
    // Then set_is_idle(false) allows that task to be stolen; it's OK.
    if (isolation != no_isolation && !result && !inbox.empty() && inbox.is_idle_state(true)) {
        // We have proxy tasks in our mailbox but the isolation blocks their execution.
        // So publish the proxy tasks in mailbox to be available for stealing from owner's task pool.
        inbox.set_is_idle( false );
    }
    return result;
}

inline d1::task* task_dispatcher::get_stream_or_critical_task(
    execution_data_ext& ed, arena& a, task_stream<front_accessor>& stream, unsigned& hint,
    isolation_type isolation, bool critical_allowed)
{
    if (stream.empty())
        return nullptr;
    d1::task* result = get_critical_task(nullptr, ed, isolation, critical_allowed);
    if (result)
        return result;
    return a.get_stream_task(stream, hint);
}

inline d1::task* task_dispatcher::steal_or_get_critical(
    execution_data_ext& ed, arena& a, unsigned arena_index, FastRandom& random,
    isolation_type isolation, bool critical_allowed)
{
    if (d1::task* t = a.steal_task(arena_index, random, ed, isolation)) {
        ed.context = task_accessor::context(*t);
        ed.isolation = task_accessor::isolation(*t);
        return get_critical_task(t, ed, isolation, critical_allowed);
    }
    return nullptr;
}

template <bool ITTPossible, typename Waiter>
d1::task* task_dispatcher::receive_or_steal_task(
    thread_data& tls, execution_data_ext& ed, Waiter& waiter, isolation_type isolation,
    bool fifo_allowed, bool critical_allowed)
{
    __TBB_ASSERT(governor::is_thread_data_set(&tls), nullptr);
    // Task to return
    d1::task* t = nullptr;
    // Get tls data (again)
    arena& a = *tls.my_arena;
    arena_slot& slot = *tls.my_arena_slot;
    unsigned arena_index = tls.my_arena_index;
    mail_inbox& inbox = tls.my_inbox;
    task_stream<front_accessor>& resume_stream = a.my_resume_task_stream;
    unsigned& resume_hint = slot.hint_for_resume_stream;
    task_stream<front_accessor>& fifo_stream = a.my_fifo_task_stream;
    unsigned& fifo_hint = slot.hint_for_fifo_stream;

    waiter.reset_wait();
    // Thread is in idle state now
    inbox.set_is_idle(true);

    bool stealing_is_allowed = can_steal();

    // Stealing loop mailbox/enqueue/other_slots
    for (;;) {
        __TBB_ASSERT(t == nullptr, nullptr);
        // Check if the resource manager requires our arena to relinquish some threads
        // For the external thread restore idle state to true after dispatch loop
        if (!waiter.continue_execution(slot, t)) {
            __TBB_ASSERT(t == nullptr, nullptr);
            break;
        }
        // Start searching
        if (t != nullptr) {
            // continue_execution returned a task
        }
        else if ((t = get_inbox_or_critical_task(ed, inbox, isolation, critical_allowed))) {
            // Successfully got the task from mailbox or critical task
        }
        else if ((t = get_stream_or_critical_task(ed, a, resume_stream, resume_hint, isolation, critical_allowed))) {
            // Successfully got the resume or critical task
        }
        else if (fifo_allowed && isolation == no_isolation
                 && (t = get_stream_or_critical_task(ed, a, fifo_stream, fifo_hint, isolation, critical_allowed))) {
            // Checked if there are tasks in starvation-resistant stream. Only allowed at the outermost dispatch level without isolation.
        }
        else if (stealing_is_allowed
                 && (t = steal_or_get_critical(ed, a, arena_index, tls.my_random, isolation, critical_allowed))) {
            // Stole a task from a random arena slot
        }
        else {
            t = get_critical_task(t, ed, isolation, critical_allowed);
        }

        if (t != nullptr) {
            ed.context = task_accessor::context(*t);
            ed.isolation = task_accessor::isolation(*t);
            a.my_observers.notify_entry_observers(tls.my_last_observer, tls.my_is_worker);
            break; // Stealing success, end of stealing attempt
        }
        // Nothing to do, pause a little.
        waiter.pause(slot);
    } // end of nonlocal task retrieval loop

    __TBB_ASSERT(is_alive(a.my_guard), nullptr);
    if (inbox.is_idle_state(true)) {
        inbox.set_is_idle(false);
    }
    return t;
}

template <bool ITTPossible, typename Waiter>
d1::task* task_dispatcher::local_wait_for_all(d1::task* t, Waiter& waiter ) {
    assert_pointer_valid(m_thread_data);
    __TBB_ASSERT(m_thread_data->my_task_dispatcher == this, nullptr);

    // Guard an outer/default execution state
    struct dispatch_loop_guard {
        task_dispatcher& task_disp;
        execution_data_ext old_execute_data_ext;
        properties old_properties;

        ~dispatch_loop_guard() {
            task_disp.m_execute_data_ext = old_execute_data_ext;
            task_disp.m_properties = old_properties;

            __TBB_ASSERT(task_disp.m_thread_data && governor::is_thread_data_set(task_disp.m_thread_data), nullptr);
            __TBB_ASSERT(task_disp.m_thread_data->my_task_dispatcher == &task_disp, nullptr);
        }
    } dl_guard{ *this, m_execute_data_ext, m_properties };

    // The context guard to track fp setting and itt tasks.
    context_guard_helper</*report_tasks=*/ITTPossible> context_guard;

    // Current isolation context
    const isolation_type isolation = dl_guard.old_execute_data_ext.isolation;

    // Critical work inflection point. Once turned false current execution context has taken
    // critical task on the previous stack frame and cannot take more until that critical path is
    // finished.
    bool critical_allowed = dl_guard.old_properties.critical_task_allowed;

    // Extended execution data that is used for dispatching.
    // Base version is passed to the task::execute method.
    execution_data_ext& ed = m_execute_data_ext;
    ed.context = t ? task_accessor::context(*t) : nullptr;
    ed.original_slot = m_thread_data->my_arena_index;
    ed.affinity_slot = d1::no_slot;
    ed.task_disp = this;
    ed.wait_ctx = waiter.wait_ctx();

    m_properties.outermost = false;
    m_properties.fifo_tasks_allowed = false;

    t = get_critical_task(t, ed, isolation, critical_allowed);
    if (t && m_thread_data->my_inbox.is_idle_state(true)) {
        // The thread has a work to do. Therefore, marking its inbox as not idle so that
        // affinitized tasks can be stolen from it.
        m_thread_data->my_inbox.set_is_idle(false);
    }

    // Infinite exception loop
    for (;;) {
        try {
            // Main execution loop
            do {
                // We assume that bypass tasks are from the same task group.
                context_guard.set_ctx(ed.context);
                // Inner level evaluates tasks coming from nesting loops and those returned
                // by just executed tasks (bypassing spawn or enqueue calls).
                while (t != nullptr) {
                    assert_task_valid(t);
                    assert_pointer_valid</*alignment = */alignof(void*)>(ed.context);
                    __TBB_ASSERT(ed.context->my_state == d1::task_group_context::state::bound ||
                        ed.context->my_state == d1::task_group_context::state::isolated, nullptr);
                    __TBB_ASSERT(m_thread_data->my_inbox.is_idle_state(false), nullptr);
                    __TBB_ASSERT(task_accessor::is_resume_task(*t) || isolation == no_isolation || isolation == ed.isolation, nullptr);
                    // Check premature leave
                    if (Waiter::postpone_execution(*t)) {
                        __TBB_ASSERT(task_accessor::is_resume_task(*t) && dl_guard.old_properties.outermost,
                            "Currently, the bypass loop can be interrupted only for resume task on outermost level");
                        return t;
                    }
                    // Copy itt_caller to a stack because the context might be destroyed after t->execute.
                    void* itt_caller = ed.context->my_itt_caller;
                    suppress_unused_warning(itt_caller);

                    ITT_CALLEE_ENTER(ITTPossible, t, itt_caller);

                    if (ed.context->is_group_execution_cancelled()) {
                        t = t->cancel(ed);
                    } else {
                        t = t->execute(ed);
                    }

                    ITT_CALLEE_LEAVE(ITTPossible, itt_caller);

                    // The task affinity in execution data is set for affinitized tasks.
                    // So drop it after the task execution.
                    ed.affinity_slot = d1::no_slot;
                    // Reset task owner id for bypassed task
                    ed.original_slot = m_thread_data->my_arena_index;
                    t = get_critical_task(t, ed, isolation, critical_allowed);
                }
                __TBB_ASSERT(m_thread_data && governor::is_thread_data_set(m_thread_data), nullptr);
                __TBB_ASSERT(m_thread_data->my_task_dispatcher == this, nullptr);
                // When refactoring, pay attention that m_thread_data can be changed after t->execute()
                __TBB_ASSERT(m_thread_data->my_arena_slot != nullptr, nullptr);
                arena_slot& slot = *m_thread_data->my_arena_slot;
                if (!waiter.continue_execution(slot, t)) {
                    break;
                }
                // Retrieve the task from local task pool
                if (t || (slot.is_task_pool_published() && (t = slot.get_task(ed, isolation)))) {
                    __TBB_ASSERT(ed.original_slot == m_thread_data->my_arena_index, nullptr);
                    ed.context = task_accessor::context(*t);
                    ed.isolation = task_accessor::isolation(*t);
                    continue;
                }
                // Retrieve the task from global sources
                t = receive_or_steal_task<ITTPossible>(
                    *m_thread_data, ed, waiter, isolation, dl_guard.old_properties.fifo_tasks_allowed,
                    critical_allowed
                );
            } while (t != nullptr); // main dispatch loop
            break; // Exit exception loop;
        } catch (...) {
            if (global_control::active_value(global_control::terminate_on_exception) == 1) {
                do_throw_noexcept([] { throw; });
            }
            if (ed.context->cancel_group_execution()) {
                /* We are the first to signal cancellation, so store the exception that caused it. */
                ed.context->my_exception.store(tbb_exception_ptr::allocate(), std::memory_order_release);
            }
        }
    } // Infinite exception loop
    __TBB_ASSERT(t == nullptr, nullptr);


#if __TBB_RESUMABLE_TASKS
    if (dl_guard.old_properties.outermost) {
        recall_point();
    }
#endif /* __TBB_RESUMABLE_TASKS */

    return nullptr;
}

#if __TBB_RESUMABLE_TASKS
inline void task_dispatcher::recall_point() {
    if (this != &m_thread_data->my_arena_slot->default_task_dispatcher()) {
        __TBB_ASSERT(m_suspend_point != nullptr, nullptr);
        __TBB_ASSERT(m_suspend_point->m_is_owner_recalled.load(std::memory_order_relaxed) == false, nullptr);

        m_thread_data->set_post_resume_action(post_resume_action::notify, get_suspend_point());
        internal_suspend();

        if (m_thread_data->my_inbox.is_idle_state(true)) {
            m_thread_data->my_inbox.set_is_idle(false);
        }
    }
}
#endif /* __TBB_RESUMABLE_TASKS */

#if __TBB_PREVIEW_CRITICAL_TASKS
inline d1::task* task_dispatcher::get_critical_task(d1::task* t, execution_data_ext& ed, isolation_type isolation, bool critical_allowed) {
    __TBB_ASSERT( critical_allowed || !m_properties.critical_task_allowed, nullptr );

    if (!critical_allowed) {
        // The stack is already in the process of critical path execution. Cannot take another
        // critical work until finish with the current one.
        __TBB_ASSERT(!m_properties.critical_task_allowed, nullptr);
        return t;
    }

    assert_pointers_valid(m_thread_data, m_thread_data->my_arena, m_thread_data->my_arena_slot);
    thread_data& td = *m_thread_data;
    arena& a = *td.my_arena;
    arena_slot& slot = *td.my_arena_slot;

    d1::task* crit_t = a.get_critical_task(slot.hint_for_critical_stream, isolation);
    if (crit_t != nullptr) {
        assert_task_valid(crit_t);
        if (t != nullptr) {
            assert_pointer_valid</*alignment = */alignof(void*)>(ed.context);
            r1::spawn(*t, *ed.context);
        }
        ed.context = task_accessor::context(*crit_t);
        ed.isolation = task_accessor::isolation(*crit_t);

        // We cannot execute more than one critical task on the same stack.
        // In other words, we prevent nested critical tasks.
        m_properties.critical_task_allowed = false;

        // TODO: add a test that the observer is called when critical task is taken.
        a.my_observers.notify_entry_observers(td.my_last_observer, td.my_is_worker);
        t = crit_t;
    } else {
        // Was unable to find critical work in the queue. Allow inspecting the queue in nested
        // invocations. Handles the case when critical task has been just completed.
        m_properties.critical_task_allowed = true;
    }
    return t;
}
#else
inline d1::task* task_dispatcher::get_critical_task(d1::task* t, execution_data_ext&, isolation_type, bool /*critical_allowed*/) {
    return t;
}
#endif

inline d1::task* task_dispatcher::get_mailbox_task(mail_inbox& my_inbox, execution_data_ext& ed, isolation_type isolation) {
    while (task_proxy* const tp = my_inbox.pop(isolation)) {
        if (d1::task* result = tp->extract_task<task_proxy::mailbox_bit>()) {
            ed.original_slot = (unsigned short)(-2);
            ed.affinity_slot = ed.task_disp->m_thread_data->my_arena_index;
            return result;
        }
        // We have exclusive access to the proxy, and can destroy it.
        tp->allocator.delete_object(tp, ed);
    }
    return nullptr;
}

template <typename Waiter>
d1::task* task_dispatcher::local_wait_for_all(d1::task* t, Waiter& waiter) {
    if (governor::is_itt_present()) {
        return local_wait_for_all</*ITTPossible = */ true>(t, waiter);
    } else {
        return local_wait_for_all</*ITTPossible = */ false>(t, waiter);
    }
}

} // namespace r1
} // namespace detail
} // namespace tbb

#endif // _TBB_task_dispatcher_H

