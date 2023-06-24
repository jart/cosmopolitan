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

#include "third_party/tbb/version.h"

#include "third_party/tbb/detail/_exception.h"
#include "third_party/tbb/detail/_assert.h"
#include "third_party/tbb/detail/_utils.h"
#include "third_party/tbb/tbb_allocator.h" // Is this OK?
#include "third_party/tbb/cache_aligned_allocator.h"

#include "third_party/tbb/dynamic_link.h"
#include "third_party/tbb/misc.h"

#include "third_party/libcxx/cstdlib"

#ifdef _WIN32
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
#include "libc/runtime/dlfcn.h"
#endif

#if (!defined(_WIN32) && !defined(_WIN64)) || defined(__CYGWIN__)
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/mem/alg.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/rand48.h" // posix_memalign, free
// With glibc, uClibc and musl on Linux and bionic on Android it is safe to use memalign(), as the allocated memory
// can be freed with free(). It is also better to use memalign() since posix_memalign() is just a wrapper on top of
// memalign() and it offers nothing but overhead due to inconvenient interface. This is likely the case with other
// standard libraries as well, and more libraries can be added to the preprocessor check below. Unfortunately, we
// can't detect musl, so we simply enable memalign() on Linux and Android in general.
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__ANDROID__)
#include "libc/mem/mem.h" // memalign
#define __TBB_USE_MEMALIGN
#else
#define __TBB_USE_POSIX_MEMALIGN
#endif
#elif defined(_MSC_VER) || defined(__MINGW32__)
#include "libc/mem/mem.h" // _aligned_malloc, _aligned_free
#define __TBB_USE_MSVC_ALIGNED_MALLOC
#endif

#if __TBB_WEAK_SYMBOLS_PRESENT

#pragma weak scalable_malloc
#pragma weak scalable_free
#pragma weak scalable_aligned_malloc
#pragma weak scalable_aligned_free

extern "C" {
    void* scalable_malloc(std::size_t);
    void  scalable_free(void*);
    void* scalable_aligned_malloc(std::size_t, std::size_t);
    void  scalable_aligned_free(void*);
}

#endif /* __TBB_WEAK_SYMBOLS_PRESENT */

namespace tbb {
namespace detail {
namespace r1 {

//! Initialization routine used for first indirect call via allocate_handler.
static void* initialize_allocate_handler(std::size_t size);

//! Handler for memory allocation
using allocate_handler_type = void* (*)(std::size_t size);
static std::atomic<allocate_handler_type> allocate_handler{ &initialize_allocate_handler };
allocate_handler_type allocate_handler_unsafe = nullptr;

//! Handler for memory deallocation
static void  (*deallocate_handler)(void* pointer) = nullptr;

//! Initialization routine used for first indirect call via cache_aligned_allocate_handler.
static void* initialize_cache_aligned_allocate_handler(std::size_t n, std::size_t alignment);

//! Allocates overaligned memory using standard memory allocator. It is used when scalable_allocator is not available.
static void* std_cache_aligned_allocate(std::size_t n, std::size_t alignment);

//! Deallocates overaligned memory using standard memory allocator. It is used when scalable_allocator is not available.
static void  std_cache_aligned_deallocate(void* p);

//! Handler for padded memory allocation
using cache_aligned_allocate_handler_type = void* (*)(std::size_t n, std::size_t alignment);
static std::atomic<cache_aligned_allocate_handler_type> cache_aligned_allocate_handler{ &initialize_cache_aligned_allocate_handler };
cache_aligned_allocate_handler_type cache_aligned_allocate_handler_unsafe = nullptr;

//! Handler for padded memory deallocation
static void (*cache_aligned_deallocate_handler)(void* p) = nullptr;

//! Table describing how to link the handlers.
static const dynamic_link_descriptor MallocLinkTable[] = {
    DLD(scalable_malloc, allocate_handler_unsafe),
    DLD(scalable_free, deallocate_handler),
    DLD(scalable_aligned_malloc, cache_aligned_allocate_handler_unsafe),
    DLD(scalable_aligned_free, cache_aligned_deallocate_handler),
};


#if TBB_USE_DEBUG
#define DEBUG_SUFFIX "_debug"
#else
#define DEBUG_SUFFIX
#endif /* TBB_USE_DEBUG */

// MALLOCLIB_NAME is the name of the oneTBB memory allocator library.
#if _WIN32||_WIN64
#define MALLOCLIB_NAME "tbbmalloc" DEBUG_SUFFIX ".dll"
#elif __APPLE__
#define MALLOCLIB_NAME "libtbbmalloc" DEBUG_SUFFIX ".2.dylib"
#elif __FreeBSD__ || __NetBSD__ || __OpenBSD__ || __sun || _AIX || __ANDROID__
#define MALLOCLIB_NAME "libtbbmalloc" DEBUG_SUFFIX ".so"
#elif __unix__  // Note that order of these #elif's is important!
#define MALLOCLIB_NAME "libtbbmalloc" DEBUG_SUFFIX ".so.2"
#else
#error Unknown OS
#endif

//! Initialize the allocation/free handler pointers.
/** Caller is responsible for ensuring this routine is called exactly once.
    The routine attempts to dynamically link with the TBB memory allocator.
    If that allocator is not found, it links to malloc and free. */
void initialize_handler_pointers() {
    __TBB_ASSERT(allocate_handler == &initialize_allocate_handler, nullptr);
    bool success = dynamic_link(MALLOCLIB_NAME, MallocLinkTable, 4);
    if(!success) {
        // If unsuccessful, set the handlers to the default routines.
        // This must be done now, and not before FillDynamicLinks runs, because if other
        // threads call the handlers, we want them to go through the DoOneTimeInitializations logic,
        // which forces them to wait.
        allocate_handler_unsafe = &std::malloc;
        deallocate_handler = &std::free;
        cache_aligned_allocate_handler_unsafe = &std_cache_aligned_allocate;
        cache_aligned_deallocate_handler = &std_cache_aligned_deallocate;
    }

    allocate_handler.store(allocate_handler_unsafe, std::memory_order_release);
    cache_aligned_allocate_handler.store(cache_aligned_allocate_handler_unsafe, std::memory_order_release);

    PrintExtraVersionInfo( "ALLOCATOR", success?"scalable_malloc":"malloc" );
}

static std::once_flag initialization_state;
void initialize_cache_aligned_allocator() {
    std::call_once(initialization_state, &initialize_handler_pointers);
}

//! Executed on very first call through allocate_handler
static void* initialize_allocate_handler(std::size_t size) {
    initialize_cache_aligned_allocator();
    __TBB_ASSERT(allocate_handler != &initialize_allocate_handler, nullptr);
    return (*allocate_handler)(size);
}

//! Executed on very first call through cache_aligned_allocate_handler
static void* initialize_cache_aligned_allocate_handler(std::size_t bytes, std::size_t alignment) {
    initialize_cache_aligned_allocator();
    __TBB_ASSERT(cache_aligned_allocate_handler != &initialize_cache_aligned_allocate_handler, nullptr);
    return (*cache_aligned_allocate_handler)(bytes, alignment);
}

// TODO: use CPUID to find actual line size, though consider backward compatibility
// nfs - no false sharing
static constexpr std::size_t nfs_size = 128;

std::size_t __TBB_EXPORTED_FUNC cache_line_size() {
    return nfs_size;
}

void* __TBB_EXPORTED_FUNC cache_aligned_allocate(std::size_t size) {
    const std::size_t cache_line_size = nfs_size;
    __TBB_ASSERT(is_power_of_two(cache_line_size), "must be power of two");

    // Check for overflow
    if (size + cache_line_size < size) {
        throw_exception(exception_id::bad_alloc);
    }
    // scalable_aligned_malloc considers zero size request an error, and returns nullptr
    if (size == 0) size = 1;

    void* result = cache_aligned_allocate_handler.load(std::memory_order_acquire)(size, cache_line_size);
    if (!result) {
        throw_exception(exception_id::bad_alloc);
    }
    __TBB_ASSERT(is_aligned(result, cache_line_size), "The returned address isn't aligned");
    return result;
}

void __TBB_EXPORTED_FUNC cache_aligned_deallocate(void* p) {
    __TBB_ASSERT(cache_aligned_deallocate_handler, "Initialization has not been yet.");
    (*cache_aligned_deallocate_handler)(p);
}

static void* std_cache_aligned_allocate(std::size_t bytes, std::size_t alignment) {
#if defined(__TBB_USE_MEMALIGN)
    return memalign(alignment, bytes);
#elif defined(__TBB_USE_POSIX_MEMALIGN)
    void* p = nullptr;
    int res = posix_memalign(&p, alignment, bytes);
    if (res != 0)
        p = nullptr;
    return p;
#elif defined(__TBB_USE_MSVC_ALIGNED_MALLOC)
    return _aligned_malloc(bytes, alignment);
#else
    // TODO: make it common with cache_aligned_resource
    std::size_t space = alignment + bytes;
    std::uintptr_t base = reinterpret_cast<std::uintptr_t>(std::malloc(space));
    if (!base) {
        return nullptr;
    }
    std::uintptr_t result = (base + nfs_size) & ~(nfs_size - 1);
    // Round up to the next cache line (align the base address)
    __TBB_ASSERT((result - base) >= sizeof(std::uintptr_t), "Cannot store a base pointer to the header");
    __TBB_ASSERT(space - (result - base) >= bytes, "Not enough space for the storage");

    // Record where block actually starts.
    (reinterpret_cast<std::uintptr_t*>(result))[-1] = base;
    return reinterpret_cast<void*>(result);
#endif
}

static void std_cache_aligned_deallocate(void* p) {
#if defined(__TBB_USE_MEMALIGN) || defined(__TBB_USE_POSIX_MEMALIGN)
    free(p);
#elif defined(__TBB_USE_MSVC_ALIGNED_MALLOC)
    _aligned_free(p);
#else
    if (p) {
        __TBB_ASSERT(reinterpret_cast<std::uintptr_t>(p) >= 0x4096, "attempt to free block not obtained from cache_aligned_allocator");
        // Recover where block actually starts
        std::uintptr_t base = (reinterpret_cast<std::uintptr_t*>(p))[-1];
        __TBB_ASSERT(((base + nfs_size) & ~(nfs_size - 1)) == reinterpret_cast<std::uintptr_t>(p), "Incorrect alignment or not allocated by std_cache_aligned_deallocate?");
        std::free(reinterpret_cast<void*>(base));
    }
#endif
}

void* __TBB_EXPORTED_FUNC allocate_memory(std::size_t size) {
    void* result = allocate_handler.load(std::memory_order_acquire)(size);
    if (!result) {
        throw_exception(exception_id::bad_alloc);
    }
    return result;
}

void __TBB_EXPORTED_FUNC deallocate_memory(void* p) {
    if (p) {
        __TBB_ASSERT(deallocate_handler, "Initialization has not been yet.");
        (*deallocate_handler)(p);
    }
}

bool __TBB_EXPORTED_FUNC is_tbbmalloc_used() {
    auto handler_snapshot = allocate_handler.load(std::memory_order_acquire);
    if (handler_snapshot == &initialize_allocate_handler) {
        initialize_cache_aligned_allocator();
    }
    handler_snapshot = allocate_handler.load(std::memory_order_relaxed);
    __TBB_ASSERT(handler_snapshot != &initialize_allocate_handler && deallocate_handler != nullptr, nullptr);
    // Cast to void avoids type mismatch errors on some compilers (e.g. __IBMCPP__)
    __TBB_ASSERT((reinterpret_cast<void*>(handler_snapshot) == reinterpret_cast<void*>(&std::malloc)) == (reinterpret_cast<void*>(deallocate_handler) == reinterpret_cast<void*>(&std::free)),
                  "Both shim pointers must refer to routines from the same package (either TBB or CRT)");
    return reinterpret_cast<void*>(handler_snapshot) == reinterpret_cast<void*>(&std::malloc);
}

} // namespace r1
} // namespace detail
} // namespace tbb
