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

#ifndef _TBB_tbb_misc_H
#define _TBB_tbb_misc_H

#include "third_party/tbb/detail/_config.h"
#include "third_party/tbb/detail/_assert.h"
#include "third_party/tbb/detail/_utils.h"

#if __TBB_ARENA_BINDING
#include "third_party/tbb/info.h"
#endif /*__TBB_ARENA_BINDING*/

#if __unix__
#include "libc/intrin/newbie.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/sysparam.h"
#include "libc/calls/weirdtypes.h"
#include "libc/limits.h"
#include "libc/sysv/consts/endian.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/rusage.h"  // __FreeBSD_version
#if __FreeBSD_version >= 701000
// MISSING #include <sys/cpuset.h>
#endif
#endif

#include "third_party/libcxx/atomic"

// Does the operating system have a system call to pin a thread to a set of OS processors?
#define __TBB_OS_AFFINITY_SYSCALL_PRESENT ((__linux__ && !__ANDROID__) || (__FreeBSD_version >= 701000))
// On IBM* Blue Gene* CNK nodes, the affinity API has restrictions that prevent its usability for TBB,
// and also sysconf(_SC_NPROCESSORS_ONLN) already takes process affinity into account.
#define __TBB_USE_OS_AFFINITY_SYSCALL (__TBB_OS_AFFINITY_SYSCALL_PRESENT && !__bg__)

namespace tbb {
namespace detail {
namespace r1 {

void runtime_warning(const char* format, ... );

#if __TBB_ARENA_BINDING
class task_arena;
class task_scheduler_observer;
#endif /*__TBB_ARENA_BINDING*/

const std::size_t MByte = 1024*1024;

#if __TBB_USE_WINAPI
// The Microsoft Documentation about Thread Stack Size states that
// "The default stack reservation size used by the linker is 1 MB"
const std::size_t ThreadStackSize = 1*MByte;
#else
const std::size_t ThreadStackSize = (sizeof(uintptr_t) <= 4 ? 2 : 4 )*MByte;
#endif

#ifndef __TBB_HardwareConcurrency

//! Returns maximal parallelism level supported by the current OS configuration.
int AvailableHwConcurrency();

#else

inline int AvailableHwConcurrency() {
    int n = __TBB_HardwareConcurrency();
    return n > 0 ? n : 1; // Fail safety strap
}
#endif /* __TBB_HardwareConcurrency */

//! Returns OS regular memory page size
size_t DefaultSystemPageSize();

//! Returns number of processor groups in the current OS configuration.
/** AvailableHwConcurrency must be called at least once before calling this method. **/
int NumberOfProcessorGroups();

#if _WIN32||_WIN64

//! Retrieves index of processor group containing processor with the given index
int FindProcessorGroupIndex ( int processorIndex );

//! Affinitizes the thread to the specified processor group
void MoveThreadIntoProcessorGroup( void* hThread, int groupIndex );

#endif /* _WIN32||_WIN64 */

//! Prints TBB version information on stderr
void PrintVersion();

//! Prints arbitrary extra TBB version information on stderr
void PrintExtraVersionInfo( const char* category, const char* format, ... );

//! A callback routine to print RML version information on stderr
void PrintRMLVersionInfo( void* arg, const char* server_info );

// For TBB compilation only; not to be used in public headers
#if defined(min) || defined(max)
#undef min
#undef max
#endif

//! Utility template function returning lesser of the two values.
/** Provided here to avoid including not strict safe <algorithm>.\n
    In case operands cause signed/unsigned or size mismatch warnings it is caller's
    responsibility to do the appropriate cast before calling the function. **/
template<typename T>
T min ( const T& val1, const T& val2 ) {
    return val1 < val2 ? val1 : val2;
}

//! Utility template function returning greater of the two values.
/** Provided here to avoid including not strict safe <algorithm>.\n
    In case operands cause signed/unsigned or size mismatch warnings it is caller's
    responsibility to do the appropriate cast before calling the function. **/
template<typename T>
T max ( const T& val1, const T& val2 ) {
    return val1 < val2 ? val2 : val1;
}

//! Utility helper structure to ease overload resolution
template<int > struct int_to_type {};

//------------------------------------------------------------------------
// FastRandom
//------------------------------------------------------------------------

//! A fast random number generator.
/** Uses linear congruential method. */
class FastRandom {
private:
    unsigned x, c;
    static const unsigned a = 0x9e3779b1; // a big prime number
public:
    //! Get a random number.
    unsigned short get() {
        return get(x);
    }
    //! Get a random number for the given seed; update the seed for next use.
    unsigned short get( unsigned& seed ) {
        unsigned short r = (unsigned short)(seed>>16);
        __TBB_ASSERT(c&1, "c must be odd for big rng period");
        seed = seed*a+c;
        return r;
    }
    //! Construct a random number generator.
    FastRandom( void* unique_ptr ) { init(uintptr_t(unique_ptr)); }

    template <typename T>
    void init( T seed ) {
        init(seed,int_to_type<sizeof(seed)>());
    }
    void init( uint64_t seed , int_to_type<8> ) {
        init(uint32_t((seed>>32)+seed), int_to_type<4>());
    }
    void init( uint32_t seed, int_to_type<4> ) {
        // threads use different seeds for unique sequences
        c = (seed|1)*0xba5703f5; // c must be odd, shuffle by a prime number
        x = c^(seed>>1); // also shuffle x for the first get() invocation
    }
};

//------------------------------------------------------------------------
// Atomic extensions
//------------------------------------------------------------------------

//! Atomically replaces value of dst with newValue if they satisfy condition of compare predicate
/** Return value semantics is the same as for CAS. **/
template<typename T1, class Pred>
T1 atomic_update(std::atomic<T1>& dst, T1 newValue, Pred compare) {
    T1 oldValue = dst.load(std::memory_order_acquire);
    while ( compare(oldValue, newValue) ) {
        if ( dst.compare_exchange_strong(oldValue, newValue) )
            break;
    }
    return oldValue;
}

#if __TBB_USE_OS_AFFINITY_SYSCALL
  #if __linux__
    typedef cpu_set_t basic_mask_t;
  #elif __FreeBSD_version >= 701000
    typedef cpuset_t basic_mask_t;
  #else
    #error affinity_helper is not implemented in this OS
  #endif
    class affinity_helper : no_copy {
        basic_mask_t* threadMask;
        int is_changed;
    public:
        affinity_helper() : threadMask(nullptr), is_changed(0) {}
        ~affinity_helper();
        void protect_affinity_mask( bool restore_process_mask  );
        void dismiss();
    };
    void destroy_process_mask();
#else
    class affinity_helper : no_copy {
    public:
        void protect_affinity_mask( bool ) {}
    };
    inline void destroy_process_mask(){}
#endif /* __TBB_USE_OS_AFFINITY_SYSCALL */

struct cpu_features_type {
    bool rtm_enabled{false};
    bool waitpkg_enabled{false};
};

void detect_cpu_features(cpu_features_type& cpu_features);

#if __TBB_ARENA_BINDING
class binding_handler;

binding_handler* construct_binding_handler(int slot_num, int numa_id, int core_type_id, int max_threads_per_core);
void destroy_binding_handler(binding_handler* handler_ptr);
void apply_affinity_mask(binding_handler* handler_ptr, int slot_num);
void restore_affinity_mask(binding_handler* handler_ptr, int slot_num);

#endif /*__TBB_ARENA_BINDING*/

// RTM specific section
// abort code for mutexes that detect a conflict with another thread.
enum {
    speculation_not_supported       = 0x00,
    speculation_transaction_aborted = 0x01,
    speculation_can_retry           = 0x02,
    speculation_memadd_conflict     = 0x04,
    speculation_buffer_overflow     = 0x08,
    speculation_breakpoint_hit      = 0x10,
    speculation_nested_abort        = 0x20,
    speculation_xabort_mask         = 0xFF000000,
    speculation_xabort_shift        = 24,
    speculation_xabort_not_free     = 0xFF, // The value (0xFF) below comes from the Intel(R) 64 and IA-32 Architectures Optimization Reference Manual 12.4.5 lock not free
    speculation_successful_begin    = 0xFFFFFFFF,
    speculation_retry               = speculation_transaction_aborted
                                      | speculation_can_retry
                                      | speculation_memadd_conflict
};

// We suppose that successful transactions are sequentially ordered and
// do not require additional memory fences around them.
// Technically it can be achieved only if xbegin has implicit
// acquire memory semantics an xend/xabort has release memory semantics on compiler and hardware level.
// See the article: https://arxiv.org/pdf/1710.04839.pdf
static inline unsigned int begin_transaction() {
#if __TBB_TSX_INTRINSICS_PRESENT
    return _xbegin();
#else
    return speculation_not_supported; // return unsuccessful code
#endif
}

static inline void end_transaction() {
#if __TBB_TSX_INTRINSICS_PRESENT
    _xend();
#endif
}

static inline void abort_transaction() {
#if __TBB_TSX_INTRINSICS_PRESENT
    _xabort(speculation_xabort_not_free);
#endif
}

#if TBB_USE_ASSERT
static inline unsigned char is_in_transaction() {
#if __TBB_TSX_INTRINSICS_PRESENT
    return _xtest();
#else
    return 0;
#endif
}
#endif // TBB_USE_ASSERT

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* _TBB_tbb_misc_H */
