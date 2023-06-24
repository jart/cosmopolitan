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

#ifndef _TBB_co_context_H
#define _TBB_co_context_H

#include "third_party/tbb/detail/_config.h"

#if __TBB_RESUMABLE_TASKS

#include "third_party/libcxx/cstddef"
#include "third_party/libcxx/cstdint"

#if __TBB_RESUMABLE_TASKS_USE_THREADS

#if _WIN32 || _WIN64
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
#else
#include "libc/calls/weirdtypes.h"
#include "libc/sysv/consts/clock.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#endif

#include "third_party/libcxx/condition_variable"
#include "third_party/tbb/governor.h"

#elif _WIN32 || _WIN64
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
#else
// ucontext.h API is deprecated since macOS 10.6
#if __APPLE__
    #if __INTEL_COMPILER
        #pragma warning(push)
        #pragma warning(disable:1478)
    #elif __clang__
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    #endif
#endif // __APPLE__

// MISSING #include <ucontext.h>
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mlock.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/posix.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/mfd.h"
#include "libc/sysv/consts/mremap.h" // mprotect

#include "third_party/tbb/governor.h" // default_page_size()

#ifndef MAP_STACK
// macOS* does not define MAP_STACK
#define MAP_STACK 0
#endif
#ifndef MAP_ANONYMOUS
// macOS* defines MAP_ANON, which is deprecated in Linux*.
#define MAP_ANONYMOUS MAP_ANON
#endif
#endif // _WIN32 || _WIN64

namespace tbb {
namespace detail {
namespace r1 {

#if __TBB_RESUMABLE_TASKS_USE_THREADS
    struct coroutine_type {
#if _WIN32 || _WIN64
        using handle_type = HANDLE;
#else
        using handle_type = pthread_t;
#endif

        handle_type my_thread;
        std::condition_variable my_condvar;
        std::mutex my_mutex;
        thread_data* my_thread_data{ nullptr };
        bool my_is_active{ true };
    };
#elif _WIN32 || _WIN64
    typedef LPVOID coroutine_type;
#else
    struct coroutine_type {
        coroutine_type() : my_context(), my_stack(), my_stack_size() {}
        ucontext_t my_context;
        void* my_stack;
        std::size_t my_stack_size;
    };
#endif

    // Forward declaration of the coroutine API.
    void create_coroutine(coroutine_type& c, std::size_t stack_size, void* arg);
    void current_coroutine(coroutine_type& c);
    void swap_coroutine(coroutine_type& prev_coroutine, coroutine_type& new_coroutine);
    void destroy_coroutine(coroutine_type& c);

class co_context {
    enum co_state {
        co_invalid,
        co_suspended,
        co_executing,
        co_destroyed
    };
    coroutine_type      my_coroutine;
    co_state            my_state;

public:
    co_context(std::size_t stack_size, void* arg)
        : my_state(stack_size ? co_suspended : co_executing)
    {
        if (stack_size) {
            __TBB_ASSERT(arg != nullptr, nullptr);
            create_coroutine(my_coroutine, stack_size, arg);
        } else {
            current_coroutine(my_coroutine);
        }
    }

    ~co_context() {
        __TBB_ASSERT(1 << my_state & (1 << co_suspended | 1 << co_executing), nullptr);
        if (my_state == co_suspended) {
#if __TBB_RESUMABLE_TASKS_USE_THREADS
            my_state = co_executing;
#endif
            destroy_coroutine(my_coroutine);
        }
        my_state = co_destroyed;
    }

    void resume(co_context& target) {
        // Do not create non-trivial objects on the stack of this function. They might never be destroyed.
        __TBB_ASSERT(my_state == co_executing, nullptr);
        __TBB_ASSERT(target.my_state == co_suspended, nullptr);

        my_state = co_suspended;
        target.my_state = co_executing;

        // 'target' can reference an invalid object after swap_coroutine. Do not access it.
        swap_coroutine(my_coroutine, target.my_coroutine);

        __TBB_ASSERT(my_state == co_executing, nullptr);
    }
};

#if _WIN32 || _WIN64
/* [[noreturn]] */ void __stdcall co_local_wait_for_all(void* arg) noexcept;
#else
/* [[noreturn]] */ void co_local_wait_for_all(unsigned hi, unsigned lo) noexcept;
#endif

#if __TBB_RESUMABLE_TASKS_USE_THREADS
void handle_perror(int error_code, const char* what);

inline void check(int error_code, const char* routine) {
    if (error_code) {
        handle_perror(error_code, routine);
    }
}

using thread_data_t = std::pair<coroutine_type&, void*&>;

#if _WIN32 || _WIN64
inline unsigned WINAPI coroutine_thread_func(void* d)
#else
inline void* coroutine_thread_func(void* d)
#endif
{
    thread_data_t& data = *static_cast<thread_data_t*>(d);
    coroutine_type& c = data.first;
    void* arg = data.second;
    {
        std::unique_lock<std::mutex> lock(c.my_mutex);
        __TBB_ASSERT(c.my_thread_data == nullptr, nullptr);
        c.my_is_active = false;

        // We read the data notify the waiting thread
        data.second = nullptr;
        c.my_condvar.notify_one();

        c.my_condvar.wait(lock, [&c] { return c.my_is_active == true; });
    }
    __TBB_ASSERT(c.my_thread_data != nullptr, nullptr);
    governor::set_thread_data(*c.my_thread_data);

#if _WIN32 || _WIN64
    co_local_wait_for_all(arg);

    return 0;
#else
    std::uintptr_t addr = std::uintptr_t(arg);
    unsigned lo = unsigned(addr);
    unsigned hi = unsigned(std::uint64_t(addr) >> 32);
    __TBB_ASSERT(sizeof(addr) == 8 || hi == 0, nullptr);

    co_local_wait_for_all(hi, lo);

    return nullptr;
#endif
};

inline void create_coroutine(coroutine_type& c, std::size_t stack_size, void* arg) {
    thread_data_t data{ c, arg };

#if _WIN32 || _WIN64
    c.my_thread = (HANDLE)_beginthreadex(nullptr, unsigned(stack_size), coroutine_thread_func, &data, STACK_SIZE_PARAM_IS_A_RESERVATION, nullptr);
    if (!c.my_thread) {
        handle_perror(0, "create_coroutine: _beginthreadex failed\n");
    }
#else
    pthread_attr_t s;
    check(pthread_attr_init(&s), "pthread_attr_init has failed");
    if (stack_size > 0) {
        check(pthread_attr_setstacksize(&s, stack_size), "pthread_attr_setstack_size has failed");
    }
    check(pthread_create(&c.my_thread, &s, coroutine_thread_func, &data), "pthread_create has failed");
    check(pthread_attr_destroy(&s), "pthread_attr_destroy has failed");
#endif

    // Wait for the just created thread to read the data
    std::unique_lock<std::mutex> lock(c.my_mutex);
    c.my_condvar.wait(lock, [&arg] { return arg == nullptr; });
}

inline void current_coroutine(coroutine_type& c) {
#if _WIN32 || _WIN64
    c.my_thread = GetCurrentThread();
#else
    c.my_thread = pthread_self();
#endif
}

inline void swap_coroutine(coroutine_type& prev_coroutine, coroutine_type& new_coroutine) {
    thread_data* td = governor::get_thread_data();
    __TBB_ASSERT(prev_coroutine.my_is_active == true, "The current thread should be active");

    // Detach our state before notification other thread
    // (because we might be notified just after other thread notification)
    prev_coroutine.my_thread_data = nullptr;
    prev_coroutine.my_is_active = false;
    governor::clear_thread_data();

    {
        std::unique_lock<std::mutex> lock(new_coroutine.my_mutex);
        __TBB_ASSERT(new_coroutine.my_is_active == false, "The sleeping thread should not be active");
        __TBB_ASSERT(new_coroutine.my_thread_data == nullptr, "The sleeping thread should not be active");

        new_coroutine.my_thread_data = td;
        new_coroutine.my_is_active = true;
        new_coroutine.my_condvar.notify_one();
    }

    std::unique_lock<std::mutex> lock(prev_coroutine.my_mutex);
    prev_coroutine.my_condvar.wait(lock, [&prev_coroutine] { return prev_coroutine.my_is_active == true; });
    __TBB_ASSERT(governor::get_thread_data() != nullptr, nullptr);
    governor::set_thread_data(*prev_coroutine.my_thread_data);
}

inline void destroy_coroutine(coroutine_type& c) {
    {
        std::unique_lock<std::mutex> lock(c.my_mutex);
        __TBB_ASSERT(c.my_thread_data == nullptr, "The sleeping thread should not be active");
        __TBB_ASSERT(c.my_is_active == false, "The sleeping thread should not be active");
        c.my_is_active = true;
        c.my_condvar.notify_one();
    }
#if _WIN32 || _WIN64
    WaitForSingleObject(c.my_thread, INFINITE);
    CloseHandle(c.my_thread);
#else
    check(pthread_join(c.my_thread, nullptr), "pthread_join has failed");
#endif
}
#elif _WIN32 || _WIN64
inline void create_coroutine(coroutine_type& c, std::size_t stack_size, void* arg) {
    __TBB_ASSERT(arg, nullptr);
    c = CreateFiber(stack_size, co_local_wait_for_all, arg);
    __TBB_ASSERT(c, nullptr);
}

inline void current_coroutine(coroutine_type& c) {
    c = IsThreadAFiber() ? GetCurrentFiber() :
        ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
    __TBB_ASSERT(c, nullptr);
}

inline void swap_coroutine(coroutine_type& prev_coroutine, coroutine_type& new_coroutine) {
    if (!IsThreadAFiber()) {
        ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
    }
    __TBB_ASSERT(new_coroutine, nullptr);
    prev_coroutine = GetCurrentFiber();
    __TBB_ASSERT(prev_coroutine, nullptr);
    SwitchToFiber(new_coroutine);
}

inline void destroy_coroutine(coroutine_type& c) {
    __TBB_ASSERT(c, nullptr);
    DeleteFiber(c);
}
#else // !(_WIN32 || _WIN64)

inline void create_coroutine(coroutine_type& c, std::size_t stack_size, void* arg) {
    const std::size_t REG_PAGE_SIZE = governor::default_page_size();
    const std::size_t page_aligned_stack_size = (stack_size + (REG_PAGE_SIZE - 1)) & ~(REG_PAGE_SIZE - 1);
    const std::size_t protected_stack_size = page_aligned_stack_size + 2 * REG_PAGE_SIZE;

    // Allocate the stack with protection property
    std::uintptr_t stack_ptr = (std::uintptr_t)mmap(nullptr, protected_stack_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    __TBB_ASSERT((void*)stack_ptr != MAP_FAILED, nullptr);

    // Allow read write on our stack (guarded pages are still protected)
    int err = mprotect((void*)(stack_ptr + REG_PAGE_SIZE), page_aligned_stack_size, PROT_READ | PROT_WRITE);
    __TBB_ASSERT_EX(!err, nullptr);

    // Remember the stack state
    c.my_stack = (void*)(stack_ptr + REG_PAGE_SIZE);
    c.my_stack_size = page_aligned_stack_size;

    err = getcontext(&c.my_context);
    __TBB_ASSERT_EX(!err, nullptr);

    c.my_context.uc_link = nullptr;
    // cast to char* to disable FreeBSD clang-3.4.1 'incompatible type' error
    c.my_context.uc_stack.ss_sp = (char*)c.my_stack;
    c.my_context.uc_stack.ss_size = c.my_stack_size;
    c.my_context.uc_stack.ss_flags = 0;

    typedef void(*coroutine_func_t)();

    std::uintptr_t addr = std::uintptr_t(arg);
    unsigned lo = unsigned(addr);
    unsigned hi = unsigned(std::uint64_t(addr) >> 32);
    __TBB_ASSERT(sizeof(addr) == 8 || hi == 0, nullptr);

    makecontext(&c.my_context, (coroutine_func_t)co_local_wait_for_all, 2, hi, lo);
}

inline void current_coroutine(coroutine_type& c) {
    int err = getcontext(&c.my_context);
    __TBB_ASSERT_EX(!err, nullptr);
}

inline void swap_coroutine(coroutine_type& prev_coroutine, coroutine_type& new_coroutine) {
    int err = swapcontext(&prev_coroutine.my_context, &new_coroutine.my_context);
    __TBB_ASSERT_EX(!err, nullptr);
}

inline void destroy_coroutine(coroutine_type& c) {
    const std::size_t REG_PAGE_SIZE = governor::default_page_size();
    // Free stack memory with guarded pages
    munmap((void*)((std::uintptr_t)c.my_stack - REG_PAGE_SIZE), c.my_stack_size + 2 * REG_PAGE_SIZE);
    // Clear the stack state afterwards
    c.my_stack = nullptr;
    c.my_stack_size = 0;
}

#if __APPLE__
    #if __INTEL_COMPILER
        #pragma warning(pop) // 1478 warning
    #elif __clang__
        #pragma clang diagnostic pop // "-Wdeprecated-declarations"
    #endif
#endif

#endif // _WIN32 || _WIN64

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* __TBB_RESUMABLE_TASKS */

#endif /* _TBB_co_context_H */
