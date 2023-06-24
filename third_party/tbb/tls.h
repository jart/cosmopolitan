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

#ifndef _TBB_tls_H
#define _TBB_tls_H

#include "third_party/tbb/detail/_config.h"

#if __TBB_USE_POSIX
#include "libc/calls/weirdtypes.h"
#include "libc/sysv/consts/clock.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#else /* assume __TBB_USE_WINAPI */
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
#endif

namespace tbb {
namespace detail {
namespace r1 {

typedef void (*tls_dtor_t)(void*);

//! Basic cross-platform wrapper class for TLS operations.
template <typename T>
class basic_tls {
#if __TBB_USE_POSIX
    typedef pthread_key_t tls_key_t;
public:
    int  create( tls_dtor_t dtor = nullptr ) {
        return pthread_key_create(&my_key, dtor);
    }
    int  destroy()      { return pthread_key_delete(my_key); }
    void set( T value ) { pthread_setspecific(my_key, (void*)value); }
    T    get()          { return (T)pthread_getspecific(my_key); }
#else /* __TBB_USE_WINAPI */
    typedef DWORD tls_key_t;
public:
#if !__TBB_WIN8UI_SUPPORT
    int create() {
        tls_key_t tmp = TlsAlloc();
        if( tmp==TLS_OUT_OF_INDEXES )
            return TLS_OUT_OF_INDEXES;
        my_key = tmp;
        return 0;
    }
    int  destroy()      { TlsFree(my_key); my_key=0; return 0; }
    void set( T value ) { TlsSetValue(my_key, (LPVOID)value); }
    T    get()          { return (T)TlsGetValue(my_key); }
#else /*!__TBB_WIN8UI_SUPPORT*/
    int create() {
        tls_key_t tmp = FlsAlloc(nullptr);
        if( tmp== (DWORD)0xFFFFFFFF )
            return (DWORD)0xFFFFFFFF;
        my_key = tmp;
        return 0;
    }
    int  destroy()      { FlsFree(my_key); my_key=0; return 0; }
    void set( T value ) { FlsSetValue(my_key, (LPVOID)value); }
    T    get()          { return (T)FlsGetValue(my_key); }
#endif /* !__TBB_WIN8UI_SUPPORT */
#endif /* __TBB_USE_WINAPI */
private:
    tls_key_t my_key;
};

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* _TBB_tls_H */
