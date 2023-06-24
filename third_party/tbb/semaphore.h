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

#ifndef __TBB_semaphore_H
#define __TBB_semaphore_H

#include "third_party/tbb/detail/_utils.h"

#if _WIN32||_WIN64
#include "libc/nt/accounting.h"
#include "libc/nt/automation.h"
#include "libc/nt/console.h"
#include "libc/nt/debug.h"
#include "libc/nt/dll.h"
#include "libc/nt/enum/keyaccess.h"
#include "libc/nt/enum/regtype.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/memory.h"
#include "libc/nt/paint.h"
#include "libc/nt/process.h"
#include "libc/nt/registry.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/windows.h"
#include "libc/nt/winsock.h"
#elif __APPLE__
// MISSING #include <dispatch/dispatch.h>
#else
#include "libc/thread/semaphore.h"
#ifdef TBB_USE_DEBUG
#include "third_party/libcxx/cerrno"
#endif
#endif /*_WIN32||_WIN64*/

#include "third_party/libcxx/atomic"

#if __unix__
#if defined(__has_include)
#define __TBB_has_include __has_include
#else
#define __TBB_has_include(x) 0
#endif

/* Futex definitions */
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/pathconf.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/time/time.h"
#include "third_party/getopt/getopt.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/lockf.h"
#if defined(__linux__) || __TBB_has_include(<sys/syscall.h>)
#include "libc/stdio/syscall.h"
#endif

#if defined(SYS_futex)

/* This section is included for Linux and some other systems that may support futexes.*/

#define __TBB_USE_FUTEX 1

/*
If available, use typical headers where futex API is defined. While Linux and OpenBSD
are known to provide such headers, other systems might have them as well.
*/
#if defined(__linux__) || __TBB_has_include(<linux/futex.h>)
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/nr.h"
#elif defined(__OpenBSD__) || __TBB_has_include(<sys/futex.h>)
// MISSING #include <sys/futex.h>
#endif

#include "third_party/libcxx/climits"
#include "third_party/libcxx/cerrno"

/*
Some systems might not define the macros or use different names. In such case we expect
the actual parameter values to match Linux: 0 for wait, 1 for wake.
*/
#if defined(FUTEX_WAIT_PRIVATE)
#define __TBB_FUTEX_WAIT FUTEX_WAIT_PRIVATE
#elif defined(FUTEX_WAIT)
#define __TBB_FUTEX_WAIT FUTEX_WAIT
#else
#define __TBB_FUTEX_WAIT 0
#endif

#if defined(FUTEX_WAKE_PRIVATE)
#define __TBB_FUTEX_WAKE FUTEX_WAKE_PRIVATE
#elif defined(FUTEX_WAKE)
#define __TBB_FUTEX_WAKE FUTEX_WAKE
#else
#define __TBB_FUTEX_WAKE 1
#endif

#endif // SYS_futex
#endif // __unix__

namespace tbb {
namespace detail {
namespace r1 {

////////////////////////////////////////////////////////////////////////////////////////////////////
// Futex implementation
////////////////////////////////////////////////////////////////////////////////////////////////////

#if __TBB_USE_FUTEX

static inline int futex_wait( void *futex, int comparand ) {
    int r = ::syscall(SYS_futex, futex, __TBB_FUTEX_WAIT, comparand, nullptr, nullptr, 0);
#if TBB_USE_ASSERT
    int e = errno;
    __TBB_ASSERT(r == 0 || r == EWOULDBLOCK || (r == -1 && (e == EAGAIN || e == EINTR)), "futex_wait failed.");
#endif /* TBB_USE_ASSERT */
    return r;
}

static inline int futex_wakeup_one( void *futex ) {
    int r = ::syscall(SYS_futex, futex, __TBB_FUTEX_WAKE, 1, nullptr, nullptr, 0);
    __TBB_ASSERT(r == 0 || r == 1, "futex_wakeup_one: more than one thread woken up?");
    return r;
}

// Additional possible methods that are not required right now
// static inline int futex_wakeup_all( void *futex ) {
//     int r = ::syscall( SYS_futex,futex,__TBB_FUTEX_WAKE,INT_MAX,nullptr,nullptr,0 );
//     __TBB_ASSERT( r>=0, "futex_wakeup_all: error in waking up threads" );
//     return r;
// }

#endif // __TBB_USE_FUTEX

////////////////////////////////////////////////////////////////////////////////////////////////////
#if _WIN32||_WIN64
typedef LONG sem_count_t;
//! Edsger Dijkstra's counting semaphore
class semaphore : no_copy {
    static const int max_semaphore_cnt = MAXLONG;
public:
    //! ctor
    semaphore(size_t start_cnt_ = 0) {init_semaphore(start_cnt_);}
    //! dtor
    ~semaphore() {CloseHandle( sem );}
    //! wait/acquire
    void P() {WaitForSingleObjectEx( sem, INFINITE, FALSE );}
    //! post/release
    void V() {ReleaseSemaphore( sem, 1, nullptr);}
private:
    HANDLE sem;
    void init_semaphore(size_t start_cnt_) {
        sem = CreateSemaphoreEx( nullptr, LONG(start_cnt_), max_semaphore_cnt, nullptr, 0, SEMAPHORE_ALL_ACCESS );
    }
};
#elif __APPLE__
//! Edsger Dijkstra's counting semaphore
class semaphore : no_copy {
public:
    //! ctor
    semaphore(int start_cnt_ = 0) { my_sem = dispatch_semaphore_create(start_cnt_); }
    //! dtor
    ~semaphore() { dispatch_release(my_sem); }
    //! wait/acquire
    void P() {
        std::intptr_t ret = dispatch_semaphore_wait(my_sem, DISPATCH_TIME_FOREVER);
        __TBB_ASSERT_EX(ret == 0, "dispatch_semaphore_wait() failed");
    }
    //! post/release
    void V() { dispatch_semaphore_signal(my_sem); }
private:
    dispatch_semaphore_t my_sem;
};
#else /* Linux/Unix */
typedef uint32_t sem_count_t;
//! Edsger Dijkstra's counting semaphore
class semaphore : no_copy {
public:
    //! ctor
    semaphore(int start_cnt_ = 0 ) { init_semaphore( start_cnt_ ); }

    //! dtor
    ~semaphore() {
        int ret = sem_destroy( &sem );
        __TBB_ASSERT_EX( !ret, nullptr);
    }
    //! wait/acquire
    void P() {
        while( sem_wait( &sem )!=0 )
            __TBB_ASSERT( errno==EINTR, nullptr);
    }
    //! post/release
    void V() { sem_post( &sem ); }
private:
    sem_t sem;
    void init_semaphore(int start_cnt_) {
        int ret = sem_init( &sem, /*shared among threads*/ 0, start_cnt_ );
        __TBB_ASSERT_EX( !ret, nullptr);
    }
};
#endif /* _WIN32||_WIN64 */


//! for performance reasons, we want specialized binary_semaphore
#if _WIN32||_WIN64
#if !__TBB_USE_SRWLOCK
//! binary_semaphore for concurrent_monitor
class binary_semaphore : no_copy {
public:
    //! ctor
    binary_semaphore() { my_sem = CreateEventEx( nullptr, nullptr, 0, EVENT_ALL_ACCESS );  }
    //! dtor
    ~binary_semaphore() { CloseHandle( my_sem ); }
    //! wait/acquire
    void P() { WaitForSingleObjectEx( my_sem, INFINITE, FALSE ); }
    //! post/release
    void V() { SetEvent( my_sem ); }
private:
    HANDLE my_sem;
};
#else /* __TBB_USE_SRWLOCK */

union srwl_or_handle {
    SRWLOCK lock;
    HANDLE  h;
};

//! binary_semaphore for concurrent_monitor
class binary_semaphore : no_copy {
public:
    //! ctor
    binary_semaphore();
    //! dtor
    ~binary_semaphore();
    //! wait/acquire
    void P();
    //! post/release
    void V();
private:
    srwl_or_handle my_sem;
};
#endif /* !__TBB_USE_SRWLOCK */
#elif __APPLE__
//! binary_semaphore for concurrent monitor
using binary_semaphore = semaphore;
#else /* Linux/Unix */

#if __TBB_USE_FUTEX
class binary_semaphore : no_copy {
// The implementation is equivalent to the "Mutex, Take 3" one
// in the paper "Futexes Are Tricky" by Ulrich Drepper
public:
    //! ctor
    binary_semaphore() { my_sem = 1; }
    //! dtor
    ~binary_semaphore() {}
    //! wait/acquire
    void P() {
        int s = 0;
        if( !my_sem.compare_exchange_strong( s, 1 ) ) {
            if( s!=2 )
                s = my_sem.exchange( 2 );
            while( s!=0 ) { // This loop deals with spurious wakeup
                futex_wait( &my_sem, 2 );
                s = my_sem.exchange( 2 );
            }
        }
    }
    //! post/release
    void V() {
        __TBB_ASSERT( my_sem.load(std::memory_order_relaxed)>=1, "multiple V()'s in a row?" );
        if( my_sem.exchange( 0 )==2 )
            futex_wakeup_one( &my_sem );
    }
private:
    std::atomic<int> my_sem; // 0 - open; 1 - closed, no waits; 2 - closed, possible waits
};
#else
typedef uint32_t sem_count_t;
//! binary_semaphore for concurrent monitor
class binary_semaphore : no_copy {
public:
    //! ctor
    binary_semaphore() {
        int ret = sem_init( &my_sem, /*shared among threads*/ 0, 0 );
        __TBB_ASSERT_EX( !ret, nullptr);
    }
    //! dtor
    ~binary_semaphore() {
        int ret = sem_destroy( &my_sem );
        __TBB_ASSERT_EX( !ret, nullptr);
    }
    //! wait/acquire
    void P() {
        while( sem_wait( &my_sem )!=0 )
            __TBB_ASSERT( errno==EINTR, nullptr);
    }
    //! post/release
    void V() { sem_post( &my_sem ); }
private:
    sem_t my_sem;
};
#endif /* __TBB_USE_FUTEX */
#endif /* _WIN32||_WIN64 */

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* __TBB_semaphore_H */
