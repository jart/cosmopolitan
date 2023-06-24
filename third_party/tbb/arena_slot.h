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

#ifndef _TBB_arena_slot_H
#define _TBB_arena_slot_H

#include "third_party/tbb/detail/_config.h"
#include "third_party/tbb/detail/_utils.h"
#include "third_party/tbb/detail/_template_helpers.h"
#include "third_party/tbb/detail/_task.h"

#include "third_party/tbb/cache_aligned_allocator.h"

#include "third_party/tbb/misc.h"
#include "third_party/tbb/mailbox.h"
#include "third_party/tbb/scheduler_common.h"

#include "third_party/libcxx/atomic"

namespace tbb {
namespace detail {
namespace r1 {

class arena;
class task_group_context;

//--------------------------------------------------------------------------------------------------------
// Arena Slot
//--------------------------------------------------------------------------------------------------------

static d1::task** const EmptyTaskPool  = nullptr;
static d1::task** const LockedTaskPool = reinterpret_cast<d1::task**>(~std::intptr_t(0));

struct alignas(max_nfs_size) arena_slot_shared_state {
    //! Scheduler of the thread attached to the slot
    /** Marks the slot as busy, and is used to iterate through the schedulers belonging to this arena **/
    std::atomic<bool> my_is_occupied;

    // Synchronization of access to Task pool
    /** Also is used to specify if the slot is empty or locked:
         0 - empty
        -1 - locked **/
    std::atomic<d1::task**> task_pool;

    //! Index of the first ready task in the deque.
    /** Modified by thieves, and by the owner during compaction/reallocation **/
    std::atomic<std::size_t> head;
};

struct alignas(max_nfs_size) arena_slot_private_state {
    //! Hint provided for operations with the container of starvation-resistant tasks.
    /** Modified by the owner thread (during these operations). **/
    unsigned hint_for_fifo_stream;

#if __TBB_PREVIEW_CRITICAL_TASKS
    //! Similar to 'hint_for_fifo_stream' but for critical tasks.
    unsigned hint_for_critical_stream;
#endif

    //! Similar to 'hint_for_fifo_stream' but for the resume tasks.
    unsigned hint_for_resume_stream;

    //! Index of the element following the last ready task in the deque.
    /** Modified by the owner thread. **/
    std::atomic<std::size_t> tail;

    //! Capacity of the primary task pool (number of elements - pointers to task).
    std::size_t my_task_pool_size;

    //! Task pool of the scheduler that owns this slot
    // TODO: previously was task**__TBB_atomic, but seems like not accessed on other thread
    d1::task** task_pool_ptr;
};

class arena_slot : private arena_slot_shared_state, private arena_slot_private_state {
    friend class arena;
    friend class outermost_worker_waiter;
    friend class task_dispatcher;
    friend class thread_data;
    friend class nested_arena_context;

    //! The original task dispather associated with this slot
    task_dispatcher* my_default_task_dispatcher;

#if TBB_USE_ASSERT
    void fill_with_canary_pattern ( std::size_t first, std::size_t last ) {
        for ( std::size_t i = first; i < last; ++i )
            poison_pointer(task_pool_ptr[i]);
    }
#else
    void fill_with_canary_pattern ( size_t, std::size_t ) {}
#endif /* TBB_USE_ASSERT */

    static constexpr std::size_t min_task_pool_size = 64;

    void allocate_task_pool( std::size_t n ) {
        std::size_t byte_size = ((n * sizeof(d1::task*) + max_nfs_size - 1) / max_nfs_size) * max_nfs_size;
        my_task_pool_size = byte_size / sizeof(d1::task*);
        task_pool_ptr = (d1::task**)cache_aligned_allocate(byte_size);
        // No need to clear the fresh deque since valid items are designated by the head and tail members.
        // But fill it with a canary pattern in the high vigilance debug mode.
        fill_with_canary_pattern( 0, my_task_pool_size );
    }

public:
    //! Deallocate task pool that was allocated by means of allocate_task_pool.
    void free_task_pool( ) {
        // TODO: understand the assertion and modify
        // __TBB_ASSERT( !task_pool /* TODO: == EmptyTaskPool */, nullptr);
        if( task_pool_ptr ) {
           __TBB_ASSERT( my_task_pool_size, nullptr);
           cache_aligned_deallocate( task_pool_ptr );
           task_pool_ptr = nullptr;
           my_task_pool_size = 0;
        }
    }

    //! Get a task from the local pool.
    /** Called only by the pool owner.
        Returns the pointer to the task or nullptr if a suitable task is not found.
        Resets the pool if it is empty. **/
    d1::task* get_task(execution_data_ext&, isolation_type);

    //! Steal task from slot's ready pool
    d1::task* steal_task(arena&, isolation_type, std::size_t);

    //! Some thread is now the owner of this slot
    void occupy() {
        __TBB_ASSERT(!my_is_occupied.load(std::memory_order_relaxed), nullptr);
        my_is_occupied.store(true, std::memory_order_release);
    }

    //! Try to occupy the slot
    bool try_occupy() {
        return !is_occupied() && my_is_occupied.exchange(true) == false;
    }

    //! Some thread is now the owner of this slot
    void release() {
        __TBB_ASSERT(my_is_occupied.load(std::memory_order_relaxed), nullptr);
        my_is_occupied.store(false, std::memory_order_release);
    }

    //! Spawn newly created tasks
    void spawn(d1::task& t) {
        std::size_t T = prepare_task_pool(1);
        __TBB_ASSERT(is_poisoned(task_pool_ptr[T]), nullptr);
        task_pool_ptr[T] = &t;
        commit_spawned_tasks(T + 1);
        if (!is_task_pool_published()) {
            publish_task_pool();
        }
    }

    bool is_task_pool_published() const {
        return task_pool.load(std::memory_order_relaxed) != EmptyTaskPool;
    }

    bool is_empty() const {
        return task_pool.load(std::memory_order_relaxed) == EmptyTaskPool ||
               head.load(std::memory_order_relaxed) >= tail.load(std::memory_order_relaxed);
    }

    bool is_occupied() const {
        return my_is_occupied.load(std::memory_order_relaxed);
    }

    task_dispatcher& default_task_dispatcher() {
        __TBB_ASSERT(my_default_task_dispatcher != nullptr, nullptr);
        return *my_default_task_dispatcher;
    }

    void init_task_streams(unsigned h) {
        hint_for_fifo_stream = h;
#if __TBB_RESUMABLE_TASKS
        hint_for_resume_stream = h;
#endif
#if __TBB_PREVIEW_CRITICAL_TASKS
        hint_for_critical_stream = h;
#endif
    }

#if __TBB_PREVIEW_CRITICAL_TASKS
    unsigned& critical_hint() {
        return hint_for_critical_stream;
    }
#endif
private:
    //! Get a task from the local pool at specified location T.
    /** Returns the pointer to the task or nullptr if the task cannot be executed,
        e.g. proxy has been deallocated or isolation constraint is not met.
        tasks_omitted tells if some tasks have been omitted.
        Called only by the pool owner. The caller should guarantee that the
        position T is not available for a thief. **/
    d1::task* get_task_impl(size_t T, execution_data_ext& ed, bool& tasks_omitted, isolation_type isolation);

    //! Makes sure that the task pool can accommodate at least n more elements
    /** If necessary relocates existing task pointers or grows the ready task deque.
     *  Returns (possible updated) tail index (not accounting for n). **/
    std::size_t prepare_task_pool(std::size_t num_tasks) {
        std::size_t T = tail.load(std::memory_order_relaxed); // mirror
        if ( T + num_tasks <= my_task_pool_size ) {
            return T;
        }

        std::size_t new_size = num_tasks;
        if ( !my_task_pool_size ) {
            __TBB_ASSERT( !is_task_pool_published() && is_quiescent_local_task_pool_reset(), nullptr);
            __TBB_ASSERT( !task_pool_ptr, nullptr);
            if ( num_tasks < min_task_pool_size ) new_size = min_task_pool_size;
            allocate_task_pool( new_size );
            return 0;
        }
        acquire_task_pool();
        std::size_t H =  head.load(std::memory_order_relaxed); // mirror
        d1::task** new_task_pool = task_pool_ptr;
        __TBB_ASSERT( my_task_pool_size >= min_task_pool_size, nullptr);
        // Count not skipped tasks. Consider using std::count_if.
        for ( std::size_t i = H; i < T; ++i )
            if ( new_task_pool[i] ) ++new_size;
        // If the free space at the beginning of the task pool is too short, we
        // are likely facing a pathological single-producer-multiple-consumers
        // scenario, and thus it's better to expand the task pool
        bool allocate = new_size > my_task_pool_size - min_task_pool_size/4;
        if ( allocate ) {
            // Grow task pool. As this operation is rare, and its cost is asymptotically
            // amortizable, we can tolerate new task pool allocation done under the lock.
            if ( new_size < 2 * my_task_pool_size )
                new_size = 2 * my_task_pool_size;
            allocate_task_pool( new_size ); // updates my_task_pool_size
        }
        // Filter out skipped tasks. Consider using std::copy_if.
        std::size_t T1 = 0;
        for ( std::size_t i = H; i < T; ++i ) {
            if ( new_task_pool[i] ) {
                task_pool_ptr[T1++] = new_task_pool[i];
            }
        }
        // Deallocate the previous task pool if a new one has been allocated.
        if ( allocate )
            cache_aligned_deallocate( new_task_pool );
        else
            fill_with_canary_pattern( T1, tail );
        // Publish the new state.
        commit_relocated_tasks( T1 );
        // assert_task_pool_valid();
        return T1;
    }

    //! Makes newly spawned tasks visible to thieves
    void commit_spawned_tasks(std::size_t new_tail) {
        __TBB_ASSERT (new_tail <= my_task_pool_size, "task deque end was overwritten");
        // emit "task was released" signal
        // Release fence is necessary to make sure that previously stored task pointers
        // are visible to thieves.
        tail.store(new_tail, std::memory_order_release);
    }

    //! Used by workers to enter the task pool
    /** Does not lock the task pool in case if arena slot has been successfully grabbed. **/
    void publish_task_pool() {
        __TBB_ASSERT ( task_pool == EmptyTaskPool, "someone else grabbed my arena slot?" );
        __TBB_ASSERT ( head.load(std::memory_order_relaxed) < tail.load(std::memory_order_relaxed),
                "entering arena without tasks to share" );
        // Release signal on behalf of previously spawned tasks (when this thread was not in arena yet)
        task_pool.store(task_pool_ptr, std::memory_order_release );
    }

    //! Locks the local task pool
    /** Garbles task_pool for the duration of the lock. Requires correctly set task_pool_ptr.
        ATTENTION: This method is mostly the same as generic_scheduler::lock_task_pool(), with
        a little different logic of slot state checks (slot is either locked or points
        to our task pool). Thus if either of them is changed, consider changing the counterpart as well. **/
    void acquire_task_pool() {
        if (!is_task_pool_published()) {
            return; // we are not in arena - nothing to lock
        }
        bool sync_prepare_done = false;
        for( atomic_backoff b;;b.pause() ) {
#if TBB_USE_ASSERT
            // Local copy of the arena slot task pool pointer is necessary for the next
            // assertion to work correctly to exclude asynchronous state transition effect.
            d1::task** tp = task_pool.load(std::memory_order_relaxed);
            __TBB_ASSERT( tp == LockedTaskPool || tp == task_pool_ptr, "slot ownership corrupt?" );
#endif
            d1::task** expected = task_pool_ptr;
            if( task_pool.load(std::memory_order_relaxed) != LockedTaskPool &&
                task_pool.compare_exchange_strong(expected, LockedTaskPool ) ) {
                // We acquired our own slot
                break;
            } else if( !sync_prepare_done ) {
                // Start waiting
                sync_prepare_done = true;
            }
            // Someone else acquired a lock, so pause and do exponential backoff.
        }
        __TBB_ASSERT( task_pool.load(std::memory_order_relaxed) == LockedTaskPool, "not really acquired task pool" );
    }

    //! Unlocks the local task pool
    /** Restores task_pool munged by acquire_task_pool. Requires
        correctly set task_pool_ptr. **/
    void release_task_pool() {
        if ( !(task_pool.load(std::memory_order_relaxed) != EmptyTaskPool) )
            return; // we are not in arena - nothing to unlock
        __TBB_ASSERT( task_pool.load(std::memory_order_relaxed) == LockedTaskPool, "arena slot is not locked" );
        task_pool.store( task_pool_ptr, std::memory_order_release );
    }

    //! Locks victim's task pool, and returns pointer to it. The pointer can be nullptr.
    /** Garbles victim_arena_slot->task_pool for the duration of the lock. **/
    d1::task** lock_task_pool() {
        d1::task** victim_task_pool;
        for ( atomic_backoff backoff;; /*backoff pause embedded in the loop*/) {
            victim_task_pool = task_pool.load(std::memory_order_relaxed);
            // Microbenchmarks demonstrated that aborting stealing attempt when the
            // victim's task pool is locked degrade performance.
            // NOTE: Do not use comparison of head and tail indices to check for
            // the presence of work in the victim's task pool, as they may give
            // incorrect indication because of task pool relocations and resizes.
            if (victim_task_pool == EmptyTaskPool) {
                break;
            }
            d1::task** expected = victim_task_pool;
            if (victim_task_pool != LockedTaskPool && task_pool.compare_exchange_strong(expected, LockedTaskPool) ) {
                // We've locked victim's task pool
                break;
            } 
            // Someone else acquired a lock, so pause and do exponential backoff.
            backoff.pause();
        }
        __TBB_ASSERT(victim_task_pool == EmptyTaskPool ||
                    (task_pool.load(std::memory_order_relaxed) == LockedTaskPool &&
                    victim_task_pool != LockedTaskPool), "not really locked victim's task pool?");
        return victim_task_pool;
    }

    //! Unlocks victim's task pool
    /** Restores victim_arena_slot->task_pool munged by lock_task_pool. **/
    void unlock_task_pool(d1::task** victim_task_pool) {
        __TBB_ASSERT(task_pool.load(std::memory_order_relaxed) == LockedTaskPool, "victim arena slot is not locked");
        __TBB_ASSERT(victim_task_pool != LockedTaskPool, nullptr);
        task_pool.store(victim_task_pool, std::memory_order_release);
    }

#if TBB_USE_ASSERT
    bool is_local_task_pool_quiescent() const {
        d1::task** tp = task_pool.load(std::memory_order_relaxed);
        return tp == EmptyTaskPool || tp == LockedTaskPool;
    }

    bool is_quiescent_local_task_pool_empty() const {
        __TBB_ASSERT(is_local_task_pool_quiescent(), "Task pool is not quiescent");
        return head.load(std::memory_order_relaxed) == tail.load(std::memory_order_relaxed);
    }

    bool is_quiescent_local_task_pool_reset() const {
        __TBB_ASSERT(is_local_task_pool_quiescent(), "Task pool is not quiescent");
        return head.load(std::memory_order_relaxed) == 0 && tail.load(std::memory_order_relaxed) == 0;
    }
#endif // TBB_USE_ASSERT

    //! Leave the task pool
    /** Leaving task pool automatically releases the task pool if it is locked. **/
    void leave_task_pool() {
        __TBB_ASSERT(is_task_pool_published(), "Not in arena");
        // Do not reset my_arena_index. It will be used to (attempt to) re-acquire the slot next time
        __TBB_ASSERT(task_pool.load(std::memory_order_relaxed) == LockedTaskPool, "Task pool must be locked when leaving arena");
        __TBB_ASSERT(is_quiescent_local_task_pool_empty(), "Cannot leave arena when the task pool is not empty");
        // No release fence is necessary here as this assignment precludes external
        // accesses to the local task pool when becomes visible. Thus it is harmless
        // if it gets hoisted above preceding local bookkeeping manipulations.
        task_pool.store(EmptyTaskPool, std::memory_order_relaxed);
    }

    //! Resets head and tail indices to 0, and leaves task pool
    /** The task pool must be locked by the owner (via acquire_task_pool).**/
    void reset_task_pool_and_leave() {
        __TBB_ASSERT(task_pool.load(std::memory_order_relaxed) == LockedTaskPool, "Task pool must be locked when resetting task pool");
        tail.store(0, std::memory_order_relaxed);
        head.store(0, std::memory_order_relaxed);
        leave_task_pool();
    }

    //! Makes relocated tasks visible to thieves and releases the local task pool.
    /** Obviously, the task pool must be locked when calling this method. **/
    void commit_relocated_tasks(std::size_t new_tail) {
        __TBB_ASSERT(is_local_task_pool_quiescent(), "Task pool must be locked when calling commit_relocated_tasks()");
        head.store(0, std::memory_order_relaxed);
        // Tail is updated last to minimize probability of a thread making arena
        // snapshot being misguided into thinking that this task pool is empty.
        tail.store(new_tail, std::memory_order_release);
        release_task_pool();
    }
};

} // namespace r1
} // namespace detail
} // namespace tbb

#endif // __TBB_arena_slot_H
