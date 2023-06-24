// clang-format off
/*
    Copyright (c) 2005-2022 Intel Corporation

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
#include "third_party/tbb/detail/_utils.h"

#include "third_party/tbb/observer_proxy.h"
#include "third_party/tbb/arena.h"
#include "third_party/tbb/main.h"
#include "third_party/tbb/thread_data.h"

#include "third_party/libcxx/atomic"

namespace tbb {
namespace detail {
namespace r1 {

#if TBB_USE_ASSERT
extern std::atomic<int> the_observer_proxy_count;
#endif /* TBB_USE_ASSERT */

observer_proxy::observer_proxy( d1::task_scheduler_observer& tso )
    : my_ref_count(1), my_list(nullptr), my_next(nullptr), my_prev(nullptr), my_observer(&tso)
{
#if TBB_USE_ASSERT
    ++the_observer_proxy_count;
#endif /* TBB_USE_ASSERT */
}

observer_proxy::~observer_proxy() {
    __TBB_ASSERT( !my_ref_count, "Attempt to destroy proxy still in use" );
    poison_value(my_ref_count);
    poison_pointer(my_prev);
    poison_pointer(my_next);
#if TBB_USE_ASSERT
    --the_observer_proxy_count;
#endif /* TBB_USE_ASSERT */
}

void observer_list::clear() {
    {
        scoped_lock lock(mutex(), /*is_writer=*/true);
        observer_proxy *next = my_head.load(std::memory_order_relaxed);
        while ( observer_proxy *p = next ) {
            next = p->my_next;
            // Both proxy p and observer p->my_observer (if non-null) are guaranteed
            // to be alive while the list is locked.
            d1::task_scheduler_observer *obs = p->my_observer;
            // Make sure that possible concurrent observer destruction does not
            // conflict with the proxy list cleanup.
            if (!obs || !(p = obs->my_proxy.exchange(nullptr))) {
                continue;
            }
            // accessing 'obs' after detaching of obs->my_proxy leads to the race with observer destruction
            __TBB_ASSERT(!next || p == next->my_prev, nullptr);
            __TBB_ASSERT(is_alive(p->my_ref_count), "Observer's proxy died prematurely");
            __TBB_ASSERT(p->my_ref_count.load(std::memory_order_relaxed) == 1, "Reference for observer is missing");
            poison_pointer(p->my_observer);
            remove(p);
            --p->my_ref_count;
            delete p;
        }
    }

    // If observe(false) is called concurrently with the destruction of the arena,
    // need to wait until all proxies are removed.
    for (atomic_backoff backoff; ; backoff.pause()) {
        scoped_lock lock(mutex(), /*is_writer=*/false);
        if (my_head.load(std::memory_order_relaxed) == nullptr) {
            break;
        }
    }

    __TBB_ASSERT(my_head.load(std::memory_order_relaxed) == nullptr && my_tail.load(std::memory_order_relaxed) == nullptr, nullptr);
}

void observer_list::insert( observer_proxy* p ) {
    scoped_lock lock(mutex(), /*is_writer=*/true);
    if (my_head.load(std::memory_order_relaxed)) {
        p->my_prev = my_tail.load(std::memory_order_relaxed);
        my_tail.load(std::memory_order_relaxed)->my_next = p;
    } else {
        my_head.store(p, std::memory_order_relaxed);
    }
    my_tail.store(p, std::memory_order_relaxed);
}

void observer_list::remove(observer_proxy* p) {
    __TBB_ASSERT(my_head.load(std::memory_order_relaxed), "Attempt to remove an item from an empty list");
    __TBB_ASSERT(!my_tail.load(std::memory_order_relaxed)->my_next, "Last item's my_next must be nullptr");
    if (p == my_tail.load(std::memory_order_relaxed)) {
        __TBB_ASSERT(!p->my_next, nullptr);
        my_tail.store(p->my_prev, std::memory_order_relaxed);
    } else {
        __TBB_ASSERT(p->my_next, nullptr);
        p->my_next->my_prev = p->my_prev;
    }
    if (p == my_head.load(std::memory_order_relaxed)) {
        __TBB_ASSERT(!p->my_prev, nullptr);
        my_head.store(p->my_next, std::memory_order_relaxed);
    } else {
        __TBB_ASSERT(p->my_prev, nullptr);
        p->my_prev->my_next = p->my_next;
    }
    __TBB_ASSERT((my_head.load(std::memory_order_relaxed) && my_tail.load(std::memory_order_relaxed)) ||
        (!my_head.load(std::memory_order_relaxed) && !my_tail.load(std::memory_order_relaxed)), nullptr);
}

void observer_list::remove_ref(observer_proxy* p) {
    std::uintptr_t r = p->my_ref_count.load(std::memory_order_acquire);
    __TBB_ASSERT(is_alive(r), nullptr);
    while (r > 1) {
        if (p->my_ref_count.compare_exchange_strong(r, r - 1)) {
            return;
        }
    }
    __TBB_ASSERT(r == 1, nullptr);
    // Reference count might go to zero
    {
        // Use lock to avoid resurrection by a thread concurrently walking the list
        observer_list::scoped_lock lock(mutex(), /*is_writer=*/true);
        r = --p->my_ref_count;
        if (!r) {
            remove(p);
        }
    }
    __TBB_ASSERT(r || !p->my_ref_count, nullptr);
    if (!r) {
        delete p;
    }
}

void observer_list::do_notify_entry_observers(observer_proxy*& last, bool worker) {
    // Pointer p marches though the list from last (exclusively) to the end.
    observer_proxy* p = last, * prev = p;
    for (;;) {
        d1::task_scheduler_observer* tso = nullptr;
        // Hold lock on list only long enough to advance to the next proxy in the list.
        {
            scoped_lock lock(mutex(), /*is_writer=*/false);
            do {
                if (p) {
                    // We were already processing the list.
                    if (observer_proxy* q = p->my_next) {
                        if (p == prev) {
                            remove_ref_fast(prev); // sets prev to nullptr if successful
                        }
                        p = q;
                    } else {
                        // Reached the end of the list.
                        if (p == prev) {
                            // Keep the reference as we store the 'last' pointer in scheduler
                            __TBB_ASSERT(int(p->my_ref_count.load(std::memory_order_relaxed)) >= 1 + (p->my_observer ? 1 : 0), nullptr);
                        } else {
                            // The last few proxies were empty
                            __TBB_ASSERT(int(p->my_ref_count.load(std::memory_order_relaxed)), nullptr);
                            ++p->my_ref_count;
                            if (prev) {
                                lock.release();
                                remove_ref(prev);
                            }
                        }
                        last = p;
                        return;
                    }
                } else {
                    // Starting pass through the list
                    p = my_head.load(std::memory_order_relaxed);
                    if (!p) {
                        return;
                    }
                }
                tso = p->my_observer;
            } while (!tso);
            ++p->my_ref_count;
            ++tso->my_busy_count;
        }
        __TBB_ASSERT(!prev || p != prev, nullptr);
        // Release the proxy pinned before p
        if (prev) {
            remove_ref(prev);
        }
        // Do not hold any locks on the list while calling user's code.
        // Do not intercept any exceptions that may escape the callback so that
        // they are either handled by the TBB scheduler or passed to the debugger.
        tso->on_scheduler_entry(worker);
        __TBB_ASSERT(p->my_ref_count.load(std::memory_order_relaxed), nullptr);
        intptr_t bc = --tso->my_busy_count;
        __TBB_ASSERT_EX(bc >= 0, "my_busy_count underflowed");
        prev = p;
    }
}

void observer_list::do_notify_exit_observers(observer_proxy* last, bool worker) {
    // Pointer p marches though the list from the beginning to last (inclusively).
    observer_proxy* p = nullptr, * prev = nullptr;
    for (;;) {
        d1::task_scheduler_observer* tso = nullptr;
        // Hold lock on list only long enough to advance to the next proxy in the list.
        {
            scoped_lock lock(mutex(), /*is_writer=*/false);
            do {
                if (p) {
                    // We were already processing the list.
                    if (p != last) {
                        __TBB_ASSERT(p->my_next, "List items before 'last' must have valid my_next pointer");
                        if (p == prev)
                            remove_ref_fast(prev); // sets prev to nullptr if successful
                        p = p->my_next;
                    } else {
                        // remove the reference from the last item
                        remove_ref_fast(p);
                        if (p) {
                            lock.release();
                            if (p != prev && prev) {
                                remove_ref(prev);
                            }
                            remove_ref(p);
                        }
                        return;
                    }
                } else {
                    // Starting pass through the list
                    p = my_head.load(std::memory_order_relaxed);
                    __TBB_ASSERT(p, "Nonzero 'last' must guarantee that the global list is non-empty");
                }
                tso = p->my_observer;
            } while (!tso);
            // The item is already refcounted
            if (p != last) // the last is already referenced since entry notification
                ++p->my_ref_count;
            ++tso->my_busy_count;
        }
        __TBB_ASSERT(!prev || p != prev, nullptr);
        if (prev)
            remove_ref(prev);
        // Do not hold any locks on the list while calling user's code.
        // Do not intercept any exceptions that may escape the callback so that
        // they are either handled by the TBB scheduler or passed to the debugger.
        tso->on_scheduler_exit(worker);
        __TBB_ASSERT(p->my_ref_count || p == last, nullptr);
        intptr_t bc = --tso->my_busy_count;
        __TBB_ASSERT_EX(bc >= 0, "my_busy_count underflowed");
        prev = p;
    }
}

void __TBB_EXPORTED_FUNC observe(d1::task_scheduler_observer &tso, bool enable) {
    if( enable ) {
        if( !tso.my_proxy.load(std::memory_order_relaxed) ) {
            observer_proxy* p = new observer_proxy(tso);
            tso.my_proxy.store(p, std::memory_order_relaxed);
            tso.my_busy_count.store(0, std::memory_order_relaxed);

            thread_data* td = governor::get_thread_data_if_initialized();
            if (p->my_observer->my_task_arena == nullptr) {
                if (!(td && td->my_arena)) {
                    td = governor::get_thread_data();
                }
                __TBB_ASSERT(__TBB_InitOnce::initialization_done(), nullptr);
                __TBB_ASSERT(td && td->my_arena, nullptr);
                p->my_list = &td->my_arena->my_observers;
            } else {
                d1::task_arena* ta = p->my_observer->my_task_arena;
                arena* a = ta->my_arena.load(std::memory_order_acquire);
                if (a == nullptr) { // Avoid recursion during arena initialization
                    ta->initialize();
                    a = ta->my_arena.load(std::memory_order_relaxed);
                }
                __TBB_ASSERT(a != nullptr, nullptr);
                p->my_list = &a->my_observers;
            }
            p->my_list->insert(p);
            // Notify newly activated observer and other pending ones if it belongs to current arena
            if (td && td->my_arena && &td->my_arena->my_observers == p->my_list) {
                p->my_list->notify_entry_observers(td->my_last_observer, td->my_is_worker);
            }
        }
    } else {
        // Make sure that possible concurrent proxy list cleanup does not conflict
        // with the observer destruction here.
        if ( observer_proxy* proxy = tso.my_proxy.exchange(nullptr) ) {
            // List destruction should not touch this proxy after we've won the above interlocked exchange.
            __TBB_ASSERT( proxy->my_observer == &tso, nullptr);
            __TBB_ASSERT( is_alive(proxy->my_ref_count.load(std::memory_order_relaxed)), "Observer's proxy died prematurely" );
            __TBB_ASSERT( proxy->my_ref_count.load(std::memory_order_relaxed) >= 1, "reference for observer missing" );
            observer_list &list = *proxy->my_list;
            {
                // Ensure that none of the list walkers relies on observer pointer validity
                observer_list::scoped_lock lock(list.mutex(), /*is_writer=*/true);
                proxy->my_observer = nullptr;
                // Proxy may still be held by other threads (to track the last notified observer)
                if( !--proxy->my_ref_count ) {// nobody can increase it under exclusive lock
                    list.remove(proxy);
                    __TBB_ASSERT( !proxy->my_ref_count, nullptr);
                    delete proxy;
                }
            }
            spin_wait_until_eq(tso.my_busy_count, 0); // other threads are still accessing the callback
        }
    }
}

} // namespace r1
} // namespace detail
} // namespace tbb
