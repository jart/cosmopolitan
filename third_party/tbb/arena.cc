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

#include "third_party/tbb/task_dispatcher.hh"
#include "third_party/tbb/governor.hh"
#include "third_party/tbb/threading_control.hh"
#include "third_party/tbb/arena.hh"
#include "third_party/tbb/itt_notify.hh"
#include "third_party/tbb/semaphore.hh"
#include "third_party/tbb/waiters.hh"
#include "third_party/tbb/detail/_task.hh"
#include "third_party/tbb/info.hh"
#include "third_party/tbb/tbb_allocator.hh"

#include "third_party/libcxx/atomic"
#include "third_party/libcxx/cstring"
#include "third_party/libcxx/functional"

namespace tbb {
namespace detail {
namespace r1 {

#if __TBB_ARENA_BINDING
class numa_binding_observer : public tbb::task_scheduler_observer {
    binding_handler* my_binding_handler;
public:
    numa_binding_observer( d1::task_arena* ta, int num_slots, int numa_id, core_type_id core_type, int max_threads_per_core )
        : task_scheduler_observer(*ta)
        , my_binding_handler(construct_binding_handler(num_slots, numa_id, core_type, max_threads_per_core))
    {}

    void on_scheduler_entry( bool ) override {
        apply_affinity_mask(my_binding_handler, this_task_arena::current_thread_index());
    }

    void on_scheduler_exit( bool ) override {
        restore_affinity_mask(my_binding_handler, this_task_arena::current_thread_index());
    }

    ~numa_binding_observer() override{
        destroy_binding_handler(my_binding_handler);
    }
};

numa_binding_observer* construct_binding_observer( d1::task_arena* ta, int num_slots, int numa_id, core_type_id core_type, int max_threads_per_core ) {
    numa_binding_observer* binding_observer = nullptr;
    if ((core_type >= 0 && core_type_count() > 1) || (numa_id >= 0 && numa_node_count() > 1) || max_threads_per_core > 0) {
        binding_observer = new(allocate_memory(sizeof(numa_binding_observer))) numa_binding_observer(ta, num_slots, numa_id, core_type, max_threads_per_core);
        __TBB_ASSERT(binding_observer, "Failure during NUMA binding observer allocation and construction");
        binding_observer->observe(true);
    }
    return binding_observer;
}

void destroy_binding_observer( numa_binding_observer* binding_observer ) {
    __TBB_ASSERT(binding_observer, "Trying to deallocate nullptr pointer");
    binding_observer->observe(false);
    binding_observer->~numa_binding_observer();
    deallocate_memory(binding_observer);
}
#endif /*!__TBB_ARENA_BINDING*/

void arena::on_thread_leaving(unsigned ref_param) {
    //
    // Implementation of arena destruction synchronization logic contained various
    // bugs/flaws at the different stages of its evolution, so below is a detailed
    // description of the issues taken into consideration in the framework of the
    // current design.
    //
    // In case of using fire-and-forget tasks (scheduled via task::enqueue())
    // external thread is allowed to leave its arena before all its work is executed,
    // and market may temporarily revoke all workers from this arena. Since revoked
    // workers never attempt to reset arena state to EMPTY and cancel its request
    // to RML for threads, the arena object is destroyed only when both the last
    // thread is leaving it and arena's state is EMPTY (that is its external thread
    // left and it does not contain any work).
    // Thus resetting arena to EMPTY state (as earlier TBB versions did) should not
    // be done here (or anywhere else in the external thread to that matter); doing so
    // can result either in arena's premature destruction (at least without
    // additional costly checks in workers) or in unnecessary arena state changes
    // (and ensuing workers migration).
    //
    // A worker that checks for work presence and transitions arena to the EMPTY
    // state (in snapshot taking procedure arena::out_of_work()) updates
    // arena::my_pool_state first and only then arena::my_num_workers_requested.
    // So the check for work absence must be done against the latter field.
    //
    // In a time window between decrementing the active threads count and checking
    // if there is an outstanding request for workers. New worker thread may arrive,
    // finish remaining work, set arena state to empty, and leave decrementing its
    // refcount and destroying. Then the current thread will destroy the arena
    // the second time. To preclude it a local copy of the outstanding request
    // value can be stored before decrementing active threads count.
    //
    // But this technique may cause two other problem. When the stored request is
    // zero, it is possible that arena still has threads and they can generate new
    // tasks and thus re-establish non-zero requests. Then all the threads can be
    // revoked (as described above) leaving this thread the last one, and causing
    // it to destroy non-empty arena.
    //
    // The other problem takes place when the stored request is non-zero. Another
    // thread may complete the work, set arena state to empty, and leave without
    // arena destruction before this thread decrements the refcount. This thread
    // cannot destroy the arena either. Thus the arena may be "orphaned".
    //
    // In both cases we cannot dereference arena pointer after the refcount is
    // decremented, as our arena may already be destroyed.
    //
    // If this is the external thread, the market is protected by refcount to it.
    // In case of workers market's liveness is ensured by the RML connection
    // rundown protocol, according to which the client (i.e. the market) lives
    // until RML server notifies it about connection termination, and this
    // notification is fired only after all workers return into RML.
    //
    // Thus if we decremented refcount to zero we ask the market to check arena
    // state (including the fact if it is alive) under the lock.
    //

    __TBB_ASSERT(my_references.load(std::memory_order_relaxed) >= ref_param, "broken arena reference counter");

    // When there is no workers someone must free arena, as
    // without workers, no one calls out_of_work().
    if (ref_param == ref_external && !my_mandatory_concurrency.test()) {
        out_of_work();
    }

    threading_control* tc = my_threading_control;
    auto tc_client_snapshot = tc->prepare_client_destruction(my_tc_client);
    // Release our reference to sync with destroy_client
    unsigned remaining_ref = my_references.fetch_sub(ref_param, std::memory_order_release) - ref_param;
    // do not access `this` it might be destroyed already
    if (remaining_ref == 0) {
        if (tc->try_destroy_client(tc_client_snapshot)) {
            // We are requested to destroy ourself
            free_arena();
        }
    }
}

std::size_t arena::occupy_free_slot_in_range( thread_data& tls, std::size_t lower, std::size_t upper ) {
    if ( lower >= upper ) return out_of_arena;
    // Start search for an empty slot from the one we occupied the last time
    std::size_t index = tls.my_arena_index;
    if ( index < lower || index >= upper ) index = tls.my_random.get() % (upper - lower) + lower;
    __TBB_ASSERT( index >= lower && index < upper, nullptr);
    // Find a free slot
    for ( std::size_t i = index; i < upper; ++i )
        if (my_slots[i].try_occupy()) return i;
    for ( std::size_t i = lower; i < index; ++i )
        if (my_slots[i].try_occupy()) return i;
    return out_of_arena;
}

template <bool as_worker>
std::size_t arena::occupy_free_slot(thread_data& tls) {
    // Firstly, external threads try to occupy reserved slots
    std::size_t index = as_worker ? out_of_arena : occupy_free_slot_in_range( tls,  0, my_num_reserved_slots );
    if ( index == out_of_arena ) {
        // Secondly, all threads try to occupy all non-reserved slots
        index = occupy_free_slot_in_range(tls, my_num_reserved_slots, my_num_slots );
        // Likely this arena is already saturated
        if ( index == out_of_arena )
            return out_of_arena;
    }

    atomic_update( my_limit, (unsigned)(index + 1), std::less<unsigned>() );
    return index;
}

std::uintptr_t arena::calculate_stealing_threshold() {
    stack_anchor_type anchor;
    return r1::calculate_stealing_threshold(reinterpret_cast<std::uintptr_t>(&anchor), my_threading_control->worker_stack_size());
}

void arena::process(thread_data& tls) {
    governor::set_thread_data(tls); // TODO: consider moving to create_one_job.
    __TBB_ASSERT( is_alive(my_guard), nullptr);
    __TBB_ASSERT( my_num_slots >= 1, nullptr);

    std::size_t index = occupy_free_slot</*as_worker*/true>(tls);
    if (index == out_of_arena) {
        on_thread_leaving(ref_worker);
        return;
    }

    __TBB_ASSERT( index >= my_num_reserved_slots, "Workers cannot occupy reserved slots" );
    tls.attach_arena(*this, index);
    // worker thread enters the dispatch loop to look for a work
    tls.my_inbox.set_is_idle(true);
    if (tls.my_arena_slot->is_task_pool_published()) {
        tls.my_inbox.set_is_idle(false);
    }

    task_dispatcher& task_disp = tls.my_arena_slot->default_task_dispatcher();
    tls.enter_task_dispatcher(task_disp, calculate_stealing_threshold());
    __TBB_ASSERT(task_disp.can_steal(), nullptr);

    __TBB_ASSERT( !tls.my_last_observer, "There cannot be notified local observers when entering arena" );
    my_observers.notify_entry_observers(tls.my_last_observer, tls.my_is_worker);

    // Waiting on special object tied to this arena
    outermost_worker_waiter waiter(*this);
    d1::task* t = tls.my_task_dispatcher->local_wait_for_all(nullptr, waiter);
    // For purposes of affinity support, the slot's mailbox is considered idle while no thread is
    // attached to it.
    tls.my_inbox.set_is_idle(true);

    __TBB_ASSERT_EX(t == nullptr, "Outermost worker must not leave dispatch loop with a task");
    __TBB_ASSERT(governor::is_thread_data_set(&tls), nullptr);
    __TBB_ASSERT(tls.my_task_dispatcher == &task_disp, nullptr);

    my_observers.notify_exit_observers(tls.my_last_observer, tls.my_is_worker);
    tls.my_last_observer = nullptr;

    tls.leave_task_dispatcher();

    // Arena slot detach (arena may be used in market::process)
    // TODO: Consider moving several calls below into a new method(e.g.detach_arena).
    tls.my_arena_slot->release();
    tls.my_arena_slot = nullptr;
    tls.my_inbox.detach();
    __TBB_ASSERT(tls.my_inbox.is_idle_state(true), nullptr);
    __TBB_ASSERT(is_alive(my_guard), nullptr);

    // In contrast to earlier versions of TBB (before 3.0 U5) now it is possible
    // that arena may be temporarily left unpopulated by threads. See comments in
    // arena::on_thread_leaving() for more details.
    on_thread_leaving(ref_worker);
    __TBB_ASSERT(tls.my_arena == this, "my_arena is used as a hint when searching the arena to join");
}

arena::arena(threading_control* control, unsigned num_slots, unsigned num_reserved_slots, unsigned priority_level) {
    __TBB_ASSERT( !my_guard, "improperly allocated arena?" );
    __TBB_ASSERT( sizeof(my_slots[0]) % cache_line_size()==0, "arena::slot size not multiple of cache line size" );
    __TBB_ASSERT( is_aligned(this, cache_line_size()), "arena misaligned" );
    my_threading_control = control;
    my_limit = 1;
    // Two slots are mandatory: for the external thread, and for 1 worker (required to support starvation resistant tasks).
    my_num_slots = num_arena_slots(num_slots, num_reserved_slots);
    my_num_reserved_slots = num_reserved_slots;
    my_max_num_workers = num_slots-num_reserved_slots;
    my_priority_level = priority_level;
    my_references = ref_external; // accounts for the external thread
    my_observers.my_arena = this;
    my_co_cache.init(4 * num_slots);
    __TBB_ASSERT ( my_max_num_workers <= my_num_slots, nullptr);
    // Initialize the default context. It should be allocated before task_dispatch construction.
    my_default_ctx = new (cache_aligned_allocate(sizeof(d1::task_group_context)))
        d1::task_group_context{ d1::task_group_context::isolated, d1::task_group_context::fp_settings };
    // Construct slots. Mark internal synchronization elements for the tools.
    task_dispatcher* base_td_pointer = reinterpret_cast<task_dispatcher*>(my_slots + my_num_slots);
    for( unsigned i = 0; i < my_num_slots; ++i ) {
        // __TBB_ASSERT( !my_slots[i].my_scheduler && !my_slots[i].task_pool, nullptr);
        __TBB_ASSERT( !my_slots[i].task_pool_ptr, nullptr);
        __TBB_ASSERT( !my_slots[i].my_task_pool_size, nullptr);
        mailbox(i).construct();
        my_slots[i].init_task_streams(i);
        my_slots[i].my_default_task_dispatcher = new(base_td_pointer + i) task_dispatcher(this);
        my_slots[i].my_is_occupied.store(false, std::memory_order_relaxed);
    }
    my_fifo_task_stream.initialize(my_num_slots);
    my_resume_task_stream.initialize(my_num_slots);
#if __TBB_PREVIEW_CRITICAL_TASKS
    my_critical_task_stream.initialize(my_num_slots);
#endif
    my_mandatory_requests = 0;
}

arena& arena::allocate_arena(threading_control* control, unsigned num_slots, unsigned num_reserved_slots,
                              unsigned priority_level)
{
    __TBB_ASSERT( sizeof(base_type) + sizeof(arena_slot) == sizeof(arena), "All arena data fields must go to arena_base" );
    __TBB_ASSERT( sizeof(base_type) % cache_line_size() == 0, "arena slots area misaligned: wrong padding" );
    __TBB_ASSERT( sizeof(mail_outbox) == max_nfs_size, "Mailbox padding is wrong" );
    std::size_t n = allocation_size(num_arena_slots(num_slots, num_reserved_slots));
    unsigned char* storage = (unsigned char*)cache_aligned_allocate(n);
    // Zero all slots to indicate that they are empty
    std::memset( storage, 0, n );

    return *new( storage + num_arena_slots(num_slots, num_reserved_slots) * sizeof(mail_outbox) )
        arena(control, num_slots, num_reserved_slots, priority_level);
}

void arena::free_arena () {
    __TBB_ASSERT( is_alive(my_guard), nullptr);
    __TBB_ASSERT( !my_references.load(std::memory_order_relaxed), "There are threads in the dying arena" );
    __TBB_ASSERT( !my_total_num_workers_requested && !my_num_workers_allotted, "Dying arena requests workers" );
    __TBB_ASSERT( is_empty(), "Inconsistent state of a dying arena" );
#if __TBB_ARENA_BINDING
    if (my_numa_binding_observer != nullptr) {
        destroy_binding_observer(my_numa_binding_observer);
        my_numa_binding_observer = nullptr;
    }
#endif /*__TBB_ARENA_BINDING*/
    poison_value( my_guard );
    for ( unsigned i = 0; i < my_num_slots; ++i ) {
        // __TBB_ASSERT( !my_slots[i].my_scheduler, "arena slot is not empty" );
        // TODO: understand the assertion and modify
        // __TBB_ASSERT( my_slots[i].task_pool == EmptyTaskPool, nullptr);
        __TBB_ASSERT( my_slots[i].head == my_slots[i].tail, nullptr); // TODO: replace by is_quiescent_local_task_pool_empty
        my_slots[i].free_task_pool();
        mailbox(i).drain();
        my_slots[i].my_default_task_dispatcher->~task_dispatcher();
    }
    __TBB_ASSERT(my_fifo_task_stream.empty(), "Not all enqueued tasks were executed");
    __TBB_ASSERT(my_resume_task_stream.empty(), "Not all enqueued tasks were executed");
    // Cleanup coroutines/schedulers cache
    my_co_cache.cleanup();
    my_default_ctx->~task_group_context();
    cache_aligned_deallocate(my_default_ctx);
#if __TBB_PREVIEW_CRITICAL_TASKS
    __TBB_ASSERT( my_critical_task_stream.empty(), "Not all critical tasks were executed");
#endif
    // Clear enfources synchronization with observe(false)
    my_observers.clear();

    void* storage  = &mailbox(my_num_slots-1);
    __TBB_ASSERT( my_references.load(std::memory_order_relaxed) == 0, nullptr);
    this->~arena();
#if TBB_USE_ASSERT > 1
    std::memset( storage, 0, allocation_size(my_num_slots) );
#endif /* TBB_USE_ASSERT */
    cache_aligned_deallocate( storage );
}

bool arena::has_enqueued_tasks() {
    return !my_fifo_task_stream.empty();
}

void arena::request_workers(int mandatory_delta, int workers_delta, bool wakeup_threads) {
    my_threading_control->adjust_demand(my_tc_client, mandatory_delta, workers_delta);

    if (wakeup_threads) {
        // Notify all sleeping threads that work has appeared in the arena.
        get_waiting_threads_monitor().notify([&] (market_context context) {
            return this == context.my_arena_addr;
        });
    }
}

bool arena::has_tasks() {
    // TODO: rework it to return at least a hint about where a task was found; better if the task itself.
    std::size_t n = my_limit.load(std::memory_order_acquire);
    bool tasks_are_available = false;
    for (std::size_t k = 0; k < n && !tasks_are_available; ++k) {
        tasks_are_available = !my_slots[k].is_empty();
    }
    tasks_are_available = tasks_are_available || has_enqueued_tasks() || !my_resume_task_stream.empty();
#if __TBB_PREVIEW_CRITICAL_TASKS
    tasks_are_available = tasks_are_available || !my_critical_task_stream.empty();
#endif
    return tasks_are_available;
}

void arena::out_of_work() {
    // We should try unset my_pool_state first due to keep arena invariants in consistent state
    // Otherwise, we might have my_pool_state = false and my_mandatory_concurrency = true that is broken invariant
    bool disable_mandatory = my_mandatory_concurrency.try_clear_if([this] { return !has_enqueued_tasks(); });
    bool release_workers = my_pool_state.try_clear_if([this] { return !has_tasks(); });

    if (disable_mandatory || release_workers) {
        int mandatory_delta = disable_mandatory ? -1 : 0;
        int workers_delta = release_workers ? -(int)my_max_num_workers : 0;

        if (disable_mandatory && is_arena_workerless()) {
            // We had set workers_delta to 1 when enabled mandatory concurrency, so revert it now
            workers_delta = -1;
        }
        request_workers(mandatory_delta, workers_delta);
    }
}

void arena::set_top_priority(bool is_top_priority) {
    my_is_top_priority.store(is_top_priority, std::memory_order_relaxed);
}

bool arena::is_top_priority() const {
    return my_is_top_priority.load(std::memory_order_relaxed);
}

bool arena::try_join() {
    if (num_workers_active() < my_num_workers_allotted.load(std::memory_order_relaxed)) {
        my_references += arena::ref_worker;
        return true;
    }
    return false;
}

void arena::set_allotment(unsigned allotment) {
    if (my_num_workers_allotted.load(std::memory_order_relaxed) != allotment) {
        my_num_workers_allotted.store(allotment, std::memory_order_relaxed);
    }
}

std::pair<int, int> arena::update_request(int mandatory_delta, int workers_delta) {
    __TBB_ASSERT(-1 <= mandatory_delta && mandatory_delta <= 1, nullptr);

    int min_workers_request = 0;
    int max_workers_request = 0;

    // Calculate min request
    my_mandatory_requests += mandatory_delta;
    min_workers_request = my_mandatory_requests > 0 ? 1 : 0;

    // Calculate max request
    my_total_num_workers_requested += workers_delta;
    // Clamp worker request into interval [0, my_max_num_workers]
    max_workers_request = clamp(my_total_num_workers_requested, 0,
        min_workers_request > 0 && is_arena_workerless() ? 1 : (int)my_max_num_workers);

    return { min_workers_request, max_workers_request };
}

thread_control_monitor& arena::get_waiting_threads_monitor() {
    return my_threading_control->get_waiting_threads_monitor();
}

void arena::enqueue_task(d1::task& t, d1::task_group_context& ctx, thread_data& td) {
    task_group_context_impl::bind_to(ctx, &td);
    task_accessor::context(t) = &ctx;
    task_accessor::isolation(t) = no_isolation;
    my_fifo_task_stream.push( &t, random_lane_selector(td.my_random) );
    advertise_new_work<work_enqueued>();
}

arena& arena::create(threading_control* control, unsigned num_slots, unsigned num_reserved_slots, unsigned arena_priority_level)
{
    __TBB_ASSERT(num_slots > 0, NULL);
    __TBB_ASSERT(num_reserved_slots <= num_slots, NULL);
    // Add public market reference for an external thread/task_arena (that adds an internal reference in exchange).
    arena& a = arena::allocate_arena(control, num_slots, num_reserved_slots, arena_priority_level);
    a.my_tc_client = control->create_client(a);
    // We should not publish arena until all fields are initialized
    control->publish_client(a.my_tc_client);
    return a;
}

} // namespace r1
} // namespace detail
} // namespace tbb

// Enable task_arena.h
#include "third_party/tbb/task_arena.hh" // task_arena_base

namespace tbb {
namespace detail {
namespace r1 {

#if TBB_USE_ASSERT
void assert_arena_priority_valid( tbb::task_arena::priority a_priority ) {
    bool is_arena_priority_correct =
        a_priority == tbb::task_arena::priority::high   ||
        a_priority == tbb::task_arena::priority::normal ||
        a_priority == tbb::task_arena::priority::low;
    __TBB_ASSERT( is_arena_priority_correct,
                  "Task arena priority should be equal to one of the predefined values." );
}
#else
void assert_arena_priority_valid( tbb::task_arena::priority ) {}
#endif

unsigned arena_priority_level( tbb::task_arena::priority a_priority ) {
    assert_arena_priority_valid( a_priority );
    return d1::num_priority_levels - unsigned(int(a_priority) / d1::priority_stride);
}

tbb::task_arena::priority arena_priority( unsigned priority_level ) {
    auto priority = tbb::task_arena::priority(
        (d1::num_priority_levels - priority_level) * d1::priority_stride
    );
    assert_arena_priority_valid( priority );
    return priority;
}

struct task_arena_impl {
    static void initialize(d1::task_arena_base&);
    static void terminate(d1::task_arena_base&);
    static bool attach(d1::task_arena_base&);
    static void execute(d1::task_arena_base&, d1::delegate_base&);
    static void wait(d1::task_arena_base&);
    static int max_concurrency(const d1::task_arena_base*);
    static void enqueue(d1::task&, d1::task_group_context*, d1::task_arena_base*);
};

void __TBB_EXPORTED_FUNC initialize(d1::task_arena_base& ta) {
    task_arena_impl::initialize(ta);
}
void __TBB_EXPORTED_FUNC terminate(d1::task_arena_base& ta) {
    task_arena_impl::terminate(ta);
}
bool __TBB_EXPORTED_FUNC attach(d1::task_arena_base& ta) {
    return task_arena_impl::attach(ta);
}
void __TBB_EXPORTED_FUNC execute(d1::task_arena_base& ta, d1::delegate_base& d) {
    task_arena_impl::execute(ta, d);
}
void __TBB_EXPORTED_FUNC wait(d1::task_arena_base& ta) {
    task_arena_impl::wait(ta);
}

int __TBB_EXPORTED_FUNC max_concurrency(const d1::task_arena_base* ta) {
    return task_arena_impl::max_concurrency(ta);
}

void __TBB_EXPORTED_FUNC enqueue(d1::task& t, d1::task_arena_base* ta) {
    task_arena_impl::enqueue(t, nullptr, ta);
}

void __TBB_EXPORTED_FUNC enqueue(d1::task& t, d1::task_group_context& ctx, d1::task_arena_base* ta) {
    task_arena_impl::enqueue(t, &ctx, ta);
}

void task_arena_impl::initialize(d1::task_arena_base& ta) {
    // Enforce global market initialization to properly initialize soft limit
    (void)governor::get_thread_data();
    if (ta.my_max_concurrency < 1) {
#if __TBB_ARENA_BINDING
        d1::constraints arena_constraints = d1::constraints{}
            .set_core_type(ta.core_type())
            .set_max_threads_per_core(ta.max_threads_per_core())
            .set_numa_id(ta.my_numa_id);
        ta.my_max_concurrency = (int)default_concurrency(arena_constraints);
#else /*!__TBB_ARENA_BINDING*/
        ta.my_max_concurrency = (int)governor::default_num_threads();
#endif /*!__TBB_ARENA_BINDING*/
    }

    __TBB_ASSERT(ta.my_arena.load(std::memory_order_relaxed) == nullptr, "Arena already initialized");
    unsigned priority_level = arena_priority_level(ta.my_priority);
    threading_control* thr_control = threading_control::register_public_reference();
    arena& a = arena::create(thr_control, unsigned(ta.my_max_concurrency), ta.my_num_reserved_slots, priority_level);
    ta.my_arena.store(&a, std::memory_order_release);
#if __TBB_ARENA_BINDING
    a.my_numa_binding_observer = construct_binding_observer(
        static_cast<d1::task_arena*>(&ta), a.my_num_slots, ta.my_numa_id, ta.core_type(), ta.max_threads_per_core());
#endif /*__TBB_ARENA_BINDING*/
}

void task_arena_impl::terminate(d1::task_arena_base& ta) {
    arena* a = ta.my_arena.load(std::memory_order_relaxed);
    assert_pointer_valid(a);
    threading_control::unregister_public_reference(/*blocking_terminate=*/false);
    a->on_thread_leaving(arena::ref_external);
    ta.my_arena.store(nullptr, std::memory_order_relaxed);
}

bool task_arena_impl::attach(d1::task_arena_base& ta) {
    __TBB_ASSERT(!ta.my_arena.load(std::memory_order_relaxed), nullptr);
    thread_data* td = governor::get_thread_data_if_initialized();
    if( td && td->my_arena ) {
        arena* a = td->my_arena;
        // There is an active arena to attach to.
        // It's still used by s, so won't be destroyed right away.
        __TBB_ASSERT(a->my_references > 0, nullptr);
        a->my_references += arena::ref_external;
        ta.my_num_reserved_slots = a->my_num_reserved_slots;
        ta.my_priority = arena_priority(a->my_priority_level);
        ta.my_max_concurrency = ta.my_num_reserved_slots + a->my_max_num_workers;
        __TBB_ASSERT(arena::num_arena_slots(ta.my_max_concurrency, ta.my_num_reserved_slots) == a->my_num_slots, nullptr);
        ta.my_arena.store(a, std::memory_order_release);
        // increases threading_control's ref count for task_arena
        threading_control::register_public_reference();
        return true;
    }
    return false;
}

void task_arena_impl::enqueue(d1::task& t, d1::task_group_context* c, d1::task_arena_base* ta) {
    thread_data* td = governor::get_thread_data();  // thread data is only needed for FastRandom instance
    assert_pointer_valid(td, "thread_data pointer should not be null");
    arena* a = ta ?
              ta->my_arena.load(std::memory_order_relaxed)
            : td->my_arena
    ;
    assert_pointer_valid(a, "arena pointer should not be null");
    auto* ctx = c ? c : a->my_default_ctx;
    assert_pointer_valid(ctx, "context pointer should not be null");
    // Is there a better place for checking the state of ctx?
     __TBB_ASSERT(!a->my_default_ctx->is_group_execution_cancelled(),
                  "The task will not be executed because its task_group_context is cancelled.");
     a->enqueue_task(t, *ctx, *td);
}

class nested_arena_context : no_copy {
public:
    nested_arena_context(thread_data& td, arena& nested_arena, std::size_t slot_index)
        : m_orig_execute_data_ext(td.my_task_dispatcher->m_execute_data_ext)
    {
        if (td.my_arena != &nested_arena) {
            m_orig_arena = td.my_arena;
            m_orig_slot_index = td.my_arena_index;
            m_orig_last_observer = td.my_last_observer;

            td.detach_task_dispatcher();
            td.attach_arena(nested_arena, slot_index);
            if (td.my_inbox.is_idle_state(true))
                td.my_inbox.set_is_idle(false);
            task_dispatcher& task_disp = td.my_arena_slot->default_task_dispatcher();
            td.enter_task_dispatcher(task_disp, m_orig_execute_data_ext.task_disp->m_stealing_threshold);

            // If the calling thread occupies the slots out of external thread reserve we need to notify the
            // market that this arena requires one worker less.
            if (td.my_arena_index >= td.my_arena->my_num_reserved_slots) {
                td.my_arena->request_workers(/* mandatory_delta = */ 0, /* workers_delta = */ -1);
            }

            td.my_last_observer = nullptr;
            // The task_arena::execute method considers each calling thread as an external thread.
            td.my_arena->my_observers.notify_entry_observers(td.my_last_observer, /* worker*/false);
        }

        m_task_dispatcher = td.my_task_dispatcher;
        m_orig_fifo_tasks_allowed = m_task_dispatcher->allow_fifo_task(true);
        m_orig_critical_task_allowed = m_task_dispatcher->m_properties.critical_task_allowed;
        m_task_dispatcher->m_properties.critical_task_allowed = true;

        execution_data_ext& ed_ext = td.my_task_dispatcher->m_execute_data_ext;
        ed_ext.context = td.my_arena->my_default_ctx;
        ed_ext.original_slot = td.my_arena_index;
        ed_ext.affinity_slot = d1::no_slot;
        ed_ext.task_disp = td.my_task_dispatcher;
        ed_ext.isolation = no_isolation;

        __TBB_ASSERT(td.my_arena_slot, nullptr);
        __TBB_ASSERT(td.my_arena_slot->is_occupied(), nullptr);
        __TBB_ASSERT(td.my_task_dispatcher, nullptr);
    }
    ~nested_arena_context() {
        thread_data& td = *m_task_dispatcher->m_thread_data;
        __TBB_ASSERT(governor::is_thread_data_set(&td), nullptr);
        m_task_dispatcher->allow_fifo_task(m_orig_fifo_tasks_allowed);
        m_task_dispatcher->m_properties.critical_task_allowed = m_orig_critical_task_allowed;
        if (m_orig_arena) {
            td.my_arena->my_observers.notify_exit_observers(td.my_last_observer, /*worker*/ false);
            td.my_last_observer = m_orig_last_observer;

            // Notify the market that this thread releasing a one slot
            // that can be used by a worker thread.
            if (td.my_arena_index >= td.my_arena->my_num_reserved_slots) {
                td.my_arena->request_workers(/* mandatory_delta = */ 0, /* workers_delta = */ 1);
            }

            td.leave_task_dispatcher();
            td.my_arena_slot->release();
            td.my_arena->my_exit_monitors.notify_one(); // do not relax!

            td.attach_arena(*m_orig_arena, m_orig_slot_index);
            td.attach_task_dispatcher(*m_orig_execute_data_ext.task_disp);
            __TBB_ASSERT(td.my_inbox.is_idle_state(false), nullptr);
        }
        td.my_task_dispatcher->m_execute_data_ext = m_orig_execute_data_ext;
    }

private:
    execution_data_ext    m_orig_execute_data_ext{};
    arena*              m_orig_arena{ nullptr };
    observer_proxy*     m_orig_last_observer{ nullptr };
    task_dispatcher*    m_task_dispatcher{ nullptr };
    unsigned            m_orig_slot_index{};
    bool                m_orig_fifo_tasks_allowed{};
    bool                m_orig_critical_task_allowed{};
};

class delegated_task : public d1::task {
    d1::delegate_base&  m_delegate;
    concurrent_monitor& m_monitor;
    d1::wait_context&   m_wait_ctx;
    std::atomic<bool>   m_completed;
    d1::task* execute(d1::execution_data& ed) override {
        const execution_data_ext& ed_ext = static_cast<const execution_data_ext&>(ed);
        execution_data_ext orig_execute_data_ext = ed_ext.task_disp->m_execute_data_ext;
        __TBB_ASSERT(&ed_ext.task_disp->m_execute_data_ext == &ed,
            "The execute data shall point to the current task dispatcher execute data");
        __TBB_ASSERT(ed_ext.task_disp->m_execute_data_ext.isolation == no_isolation, nullptr);

        ed_ext.task_disp->m_execute_data_ext.context = ed_ext.task_disp->get_thread_data().my_arena->my_default_ctx;
        bool fifo_task_allowed = ed_ext.task_disp->allow_fifo_task(true);
        try_call([&] {
            m_delegate();
        }).on_completion([&] {
            ed_ext.task_disp->m_execute_data_ext = orig_execute_data_ext;
            ed_ext.task_disp->allow_fifo_task(fifo_task_allowed);
        });

        finalize();
        return nullptr;
    }
    d1::task* cancel(d1::execution_data&) override {
        finalize();
        return nullptr;
    }
    void finalize() {
        m_wait_ctx.release(); // must precede the wakeup
        m_monitor.notify([this] (std::uintptr_t ctx) {
            return ctx == std::uintptr_t(&m_delegate);
        }); // do not relax, it needs a fence!
        m_completed.store(true, std::memory_order_release);
    }
public:
    delegated_task(d1::delegate_base& d, concurrent_monitor& s, d1::wait_context& wo)
        : m_delegate(d), m_monitor(s), m_wait_ctx(wo), m_completed{ false }{}
    ~delegated_task() override {
        // The destructor can be called earlier than the m_monitor is notified
        // because the waiting thread can be released after m_wait_ctx.release_wait.
        // To close that race we wait for the m_completed signal.
        spin_wait_until_eq(m_completed, true);
    }
};

void task_arena_impl::execute(d1::task_arena_base& ta, d1::delegate_base& d) {
    arena* a = ta.my_arena.load(std::memory_order_relaxed);
    __TBB_ASSERT(a != nullptr, nullptr);
    thread_data* td = governor::get_thread_data();

    bool same_arena = td->my_arena == a;
    std::size_t index1 = td->my_arena_index;
    if (!same_arena) {
        index1 = a->occupy_free_slot</*as_worker */false>(*td);
        if (index1 == arena::out_of_arena) {
            concurrent_monitor::thread_context waiter((std::uintptr_t)&d);
            d1::wait_context wo(1);
            d1::task_group_context exec_context(d1::task_group_context::isolated);
            task_group_context_impl::copy_fp_settings(exec_context, *a->my_default_ctx);

            delegated_task dt(d, a->my_exit_monitors, wo);
            a->enqueue_task( dt, exec_context, *td);
            size_t index2 = arena::out_of_arena;
            do {
                a->my_exit_monitors.prepare_wait(waiter);
                if (!wo.continue_execution()) {
                    a->my_exit_monitors.cancel_wait(waiter);
                    break;
                }
                index2 = a->occupy_free_slot</*as_worker*/false>(*td);
                if (index2 != arena::out_of_arena) {
                    a->my_exit_monitors.cancel_wait(waiter);
                    nested_arena_context scope(*td, *a, index2 );
                    r1::wait(wo, exec_context);
                    __TBB_ASSERT(!exec_context.my_exception.load(std::memory_order_relaxed), nullptr); // exception can be thrown above, not deferred
                    break;
                }
                a->my_exit_monitors.commit_wait(waiter);
            } while (wo.continue_execution());
            if (index2 == arena::out_of_arena) {
                // notify a waiting thread even if this thread did not enter arena,
                // in case it was woken by a leaving thread but did not need to enter
                a->my_exit_monitors.notify_one(); // do not relax!
            }
            // process possible exception
            auto exception = exec_context.my_exception.load(std::memory_order_acquire);
            if (exception) {
                __TBB_ASSERT(exec_context.is_group_execution_cancelled(), "The task group context with an exception should be canceled.");
                exception->throw_self();
            }
            __TBB_ASSERT(governor::is_thread_data_set(td), nullptr);
            return;
        } // if (index1 == arena::out_of_arena)
    } // if (!same_arena)

    context_guard_helper</*report_tasks=*/false> context_guard;
    context_guard.set_ctx(a->my_default_ctx);
    nested_arena_context scope(*td, *a, index1);
#if _WIN64
    try {
#endif
        d();
        __TBB_ASSERT(same_arena || governor::is_thread_data_set(td), nullptr);
#if _WIN64
    } catch (...) {
        context_guard.restore_default();
        throw;
    }
#endif
}

void task_arena_impl::wait(d1::task_arena_base& ta) {
    arena* a = ta.my_arena.load(std::memory_order_relaxed);
    __TBB_ASSERT(a != nullptr, nullptr);
    thread_data* td = governor::get_thread_data();
    __TBB_ASSERT_EX(td, "Scheduler is not initialized");
    __TBB_ASSERT(td->my_arena != a || td->my_arena_index == 0, "internal_wait is not supported within a worker context" );
    if (a->my_max_num_workers != 0) {
        while (a->num_workers_active() || !a->is_empty()) {
            yield();
        }
    }
}

int task_arena_impl::max_concurrency(const d1::task_arena_base *ta) {
    arena* a = nullptr;
    if( ta ) // for special cases of ta->max_concurrency()
        a = ta->my_arena.load(std::memory_order_relaxed);
    else if( thread_data* td = governor::get_thread_data_if_initialized() )
        a = td->my_arena; // the current arena if any

    if( a ) { // Get parameters from the arena
        __TBB_ASSERT( !ta || ta->my_max_concurrency==1, nullptr);
        int mandatory_worker = 0;
        if (a->is_arena_workerless() && a->my_num_reserved_slots == 1) {
            mandatory_worker = a->my_mandatory_concurrency.test() ? 1 : 0;
        }
        return a->my_num_reserved_slots + a->my_max_num_workers + mandatory_worker;
    }

    if (ta && ta->my_max_concurrency == 1) {
        return 1;
    }

#if __TBB_ARENA_BINDING
    if (ta) {
        d1::constraints arena_constraints = d1::constraints{}
            .set_numa_id(ta->my_numa_id)
            .set_core_type(ta->core_type())
            .set_max_threads_per_core(ta->max_threads_per_core());
        return (int)default_concurrency(arena_constraints);
    }
#endif /*!__TBB_ARENA_BINDING*/

    __TBB_ASSERT(!ta || ta->my_max_concurrency==d1::task_arena_base::automatic, nullptr);
    return int(governor::default_num_threads());
}

void isolate_within_arena(d1::delegate_base& d, std::intptr_t isolation) {
    // TODO: Decide what to do if the scheduler is not initialized. Is there a use case for it?
    thread_data* tls = governor::get_thread_data();
    assert_pointers_valid(tls, tls->my_task_dispatcher);
    task_dispatcher* dispatcher = tls->my_task_dispatcher;
    isolation_type previous_isolation = dispatcher->m_execute_data_ext.isolation;
    try_call([&] {
        // We temporarily change the isolation tag of the currently running task. It will be restored in the destructor of the guard.
        isolation_type current_isolation = isolation ? isolation : reinterpret_cast<isolation_type>(&d);
        // Save the current isolation value and set new one
        previous_isolation = dispatcher->set_isolation(current_isolation);
        // Isolation within this callable
        d();
    }).on_completion([&] {
        __TBB_ASSERT(governor::get_thread_data()->my_task_dispatcher == dispatcher, nullptr);
        dispatcher->set_isolation(previous_isolation);
    });
}

} // namespace r1
} // namespace detail
} // namespace tbb
