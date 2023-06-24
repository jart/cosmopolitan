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

#if __TBB_USE_ITT_NOTIFY

#if _WIN32||_WIN64
    #ifndef UNICODE
        #define UNICODE
    #endif
#else
    #pragma weak dlopen
    #pragma weak dlsym
    #pragma weak dlerror
#endif /* WIN */

#if __TBB_BUILD

extern "C" void ITT_DoOneTimeInitialization();
#define __itt_init_ittlib_name(x,y) (ITT_DoOneTimeInitialization(), true)

#elif __TBBMALLOC_BUILD

extern "C" void MallocInitializeITT();
#define __itt_init_ittlib_name(x,y) (MallocInitializeITT(), true)

#else
#error This file is expected to be used for either TBB or TBB allocator build.
#endif // __TBB_BUILD

// MISSING #include "tools_api/ittnotify_static.c"

namespace tbb {
namespace detail {
namespace r1 {

/** This extra proxy method is necessary since __itt_init_lib is declared as static **/
int __TBB_load_ittnotify() {
#if !(_WIN32||_WIN64)
    // tool_api crashes without dlopen, check that it's present. Common case
    // for lack of dlopen is static binaries, i.e. ones build with -static.
    if (dlopen == nullptr)
        return 0;
#endif
    return __itt_init_ittlib(nullptr,       // groups for:
      (__itt_group_id)(__itt_group_sync     // prepare/cancel/acquired/releasing
                       | __itt_group_thread // name threads
                       | __itt_group_stitch // stack stitching
                       | __itt_group_structure
                           ));
}

} //namespace r1
} //namespace detail
} // namespace tbb

#endif /* __TBB_USE_ITT_NOTIFY */
