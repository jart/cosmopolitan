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

#ifndef __TBB_task_group_H
#define __TBB_task_group_H

#include "third_party/tbb/detail/_config.hh"
#include "third_party/tbb/detail/_namespace_injection.hh"
#include "third_party/tbb/detail/_assert.hh"
#include "third_party/tbb/detail/_utils.hh"
#include "third_party/tbb/detail/_template_helpers.hh"
#include "third_party/tbb/detail/_exception.hh"
#include "third_party/tbb/detail/_task.hh"
#include "third_party/tbb/detail/_small_object_pool.hh"
#include "third_party/tbb/detail/_intrusive_list_node.hh"
#include "third_party/tbb/detail/_task_handle.hh"

#include "third_party/tbb/profiling.hh"

#include "third_party/libcxx/type_traits"

#if _MSC_VER && !defined(__INTEL_COMPILER)
    // Suppress warning: structure was padded due to alignment specifier
    #pragma warning(push)
    #pragma warning(disable:4324)
#endif

namespace tbb {
namespace detail {

namespace d1 {
class delegate_base;
class task_arena_base;
class task_group_context;
class task_group_base;
}

namespace r1 {
// Forward declarations
class tbb_exception_ptr;
class cancellation_disseminator;
class thread_data;
class task_dispatcher;
template <bool>
class context_guard_helper;
struct task_arena_impl;
class context_list;

TBB_EXPORT void __TBB_EXPORTED_FUNC execute(d1::task_arena_base&, d1::delegate_base&);
TBB_EXPORT void __TBB_EXPORTED_FUNC isolate_within_arena(d1::delegate_base&, std::intptr_t);

TBB_EXPORT void __TBB_EXPORTED_FUNC initialize(d1::task_group_context&);
TBB_EXPORT void __TBB_EXPORTED_FUNC destroy(d1::task_group_context&);
TBB_EXPORT void __TBB_EXPORTED_FUNC reset(d1::task_group_context&);
TBB_EXPORT bool __TBB_EXPORTED_FUNC cancel_group_execution(d1::task_group_context&);
TBB_EXPORT bool __TBB_EXPORTED_FUNC is_group_execution_cancelled(d1::task_group_context&);
TBB_EXPORT void __TBB_EXPORTED_FUNC capture_fp_settings(d1::task_group_context&);

struct task_group_context_impl;
}

namespace d2 {

namespace {
template<typename F>
d1::task* task_ptr_or_nullptr(F&& f);
}

template<typename F>
class function_task : public task_handle_task  {
    //TODO: apply empty base optimization here
    const F m_func;

private:
    d1::task* execute(d1::execution_data& ed) override {
        __TBB_ASSERT(ed.context == &this->ctx(), "The task group context should be used for all tasks");
        task* res = task_ptr_or_nullptr(m_func);
        finalize(&ed);
        return res;
    }
    d1::task* cancel(d1::execution_data& ed) override {
        finalize(&ed);
        return nullptr;
    }
public:
    template<typename FF>
    function_task(FF&& f, d1::wait_context& wo, d1::task_group_context& ctx, d1::small_object_allocator& alloc)
        : task_handle_task{wo, ctx, alloc},
          m_func(std::forward<FF>(f)) {}
};

#if __TBB_PREVIEW_TASK_GROUP_EXTENSIONS
namespace {
    template<typename F>
    d1::task* task_ptr_or_nullptr_impl(std::false_type, F&& f){
        task_handle th = std::forward<F>(f)();
        return task_handle_accessor::release(th);
    }

    template<typename F>
    d1::task* task_ptr_or_nullptr_impl(std::true_type, F&& f){
        std::forward<F>(f)();
        return nullptr;
    }

    template<typename F>
    d1::task* task_ptr_or_nullptr(F&& f){
        using is_void_t = std::is_void<
            decltype(std::forward<F>(f)())
            >;

        return  task_ptr_or_nullptr_impl(is_void_t{}, std::forward<F>(f));
    }
}
#else
namespace {
    template<typename F>
    d1::task* task_ptr_or_nullptr(F&& f){
        std::forward<F>(f)();
        return nullptr;
    }
}  // namespace
#endif // __TBB_PREVIEW_TASK_GROUP_EXTENSIONS
} // namespace d2

namespace d1 {

// This structure is left here for backward compatibility check
struct context_list_node {
    std::atomic<context_list_node*> prev{};
    std::atomic<context_list_node*> next{};
};

//! Used to form groups of tasks
/** @ingroup task_scheduling
    The context services explicit cancellation requests from user code, and unhandled
    exceptions intercepted during tasks execution. Intercepting an exception results
    in generating internal cancellation requests (which is processed in exactly the
    same way as external ones).

    The context is associated with one or more root tasks and defines the cancellation
    group that includes all the descendants of the corresponding root task(s). Association
    is established when a context object is passed as an argument to the task::allocate_root()
    method. See task_group_context::task_group_context for more details.

    The context can be bound to another one, and other contexts can be bound to it,
    forming a tree-like structure: parent -> this -> children. Arrows here designate
    cancellation propagation direction. If a task in a cancellation group is cancelled
    all the other tasks in this group and groups bound to it (as children) get cancelled too.
**/
class task_group_context : no_copy {
public:
    enum traits_type {
        fp_settings     = 1 << 1,
        concurrent_wait = 1 << 2,
        default_traits  = 0
    };
    enum kind_type {
        isolated,
        bound
    };
private:
    //! Space for platform-specific FPU settings.
    /** Must only be accessed inside TBB binaries, and never directly in user
    code or inline methods. */
    std::uint64_t my_cpu_ctl_env;

    //! Specifies whether cancellation was requested for this task group.
    std::atomic<std::uint32_t> my_cancellation_requested;

    //! Versioning for run-time checks and behavioral traits of the context.
    enum class task_group_context_version : std::uint8_t {
        unused = 1       // ensure that new versions, if any, will not clash with previously used ones
    };
    task_group_context_version my_version;

    //! The context traits.
    struct context_traits {
        bool fp_settings        : 1;
        bool concurrent_wait    : 1;
        bool bound              : 1;
        bool reserved1          : 1;
        bool reserved2          : 1;
        bool reserved3          : 1;
        bool reserved4          : 1;
        bool reserved5          : 1;
    } my_traits;

    static_assert(sizeof(context_traits) == 1, "Traits shall fit into one byte.");

    static constexpr std::uint8_t may_have_children = 1;
    //! The context internal state (currently only may_have_children).
    std::atomic<std::uint8_t> my_may_have_children;

    enum class state : std::uint8_t {
        created,
        locked,
        isolated,
        bound,
        dead,
        proxy = std::uint8_t(-1) //the context is not the real one, but proxy to other one
    };

    //! The synchronization machine state to manage lifetime.
    std::atomic<state> my_state;

    union {
        //! Pointer to the context of the parent cancellation group. nullptr for isolated contexts.
        task_group_context* my_parent;

        //! Pointer to the actual context 'this' context represents a proxy of.
        task_group_context* my_actual_context;
    };

    //! Thread data instance that registered this context in its list.
    r1::context_list* my_context_list;
    static_assert(sizeof(std::atomic<r1::thread_data*>) == sizeof(r1::context_list*), "To preserve backward compatibility these types should have the same size");

    //! Used to form the thread specific list of contexts without additional memory allocation.
    /** A context is included into the list of the current thread when its binding to
        its parent happens. Any context can be present in the list of one thread only. **/
    intrusive_list_node my_node;
    static_assert(sizeof(intrusive_list_node) == sizeof(context_list_node), "To preserve backward compatibility these types should have the same size");

    //! Pointer to the container storing exception being propagated across this task group.
    std::atomic<r1::tbb_exception_ptr*> my_exception;
    static_assert(sizeof(std::atomic<r1::tbb_exception_ptr*>) == sizeof(r1::tbb_exception_ptr*),
        "backward compatibility check");

    //! Used to set and maintain stack stitching point for Intel Performance Tools.
    void* my_itt_caller;

    //! Description of algorithm for scheduler based instrumentation.
    string_resource_index my_name;

    char padding[max_nfs_size
        - sizeof(std::uint64_t)                          // my_cpu_ctl_env
        - sizeof(std::atomic<std::uint32_t>)             // my_cancellation_requested
        - sizeof(std::uint8_t)                           // my_version
        - sizeof(context_traits)                         // my_traits
        - sizeof(std::atomic<std::uint8_t>)              // my_state
        - sizeof(std::atomic<state>)                     // my_state
        - sizeof(task_group_context*)                    // my_parent
        - sizeof(r1::context_list*)                      // my_context_list
        - sizeof(intrusive_list_node)                    // my_node
        - sizeof(std::atomic<r1::tbb_exception_ptr*>)    // my_exception
        - sizeof(void*)                                  // my_itt_caller
        - sizeof(string_resource_index)                  // my_name
    ];

    task_group_context(context_traits t, string_resource_index name)
        : my_version{task_group_context_version::unused}, my_name{name}
    {
        my_traits = t; // GCC4.8 issues warning list initialization for bitset (missing-field-initializers)
        r1::initialize(*this);
    }

    task_group_context(task_group_context* actual_context)
        : my_version{task_group_context_version::unused}
        , my_state{state::proxy}
        , my_actual_context{actual_context}
    {
        __TBB_ASSERT(my_actual_context, "Passed pointer value points to nothing.");
        my_name = actual_context->my_name;

        // no need to initialize 'this' context as it acts as a proxy for my_actual_context, which
        // initialization is a user-side responsibility.
    }

    static context_traits make_traits(kind_type relation_with_parent, std::uintptr_t user_traits) {
        context_traits ct;
        ct.fp_settings = (user_traits & fp_settings) == fp_settings;
        ct.concurrent_wait = (user_traits & concurrent_wait) == concurrent_wait;
        ct.bound = relation_with_parent == bound;
        ct.reserved1 = ct.reserved2 = ct.reserved3 = ct.reserved4 = ct.reserved5 = false;
        return ct;
    }

    bool is_proxy() const {
        return my_state.load(std::memory_order_relaxed) == state::proxy;
    }

    task_group_context& actual_context() noexcept {
        if (is_proxy()) {
            __TBB_ASSERT(my_actual_context, "Actual task_group_context is not set.");
            return *my_actual_context;
        }
        return *this;
    }

    const task_group_context& actual_context() const noexcept {
        if (is_proxy()) {
            __TBB_ASSERT(my_actual_context, "Actual task_group_context is not set.");
            return *my_actual_context;
        }
        return *this;
    }

public:
    //! Default & binding constructor.
    /** By default a bound context is created. That is this context will be bound
        (as child) to the context of the currently executing task . Cancellation
        requests passed to the parent context are propagated to all the contexts
        bound to it. Similarly priority change is propagated from the parent context
        to its children.

        If task_group_context::isolated is used as the argument, then the tasks associated
        with this context will never be affected by events in any other context.

        Creating isolated contexts involve much less overhead, but they have limited
        utility. Normally when an exception occurs in an algorithm that has nested
        ones running, it is desirably to have all the nested algorithms cancelled
        as well. Such a behavior requires nested algorithms to use bound contexts.

        There is one good place where using isolated algorithms is beneficial. It is
        an external thread. That is if a particular algorithm is invoked directly from
        the external thread (not from a TBB task), supplying it with explicitly
        created isolated context will result in a faster algorithm startup.

        VERSIONING NOTE:
        Implementation(s) of task_group_context constructor(s) cannot be made
        entirely out-of-line because the run-time version must be set by the user
        code. This will become critically important for binary compatibility, if
        we ever have to change the size of the context object. **/

    task_group_context(kind_type relation_with_parent = bound,
                       std::uintptr_t t = default_traits)
        : task_group_context(make_traits(relation_with_parent, t), CUSTOM_CTX) {}

    // Custom constructor for instrumentation of oneTBB algorithm
    task_group_context(string_resource_index name )
        : task_group_context(make_traits(bound, default_traits), name) {}

    // Do not introduce any logic on user side since it might break state propagation assumptions
    ~task_group_context() {
        // When 'this' serves as a proxy, the initialization does not happen - nor should the
        // destruction.
        if (!is_proxy())
        {
            r1::destroy(*this);
        }
    }

    //! Forcefully reinitializes the context after the task tree it was associated with is completed.
    /** Because the method assumes that all the tasks that used to be associated with
        this context have already finished, calling it while the context is still
        in use somewhere in the task hierarchy leads to undefined behavior.

        IMPORTANT: This method is not thread safe!

        The method does not change the context's parent if it is set. **/
    void reset() {
        r1::reset(actual_context());
    }

    //! Initiates cancellation of all tasks in this cancellation group and its subordinate groups.
    /** \return false if cancellation has already been requested, true otherwise.

        Note that canceling never fails. When false is returned, it just means that
        another thread (or this one) has already sent cancellation request to this
        context or to one of its ancestors (if this context is bound). It is guaranteed
        that when this method is concurrently called on the same not yet cancelled
        context, true will be returned by one and only one invocation. **/
    bool cancel_group_execution() {
        return r1::cancel_group_execution(actual_context());
    }

    //! Returns true if the context received cancellation request.
    bool is_group_execution_cancelled() {
        return r1::is_group_execution_cancelled(actual_context());
    }

#if __TBB_FP_CONTEXT
    //! Captures the current FPU control settings to the context.
    /** Because the method assumes that all the tasks that used to be associated with
        this context have already finished, calling it while the context is still
        in use somewhere in the task hierarchy leads to undefined behavior.

        IMPORTANT: This method is not thread safe!

        The method does not change the FPU control settings of the context's parent. **/
    void capture_fp_settings() {
        r1::capture_fp_settings(actual_context());
    }
#endif

    //! Returns the user visible context trait
    std::uintptr_t traits() const {
        std::uintptr_t t{};
        const task_group_context& ctx = actual_context();
        t |= ctx.my_traits.fp_settings ? fp_settings : 0;
        t |= ctx.my_traits.concurrent_wait ? concurrent_wait : 0;
        return t;
    }
private:
    //// TODO: cleanup friends
    friend class r1::cancellation_disseminator;
    friend class r1::thread_data;
    friend class r1::task_dispatcher;
    template <bool>
    friend class r1::context_guard_helper;
    friend struct r1::task_arena_impl;
    friend struct r1::task_group_context_impl;
    friend class task_group_base;
}; // class task_group_context

static_assert(sizeof(task_group_context) == 128, "Wrong size of task_group_context");

enum task_group_status {
    not_complete,
    complete,
    canceled
};

class task_group;
class structured_task_group;
#if TBB_PREVIEW_ISOLATED_TASK_GROUP
class isolated_task_group;
#endif

template<typename F>
class function_task : public task {
    const F m_func;
    wait_context& m_wait_ctx;
    small_object_allocator m_allocator;

    void finalize(const execution_data& ed) {
        // Make a local reference not to access this after destruction.
        wait_context& wo = m_wait_ctx;
        // Copy allocator to the stack
        auto allocator = m_allocator;
        // Destroy user functor before release wait.
        this->~function_task();
        wo.release();

        allocator.deallocate(this, ed);
    }
    task* execute(execution_data& ed) override {
        task* res = d2::task_ptr_or_nullptr(m_func);
        finalize(ed);
        return res;
    }
    task* cancel(execution_data& ed) override {
        finalize(ed);
        return nullptr;
    }
public:
    function_task(const F& f, wait_context& wo, small_object_allocator& alloc)
        : m_func(f)
        , m_wait_ctx(wo)
        , m_allocator(alloc) {}

    function_task(F&& f, wait_context& wo, small_object_allocator& alloc)
        : m_func(std::move(f))
        , m_wait_ctx(wo)
        , m_allocator(alloc) {}
};

template <typename F>
class function_stack_task : public task {
    const F& m_func;
    wait_context& m_wait_ctx;

    void finalize() {
        m_wait_ctx.release();
    }
    task* execute(execution_data&) override {
        task* res = d2::task_ptr_or_nullptr(m_func);
        finalize();
        return res;
    }
    task* cancel(execution_data&) override {
        finalize();
        return nullptr;
    }
public:
    function_stack_task(const F& f, wait_context& wo) : m_func(f), m_wait_ctx(wo) {}
};

class task_group_base : no_copy {
protected:
    wait_context m_wait_ctx;
    task_group_context m_context;

    template<typename F>
    task_group_status internal_run_and_wait(const F& f) {
        function_stack_task<F> t{ f, m_wait_ctx };
        m_wait_ctx.reserve();
        bool cancellation_status = false;
        try_call([&] {
            execute_and_wait(t, context(), m_wait_ctx, context());
        }).on_completion([&] {
            // TODO: the reset method is not thread-safe. Ensure the correct behavior.
            cancellation_status = context().is_group_execution_cancelled();
            context().reset();
        });
        return cancellation_status ? canceled : complete;
    }

    task_group_status internal_run_and_wait(d2::task_handle&& h) {
        __TBB_ASSERT(h != nullptr, "Attempt to schedule empty task_handle");

        using acs = d2::task_handle_accessor;
        __TBB_ASSERT(&acs::ctx_of(h) == &context(), "Attempt to schedule task_handle into different task_group");

        bool cancellation_status = false;
        try_call([&] {
            execute_and_wait(*acs::release(h), context(), m_wait_ctx, context());
        }).on_completion([&] {
            // TODO: the reset method is not thread-safe. Ensure the correct behavior.
            cancellation_status = context().is_group_execution_cancelled();
            context().reset();
        });
        return cancellation_status ? canceled : complete;
    }

    template<typename F>
    task* prepare_task(F&& f) {
        m_wait_ctx.reserve();
        small_object_allocator alloc{};
        return alloc.new_object<function_task<typename std::decay<F>::type>>(std::forward<F>(f), m_wait_ctx, alloc);
    }

    task_group_context& context() noexcept {
        return m_context.actual_context();
    }

    template<typename F>
    d2::task_handle prepare_task_handle(F&& f) {
        m_wait_ctx.reserve();
        small_object_allocator alloc{};
        using function_task_t =  d2::function_task<typename std::decay<F>::type>;
        d2::task_handle_task* function_task_p =  alloc.new_object<function_task_t>(std::forward<F>(f), m_wait_ctx, context(), alloc);

        return d2::task_handle_accessor::construct(function_task_p);
    }

public:
    task_group_base(uintptr_t traits = 0)
        : m_wait_ctx(0)
        , m_context(task_group_context::bound, task_group_context::default_traits | traits)
    {}

    task_group_base(task_group_context& ctx)
        : m_wait_ctx(0)
        , m_context(&ctx)
    {}

    ~task_group_base() noexcept(false) {
        if (m_wait_ctx.continue_execution()) {
#if __TBB_CPP17_UNCAUGHT_EXCEPTIONS_PRESENT
            bool stack_unwinding_in_progress = std::uncaught_exceptions() > 0;
#else
            bool stack_unwinding_in_progress = std::uncaught_exception();
#endif
            // Always attempt to do proper cleanup to avoid inevitable memory corruption
            // in case of missing wait (for the sake of better testability & debuggability)
            if (!context().is_group_execution_cancelled())
                cancel();
            d1::wait(m_wait_ctx, context());
            if (!stack_unwinding_in_progress)
                throw_exception(exception_id::missing_wait);
        }
    }

    task_group_status wait() {
        bool cancellation_status = false;
        try_call([&] {
            d1::wait(m_wait_ctx, context());
        }).on_completion([&] {
            // TODO: the reset method is not thread-safe. Ensure the correct behavior.
            cancellation_status = m_context.is_group_execution_cancelled();
            context().reset();
        });
        return cancellation_status ? canceled : complete;
    }

    void cancel() {
        context().cancel_group_execution();
    }
}; // class task_group_base

class task_group : public task_group_base {
public:
    task_group() : task_group_base(task_group_context::concurrent_wait) {}
    task_group(task_group_context& ctx) : task_group_base(ctx) {}

    template<typename F>
    void run(F&& f) {
        spawn(*prepare_task(std::forward<F>(f)), context());
    }

    void run(d2::task_handle&& h) {
        __TBB_ASSERT(h != nullptr, "Attempt to schedule empty task_handle");

        using acs = d2::task_handle_accessor;
        __TBB_ASSERT(&acs::ctx_of(h) == &context(), "Attempt to schedule task_handle into different task_group");

        spawn(*acs::release(h), context());
    }

    template<typename F>
    d2::task_handle defer(F&& f) {
        return prepare_task_handle(std::forward<F>(f));

    }

    template<typename F>
    task_group_status run_and_wait(const F& f) {
        return internal_run_and_wait(f);
    }

    task_group_status run_and_wait(d2::task_handle&& h) {
        return internal_run_and_wait(std::move(h));
    }
}; // class task_group

#if TBB_PREVIEW_ISOLATED_TASK_GROUP
class spawn_delegate : public delegate_base {
    task* task_to_spawn;
    task_group_context& context;
    bool operator()() const override {
        spawn(*task_to_spawn, context);
        return true;
    }
public:
    spawn_delegate(task* a_task, task_group_context& ctx)
        : task_to_spawn(a_task), context(ctx)
    {}
};

class wait_delegate : public delegate_base {
    bool operator()() const override {
        status = tg.wait();
        return true;
    }
protected:
    task_group& tg;
    task_group_status& status;
public:
    wait_delegate(task_group& a_group, task_group_status& tgs)
        : tg(a_group), status(tgs) {}
};

template<typename F>
class run_wait_delegate : public wait_delegate {
    F& func;
    bool operator()() const override {
        status = tg.run_and_wait(func);
        return true;
    }
public:
    run_wait_delegate(task_group& a_group, F& a_func, task_group_status& tgs)
        : wait_delegate(a_group, tgs), func(a_func) {}
};

class isolated_task_group : public task_group {
    intptr_t this_isolation() {
        return reinterpret_cast<intptr_t>(this);
    }
public:
    isolated_task_group() : task_group() {}

    isolated_task_group(task_group_context& ctx) : task_group(ctx) {}

    template<typename F>
    void run(F&& f) {
        spawn_delegate sd(prepare_task(std::forward<F>(f)), context());
        r1::isolate_within_arena(sd, this_isolation());
    }

    void run(d2::task_handle&& h) {
        __TBB_ASSERT(h != nullptr, "Attempt to schedule empty task_handle");

        using acs = d2::task_handle_accessor;
        __TBB_ASSERT(&acs::ctx_of(h) == &context(), "Attempt to schedule task_handle into different task_group");

        spawn_delegate sd(acs::release(h), context());
        r1::isolate_within_arena(sd, this_isolation());
    }

    template<typename F>
    task_group_status run_and_wait( const F& f ) {
        task_group_status result = not_complete;
        run_wait_delegate<const F> rwd(*this, f, result);
        r1::isolate_within_arena(rwd, this_isolation());
        __TBB_ASSERT(result != not_complete, "premature exit from wait?");
        return result;
    }

    task_group_status wait() {
        task_group_status result = not_complete;
        wait_delegate wd(*this, result);
        r1::isolate_within_arena(wd, this_isolation());
        __TBB_ASSERT(result != not_complete, "premature exit from wait?");
        return result;
    }
}; // class isolated_task_group
#endif // TBB_PREVIEW_ISOLATED_TASK_GROUP

inline bool is_current_task_group_canceling() {
    task_group_context* ctx = current_context();
    return ctx ? ctx->is_group_execution_cancelled() : false;
}

} // namespace d1
} // namespace detail

inline namespace v1 {
using detail::d1::task_group_context;
using detail::d1::task_group;
#if TBB_PREVIEW_ISOLATED_TASK_GROUP
using detail::d1::isolated_task_group;
#endif

using detail::d1::task_group_status;
using detail::d1::not_complete;
using detail::d1::complete;
using detail::d1::canceled;

using detail::d1::is_current_task_group_canceling;
using detail::r1::missing_wait;

using detail::d2::task_handle;
}

} // namespace tbb

#if _MSC_VER && !defined(__INTEL_COMPILER)
    #pragma warning(pop) // 4324 warning
#endif

#endif // __TBB_task_group_H
