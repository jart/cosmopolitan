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

#ifndef _TBB_arena_H
#define _TBB_arena_H

#include "third_party/libcxx/atomic"
#include "third_party/libcxx/cstring"

#include "third_party/tbb/detail/_task.h"
#include "third_party/tbb/detail/_utils.h"
#include "third_party/tbb/spin_mutex.h"

#include "third_party/tbb/scheduler_common.h"
#include "third_party/tbb/intrusive_list.h"
#include "third_party/tbb/task_stream.h"
#include "third_party/tbb/arena_slot.h"
#include "third_party/tbb/rml_tbb.h"
#include "third_party/tbb/mailbox.h"
#include "third_party/tbb/governor.h"
#include "third_party/tbb/concurrent_monitor.h"
#include "third_party/tbb/observer_proxy.h"
#include "third_party/tbb/thread_control_monitor.h"
#include "third_party/tbb/threading_control_client.h"

namespace tbb {
namespace detail {
namespace r1 {

class task_dispatcher;
class task_group_context;
class threading_control;
class allocate_root_with_context_proxy;

#if __TBB_ARENA_BINDING
class numa_binding_observer;
#endif /*__TBB_ARENA_BINDING*/

//! Bounded coroutines cache LIFO ring buffer
class arena_co_cache {
    //! Ring buffer storage
    task_dispatcher** my_co_scheduler_cache;
    //! Current cache index
    unsigned my_head;
    //! Cache capacity for arena
    unsigned my_max_index;
    //! Accessor lock for modification operations
    tbb::spin_mutex my_co_cache_mutex;

    unsigned next_index() {
        return ( my_head == my_max_index ) ? 0 : my_head + 1;
    }

    unsigned prev_index() {
        return ( my_head == 0 ) ? my_max_index : my_head - 1;
    }

    bool internal_empty() {
        return my_co_scheduler_cache[prev_index()] == nullptr;
    }

    void internal_task_dispatcher_cleanup(task_dispatcher* to_cleanup) {
        to_cleanup->~task_dispatcher();
        cache_aligned_deallocate(to_cleanup);
    }

public:
    void init(unsigned cache_capacity) {
        std::size_t alloc_size = cache_capacity * sizeof(task_dispatcher*);
        my_co_scheduler_cache = (task_dispatcher**)cache_aligned_allocate(alloc_size);
        std::memset( my_co_scheduler_cache, 0, alloc_size );
        my_head = 0;
        my_max_index = cache_capacity - 1;
    }

    void cleanup() {
        while (task_dispatcher* to_cleanup = pop()) {
            internal_task_dispatcher_cleanup(to_cleanup);
        }
        cache_aligned_deallocate(my_co_scheduler_cache);
    }

    //! Insert scheduler to the current available place.
    //! Replace an old value, if necessary.
    void push(task_dispatcher* s) {
        task_dispatcher* to_cleanup = nullptr;
        {
            tbb::spin_mutex::scoped_lock lock(my_co_cache_mutex);
            // Check if we are replacing some existing buffer entrance
            if (my_co_scheduler_cache[my_head] != nullptr) {
                to_cleanup = my_co_scheduler_cache[my_head];
            }
            // Store the cached value
            my_co_scheduler_cache[my_head] = s;
            // Move head index to the next slot
            my_head = next_index();
        }
        // Cleanup replaced buffer if any
        if (to_cleanup) {
            internal_task_dispatcher_cleanup(to_cleanup);
        }
    }

    //! Get a cached scheduler if any
    task_dispatcher* pop() {
        tbb::spin_mutex::scoped_lock lock(my_co_cache_mutex);
        // No cached coroutine
        if (internal_empty()) {
            return nullptr;
        }
        // Move head index to the currently available value
        my_head = prev_index();
        // Retrieve the value from the buffer
        task_dispatcher* to_return = my_co_scheduler_cache[my_head];
        // Clear the previous entrance value
        my_co_scheduler_cache[my_head] = nullptr;
        return to_return;
    }
};

struct stack_anchor_type {
    stack_anchor_type() = default;
    stack_anchor_type(const stack_anchor_type&) = delete;
};

class atomic_flag {
    static const std::uintptr_t SET = 1;
    static const std::uintptr_t UNSET = 0;
    std::atomic<std::uintptr_t> my_state{UNSET};
public:
    bool test_and_set() {
        std::uintptr_t state = my_state.load(std::memory_order_acquire);
        switch (state) {
        case SET:
            return false;
        default: /* busy */
            if (my_state.compare_exchange_strong(state, SET)) {
                // We interrupted clear transaction
                return false;
            }
            if (state != UNSET) {
                // We lost our epoch
                return false;
            }
            // We are too late but still in the same epoch
            __TBB_fallthrough;
        case UNSET:
            return my_state.compare_exchange_strong(state, SET);
        }
    }
    template <typename Pred>
    bool try_clear_if(Pred&& pred) {
        std::uintptr_t busy = std::uintptr_t(&busy);
        std::uintptr_t state = my_state.load(std::memory_order_acquire);
        if (state == SET && my_state.compare_exchange_strong(state, busy)) {
            if (pred()) {
                return my_state.compare_exchange_strong(busy, UNSET);
            }
            // The result of the next operation is discarded, always false should be returned.
            my_state.compare_exchange_strong(busy, SET);
        }
        return false;
    }
    void clear() {
        my_state.store(UNSET, std::memory_order_release);
    }
    bool test(std::memory_order order = std::memory_order_acquire) {
        return my_state.load(order) != UNSET;
    }
};

//! The structure of an arena, except the array of slots.
/** Separated in order to simplify padding.
    Intrusive list node base class is used by market to form a list of arenas. **/
// TODO: Analyze arena_base cache lines placement
struct arena_base : padded<intrusive_list_node> {
    //! The number of workers that have been marked out by the resource manager to service the arena.
    std::atomic<unsigned> my_num_workers_allotted;   // heavy use in stealing loop

    //! Reference counter for the arena.
    /** Worker and external thread references are counted separately: first several bits are for references
        from external thread threads or explicit task_arenas (see arena::ref_external_bits below);
        the rest counts the number of workers servicing the arena. */
    std::atomic<unsigned> my_references;     // heavy use in stealing loop

    //! The maximal number of currently busy slots.
    std::atomic<unsigned> my_limit;          // heavy use in stealing loop

    //! Task pool for the tasks scheduled via task::enqueue() method.
    /** Such scheduling guarantees eventual execution even if
        - new tasks are constantly coming (by extracting scheduled tasks in
          relaxed FIFO order);
        - the enqueuing thread does not call any of wait_for_all methods. **/
    task_stream<front_accessor> my_fifo_task_stream; // heavy use in stealing loop

    //! Task pool for the tasks scheduled via tbb::resume() function.
    task_stream<front_accessor> my_resume_task_stream; // heavy use in stealing loop

#if __TBB_PREVIEW_CRITICAL_TASKS
    //! Task pool for the tasks with critical property set.
    /** Critical tasks are scheduled for execution ahead of other sources (including local task pool
        and even bypassed tasks) unless the thread already executes a critical task in an outer
        dispatch loop **/
    // used on the hot path of the task dispatch loop
    task_stream<back_nonnull_accessor> my_critical_task_stream;
#endif

    //! The total number of workers that are requested from the resource manager.
    int my_total_num_workers_requested;

    //! The index in the array of per priority lists of arenas this object is in.
    /*const*/ unsigned my_priority_level;

    //! The max priority level of arena in permit manager.
    std::atomic<bool> my_is_top_priority{false};

    //! Current task pool state and estimate of available tasks amount.
    atomic_flag my_pool_state;

    //! The list of local observers attached to this arena.
    observer_list my_observers;

#if __TBB_ARENA_BINDING
    //! Pointer to internal observer that allows to bind threads in arena to certain NUMA node.
    numa_binding_observer* my_numa_binding_observer;
#endif /*__TBB_ARENA_BINDING*/

    // Below are rarely modified members

    threading_control* my_threading_control;

    //! Default task group context.
    d1::task_group_context* my_default_ctx;

    //! Waiting object for external threads that cannot join the arena.
    concurrent_monitor my_exit_monitors;

    //! Coroutines (task_dispathers) cache buffer
    arena_co_cache my_co_cache;

    // arena needs an extra worker despite the arena limit
    atomic_flag my_mandatory_concurrency;
    // the number of local mandatory concurrency requests
    int my_mandatory_requests;

    //! The number of slots in the arena.
    unsigned my_num_slots;
    //! The number of reserved slots (can be occupied only by external threads).
    unsigned my_num_reserved_slots;
    //! The number of workers requested by the external thread owning the arena.
    unsigned my_max_num_workers;

    threading_control_client my_tc_client;

#if TBB_USE_ASSERT
    //! Used to trap accesses to the object after its destruction.
    std::uintptr_t my_guard;
#endif /* TBB_USE_ASSERT */
}; // struct arena_base

class arena: public padded<arena_base>
{
public:
    using base_type = padded<arena_base>;

    //! Types of work advertised by advertise_new_work()
    enum new_work_type {
        work_spawned,
        wakeup,
        work_enqueued
    };

    //! Constructor
    arena(threading_control* control, unsigned max_num_workers, unsigned num_reserved_slots, unsigned priority_level);

    //! Allocate an instance of arena.
    static arena& allocate_arena(threading_control* control, unsigned num_slots, unsigned num_reserved_slots,
                                  unsigned priority_level);

    static arena& create(threading_control* control, unsigned num_slots, unsigned num_reserved_slots, unsigned arena_priority_level);

    static int unsigned num_arena_slots ( unsigned num_slots, unsigned num_reserved_slots ) {
        return num_reserved_slots == 0 ? num_slots : max(2u, num_slots);
    }

    static int allocation_size( unsigned num_slots ) {
        return sizeof(base_type) + num_slots * (sizeof(mail_outbox) + sizeof(arena_slot) + sizeof(task_dispatcher));
    }

    //! Get reference to mailbox corresponding to given slot_id
    mail_outbox& mailbox( d1::slot_id slot ) {
        __TBB_ASSERT( slot != d1::no_slot, "affinity should be specified" );

        return reinterpret_cast<mail_outbox*>(this)[-(int)(slot+1)]; // cast to 'int' is redundant but left for readability
    }

    //! Completes arena shutdown, destructs and deallocates it.
    void free_arena();

    //! The number of least significant bits for external references
    static const unsigned ref_external_bits = 12; // up to 4095 external and 1M workers

    //! Reference increment values for externals and workers
    static const unsigned ref_external = 1;
    static const unsigned ref_worker   = 1 << ref_external_bits;

    //! The number of workers active in the arena.
    unsigned num_workers_active() const {
        return my_references.load(std::memory_order_acquire) >> ref_external_bits;
    }

    //! Check if the recall is requested by the market.
    bool is_recall_requested() const {
        return num_workers_active() > my_num_workers_allotted.load(std::memory_order_relaxed);
    }

    void request_workers(int mandatory_delta, int workers_delta, bool wakeup_threads = false);

    //! If necessary, raise a flag that there is new job in arena.
    template<arena::new_work_type work_type> void advertise_new_work();

    //! Attempts to steal a task from a randomly chosen arena slot
    d1::task* steal_task(unsigned arena_index, FastRandom& frnd, execution_data_ext& ed, isolation_type isolation);

    //! Get a task from a global starvation resistant queue
    template<task_stream_accessor_type accessor>
    d1::task* get_stream_task(task_stream<accessor>& stream, unsigned& hint);

#if __TBB_PREVIEW_CRITICAL_TASKS
    //! Tries to find a critical task in global critical task stream
    d1::task* get_critical_task(unsigned& hint, isolation_type isolation);
#endif

    //! Check if there is job anywhere in arena.
    void out_of_work();

    //! enqueue a task into starvation-resistance queue
    void enqueue_task(d1::task&, d1::task_group_context&, thread_data&);

    //! Registers the worker with the arena and enters TBB scheduler dispatch loop
    void process(thread_data&);

    //! Notification that the thread leaves its arena

    void on_thread_leaving(unsigned ref_param);

    //! Check for the presence of enqueued tasks
    bool has_enqueued_tasks();

    //! Check for the presence of any tasks
    bool has_tasks();

    bool is_empty() { return my_pool_state.test() == /* EMPTY */ false; }

    thread_control_monitor& get_waiting_threads_monitor();

    static const std::size_t out_of_arena = ~size_t(0);
    //! Tries to occupy a slot in the arena. On success, returns the slot index; if no slot is available, returns out_of_arena.
    template <bool as_worker>
    std::size_t occupy_free_slot(thread_data&);
    //! Tries to occupy a slot in the specified range.
    std::size_t occupy_free_slot_in_range(thread_data& tls, std::size_t lower, std::size_t upper);

    std::uintptr_t calculate_stealing_threshold();

    unsigned priority_level() { return my_priority_level; }

    bool has_request() { return my_total_num_workers_requested; }

    unsigned references() const { return my_references.load(std::memory_order_acquire); }

    bool is_arena_workerless() const { return my_max_num_workers == 0; }

    void set_top_priority(bool);

    bool is_top_priority() const;

    bool try_join();

    void set_allotment(unsigned allotment);

    std::pair</*min workers = */ int, /*max workers = */ int> update_request(int mandatory_delta, int workers_delta);

    /** Must be the last data field */
    arena_slot my_slots[1];
}; // class arena

template <arena::new_work_type work_type>
void arena::advertise_new_work() {
    bool is_mandatory_needed = false;
    bool are_workers_needed = false;

    if (work_type != work_spawned) {
        // Local memory fence here and below is required to avoid missed wakeups; see the comment below.
        // Starvation resistant tasks require concurrency, so missed wakeups are unacceptable.
        atomic_fence_seq_cst();
    }

    if (work_type == work_enqueued && my_num_slots > my_num_reserved_slots) {
        is_mandatory_needed = my_mandatory_concurrency.test_and_set();
    }

    // Double-check idiom that, in case of spawning, is deliberately sloppy about memory fences.
    // Technically, to avoid missed wakeups, there should be a full memory fence between the point we
    // released the task pool (i.e. spawned task) and read the arena's state.  However, adding such a
    // fence might hurt overall performance more than it helps, because the fence would be executed
    // on every task pool release, even when stealing does not occur.  Since TBB allows parallelism,
    // but never promises parallelism, the missed wakeup is not a correctness problem.
    are_workers_needed = my_pool_state.test_and_set();

    if (is_mandatory_needed || are_workers_needed) {
        int mandatory_delta = is_mandatory_needed ? 1 : 0;
        int workers_delta = are_workers_needed ? my_max_num_workers : 0;

        if (is_mandatory_needed && is_arena_workerless()) {
            // Set workers_delta to 1 to keep arena invariants consistent
            workers_delta = 1;
        }

        bool wakeup_workers = is_mandatory_needed || are_workers_needed;
        request_workers(mandatory_delta, workers_delta, wakeup_workers);
    }
}

inline d1::task* arena::steal_task(unsigned arena_index, FastRandom& frnd, execution_data_ext& ed, isolation_type isolation) {
    auto slot_num_limit = my_limit.load(std::memory_order_relaxed);
    if (slot_num_limit == 1) {
        // No slots to steal from
        return nullptr;
    }
    // Try to steal a task from a random victim.
    std::size_t k = frnd.get() % (slot_num_limit - 1);
    // The following condition excludes the external thread that might have
    // already taken our previous place in the arena from the list .
    // of potential victims. But since such a situation can take
    // place only in case of significant oversubscription, keeping
    // the checks simple seems to be preferable to complicating the code.
    if (k >= arena_index) {
        ++k; // Adjusts random distribution to exclude self
    }
    arena_slot* victim = &my_slots[k];
    d1::task **pool = victim->task_pool.load(std::memory_order_relaxed);
    d1::task *t = nullptr;
    if (pool == EmptyTaskPool || !(t = victim->steal_task(*this, isolation, k))) {
        return nullptr;
    }
    if (task_accessor::is_proxy_task(*t)) {
        task_proxy &tp = *(task_proxy*)t;
        d1::slot_id slot = tp.slot;
        t = tp.extract_task<task_proxy::pool_bit>();
        if (!t) {
            // Proxy was empty, so it's our responsibility to free it
            tp.allocator.delete_object(&tp, ed);
            return nullptr;
        }
        // Note affinity is called for any stolen task (proxy or general)
        ed.affinity_slot = slot;
    } else {
        // Note affinity is called for any stolen task (proxy or general)
        ed.affinity_slot = d1::any_slot;
    }
    // Update task owner thread id to identify stealing
    ed.original_slot = k;
    return t;
}

template<task_stream_accessor_type accessor>
inline d1::task* arena::get_stream_task(task_stream<accessor>& stream, unsigned& hint) {
    if (stream.empty())
        return nullptr;
    return stream.pop(subsequent_lane_selector(hint));
}

#if __TBB_PREVIEW_CRITICAL_TASKS
// Retrieves critical task respecting isolation level, if provided. The rule is:
// 1) If no outer critical task and no isolation => take any critical task
// 2) If working on an outer critical task and no isolation => cannot take any critical task
// 3) If no outer critical task but isolated => respect isolation
// 4) If working on an outer critical task and isolated => respect isolation
// Hint is used to keep some LIFO-ness, start search with the lane that was used during push operation.
inline d1::task* arena::get_critical_task(unsigned& hint, isolation_type isolation) {
    if (my_critical_task_stream.empty())
        return nullptr;

    if ( isolation != no_isolation ) {
        return my_critical_task_stream.pop_specific( hint, isolation );
    } else {
        return my_critical_task_stream.pop(preceding_lane_selector(hint));
    }
}
#endif // __TBB_PREVIEW_CRITICAL_TASKS

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* _TBB_arena_H */
