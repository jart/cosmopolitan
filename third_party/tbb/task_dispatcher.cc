// clang-format off
/*
    Copyright (c) 2020-2022 Intel Corporation

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

#include "third_party/tbb/task_dispatcher.hh"
#include "third_party/tbb/waiters.hh"

namespace tbb {
namespace detail {
namespace r1 {

static inline void spawn_and_notify(d1::task& t, arena_slot* slot, arena* a) {
    slot->spawn(t);
    a->advertise_new_work<arena::work_spawned>();
    // TODO: TBB_REVAMP_TODO slot->assert_task_pool_valid();
}

void __TBB_EXPORTED_FUNC spawn(d1::task& t, d1::task_group_context& ctx) {
    thread_data* tls = governor::get_thread_data();
    task_group_context_impl::bind_to(ctx, tls);
    arena* a = tls->my_arena;
    arena_slot* slot = tls->my_arena_slot;
    // Capture current context
    task_accessor::context(t) = &ctx;
    // Mark isolation
    task_accessor::isolation(t) = tls->my_task_dispatcher->m_execute_data_ext.isolation;
    spawn_and_notify(t, slot, a);
}

void __TBB_EXPORTED_FUNC spawn(d1::task& t, d1::task_group_context& ctx, d1::slot_id id) {
    thread_data* tls = governor::get_thread_data();
    task_group_context_impl::bind_to(ctx, tls);
    arena* a = tls->my_arena;
    arena_slot* slot = tls->my_arena_slot;
    execution_data_ext& ed = tls->my_task_dispatcher->m_execute_data_ext;

    // Capture context
    task_accessor::context(t) = &ctx;
    // Mark isolation
    task_accessor::isolation(t) = ed.isolation;

    if ( id != d1::no_slot && id != tls->my_arena_index && id < a->my_num_slots) {
        // Allocate proxy task
        d1::small_object_allocator alloc{};
        auto proxy = alloc.new_object<task_proxy>(static_cast<d1::execution_data&>(ed));
        // Mark as a proxy
        task_accessor::set_proxy_trait(*proxy);
        // Mark isolation for the proxy task
        task_accessor::isolation(*proxy) = ed.isolation;
        // Deallocation hint (tls) from the task allocator
        proxy->allocator = alloc;
        proxy->slot = id;
        proxy->outbox = &a->mailbox(id);
        // Mark proxy as present in both locations (sender's task pool and destination mailbox)
        proxy->task_and_tag = intptr_t(&t) | task_proxy::location_mask;
        // Mail the proxy - after this point t may be destroyed by another thread at any moment.
        proxy->outbox->push(proxy);
        // Spawn proxy to the local task pool
        spawn_and_notify(*proxy, slot, a);
    } else {
        spawn_and_notify(t, slot, a);
    }
}

void __TBB_EXPORTED_FUNC submit(d1::task& t, d1::task_group_context& ctx, arena* a, std::uintptr_t as_critical) {
    suppress_unused_warning(as_critical);
    assert_pointer_valid(a);
    thread_data& tls = *governor::get_thread_data();

    // TODO revamp: for each use case investigate neccesity to make this call
    task_group_context_impl::bind_to(ctx, &tls);
    task_accessor::context(t) = &ctx;
    // TODO revamp: consider respecting task isolation if this call is being made by external thread
    task_accessor::isolation(t) = tls.my_task_dispatcher->m_execute_data_ext.isolation;

    // TODO: consider code refactoring when lane selection mechanism is unified.

    if ( tls.is_attached_to(a) ) {
        arena_slot* slot = tls.my_arena_slot;
#if __TBB_PREVIEW_CRITICAL_TASKS
        if( as_critical ) {
            a->my_critical_task_stream.push( &t, subsequent_lane_selector(slot->critical_hint()) );
        } else
#endif
        {
            slot->spawn(t);
        }
    } else {
        random_lane_selector lane_selector{tls.my_random};
#if !__TBB_PREVIEW_CRITICAL_TASKS
        suppress_unused_warning(as_critical);
#else
        if ( as_critical ) {
            a->my_critical_task_stream.push( &t, lane_selector );
        } else
#endif
        {
            // Avoid joining the arena the thread is not currently in.
            a->my_fifo_task_stream.push( &t, lane_selector );
        }
    }
    // It is assumed that some thread will explicitly wait in the arena the task is submitted
    // into. Therefore, no need to utilize mandatory concurrency here.
    a->advertise_new_work<arena::work_spawned>();
}

void __TBB_EXPORTED_FUNC execute_and_wait(d1::task& t, d1::task_group_context& t_ctx, d1::wait_context& wait_ctx, d1::task_group_context& w_ctx) {
    task_accessor::context(t) = &t_ctx;
    task_dispatcher::execute_and_wait(&t, wait_ctx, w_ctx);
}

void __TBB_EXPORTED_FUNC wait(d1::wait_context& wait_ctx, d1::task_group_context& w_ctx) {
    // Enter the task dispatch loop without a task
    task_dispatcher::execute_and_wait(nullptr, wait_ctx, w_ctx);
}

d1::slot_id __TBB_EXPORTED_FUNC execution_slot(const d1::execution_data* ed) {
    if (ed) {
        const execution_data_ext* ed_ext = static_cast<const execution_data_ext*>(ed);
        assert_pointers_valid(ed_ext->task_disp, ed_ext->task_disp->m_thread_data);
        return ed_ext->task_disp->m_thread_data->my_arena_index;
    } else {
        thread_data* td = governor::get_thread_data_if_initialized();
        return td ? td->my_arena_index : d1::slot_id(-1);
    }
}

d1::task_group_context* __TBB_EXPORTED_FUNC current_context() {
    thread_data* td = governor::get_thread_data();
    assert_pointers_valid(td, td->my_task_dispatcher);

    task_dispatcher* task_disp = td->my_task_dispatcher;
    if (task_disp->m_properties.outermost) {
        // No one task is executed, so no execute_data.
        return nullptr;
    } else {
        return td->my_task_dispatcher->m_execute_data_ext.context;
    }
}

void task_dispatcher::execute_and_wait(d1::task* t, d1::wait_context& wait_ctx, d1::task_group_context& w_ctx) {
    // Get an associated task dispatcher
    thread_data* tls = governor::get_thread_data();
    __TBB_ASSERT(tls->my_task_dispatcher != nullptr, nullptr);
    task_dispatcher& local_td = *tls->my_task_dispatcher;

    // TODO: factor out the binding to execute_and_wait_impl
    if (t) {
        task_group_context_impl::bind_to(*task_accessor::context(*t), tls);
        // Propagate the isolation to the task executed without spawn.
        task_accessor::isolation(*t) = tls->my_task_dispatcher->m_execute_data_ext.isolation;
    }

    // Waiting on special object tied to a waiting thread.
    external_waiter waiter{ *tls->my_arena, wait_ctx };
    t = local_td.local_wait_for_all(t, waiter);
    __TBB_ASSERT_EX(t == nullptr, "External waiter must not leave dispatch loop with a task");

    // The external thread couldn't exit the dispatch loop in an idle state
    if (local_td.m_thread_data->my_inbox.is_idle_state(true)) {
        local_td.m_thread_data->my_inbox.set_is_idle(false);
    }

    auto exception = w_ctx.my_exception.load(std::memory_order_acquire);
    if (exception) {
        __TBB_ASSERT(w_ctx.is_group_execution_cancelled(), "The task group context with an exception should be canceled.");
        exception->throw_self();
    }
}

#if __TBB_RESUMABLE_TASKS

#if _WIN32
/* [[noreturn]] */ void __stdcall co_local_wait_for_all(void* addr) noexcept
#else
/* [[noreturn]] */ void co_local_wait_for_all(unsigned hi, unsigned lo) noexcept
#endif
{
#if !_WIN32
    std::uintptr_t addr = lo;
    __TBB_ASSERT(sizeof(addr) == 8 || hi == 0, nullptr);
    addr += std::uintptr_t(std::uint64_t(hi) << 32);
#endif
    task_dispatcher& task_disp = *reinterpret_cast<task_dispatcher*>(addr);
    assert_pointers_valid(task_disp.m_thread_data, task_disp.m_thread_data->my_arena);
    task_disp.set_stealing_threshold(task_disp.m_thread_data->my_arena->calculate_stealing_threshold());
    __TBB_ASSERT(task_disp.can_steal(), nullptr);
    task_disp.co_local_wait_for_all();
    // This code is unreachable
}

/* [[noreturn]] */ void task_dispatcher::co_local_wait_for_all() noexcept {
    // Do not create non-trivial objects on the stack of this function. They will never be destroyed.
    assert_pointer_valid(m_thread_data);

    m_suspend_point->finilize_resume();
    // Basically calls the user callback passed to the tbb::task::suspend function
    do_post_resume_action();

    // Endless loop here because coroutine could be reused
    d1::task* resume_task{};
    do {
        arena* a = m_thread_data->my_arena;
        coroutine_waiter waiter(*a);
        resume_task = local_wait_for_all(nullptr, waiter);
        assert_task_valid(resume_task);
        __TBB_ASSERT(this == m_thread_data->my_task_dispatcher, nullptr);

        m_thread_data->set_post_resume_action(post_resume_action::cleanup, this);

    } while (resume(static_cast<suspend_point_type::resume_task*>(resume_task)->m_target));
    // This code might be unreachable
}

d1::suspend_point task_dispatcher::get_suspend_point() {
    if (m_suspend_point == nullptr) {
        assert_pointer_valid(m_thread_data);
        // 0 means that we attach this task dispatcher to the current stack
        init_suspend_point(m_thread_data->my_arena, 0);
    }
    assert_pointer_valid(m_suspend_point);
    return m_suspend_point;
}
void task_dispatcher::init_suspend_point(arena* a, std::size_t stack_size) {
    __TBB_ASSERT(m_suspend_point == nullptr, nullptr);
    m_suspend_point = new(cache_aligned_allocate(sizeof(suspend_point_type)))
        suspend_point_type(a, stack_size, *this);
}
#endif /* __TBB_RESUMABLE_TASKS */
} // namespace r1
} // namespace detail
} // namespace tbb
