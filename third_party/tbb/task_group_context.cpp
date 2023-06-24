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

#include "third_party/tbb/detail/_config.h"
#include "third_party/tbb/tbb_allocator.h"
#include "third_party/tbb/task_group.h"
#include "third_party/tbb/governor.h"
#include "third_party/tbb/thread_data.h"
#include "third_party/tbb/scheduler_common.h"
#include "third_party/tbb/itt_notify.h"
#include "third_party/tbb/task_dispatcher.h"

#include "third_party/libcxx/type_traits"

namespace tbb {
namespace detail {
namespace r1 {

//------------------------------------------------------------------------
// tbb_exception_ptr
//------------------------------------------------------------------------
tbb_exception_ptr* tbb_exception_ptr::allocate() noexcept {
    tbb_exception_ptr* eptr = (tbb_exception_ptr*)allocate_memory(sizeof(tbb_exception_ptr));
    return eptr ? new (eptr) tbb_exception_ptr(std::current_exception()) : nullptr;
}

void tbb_exception_ptr::destroy() noexcept {
    this->~tbb_exception_ptr();
    deallocate_memory(this);
}

void tbb_exception_ptr::throw_self() {
    if (governor::rethrow_exception_broken()) fix_broken_rethrow();
    std::rethrow_exception(my_ptr);
}

//------------------------------------------------------------------------
// task_group_context
//------------------------------------------------------------------------

void task_group_context_impl::destroy(d1::task_group_context& ctx) {
    __TBB_ASSERT(!is_poisoned(ctx.my_context_list), nullptr);

    if (ctx.my_context_list != nullptr) {
        __TBB_ASSERT(ctx.my_state.load(std::memory_order_relaxed) == d1::task_group_context::state::bound, nullptr);
        // The owner can be destroyed at any moment. Access the associate data with caution.
        ctx.my_context_list->remove(ctx.my_node);
    }
    d1::cpu_ctl_env* ctl = reinterpret_cast<d1::cpu_ctl_env*>(&ctx.my_cpu_ctl_env);
#if _MSC_VER && _MSC_VER <= 1900 && !__INTEL_COMPILER
    suppress_unused_warning(ctl);
#endif
    ctl->~cpu_ctl_env();

    auto exception = ctx.my_exception.load(std::memory_order_relaxed);
    if (exception) {
        exception->destroy();
    }
    ITT_STACK_DESTROY(ctx.my_itt_caller);

    poison_pointer(ctx.my_parent);
    poison_pointer(ctx.my_context_list);
    poison_pointer(ctx.my_node.my_next_node);
    poison_pointer(ctx.my_node.my_prev_node);
    poison_pointer(ctx.my_exception);
    poison_pointer(ctx.my_itt_caller);

    ctx.my_state.store(d1::task_group_context::state::dead, std::memory_order_release);
}

void task_group_context_impl::initialize(d1::task_group_context& ctx) {
    ITT_TASK_GROUP(&ctx, ctx.my_name, nullptr);

    ctx.my_node.my_next_node = &ctx.my_node;
    ctx.my_node.my_prev_node = &ctx.my_node;
    ctx.my_cpu_ctl_env = 0;
    ctx.my_cancellation_requested = 0;
    ctx.my_may_have_children.store(0, std::memory_order_relaxed);
    // Set the created state to bound at the first usage.
    ctx.my_state.store(d1::task_group_context::state::created, std::memory_order_relaxed);
    ctx.my_parent = nullptr;
    ctx.my_context_list = nullptr;
    ctx.my_exception.store(nullptr, std::memory_order_relaxed);
    ctx.my_itt_caller = nullptr;

    static_assert(sizeof(d1::cpu_ctl_env) <= sizeof(ctx.my_cpu_ctl_env), "FPU settings storage does not fit to uint64_t");
    d1::cpu_ctl_env* ctl = new (&ctx.my_cpu_ctl_env) d1::cpu_ctl_env;
    if (ctx.my_traits.fp_settings)
        ctl->get_env();
}

void task_group_context_impl::register_with(d1::task_group_context& ctx, thread_data* td) {
    __TBB_ASSERT(!is_poisoned(ctx.my_context_list), nullptr);
    __TBB_ASSERT(td, nullptr);
    ctx.my_context_list = td->my_context_list;

    ctx.my_context_list->push_front(ctx.my_node);
}

void task_group_context_impl::bind_to_impl(d1::task_group_context& ctx, thread_data* td) {
    __TBB_ASSERT(!is_poisoned(ctx.my_context_list), nullptr);
    __TBB_ASSERT(ctx.my_state.load(std::memory_order_relaxed) == d1::task_group_context::state::locked, "The context can be bound only under the lock.");
    __TBB_ASSERT(!ctx.my_parent, "Parent is set before initial binding");

    ctx.my_parent = td->my_task_dispatcher->m_execute_data_ext.context;
    __TBB_ASSERT(ctx.my_parent, nullptr);

    // Inherit FPU settings only if the context has not captured FPU settings yet.
    if (!ctx.my_traits.fp_settings)
        copy_fp_settings(ctx, *ctx.my_parent);

    // Condition below prevents unnecessary thrashing parent context's cache line
    if (ctx.my_parent->my_may_have_children.load(std::memory_order_relaxed) != d1::task_group_context::may_have_children) {
        ctx.my_parent->my_may_have_children.store(d1::task_group_context::may_have_children, std::memory_order_relaxed); // full fence is below
    }
    if (ctx.my_parent->my_parent) {
        // Even if this context were made accessible for state change propagation
        // (by placing store_with_release(td->my_context_list_state.head.my_next, &ctx.my_node)
        // above), it still could be missed if state propagation from a grand-ancestor
        // was underway concurrently with binding.
        // Speculative propagation from the parent together with epoch counters
        // detecting possibility of such a race allow to avoid taking locks when
        // there is no contention.

        // Acquire fence is necessary to prevent reordering subsequent speculative
        // loads of parent state data out of the scope where epoch counters comparison
        // can reliably validate it.
        uintptr_t local_count_snapshot = ctx.my_parent->my_context_list->epoch.load(std::memory_order_acquire);
        // Speculative propagation of parent's state. The speculation will be
        // validated by the epoch counters check further on.
        ctx.my_cancellation_requested.store(ctx.my_parent->my_cancellation_requested.load(std::memory_order_relaxed), std::memory_order_relaxed);
        register_with(ctx, td); // Issues full fence

        // If no state propagation was detected by the following condition, the above
        // full fence guarantees that the parent had correct state during speculative
        // propagation before the fence. Otherwise the propagation from parent is
        // repeated under the lock.
        if (local_count_snapshot != the_context_state_propagation_epoch.load(std::memory_order_relaxed)) {
            // Another thread may be propagating state change right now. So resort to lock.
            context_state_propagation_mutex_type::scoped_lock lock(the_context_state_propagation_mutex);
            ctx.my_cancellation_requested.store(ctx.my_parent->my_cancellation_requested.load(std::memory_order_relaxed), std::memory_order_relaxed);
        }
    } else {
        register_with(ctx, td); // Issues full fence
        // As we do not have grand-ancestors, concurrent state propagation (if any)
        // may originate only from the parent context, and thus it is safe to directly
        // copy the state from it.
        ctx.my_cancellation_requested.store(ctx.my_parent->my_cancellation_requested.load(std::memory_order_relaxed), std::memory_order_relaxed);
    }
}

void task_group_context_impl::bind_to(d1::task_group_context& ctx, thread_data* td) {
    d1::task_group_context::state state = ctx.my_state.load(std::memory_order_acquire);
    if (state <= d1::task_group_context::state::locked) {
        if (state == d1::task_group_context::state::created &&
#if defined(__INTEL_COMPILER) && __INTEL_COMPILER <= 1910
            ((std::atomic<typename std::underlying_type<d1::task_group_context::state>::type>&)ctx.my_state).compare_exchange_strong(
                (typename std::underlying_type<d1::task_group_context::state>::type&)state,
                (typename std::underlying_type<d1::task_group_context::state>::type)d1::task_group_context::state::locked)
#else
            ctx.my_state.compare_exchange_strong(state, d1::task_group_context::state::locked)
#endif
            ) {
            // If we are in the outermost task dispatch loop of an external thread, then
            // there is nothing to bind this context to, and we skip the binding part
            // treating the context as isolated.
            __TBB_ASSERT(td->my_task_dispatcher->m_execute_data_ext.context != nullptr, nullptr);
            d1::task_group_context::state release_state{};
            if (td->my_task_dispatcher->m_execute_data_ext.context == td->my_arena->my_default_ctx || !ctx.my_traits.bound) {
                if (!ctx.my_traits.fp_settings) {
                    copy_fp_settings(ctx, *td->my_arena->my_default_ctx);
                }
                release_state = d1::task_group_context::state::isolated;
            } else {
                bind_to_impl(ctx, td);
                release_state = d1::task_group_context::state::bound;
            }
            ITT_STACK_CREATE(ctx.my_itt_caller);
            ctx.my_state.store(release_state, std::memory_order_release);
        }
        spin_wait_while_eq(ctx.my_state, d1::task_group_context::state::locked);
    }
    __TBB_ASSERT(ctx.my_state.load(std::memory_order_relaxed) != d1::task_group_context::state::created, nullptr);
    __TBB_ASSERT(ctx.my_state.load(std::memory_order_relaxed) != d1::task_group_context::state::locked, nullptr);
}

void task_group_context_impl::propagate_task_group_state(d1::task_group_context& ctx, std::atomic<std::uint32_t> d1::task_group_context::* mptr_state, d1::task_group_context& src, std::uint32_t new_state) {
    __TBB_ASSERT(!is_poisoned(ctx.my_context_list), nullptr);
    /*  1. if ((ctx.*mptr_state).load(std::memory_order_relaxed) == new_state):
            Nothing to do, whether descending from "src" or not, so no need to scan.
            Hopefully this happens often thanks to earlier invocations.
            This optimization is enabled by LIFO order in the context lists:
                - new contexts are bound to the beginning of lists;
                - descendants are newer than ancestors;
                - earlier invocations are therefore likely to "paint" long chains.
        2. if (&ctx != &src):
            This clause is disjunct from the traversal below, which skips src entirely.
            Note that src.*mptr_state is not necessarily still equal to new_state (another thread may have changed it again).
            Such interference is probably not frequent enough to aim for optimisation by writing new_state again (to make the other thread back down).
            Letting the other thread prevail may also be fairer.
    */
    if ((ctx.*mptr_state).load(std::memory_order_relaxed) != new_state && &ctx != &src) {
        for (d1::task_group_context* ancestor = ctx.my_parent; ancestor != nullptr; ancestor = ancestor->my_parent) {
            if (ancestor == &src) {
                for (d1::task_group_context* c = &ctx; c != ancestor; c = c->my_parent)
                    (c->*mptr_state).store(new_state, std::memory_order_relaxed);
                break;
            }
        }
    }
}

bool task_group_context_impl::cancel_group_execution(d1::task_group_context& ctx) {
    __TBB_ASSERT(!is_poisoned(ctx.my_context_list), nullptr);
    __TBB_ASSERT(ctx.my_cancellation_requested.load(std::memory_order_relaxed) <= 1, "The cancellation state can be either 0 or 1");
    if (ctx.my_cancellation_requested.load(std::memory_order_relaxed) || ctx.my_cancellation_requested.exchange(1)) {
        // This task group and any descendants have already been canceled.
        // (A newly added descendant would inherit its parent's ctx.my_cancellation_requested,
        // not missing out on any cancellation still being propagated, and a context cannot be uncanceled.)
        return false;
    }
    governor::get_thread_data()->my_arena->my_threading_control->propagate_task_group_state(&d1::task_group_context::my_cancellation_requested, ctx, uint32_t(1));
    return true;
}

bool task_group_context_impl::is_group_execution_cancelled(const d1::task_group_context& ctx) {
    return ctx.my_cancellation_requested.load(std::memory_order_relaxed) != 0;
}

// IMPORTANT: It is assumed that this method is not used concurrently!
void task_group_context_impl::reset(d1::task_group_context& ctx) {
    __TBB_ASSERT(!is_poisoned(ctx.my_context_list), nullptr);
    //! TODO: Add assertion that this context does not have children
    // No fences are necessary since this context can be accessed from another thread
    // only after stealing happened (which means necessary fences were used).

    auto exception = ctx.my_exception.load(std::memory_order_relaxed);
    if (exception) {
        exception->destroy();
        ctx.my_exception.store(nullptr, std::memory_order_relaxed);
    }
    ctx.my_cancellation_requested = 0;
}

// IMPORTANT: It is assumed that this method is not used concurrently!
void task_group_context_impl::capture_fp_settings(d1::task_group_context& ctx) {
    __TBB_ASSERT(!is_poisoned(ctx.my_context_list), nullptr);
    //! TODO: Add assertion that this context does not have children
    // No fences are necessary since this context can be accessed from another thread
    // only after stealing happened (which means necessary fences were used).
    d1::cpu_ctl_env* ctl = reinterpret_cast<d1::cpu_ctl_env*>(&ctx.my_cpu_ctl_env);
    if (!ctx.my_traits.fp_settings) {
        ctl = new (&ctx.my_cpu_ctl_env) d1::cpu_ctl_env;
        ctx.my_traits.fp_settings = true;
    }
    ctl->get_env();
}

void task_group_context_impl::copy_fp_settings(d1::task_group_context& ctx, const d1::task_group_context& src) {
    __TBB_ASSERT(!is_poisoned(ctx.my_context_list), nullptr);
    __TBB_ASSERT(!ctx.my_traits.fp_settings, "The context already has FPU settings.");
    __TBB_ASSERT(src.my_traits.fp_settings, "The source context does not have FPU settings.");

    const d1::cpu_ctl_env* src_ctl = reinterpret_cast<const d1::cpu_ctl_env*>(&src.my_cpu_ctl_env);
    new (&ctx.my_cpu_ctl_env) d1::cpu_ctl_env(*src_ctl);
    ctx.my_traits.fp_settings = true;
}

/*
    Comments:

1.  The premise of the cancellation support implementation is that cancellations are
    not part of the hot path of the program execution. Therefore all changes in its
    implementation in order to reduce the overhead of the cancellation control flow
    should be done only in ways that do not increase overhead of the normal execution.

    In general, contexts are used by all threads and their descendants are created in
    different threads as well. In order to minimize impact of the cross-thread tree
    maintenance (first of all because of the synchronization), the tree of contexts
    is split into pieces, each of which is handled by a single thread. Such pieces
    are represented as lists of contexts, members of which are contexts that were
    bound to their parents in the given thread.

    The context tree maintenance and cancellation propagation algorithms are designed
    in such a manner that cross-thread access to a context list will take place only
    when cancellation signal is sent (by user or when an exception happens), and
    synchronization is necessary only then. Thus the normal execution flow (without
    exceptions and cancellation) remains free from any synchronization done on
    behalf of exception handling and cancellation support.

2.  Consider parallel cancellations at the different levels of the context tree:

        Ctx1 <- Cancelled by Thread1            |- Thread2 started processing
         |                                      |
        Ctx2                                    |- Thread1 started processing
         |                                   T1 |- Thread2 finishes and syncs up local counters
        Ctx3 <- Cancelled by Thread2            |
         |                                      |- Ctx5 is bound to Ctx2
        Ctx4                                    |
                                             T2 |- Thread1 reaches Ctx2

    Thread-propagator of each cancellation increments global counter. However the thread
    propagating the cancellation from the outermost context (Thread1) may be the last
    to finish. Which means that the local counters may be synchronized earlier (by Thread2,
    at Time1) than it propagated cancellation into Ctx2 (at time Time2). If a new context
    (Ctx5) is created and bound to Ctx2 between Time1 and Time2, checking its parent only
    (Ctx2) may result in cancellation request being lost.

    This issue is solved by doing the whole propagation under the lock.

    If we need more concurrency while processing parallel cancellations, we could try
    the following modification of the propagation algorithm:

    advance global counter and remember it
    for each thread:
        scan thread's list of contexts
    for each thread:
        sync up its local counter only if the global counter has not been changed

    However this version of the algorithm requires more analysis and verification.
*/

void __TBB_EXPORTED_FUNC initialize(d1::task_group_context& ctx) {
    task_group_context_impl::initialize(ctx);
}
void __TBB_EXPORTED_FUNC destroy(d1::task_group_context& ctx) {
    task_group_context_impl::destroy(ctx);
}
void __TBB_EXPORTED_FUNC reset(d1::task_group_context& ctx) {
    task_group_context_impl::reset(ctx);
}
bool __TBB_EXPORTED_FUNC cancel_group_execution(d1::task_group_context& ctx) {
    return task_group_context_impl::cancel_group_execution(ctx);
}
bool __TBB_EXPORTED_FUNC is_group_execution_cancelled(d1::task_group_context& ctx) {
    return task_group_context_impl::is_group_execution_cancelled(ctx);
}
void __TBB_EXPORTED_FUNC capture_fp_settings(d1::task_group_context& ctx) {
    task_group_context_impl::capture_fp_settings(ctx);
}

} // namespace r1
} // namespace detail
} // namespace tbb

