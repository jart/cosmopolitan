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

#ifndef __TBB_thread_data_H
#define __TBB_thread_data_H

#include "third_party/tbb/detail/_task.h"
#include "third_party/tbb/task.h"

#include "third_party/tbb/rml_base.h" // rml::job

#include "third_party/tbb/scheduler_common.h"
#include "third_party/tbb/arena.h"
#include "third_party/tbb/concurrent_monitor.h"
#include "third_party/tbb/mailbox.h"
#include "third_party/tbb/misc.h" // FastRandom
#include "third_party/tbb/small_object_pool_impl.h"
#include "third_party/tbb/intrusive_list.h"

#include "third_party/libcxx/atomic"

namespace tbb {
namespace detail {
namespace r1 {

class task;
class arena_slot;
class task_group_context;
class task_dispatcher;
class thread_dispatcher_client;

class context_list : public intrusive_list<d1::intrusive_list_node> {
public:
    bool orphaned{false};

    //! Last state propagation epoch known to this thread
    /** Together with the_context_state_propagation_epoch constitute synchronization protocol
    that keeps hot path of task group context construction destruction mostly
    lock-free.
    When local epoch equals the global one, the state of task group contexts
    registered with this thread is consistent with that of the task group trees
    they belong to. **/
    std::atomic<std::uintptr_t> epoch{};

    //! Mutex protecting access to the list of task group contexts.
    d1::mutex m_mutex{};

    void destroy() {
        this->~context_list();
        cache_aligned_deallocate(this);
    }

    void remove(d1::intrusive_list_node& val) {
        mutex::scoped_lock lock(m_mutex);

        intrusive_list<d1::intrusive_list_node>::remove(val);

        if (orphaned && empty()) {
            lock.release();
            destroy();
        }
    }

    void push_front(d1::intrusive_list_node& val) {
        mutex::scoped_lock lock(m_mutex);

        intrusive_list<d1::intrusive_list_node>::push_front(val);
    }

    void orphan() {
        mutex::scoped_lock lock(m_mutex);

        orphaned = true;
        if (empty()) {
            lock.release();
            destroy();
        }
    }
};

//------------------------------------------------------------------------
// Thread Data
//------------------------------------------------------------------------
class thread_data : public ::rml::job
                  , public d1::intrusive_list_node
                  , no_copy {
public:
    thread_data(unsigned short index, bool is_worker)
        : my_arena_index{ index }
        , my_is_worker{ is_worker }
        , my_task_dispatcher{ nullptr }
        , my_arena{ nullptr }
        , my_last_client{ nullptr }
        , my_arena_slot{}
        , my_random{ this }
        , my_last_observer{ nullptr }
        , my_small_object_pool{new (cache_aligned_allocate(sizeof(small_object_pool_impl))) small_object_pool_impl{}}
        , my_context_list(new (cache_aligned_allocate(sizeof(context_list))) context_list{})
#if __TBB_RESUMABLE_TASKS
        , my_post_resume_action{ task_dispatcher::post_resume_action::none }
        , my_post_resume_arg{nullptr}
#endif /* __TBB_RESUMABLE_TASKS */
    {
        ITT_SYNC_CREATE(&my_context_list->m_mutex, SyncType_Scheduler, SyncObj_ContextsList);
    }

    ~thread_data() {
        my_context_list->orphan();
        my_small_object_pool->destroy();
        poison_pointer(my_task_dispatcher);
        poison_pointer(my_arena);
        poison_pointer(my_arena_slot);
        poison_pointer(my_last_observer);
        poison_pointer(my_small_object_pool);
        poison_pointer(my_context_list);
#if __TBB_RESUMABLE_TASKS
        poison_pointer(my_post_resume_arg);
#endif /* __TBB_RESUMABLE_TASKS */
    }

    void attach_arena(arena& a, std::size_t index);
    bool is_attached_to(arena*);
    void attach_task_dispatcher(task_dispatcher&);
    void detach_task_dispatcher();
    void enter_task_dispatcher(task_dispatcher& task_disp, std::uintptr_t stealing_threshold);
    void leave_task_dispatcher();
    void propagate_task_group_state(std::atomic<uint32_t> d1::task_group_context::* mptr_state, d1::task_group_context& src, uint32_t new_state);

    //! Index of the arena slot the scheduler occupies now, or occupied last time
    unsigned short my_arena_index;

    //! Indicates if the thread is created by RML
    const bool my_is_worker;

    //! The current task dipsatcher
    task_dispatcher* my_task_dispatcher;

    //! The arena that I own (if external thread) or am servicing at the moment (if worker)
    arena* my_arena;

    thread_dispatcher_client* my_last_client;

    //! Pointer to the slot in the arena we own at the moment
    arena_slot* my_arena_slot;

    //! The mailbox (affinity mechanism) the current thread attached to
    mail_inbox my_inbox;

    //! The random generator
    FastRandom my_random;

    //! Last observer in the observers list processed on this slot
    observer_proxy* my_last_observer;

    //! Pool of small object for fast task allocation
    small_object_pool_impl* my_small_object_pool;

    context_list* my_context_list;
#if __TBB_RESUMABLE_TASKS
    //! Suspends the current coroutine (task_dispatcher).
    void suspend(void* suspend_callback, void* user_callback);

    //! Resumes the target task_dispatcher.
    void resume(task_dispatcher& target);

    //! Set post resume action to perform after resume.
    void set_post_resume_action(task_dispatcher::post_resume_action pra, void* arg) {
        __TBB_ASSERT(my_post_resume_action == task_dispatcher::post_resume_action::none, "The Post resume action must not be set");
        __TBB_ASSERT(!my_post_resume_arg, "The post resume action must not have an argument");
        my_post_resume_action = pra;
        my_post_resume_arg = arg;
    }

    void clear_post_resume_action() {
        my_post_resume_action = task_dispatcher::post_resume_action::none;
        my_post_resume_arg = nullptr;
    }

    //! The post resume action requested after the swap contexts.
    task_dispatcher::post_resume_action my_post_resume_action;

    //! The post resume action argument.
    void* my_post_resume_arg;
#endif /* __TBB_RESUMABLE_TASKS */

    //! The default context
    // TODO: consider using common default context because it is used only to simplify
    // cancellation check.
    d1::task_group_context my_default_context;
};

inline void thread_data::attach_arena(arena& a, std::size_t index) {
    my_arena = &a;
    my_arena_index = static_cast<unsigned short>(index);
    my_arena_slot = a.my_slots + index;
    // Read the current slot mail_outbox and attach it to the mail_inbox (remove inbox later maybe)
    my_inbox.attach(my_arena->mailbox(index));
}

inline bool thread_data::is_attached_to(arena* a) { return my_arena == a; }

inline void thread_data::attach_task_dispatcher(task_dispatcher& task_disp) {
    __TBB_ASSERT(my_task_dispatcher == nullptr, nullptr);
    __TBB_ASSERT(task_disp.m_thread_data == nullptr, nullptr);
    task_disp.m_thread_data = this;
    my_task_dispatcher = &task_disp;
}

inline void thread_data::detach_task_dispatcher() {
    __TBB_ASSERT(my_task_dispatcher != nullptr, nullptr);
    __TBB_ASSERT(my_task_dispatcher->m_thread_data == this, nullptr);
    my_task_dispatcher->m_thread_data = nullptr;
    my_task_dispatcher = nullptr;
}

inline void thread_data::enter_task_dispatcher(task_dispatcher& task_disp, std::uintptr_t stealing_threshold) {
    task_disp.set_stealing_threshold(stealing_threshold);
    attach_task_dispatcher(task_disp);
}

inline void thread_data::leave_task_dispatcher() {
    my_task_dispatcher->set_stealing_threshold(0);
    detach_task_dispatcher();
}

inline void thread_data::propagate_task_group_state(std::atomic<std::uint32_t> d1::task_group_context::* mptr_state, d1::task_group_context& src, std::uint32_t new_state) {
    mutex::scoped_lock lock(my_context_list->m_mutex);
    // Acquire fence is necessary to ensure that the subsequent node->my_next load
    // returned the correct value in case it was just inserted in another thread.
    // The fence also ensures visibility of the correct ctx.my_parent value.
    for (context_list::iterator it = my_context_list->begin(); it != my_context_list->end(); ++it) {
        d1::task_group_context& ctx = __TBB_get_object_ref(d1::task_group_context, my_node, &(*it));
        if ((ctx.*mptr_state).load(std::memory_order_relaxed) != new_state)
            task_group_context_impl::propagate_task_group_state(ctx, mptr_state, src, new_state);
    }
    // Sync up local propagation epoch with the global one. Release fence prevents
    // reordering of possible store to *mptr_state after the sync point.
    my_context_list->epoch.store(the_context_state_propagation_epoch.load(std::memory_order_relaxed), std::memory_order_release);
}

} // namespace r1
} // namespace detail
} // namespace tbb

#endif // __TBB_thread_data_H

