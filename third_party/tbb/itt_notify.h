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

#ifndef _TBB_ITT_NOTIFY
#define _TBB_ITT_NOTIFY

#include "third_party/tbb/detail/_config.h"

#if __TBB_USE_ITT_NOTIFY

#if _WIN32||_WIN64
    #ifndef UNICODE
        #define UNICODE
    #endif
#endif /* WIN */

#ifndef INTEL_ITTNOTIFY_API_PRIVATE
#define INTEL_ITTNOTIFY_API_PRIVATE
#endif

// MISSING #include "tools_api/ittnotify.h"
// MISSING #include "tools_api/legacy/ittnotify.h"
extern "C" void __itt_fini_ittlib(void);
extern "C" void __itt_release_resources(void);

#if _WIN32||_WIN64
    #undef _T
#endif /* WIN */

#endif /* __TBB_USE_ITT_NOTIFY */

#if !ITT_CALLER_NULL
#define ITT_CALLER_NULL ((__itt_caller)0)
#endif

namespace tbb {
namespace detail {
namespace r1 {

//! Unicode support
#if (_WIN32||_WIN64)
    //! Unicode character type. Always wchar_t on Windows.
    /** We do not use typedefs from Windows TCHAR family to keep consistence of TBB coding style. **/
    using tchar = wchar_t;
    //! Standard Windows macro to markup the string literals.
    #define _T(string_literal) L ## string_literal
#else /* !WIN */
    using tchar = char;
    //! Standard Windows style macro to markup the string literals.
    #define _T(string_literal) string_literal
#endif /* !WIN */

//! Display names of internal synchronization types
extern const tchar
    *SyncType_Scheduler;
//! Display names of internal synchronization components/scenarios
extern const tchar
    *SyncObj_ContextsList
    ;

#if __TBB_USE_ITT_NOTIFY
// const_cast<void*>() is necessary to cast off volatility
#define ITT_NOTIFY(name,obj)            __itt_##name(const_cast<void*>(static_cast<volatile void*>(obj)))
#define ITT_THREAD_SET_NAME(name)       __itt_thread_set_name(name)
#define ITT_FINI_ITTLIB()               __itt_fini_ittlib()
#define ITT_RELEASE_RESOURCES()         __itt_release_resources()
#define ITT_SYNC_CREATE(obj, type, name) __itt_sync_create((void*)(obj), type, name, 2)
#define ITT_STACK_CREATE(obj)           obj = __itt_stack_caller_create()
#define ITT_STACK_DESTROY(obj)          (obj!=nullptr) ? __itt_stack_caller_destroy(static_cast<__itt_caller>(obj)) : ((void)0)
#define ITT_CALLEE_ENTER(cond, t, obj)  if(cond) {\
                                            __itt_stack_callee_enter(static_cast<__itt_caller>(obj));\
                                            __itt_sync_acquired(t);\
                                        }
#define ITT_CALLEE_LEAVE(cond, obj)     (cond) ? __itt_stack_callee_leave(static_cast<__itt_caller>(obj)) : ((void)0)

#define ITT_TASK_GROUP(obj,name,parent)     r1::itt_make_task_group(d1::ITT_DOMAIN_MAIN,(void*)(obj),ALGORITHM,(void*)(parent),(parent!=nullptr) ? ALGORITHM : FLOW_NULL,name)
#define ITT_TASK_BEGIN(obj,name,id)         r1::itt_task_begin(d1::ITT_DOMAIN_MAIN,(void*)(id),ALGORITHM,(void*)(obj),ALGORITHM,name)
#define ITT_TASK_END                        r1::itt_task_end(d1::ITT_DOMAIN_MAIN)


#else /* !__TBB_USE_ITT_NOTIFY */

#define ITT_NOTIFY(name,obj)            ((void)0)
#define ITT_THREAD_SET_NAME(name)       ((void)0)
#define ITT_FINI_ITTLIB()               ((void)0)
#define ITT_RELEASE_RESOURCES()         ((void)0)
#define ITT_SYNC_CREATE(obj, type, name) ((void)0)
#define ITT_STACK_CREATE(obj)           ((void)0)
#define ITT_STACK_DESTROY(obj)          ((void)0)
#define ITT_CALLEE_ENTER(cond, t, obj)  ((void)0)
#define ITT_CALLEE_LEAVE(cond, obj)     ((void)0)
#define ITT_TASK_GROUP(type,name,parent)    ((void)0)
#define ITT_TASK_BEGIN(type,name,id)        ((void)0)
#define ITT_TASK_END                        ((void)0)

#endif /* !__TBB_USE_ITT_NOTIFY */

int __TBB_load_ittnotify();

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* _TBB_ITT_NOTIFY */
