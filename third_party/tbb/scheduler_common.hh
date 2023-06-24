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

#ifndef _TBB_scheduler_common_H
#define _TBB_scheduler_common_H

#include "third_party/tbb/detail/_utils.hh"
#include "third_party/tbb/detail/_template_helpers.hh"
#include "third_party/tbb/detail/_task.hh"
#include "third_party/tbb/detail/_machine.hh"
#include "third_party/tbb/task_group.hh"
#include "third_party/tbb/cache_aligned_allocator.hh"
#include "third_party/tbb/itt_notify.hh"
#include "third_party/tbb/co_context.hh"
#include "third_party/tbb/misc.hh"
#include "third_party/tbb/governor.hh"

#ifndef __TBB_SCHEDULER_MUTEX_TYPE
#define __TBB_SCHEDULER_MUTEX_TYPE tbb::spin_mutex
#endif
// TODO: add conditional inclusion based on specified type
#include "third_party/tbb/spin_mutex.hh"
#include "third_party/tbb/mutex.hh"

#if TBB_USE_ASSERT
#include "third_party/libcxx/atomic"
#endif

#include "third_party/libcxx/cstdint"
#include "third_party/libcxx/exception"
#include "third_party/libcxx/memory" // unique_ptr

//! Mutex type for global locks in the scheduler
using scheduler_mutex_type = __TBB_SCHEDULER_MUTEX_TYPE;

#if _MSC_VER && !defined(__INTEL_COMPILER)
    // Workaround for overzealous compiler warnings
    // These particular warnings are so ubiquitous that no attempt is made to narrow
    // the scope of the warnings.
    #pragma warning (disable: 4100 4127 4312 4244 4267 4706)
#endif

namespace tbb {
namespace detail {
namespace r1 {

class arena;
class mail_inbox;
class mail_outbox;
class market;
class observer_proxy;

enum task_stream_accessor_type { front_accessor = 0, back_nonnull_accessor };
template<task_stream_accessor_type> class task_stream;

using isolation_type = std::intptr_t;
constexpr isolation_type no_isolation = 0;

struct cache_aligned_deleter {
    template <typename T>
    void operator() (T* ptr) const {
        ptr->~T();
        cache_aligned_deallocate(ptr);
    }
};

template <typename T>
using cache_aligned_unique_ptr = std::unique_ptr<T, cache_aligned_deleter>;

template <typename T, typename ...Args>
cache_aligned_unique_ptr<T> make_cache_aligned_unique(Args&& ...args) {
    return cache_aligned_unique_ptr<T>(new (cache_aligned_allocate(sizeof(T))) T(std::forward<Args>(args)...));
}

//------------------------------------------------------------------------
// Extended execute data
//------------------------------------------------------------------------

//! Execute data used on a task dispatcher side, reflects a current execution state
struct execution_data_ext : d1::execution_data {
    task_dispatcher* task_disp{};
    isolation_type isolation{};
    d1::wait_context* wait_ctx{};
};

//------------------------------------------------------------------------
// Task accessor
//------------------------------------------------------------------------

//! Interpretation of reserved task fields inside a task dispatcher
struct task_accessor {
    static constexpr std::uint64_t proxy_task_trait = 1;
    static constexpr std::uint64_t resume_task_trait = 2;
    static d1::task_group_context*& context(d1::task& t) {
        task_group_context** tgc = reinterpret_cast<task_group_context**>(&t.m_reserved[0]);
        return *tgc;
    }
    static isolation_type& isolation(d1::task& t) {
        isolation_type* tag = reinterpret_cast<isolation_type*>(&t.m_reserved[2]);
        return *tag;
    }
    static void set_proxy_trait(d1::task& t) {
        // TODO: refactor proxy tasks not to work on uninitialized memory.
        //__TBB_ASSERT((t.m_version_and_traits & proxy_task_trait) == 0, nullptr);
        t.m_version_and_traits |= proxy_task_trait;
    }
    static bool is_proxy_task(d1::task& t) {
        return (t.m_version_and_traits & proxy_task_trait) != 0;
    }
    static void set_resume_trait(d1::task& t) {
        __TBB_ASSERT((t.m_version_and_traits & resume_task_trait) == 0, nullptr);
        t.m_version_and_traits |= resume_task_trait;
    }
    static bool is_resume_task(d1::task& t) {
        return (t.m_version_and_traits & resume_task_trait) != 0;
    }
};

//------------------------------------------------------------------------
//! Extended variant of the standard offsetof macro
/** The standard offsetof macro is not sufficient for TBB as it can be used for
    POD-types only. The constant 0x1000 (not nullptr) is necessary to appease GCC. **/
#define __TBB_offsetof(class_name, member_name) \
    ((ptrdiff_t)&(reinterpret_cast<class_name*>(0x1000)->member_name) - 0x1000)

//! Returns address of the object containing a member with the given name and address
#define __TBB_get_object_ref(class_name, member_name, member_addr) \
    (*reinterpret_cast<class_name*>((char*)member_addr - __TBB_offsetof(class_name, member_name)))

//! Helper class for tracking floating point context and task group context switches
/** Assuming presence of an itt collector, in addition to keeping track of floating
    point context, this class emits itt events to indicate begin and end of task group
    context execution **/
template <bool report_tasks>
class context_guard_helper {
    const d1::task_group_context* curr_ctx;
    d1::cpu_ctl_env guard_cpu_ctl_env;
    d1::cpu_ctl_env curr_cpu_ctl_env;
public:
    context_guard_helper() : curr_ctx(nullptr) {
        guard_cpu_ctl_env.get_env();
        curr_cpu_ctl_env = guard_cpu_ctl_env;
    }
    ~context_guard_helper() {
        if (curr_cpu_ctl_env != guard_cpu_ctl_env)
            guard_cpu_ctl_env.set_env();
        if (report_tasks && curr_ctx)
            ITT_TASK_END;
    }
    // The function is called from bypass dispatch loop on the hot path.
    // Consider performance issues when refactoring.
    void set_ctx(const d1::task_group_context* ctx) {
        if (!ctx)
            return;
        const d1::cpu_ctl_env* ctl = reinterpret_cast<const d1::cpu_ctl_env*>(&ctx->my_cpu_ctl_env);
        // Compare the FPU settings directly because the context can be reused between parallel algorithms.
        if (*ctl != curr_cpu_ctl_env) {
            curr_cpu_ctl_env = *ctl;
            curr_cpu_ctl_env.set_env();
        }
        if (report_tasks && ctx != curr_ctx) {
            // if task group context was active, report end of current execution frame.
            if (curr_ctx)
                ITT_TASK_END;
            // reporting begin of new task group context execution frame.
            // using address of task group context object to group tasks (parent).
            // id of task execution frame is nullptr and reserved for future use.
            ITT_TASK_BEGIN(ctx, ctx->my_name, nullptr);
            curr_ctx = ctx;
        }
    }
#if _WIN64
    void restore_default() {
        if (curr_cpu_ctl_env != guard_cpu_ctl_env) {
            guard_cpu_ctl_env.set_env();
            curr_cpu_ctl_env = guard_cpu_ctl_env;
        }
    }
#endif // _WIN64
};

#if (_WIN32 || _WIN64 || __unix__ || __APPLE__) && (__TBB_x86_32 || __TBB_x86_64)
#if _MSC_VER
#pragma intrinsic(__rdtsc)
#endif
inline std::uint64_t machine_time_stamp() {
#if __INTEL_COMPILER
    return _rdtsc();
#elif _MSC_VER
    return __rdtsc();
#else
    std::uint32_t hi, lo;
    __asm__ __volatile__("rdtsc" : "=d"(hi), "=a"(lo));
    return (std::uint64_t(hi) << 32) | lo;
#endif
}

inline void prolonged_pause_impl() {
    // Assumption based on practice: 1000-2000 ticks seems to be a suitable invariant for the
    // majority of platforms. Currently, skip platforms that define __TBB_STEALING_PAUSE
    // because these platforms require very careful tuning.
    std::uint64_t prev = machine_time_stamp();
    const std::uint64_t finish = prev + 1000;
    atomic_backoff backoff;
    do {
        backoff.bounded_pause();
        std::uint64_t curr = machine_time_stamp();
        if (curr <= prev)
            // Possibly, the current logical thread is moved to another hardware thread or overflow is occurred.
            break;
        prev = curr;
    } while (prev < finish);
}
#else
inline void prolonged_pause_impl() {
#ifdef __TBB_ipf
    static const long PauseTime = 1500;
#else
    static const long PauseTime = 80;
#endif
    // TODO IDEA: Update PauseTime adaptively?
    machine_pause(PauseTime);
}
#endif

inline void prolonged_pause() {
#if __TBB_WAITPKG_INTRINSICS_PRESENT
    if (governor::wait_package_enabled()) {
        std::uint64_t time_stamp = machine_time_stamp();
        // _tpause function directs the processor to enter an implementation-dependent optimized state
        // until the Time Stamp Counter reaches or exceeds the value specified in second parameter.
        // Constant "700" is ticks to wait for.
        // First parameter 0 selects between a lower power (cleared) or faster wakeup (set) optimized state.
        _tpause(0, time_stamp + 700);
    }
    else
#endif
    prolonged_pause_impl();
}

// TODO: investigate possibility to work with number of CPU cycles
// because for different configurations this number of pauses + yields
// will be calculated in different amount of CPU cycles
// for example use rdtsc for it
class stealing_loop_backoff {
    const int my_pause_threshold;
    const int my_yield_threshold;
    int my_pause_count;
    int my_yield_count;
public:
    // my_yield_threshold = 100 is an experimental value. Ideally, once we start calling __TBB_Yield(),
    // the time spent spinning before calling out_of_work() should be approximately
    // the time it takes for a thread to be woken up. Doing so would guarantee that we do
    // no worse than 2x the optimal spin time. Or perhaps a time-slice quantum is the right amount.
    stealing_loop_backoff(int num_workers, int yields_multiplier)
        : my_pause_threshold{ 2 * (num_workers + 1) }
#if __APPLE__
        // threshold value tuned separately for macOS due to high cost of sched_yield there
        , my_yield_threshold{10 * yields_multiplier}
#else
        , my_yield_threshold{100 * yields_multiplier}
#endif
        , my_pause_count{}
        , my_yield_count{}
    {}
    bool pause() {
        prolonged_pause();
        if (my_pause_count++ >= my_pause_threshold) {
            my_pause_count = my_pause_threshold;
            d0::yield();
            if (my_yield_count++ >= my_yield_threshold) {
                my_yield_count = my_yield_threshold;
                return true;
            }
        }
        return false;
    }
    void reset_wait() {
        my_pause_count = my_yield_count = 0;
    }
};

//------------------------------------------------------------------------
// Exception support
//------------------------------------------------------------------------
//! Task group state change propagation global epoch
/** Together with generic_scheduler::my_context_state_propagation_epoch forms
    cross-thread signaling mechanism that allows to avoid locking at the hot path
    of normal execution flow.

    When a descendant task group context is registered or unregistered, the global
    and local epochs are compared. If they differ, a state change is being propagated,
    and thus registration/deregistration routines take slower branch that may block
    (at most one thread of the pool can be blocked at any moment). Otherwise the
    control path is lock-free and fast. **/
extern std::atomic<std::uintptr_t> the_context_state_propagation_epoch;

//! Mutex guarding state change propagation across task groups forest.
/** Also protects modification of related data structures. **/
typedef scheduler_mutex_type context_state_propagation_mutex_type;
extern context_state_propagation_mutex_type the_context_state_propagation_mutex;

class tbb_exception_ptr {
    std::exception_ptr my_ptr;
public:
    static tbb_exception_ptr* allocate() noexcept;

    //! Destroys this objects
    /** Note that objects of this type can be created only by the allocate() method. **/
    void destroy() noexcept;

    //! Throws the contained exception .
    void throw_self();

private:
    tbb_exception_ptr(const std::exception_ptr& src) : my_ptr(src) {}
}; // class tbb_exception_ptr

//------------------------------------------------------------------------
// Debugging support
//------------------------------------------------------------------------

#if TBB_USE_ASSERT
static const std::uintptr_t venom = tbb::detail::select_size_t_constant<0xDEADBEEFU, 0xDDEEAADDDEADBEEFULL>::value;

inline void poison_value(std::uintptr_t& val) { val = venom; }

inline void poison_value(std::atomic<std::uintptr_t>& val) { val.store(venom, std::memory_order_relaxed); }

/** Expected to be used in assertions only, thus no empty form is defined. **/
inline bool is_alive(std::uintptr_t v) { return v != venom; }

/** Logically, this method should be a member of class task.
    But we do not want to publish it, so it is here instead. */
inline void assert_task_valid(const d1::task* t) {
    assert_pointer_valid(t);
}
#else /* !TBB_USE_ASSERT */

/** In contrast to debug version poison_value() is a macro here because
    the variable used as its argument may be undefined in release builds. **/
#define poison_value(g) ((void)0)

inline void assert_task_valid(const d1::task*) {}

#endif /* !TBB_USE_ASSERT */

struct suspend_point_type {
#if __TBB_RESUMABLE_TASKS
    //! The arena related to this task_dispatcher
    arena* m_arena{ nullptr };
    //! The random for the resume task
    FastRandom m_random;
    //! The flag is raised when the original owner should return to this task dispatcher.
    std::atomic<bool> m_is_owner_recalled{ false };
    //! Inicates if the resume task should be placed to the critical task stream.
    bool m_is_critical{ false };
    //! Associated coroutine
    co_context m_co_context;
    //! Supend point before resume
    suspend_point_type* m_prev_suspend_point{nullptr};

    // Possible state transitions:
    // A -> S -> N -> A
    // A -> N -> S -> N -> A
    enum class stack_state {
        active, // some thread is working with this stack
        suspended, // no thread is working with this stack
        notified // some thread tried to resume this stack
    };

    //! The flag required to protect suspend finish and resume call
    std::atomic<stack_state> m_stack_state{stack_state::active};

    void resume(suspend_point_type* sp) {
        __TBB_ASSERT(m_stack_state.load(std::memory_order_relaxed) != stack_state::suspended, "The stack is expected to be active");

        sp->m_prev_suspend_point = this;

        // Do not access sp after resume
        m_co_context.resume(sp->m_co_context);
        __TBB_ASSERT(m_stack_state.load(std::memory_order_relaxed) != stack_state::active, nullptr);

        finilize_resume();
    }

    void finilize_resume() {
        m_stack_state.store(stack_state::active, std::memory_order_relaxed);
        // Set the suspended state for the stack that we left. If the state is already notified, it means that 
        // someone already tried to resume our previous stack but failed. So, we need to resume it.
        // m_prev_suspend_point might be nullptr when destroying co_context based on threads
        if (m_prev_suspend_point && m_prev_suspend_point->m_stack_state.exchange(stack_state::suspended) == stack_state::notified) {
            r1::resume(m_prev_suspend_point);
        }
        m_prev_suspend_point = nullptr;
    }

    bool try_notify_resume() {
        // Check that stack is already suspended. Return false if not yet.
        return m_stack_state.exchange(stack_state::notified) == stack_state::suspended;
    }

    void recall_owner() {
        __TBB_ASSERT(m_stack_state.load(std::memory_order_relaxed) == stack_state::suspended, nullptr);
        m_stack_state.store(stack_state::notified, std::memory_order_relaxed);
        m_is_owner_recalled.store(true, std::memory_order_release);
    }

    struct resume_task final : public d1::task {
        task_dispatcher& m_target;
        explicit resume_task(task_dispatcher& target) : m_target(target) {
            task_accessor::set_resume_trait(*this);
        }
        d1::task* execute(d1::execution_data& ed) override;
        d1::task* cancel(d1::execution_data&) override {
            __TBB_ASSERT(false, "The resume task cannot be canceled");
            return nullptr;
        }
    } m_resume_task;

    suspend_point_type(arena* a, std::size_t stack_size, task_dispatcher& target);
#endif /*__TBB_RESUMABLE_TASKS */
};

#if _MSC_VER && !defined(__INTEL_COMPILER)
// structure was padded due to alignment specifier
#pragma warning( push )
#pragma warning( disable: 4324 )
#endif

class alignas (max_nfs_size) task_dispatcher {
public:
    // TODO: reconsider low level design to better organize dependencies and files.
    friend class thread_data;
    friend class arena_slot;
    friend class nested_arena_context;
    friend class delegated_task;
    friend struct base_waiter;

    //! The list of possible post resume actions.
    enum class post_resume_action {
        invalid,
        register_waiter,
        cleanup,
        notify,
        none
    };

    //! The data of the current thread attached to this task_dispatcher
    thread_data* m_thread_data{ nullptr };

    //! The current execution data
    execution_data_ext m_execute_data_ext;

    //! Properties
    struct properties {
        bool outermost{ true };
        bool fifo_tasks_allowed{ true };
        bool critical_task_allowed{ true };
    } m_properties;

    //! Position in the call stack when stealing is still allowed.
    std::uintptr_t m_stealing_threshold{};

    //! Suspend point (null if this task dispatcher has been never suspended)
    suspend_point_type* m_suspend_point{ nullptr };

    //! Attempt to get a task from the mailbox.
    /** Gets a task only if it has not been executed by its sender or a thief
        that has stolen it from the sender's task pool. Otherwise returns nullptr.
        This method is intended to be used only by the thread extracting the proxy
        from its mailbox. (In contrast to local task pool, mailbox can be read only
        by its owner). **/
    d1::task* get_mailbox_task(mail_inbox& my_inbox, execution_data_ext& ed, isolation_type isolation);

    d1::task* get_critical_task(d1::task*, execution_data_ext&, isolation_type, bool);

    template <bool ITTPossible, typename Waiter>
    d1::task* receive_or_steal_task(thread_data& tls, execution_data_ext& ed, Waiter& waiter,
                                isolation_type isolation, bool outermost, bool criticality_absence);

    template <bool ITTPossible, typename Waiter>
    d1::task* local_wait_for_all(d1::task * t, Waiter& waiter);

    task_dispatcher(const task_dispatcher&) = delete;

    bool can_steal();
public:
    task_dispatcher(arena* a);

    ~task_dispatcher() {
        if (m_suspend_point) {
            m_suspend_point->~suspend_point_type();
            cache_aligned_deallocate(m_suspend_point);
        }
        poison_pointer(m_thread_data);
        poison_pointer(m_suspend_point);
    }

    template <typename Waiter>
    d1::task* local_wait_for_all(d1::task* t, Waiter& waiter);

    bool allow_fifo_task(bool new_state) {
        bool old_state = m_properties.fifo_tasks_allowed;
        m_properties.fifo_tasks_allowed = new_state;
        return old_state;
    }

    isolation_type set_isolation(isolation_type isolation) {
        isolation_type prev = m_execute_data_ext.isolation;
        m_execute_data_ext.isolation = isolation;
        return prev;
    }

    thread_data& get_thread_data() {
        __TBB_ASSERT(m_thread_data, nullptr);
        return *m_thread_data;
    }

    static void execute_and_wait(d1::task* t, d1::wait_context& wait_ctx, d1::task_group_context& w_ctx);

    void set_stealing_threshold(std::uintptr_t stealing_threshold) {
        bool assert_condition = (stealing_threshold == 0 && m_stealing_threshold != 0) ||
                                (stealing_threshold != 0 && m_stealing_threshold == 0);
        __TBB_ASSERT_EX( assert_condition, nullptr );
        m_stealing_threshold = stealing_threshold;
    }

    d1::task* get_inbox_or_critical_task(execution_data_ext&, mail_inbox&, isolation_type, bool);
    d1::task* get_stream_or_critical_task(execution_data_ext&, arena&, task_stream<front_accessor>&,
                                      unsigned& /*hint_for_stream*/, isolation_type,
                                      bool /*critical_allowed*/);
    d1::task* steal_or_get_critical(execution_data_ext&, arena&, unsigned /*arena_index*/, FastRandom&,
                                isolation_type, bool /*critical_allowed*/);

#if __TBB_RESUMABLE_TASKS
    /* [[noreturn]] */ void co_local_wait_for_all() noexcept;
    void suspend(suspend_callback_type suspend_callback, void* user_callback);
    void internal_suspend();
    void do_post_resume_action();

    bool resume(task_dispatcher& target);
    suspend_point_type* get_suspend_point();
    void init_suspend_point(arena* a, std::size_t stack_size);
    friend void internal_resume(suspend_point_type*);
    void recall_point();
#endif /* __TBB_RESUMABLE_TASKS */
};

#if _MSC_VER && !defined(__INTEL_COMPILER)
#pragma warning( pop )
#endif

inline std::uintptr_t calculate_stealing_threshold(std::uintptr_t base, std::size_t stack_size) {
    __TBB_ASSERT(stack_size != 0, "Stack size cannot be zero");
    __TBB_ASSERT(base > stack_size / 2, "Stack anchor calculation overflow");
    return base - stack_size / 2;
}

struct task_group_context_impl {
    static void destroy(d1::task_group_context&);
    static void initialize(d1::task_group_context&);
    static void register_with(d1::task_group_context&, thread_data*);
    static void bind_to_impl(d1::task_group_context&, thread_data*);
    static void bind_to(d1::task_group_context&, thread_data*);
    static void propagate_task_group_state(d1::task_group_context&, std::atomic<uint32_t> d1::task_group_context::*, d1::task_group_context&, uint32_t);
    static bool cancel_group_execution(d1::task_group_context&);
    static bool is_group_execution_cancelled(const d1::task_group_context&);
    static void reset(d1::task_group_context&);
    static void capture_fp_settings(d1::task_group_context&);
    static void copy_fp_settings(d1::task_group_context& ctx, const d1::task_group_context& src);
};


//! Forward declaration for scheduler entities
bool gcc_rethrow_exception_broken();
void fix_broken_rethrow();
//! Forward declaration: throws std::runtime_error with what() returning error_code description prefixed with aux_info
void handle_perror(int error_code, const char* aux_info);

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* _TBB_scheduler_common_H */
