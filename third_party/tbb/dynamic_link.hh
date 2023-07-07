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

#ifndef __TBB_dynamic_link
#define __TBB_dynamic_link

// Support for dynamic loading entry points from other shared libraries.

#include "third_party/tbb/detail/_config.hh"

#include "third_party/libcxx/atomic"
#include "third_party/libcxx/mutex"

/** By default, symbols declared and defined here go into namespace tbb::internal.
    To put them in other namespace, define macros OPEN_INTERNAL_NAMESPACE
    and CLOSE_INTERNAL_NAMESPACE to override the following default definitions. **/

#include "third_party/libcxx/cstddef"
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
#endif /* _WIN32 */

namespace tbb {
namespace detail {
namespace r1 {

//! Type definition for a pointer to a void somefunc(void)
typedef void (*pointer_to_handler)();

//! The helper to construct dynamic_link_descriptor structure
// Double cast through the void* in DLD macro is necessary to
// prevent warnings from some compilers (g++ 4.1)
#if __TBB_WEAK_SYMBOLS_PRESENT
#define DLD(s,h) {#s, (pointer_to_handler*)(void*)(&h), (pointer_to_handler)&s}
#define DLD_NOWEAK(s,h) {#s, (pointer_to_handler*)(void*)(&h), nullptr}
#else
#define DLD(s,h) {#s, (pointer_to_handler*)(void*)(&h)}
#define DLD_NOWEAK(s,h) DLD(s,h)
#endif /* __TBB_WEAK_SYMBOLS_PRESENT */
//! Association between a handler name and location of pointer to it.
struct dynamic_link_descriptor {
    //! Name of the handler
    const char* name;
    //! Pointer to the handler
    pointer_to_handler* handler;
#if __TBB_WEAK_SYMBOLS_PRESENT
    //! Weak symbol
    pointer_to_handler ptr;
#endif
};

#if _WIN32
using dynamic_link_handle = HMODULE;
#else
using dynamic_link_handle = void*;
#endif /* _WIN32 */

const int DYNAMIC_LINK_GLOBAL        = 0x01;
const int DYNAMIC_LINK_LOAD          = 0x02;
const int DYNAMIC_LINK_WEAK          = 0x04;
const int DYNAMIC_LINK_LOCAL         = 0x08;

const int DYNAMIC_LINK_LOCAL_BINDING = DYNAMIC_LINK_LOCAL | DYNAMIC_LINK_LOAD;
const int DYNAMIC_LINK_DEFAULT       = DYNAMIC_LINK_GLOBAL | DYNAMIC_LINK_LOAD | DYNAMIC_LINK_WEAK;

//! Fill in dynamically linked handlers.
/** 'library' is the name of the requested library. It should not contain a full
    path since dynamic_link adds the full path (from which the runtime itself
    was loaded) to the library name.
    'required' is the number of the initial entries in the array descriptors[]
    that have to be found in order for the call to succeed. If the library and
    all the required handlers are found, then the corresponding handler
    pointers are set, and the return value is true.  Otherwise the original
    array of descriptors is left untouched and the return value is false.
    'required' is limited by 20 (exceeding of this value will result in failure
    to load the symbols and the return value will be false).
    'handle' is the handle of the library if it is loaded. Otherwise it is left
    untouched.
    'flags' is the set of DYNAMIC_LINK_* flags. Each of the DYNAMIC_LINK_* flags
    allows its corresponding linking stage.
**/
bool dynamic_link( const char* library,
                   const dynamic_link_descriptor descriptors[],
                   std::size_t required,
                   dynamic_link_handle* handle = nullptr,
                   int flags = DYNAMIC_LINK_DEFAULT );

void dynamic_unlink( dynamic_link_handle handle );

void dynamic_unlink_all();

enum dynamic_link_error_t {
    dl_success = 0,
    dl_lib_not_found,     // char const * lib, dlerr_t err
    dl_sym_not_found,     // char const * sym, dlerr_t err
                          // Note: dlerr_t depends on OS: it is char const * on Linux* and macOS*, int on Windows*.
    dl_sys_fail,          // char const * func, int err
    dl_buff_too_small     // none
}; // dynamic_link_error_t

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* __TBB_dynamic_link */
