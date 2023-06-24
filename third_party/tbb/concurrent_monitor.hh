// clang-format off
/*
    Copyright (c) 2005-2021 Intel Corporation

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

#ifndef __TBB_concurrent_monitor_H
#define __TBB_concurrent_monitor_H

#include "third_party/tbb/spin_mutex.hh"
#include "third_party/tbb/detail/_exception.hh"
#include "third_party/tbb/detail/_aligned_space.hh"
#include "third_party/tbb/concurrent_monitor_mutex.hh"
#include "third_party/tbb/semaphore.hh"

#include "third_party/libcxx/atomic"

namespace tbb {
namespace detail {
namespace r1 {

//! Circular doubly-linked list with sentinel
/** head.next points to the front and head.prev points to the back */
class circular_doubly_linked_list_with_sentinel {
public:
    struct base_node {
        base_node* next;
        base_node* prev;

        constexpr base_node(base_node* n, base_node* p) : next(n), prev(p) {}
        explicit base_node() : next((base_node*)(uintptr_t)0xcdcdcdcd), prev((base_node*)(uintptr_t)0xcdcdcdcd) {}
    };

    // ctor
    constexpr circular_doubly_linked_list_with_sentinel() : count(0), head(&head, &head) {}

    circular_doubly_linked_list_with_sentinel(const circular_doubly_linked_list_with_sentinel&) = delete;
    circular_doubly_linked_list_with_sentinel& operator=(const circular_doubly_linked_list_with_sentinel&) = delete;

    inline std::size_t size() const { return count.load(std::memory_order_relaxed); }
    inline bool empty() const { return size() == 0; }
    inline base_node* front() const { return head.next; }
    inline base_node* last() const { return head.prev; }
    inline const base_node* end() const { return &head; }

    //! add to the back of the list
    inline void add( base_node* n ) {
        count.store(count.load(std::memory_order_relaxed) + 1, std::memory_order_relaxed);
        n->prev = head.prev;
        n->next = &head;
        head.prev->next = n;
        head.prev = n;
    }

    //! remove node 'n'
    inline void remove( base_node& n ) {
        __TBB_ASSERT(count.load(std::memory_order_relaxed) > 0, "attempt to remove an item from an empty list");
        count.store(count.load( std::memory_order_relaxed ) - 1, std::memory_order_relaxed);
        n.prev->next = n.next;
        n.next->prev = n.prev;
    }

    //! move all elements to 'lst' and initialize the 'this' list
    inline void flush_to( circular_doubly_linked_list_with_sentinel& lst ) {
        const std::size_t l_count = size();
        if (l_count > 0) {
            lst.count.store(l_count, std::memory_order_relaxed);
            lst.head.next = head.next;
            lst.head.prev = head.prev;
            head.next->prev = &lst.head;
            head.prev->next = &lst.head;
            clear();
        }
    }

    void clear() {
        head.next = &head;
        head.prev = &head;
        count.store(0, std::memory_order_relaxed);
    }
private:
    std::atomic<std::size_t> count;
    base_node head;
};

using base_list = circular_doubly_linked_list_with_sentinel;
using base_node = circular_doubly_linked_list_with_sentinel::base_node;

template <typename Context>
class concurrent_monitor_base;

template <typename Context>
class wait_node : public base_node {
public:

#if __TBB_GLIBCXX_VERSION >= 40800 && __TBB_GLIBCXX_VERSION < 40900
    wait_node(Context ctx) : my_context(ctx), my_is_in_list(false) {}
#else
    wait_node(Context ctx) : my_context(ctx) {}
#endif

    virtual ~wait_node() = default;

    virtual void init() {
        __TBB_ASSERT(!my_initialized, nullptr);
        my_initialized = true;
    }

    virtual void wait() = 0;

    virtual void reset() {
        __TBB_ASSERT(my_skipped_wakeup, nullptr);
        my_skipped_wakeup = false;
    }

    virtual void notify() = 0;

protected:
    friend class concurrent_monitor_base<Context>;
    friend class thread_data;

    Context my_context{};
#if __TBB_GLIBCXX_VERSION >= 40800 && __TBB_GLIBCXX_VERSION < 40900
    std::atomic<bool> my_is_in_list;
#else
    std::atomic<bool> my_is_in_list{false};
#endif

    bool my_initialized{false};
    bool my_skipped_wakeup{false};
    bool my_aborted{false};
    unsigned my_epoch{0};
};

template <typename Context>
class sleep_node : public wait_node<Context> {
    using base_type = wait_node<Context>;
public:
    using base_type::base_type;

    ~sleep_node() override {
        if (this->my_initialized) {
            if (this->my_skipped_wakeup) semaphore().P();
            semaphore().~binary_semaphore();
        }
    }

    binary_semaphore& semaphore() { return *sema.begin(); }

    void init() override {
        if (!this->my_initialized) {
            new (sema.begin()) binary_semaphore;
            base_type::init();
        }
    }

    void wait() override {
        __TBB_ASSERT(this->my_initialized,
            "Use of commit_wait() without prior prepare_wait()");
        semaphore().P();
        __TBB_ASSERT(!this->my_is_in_list.load(std::memory_order_relaxed), "Still in the queue?");
        if (this->my_aborted)
            throw_exception(exception_id::user_abort);
    }

    void reset() override {
        base_type::reset();
        semaphore().P();
    }

    void notify() override {
        semaphore().V();
    }

private:
    tbb::detail::aligned_space<binary_semaphore> sema;
};

//! concurrent_monitor
/** fine-grained concurrent_monitor implementation */
template <typename Context>
class concurrent_monitor_base {
public:
    //! ctor
    constexpr concurrent_monitor_base() {}
    //! dtor
    ~concurrent_monitor_base() = default;

    concurrent_monitor_base(const concurrent_monitor_base&) = delete;
    concurrent_monitor_base& operator=(const concurrent_monitor_base&) = delete;

    //! prepare wait by inserting 'thr' into the wait queue
    void prepare_wait( wait_node<Context>& node) {
        // TODO: consider making even more lazy instantiation of the semaphore, that is only when it is actually needed, e.g. move it in node::wait()
        if (!node.my_initialized) {
            node.init();
        }
        // this is good place to pump previous skipped wakeup
        else if (node.my_skipped_wakeup) {
            node.reset();
        }

        node.my_is_in_list.store(true, std::memory_order_relaxed);

        {
            concurrent_monitor_mutex::scoped_lock l(my_mutex);
            node.my_epoch = my_epoch.load(std::memory_order_relaxed);
            my_waitset.add(&node);
        }

        // Prepare wait guarantees Write Read memory barrier.
        // In C++ only full fence covers this type of barrier.
        atomic_fence_seq_cst();
    }

    //! Commit wait if event count has not changed; otherwise, cancel wait.
    /** Returns true if committed, false if canceled. */
    inline bool commit_wait( wait_node<Context>& node ) {
        const bool do_it = node.my_epoch == my_epoch.load(std::memory_order_relaxed);
        // this check is just an optimization
        if (do_it) {
           node.wait();
        } else {
            cancel_wait( node );
        }
        return do_it;
    }

    //! Cancel the wait. Removes the thread from the wait queue if not removed yet.
    void cancel_wait( wait_node<Context>& node ) {
        // possible skipped wakeup will be pumped in the following prepare_wait()
        node.my_skipped_wakeup = true;
        // try to remove node from waitset
        // Cancel wait guarantees acquire memory barrier.
        bool in_list = node.my_is_in_list.load(std::memory_order_acquire);
        if (in_list) {
            concurrent_monitor_mutex::scoped_lock l(my_mutex);
            if (node.my_is_in_list.load(std::memory_order_relaxed)) {
                my_waitset.remove(node);
                // node is removed from waitset, so there will be no wakeup
                node.my_is_in_list.store(false, std::memory_order_relaxed);
                node.my_skipped_wakeup = false;
            }
        }
    }

    //! Wait for a condition to be satisfied with waiting-on my_context
    template <typename NodeType, typename Pred>
    bool wait(Pred&& pred, NodeType&& node) {
        prepare_wait(node);
        while (!guarded_call(std::forward<Pred>(pred), node)) {
            if (commit_wait(node)) {
                return true;
            }

            prepare_wait(node);
        }

        cancel_wait(node);
        return false;
    }

    //! Notify one thread about the event
    void notify_one() {
        atomic_fence_seq_cst();
        notify_one_relaxed();
    }

    //! Notify one thread about the event. Relaxed version.
    void notify_one_relaxed() {
        if (my_waitset.empty()) {
            return;
        }

        base_node* n;
        const base_node* end = my_waitset.end();
        {
            concurrent_monitor_mutex::scoped_lock l(my_mutex);
            my_epoch.store(my_epoch.load(std::memory_order_relaxed) + 1, std::memory_order_relaxed);
            n = my_waitset.front();
            if (n != end) {
                my_waitset.remove(*n);
                to_wait_node(n)->my_is_in_list.store(false, std::memory_order_relaxed);
            }
        }

        if (n != end) {
            to_wait_node(n)->notify();
        }
    }

    //! Notify all waiting threads of the event
    void notify_all() {
        atomic_fence_seq_cst();
        notify_all_relaxed();
    }

    // ! Notify all waiting threads of the event; Relaxed version
    void notify_all_relaxed() {
        if (my_waitset.empty()) {
            return;
        }

        base_list temp;
        const base_node* end;
        {
            concurrent_monitor_mutex::scoped_lock l(my_mutex);
            my_epoch.store(my_epoch.load(std::memory_order_relaxed) + 1, std::memory_order_relaxed);
            // TODO: Possible optimization, don't change node state under lock, just do flush
            my_waitset.flush_to(temp);
            end = temp.end();
            for (base_node* n = temp.front(); n != end; n = n->next) {
                to_wait_node(n)->my_is_in_list.store(false, std::memory_order_relaxed);
            }
        }

        base_node* nxt;
        for (base_node* n = temp.front(); n != end; n=nxt) {
            nxt = n->next;
            to_wait_node(n)->notify();
        }
#if TBB_USE_ASSERT
        temp.clear();
#endif
    }

    //! Notify waiting threads of the event that satisfies the given predicate
    template <typename P>
    void notify( const P& predicate ) {
        atomic_fence_seq_cst();
        notify_relaxed( predicate );
    }

    //! Notify waiting threads of the event that satisfies the given predicate;
    //! the predicate is called under the lock. Relaxed version.
    template<typename P>
    void notify_relaxed( const P& predicate ) {
        if (my_waitset.empty()) {
            return;
        }

        base_list temp;
        base_node* nxt;
        const base_node* end = my_waitset.end();
        {
            concurrent_monitor_mutex::scoped_lock l(my_mutex);
            my_epoch.store(my_epoch.load( std::memory_order_relaxed ) + 1, std::memory_order_relaxed);
            for (base_node* n = my_waitset.last(); n != end; n = nxt) {
                nxt = n->prev;
                auto* node = static_cast<wait_node<Context>*>(n);
                if (predicate(node->my_context)) {
                    my_waitset.remove(*n);
                    node->my_is_in_list.store(false, std::memory_order_relaxed);
                    temp.add(n);
                }
            }
        }

        end = temp.end();
        for (base_node* n=temp.front(); n != end; n = nxt) {
            nxt = n->next;
            to_wait_node(n)->notify();
        }
#if TBB_USE_ASSERT
        temp.clear();
#endif
    }

    //! Notify waiting threads of the event that satisfies the given predicate;
    //! the predicate is called under the lock. Relaxed version.
    template<typename P>
    void notify_one_relaxed( const P& predicate ) {
        if (my_waitset.empty()) {
            return;
        }

        base_node* tmp = nullptr;
        base_node* next{};
        const base_node* end = my_waitset.end();
        {
            concurrent_monitor_mutex::scoped_lock l(my_mutex);
            my_epoch.store(my_epoch.load( std::memory_order_relaxed ) + 1, std::memory_order_relaxed);
            for (base_node* n = my_waitset.last(); n != end; n = next) {
                next = n->prev;
                auto* node = static_cast<wait_node<Context>*>(n);
                if (predicate(node->my_context)) {
                    my_waitset.remove(*n);
                    node->my_is_in_list.store(false, std::memory_order_relaxed);
                    tmp = n;
                    break;
                }
            }
        }

        if (tmp) {
            to_wait_node(tmp)->notify();
        }
    }

    //! Abort any sleeping threads at the time of the call
    void abort_all() {
        atomic_fence_seq_cst();
        abort_all_relaxed();
    }

    //! Abort any sleeping threads at the time of the call; Relaxed version
    void abort_all_relaxed() {
        if (my_waitset.empty()) {
            return;
        }

        base_list temp;
        const base_node* end;
        {
            concurrent_monitor_mutex::scoped_lock l(my_mutex);
            my_epoch.store(my_epoch.load(std::memory_order_relaxed) + 1, std::memory_order_relaxed);
            my_waitset.flush_to(temp);
            end = temp.end();
            for (base_node* n = temp.front(); n != end; n = n->next) {
                to_wait_node(n)->my_is_in_list.store(false, std::memory_order_relaxed);
            }
        }

        base_node* nxt;
        for (base_node* n = temp.front(); n != end; n = nxt) {
            nxt = n->next;
            to_wait_node(n)->my_aborted = true;
            to_wait_node(n)->notify();
        }
#if TBB_USE_ASSERT
        temp.clear();
#endif
    }

    void destroy() {
        this->abort_all();
        my_mutex.destroy();
        __TBB_ASSERT(this->my_waitset.empty(), "waitset not empty?");
    }

private:
    template <typename NodeType, typename Pred>
    bool guarded_call(Pred&& predicate, NodeType& node) {
        bool res = false;
        tbb::detail::d0::try_call( [&] {
            res = std::forward<Pred>(predicate)();
        }).on_exception( [&] {
            cancel_wait(node);
        });

        return res;
    }

    concurrent_monitor_mutex my_mutex{};
    base_list my_waitset{};
    std::atomic<unsigned> my_epoch{};

    wait_node<Context>* to_wait_node( base_node* node ) { return static_cast<wait_node<Context>*>(node); }
};

class concurrent_monitor : public concurrent_monitor_base<std::uintptr_t> {
    using base_type = concurrent_monitor_base<std::uintptr_t>;
public:
    using base_type::base_type;

    ~concurrent_monitor() {
        destroy();
    }

    /** per-thread descriptor for concurrent_monitor */
    using thread_context = sleep_node<std::uintptr_t>;
};

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* __TBB_concurrent_monitor_H */
