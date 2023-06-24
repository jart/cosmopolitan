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

#ifndef __TBB_observer_proxy_H
#define __TBB_observer_proxy_H

#include "third_party/tbb/detail/_config.h"
#include "third_party/tbb/detail/_aligned_space.h"

#include "third_party/tbb/task_scheduler_observer.h"
#include "third_party/tbb/spin_rw_mutex.h"

namespace tbb {
namespace detail {
namespace r1 {

class observer_list {
    friend class arena;

    // Mutex is wrapped with aligned_space to shut up warnings when its destructor
    // is called while threads are still using it.
    typedef aligned_space<spin_rw_mutex>  my_mutex_type;

    //! Pointer to the head of this list.
    std::atomic<observer_proxy*> my_head{nullptr};

    //! Pointer to the tail of this list.
    std::atomic<observer_proxy*> my_tail{nullptr};

    //! Mutex protecting this list.
    my_mutex_type my_mutex;

    //! Back-pointer to the arena this list belongs to.
    arena* my_arena;

    //! Decrement refcount of the proxy p if there are other outstanding references.
    /** In case of success sets p to nullptr. Must be invoked from under the list lock. **/
    inline static void remove_ref_fast( observer_proxy*& p );

    //! Implements notify_entry_observers functionality.
    void do_notify_entry_observers( observer_proxy*& last, bool worker );

    //! Implements notify_exit_observers functionality.
    void do_notify_exit_observers( observer_proxy* last, bool worker );

public:
    observer_list () = default;

    //! Removes and destroys all observer proxies from the list.
    /** Cannot be used concurrently with other methods. **/
    void clear ();

    //! Add observer proxy to the tail of the list.
    void insert ( observer_proxy* p );

    //! Remove observer proxy from the list.
    void remove ( observer_proxy* p );

    //! Decrement refcount of the proxy and destroy it if necessary.
    /** When refcount reaches zero removes the proxy from the list and destructs it. **/
    void remove_ref( observer_proxy* p );

    //! Type of the scoped lock for the reader-writer mutex associated with the list.
    typedef spin_rw_mutex::scoped_lock scoped_lock;

    //! Accessor to the reader-writer mutex associated with the list.
    spin_rw_mutex& mutex () { return my_mutex.begin()[0]; }

    //! Call entry notifications on observers added after last was notified.
    /** Updates last to become the last notified observer proxy (in the global list)
        or leaves it to be nullptr. The proxy has its refcount incremented. **/
    inline void notify_entry_observers( observer_proxy*& last, bool worker );

    //! Call exit notifications on last and observers added before it.
    inline void notify_exit_observers( observer_proxy*& last, bool worker );
}; // class observer_list

//! Wrapper for an observer object
/** To maintain shared lists of observers the scheduler first wraps each observer
    object into a proxy so that a list item remained valid even after the corresponding
    proxy object is destroyed by the user code. **/
class observer_proxy {
    friend class d1::task_scheduler_observer;
    friend class observer_list;
    friend void observe(d1::task_scheduler_observer&, bool);
    //! Reference count used for garbage collection.
    /** 1 for reference from my task_scheduler_observer.
        1 for each task dispatcher's last observer pointer.
        No accounting for neighbors in the shared list. */
    std::atomic<std::uintptr_t> my_ref_count;
    //! Reference to the list this observer belongs to.
    observer_list* my_list;
    //! Pointer to next observer in the list specified by my_head.
    /** nullptr for the last item in the list. **/
    observer_proxy* my_next;
    //! Pointer to the previous observer in the list specified by my_head.
    /** For the head of the list points to the last item. **/
    observer_proxy* my_prev;
    //! Associated observer
    d1::task_scheduler_observer* my_observer;

    //! Constructs proxy for the given observer and adds it to the specified list.
    observer_proxy( d1::task_scheduler_observer& );

    ~observer_proxy();
}; // class observer_proxy

void observer_list::remove_ref_fast( observer_proxy*& p ) {
    if( p->my_observer ) {
        // Can decrement refcount quickly, as it cannot drop to zero while under the lock.
        std::uintptr_t r = --p->my_ref_count;
        __TBB_ASSERT_EX( r, nullptr);
        p = nullptr;
    } else {
        // Use slow form of refcount decrementing, after the lock is released.
    }
}

void observer_list::notify_entry_observers(observer_proxy*& last, bool worker) {
    if (last == my_tail.load(std::memory_order_relaxed))
        return;
    do_notify_entry_observers(last, worker);
}

void observer_list::notify_exit_observers( observer_proxy*& last, bool worker ) {
    if (last == nullptr) {
        return;
    }
    __TBB_ASSERT(!is_poisoned(last), nullptr);
    do_notify_exit_observers( last, worker );
    __TBB_ASSERT(last != nullptr, nullptr);
    poison_pointer(last);
}

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* __TBB_observer_proxy_H */
