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

#include "third_party/tbb/detail/_config.h"
#include "third_party/tbb/detail/_template_helpers.h"

#include "third_party/tbb/main.h"
#include "third_party/tbb/itt_notify.h"

#include "third_party/tbb/profiling.h"

#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

namespace tbb {
namespace detail {
namespace r1 {

#if __TBB_USE_ITT_NOTIFY
bool ITT_Present;
static std::atomic<bool> ITT_InitializationDone;

static __itt_domain *tbb_domains[d1::ITT_NUM_DOMAINS] = {};

struct resource_string {
    const char *str;
    __itt_string_handle *itt_str_handle;
};

//
// populate resource strings
//
#define TBB_STRING_RESOURCE( index_name, str ) { str, nullptr },
static resource_string strings_for_itt[] = {
    #include "third_party/tbb/detail/_string_resource.h"
    { "num_resource_strings", nullptr }
};
#undef TBB_STRING_RESOURCE

static __itt_string_handle* ITT_get_string_handle(std::uintptr_t idx) {
    __TBB_ASSERT(idx < NUM_STRINGS, "string handle out of valid range");
    return idx < NUM_STRINGS ? strings_for_itt[idx].itt_str_handle : nullptr;
}

static void ITT_init_domains() {
    tbb_domains[d1::ITT_DOMAIN_MAIN] = __itt_domain_create( _T("tbb") );
    tbb_domains[d1::ITT_DOMAIN_MAIN]->flags = 1;
    tbb_domains[d1::ITT_DOMAIN_FLOW] = __itt_domain_create( _T("tbb.flow") );
    tbb_domains[d1::ITT_DOMAIN_FLOW]->flags = 1;
    tbb_domains[d1::ITT_DOMAIN_ALGO] = __itt_domain_create( _T("tbb.algorithm") );
    tbb_domains[d1::ITT_DOMAIN_ALGO]->flags = 1;
}

static void ITT_init_strings() {
    for ( std::uintptr_t i = 0; i < NUM_STRINGS; ++i ) {
#if _WIN32||_WIN64
        strings_for_itt[i].itt_str_handle = __itt_string_handle_createA( strings_for_itt[i].str );
#else
        strings_for_itt[i].itt_str_handle = __itt_string_handle_create( strings_for_itt[i].str );
#endif
    }
}

static void ITT_init() {
    ITT_init_domains();
    ITT_init_strings();
}

/** Thread-unsafe lazy one-time initialization of tools interop.
    Used by both dummy handlers and general TBB one-time initialization routine. **/
void ITT_DoUnsafeOneTimeInitialization () {
    // Double check ITT_InitializationDone is necessary because the first check
    // in ITT_DoOneTimeInitialization is not guarded with the __TBB_InitOnce lock.
    if ( !ITT_InitializationDone ) {
        ITT_Present = (__TBB_load_ittnotify()!=0);
        if (ITT_Present) ITT_init();
        ITT_InitializationDone = true;
    }
}

/** Thread-safe lazy one-time initialization of tools interop.
    Used by dummy handlers only. **/
extern "C"
void ITT_DoOneTimeInitialization() {
    if ( !ITT_InitializationDone ) {
        __TBB_InitOnce::lock();
        ITT_DoUnsafeOneTimeInitialization();
        __TBB_InitOnce::unlock();
    }
}

void create_itt_sync(void* ptr, const tchar* objtype, const tchar* objname) {
        ITT_SYNC_CREATE(ptr, objtype, objname);
}

void call_itt_notify(int t, void *ptr) {
    switch (t) {
    case 0: ITT_NOTIFY(sync_prepare, ptr); break;
    case 1: ITT_NOTIFY(sync_cancel, ptr); break;
    case 2: ITT_NOTIFY(sync_acquired, ptr); break;
    case 3: ITT_NOTIFY(sync_releasing, ptr); break;
    case 4: ITT_NOTIFY(sync_destroy, ptr); break;
    }
}

void itt_set_sync_name(void* obj, const tchar* name) {
    __itt_sync_rename(obj, name);
}

const __itt_id itt_null_id = { 0, 0, 0 };

static inline __itt_domain* get_itt_domain(d1::itt_domain_enum idx) {
    if (tbb_domains[idx] == nullptr) {
        ITT_DoOneTimeInitialization();
    }
    return tbb_domains[idx];
}

static inline void itt_id_make(__itt_id* id, void* addr, unsigned long long extra) {
    *id = __itt_id_make(addr, extra);
}

static inline void itt_id_create(const __itt_domain* domain, __itt_id id) {
    __itt_id_create(domain, id);
}

void itt_make_task_group(d1::itt_domain_enum domain, void* group, unsigned long long group_extra,
                         void* parent, unsigned long long parent_extra, string_resource_index name_index) {
    if (__itt_domain* d = get_itt_domain(domain)) {
        __itt_id group_id = itt_null_id;
        __itt_id parent_id = itt_null_id;
        itt_id_make(&group_id, group, group_extra);
        itt_id_create(d, group_id);
        if (parent) {
            itt_id_make(&parent_id, parent, parent_extra);
        }
        __itt_string_handle* n = ITT_get_string_handle(name_index);
        __itt_task_group(d, group_id, parent_id, n);
    }
}

void __TBB_EXPORTED_FUNC itt_metadata_str_add(d1::itt_domain_enum domain, void *addr, unsigned long long addr_extra,
                                              string_resource_index key, const char *value ) {
    if ( __itt_domain *d = get_itt_domain( domain ) ) {
        __itt_id id = itt_null_id;
        itt_id_make( &id, addr, addr_extra );
        __itt_string_handle *k = ITT_get_string_handle(key);
        size_t value_length = strlen( value );
#if _WIN32||_WIN64
        __itt_metadata_str_addA(d, id, k, value, value_length);
#else
        __itt_metadata_str_add(d, id, k, value, value_length);
#endif
    }
}

void __TBB_EXPORTED_FUNC itt_metadata_ptr_add(d1::itt_domain_enum domain, void *addr, unsigned long long addr_extra,
                                              string_resource_index key, void *value ) {
    if ( __itt_domain *d = get_itt_domain( domain ) ) {
        __itt_id id = itt_null_id;
        itt_id_make( &id, addr, addr_extra );
        __itt_string_handle *k = ITT_get_string_handle(key);
#if __TBB_x86_32
        __itt_metadata_add(d, id, k, __itt_metadata_u32, 1, value);
#else
        __itt_metadata_add(d, id, k, __itt_metadata_u64, 1, value);
#endif
    }
}

void __TBB_EXPORTED_FUNC itt_relation_add(d1::itt_domain_enum domain, void *addr0, unsigned long long addr0_extra,
                                          itt_relation relation, void *addr1, unsigned long long addr1_extra ) {
    if ( __itt_domain *d = get_itt_domain( domain ) ) {
        __itt_id id0 = itt_null_id;
        __itt_id id1 = itt_null_id;
        itt_id_make( &id0, addr0, addr0_extra );
        itt_id_make( &id1, addr1, addr1_extra );
         __itt_relation_add( d, id0, (__itt_relation)relation, id1 );
    }
}

void __TBB_EXPORTED_FUNC itt_task_begin(d1::itt_domain_enum domain, void* task, unsigned long long task_extra,
                    void* parent, unsigned long long parent_extra, string_resource_index name_index) {
    if (__itt_domain* d = get_itt_domain(domain)) {
        __itt_id task_id = itt_null_id;
        __itt_id parent_id = itt_null_id;
        if (task) {
            itt_id_make(&task_id, task, task_extra);
        }
        if (parent) {
            itt_id_make(&parent_id, parent, parent_extra);
        }
        __itt_string_handle* n = ITT_get_string_handle(name_index);
        __itt_task_begin(d, task_id, parent_id, n);
    }
}

void __TBB_EXPORTED_FUNC itt_task_end(d1::itt_domain_enum domain) {
    if (__itt_domain* d = get_itt_domain(domain)) {
        __itt_task_end(d);
    }
}

void __TBB_EXPORTED_FUNC itt_region_begin(d1::itt_domain_enum domain, void *region, unsigned long long region_extra,
                      void *parent, unsigned long long parent_extra, string_resource_index /* name_index */ ) {
    if ( __itt_domain *d = get_itt_domain( domain ) ) {
        __itt_id region_id = itt_null_id;
        __itt_id parent_id = itt_null_id;
        itt_id_make( &region_id, region, region_extra );
        if ( parent ) {
            itt_id_make( &parent_id, parent, parent_extra );
        }
         __itt_region_begin( d, region_id, parent_id, nullptr );
    }
}

void __TBB_EXPORTED_FUNC itt_region_end(d1::itt_domain_enum domain, void *region, unsigned long long region_extra ) {
    if ( __itt_domain *d = get_itt_domain( domain ) ) {
        __itt_id region_id = itt_null_id;
        itt_id_make( &region_id, region, region_extra );
         __itt_region_end( d, region_id );
    }
}

#else
void create_itt_sync(void* /*ptr*/, const tchar* /*objtype*/, const tchar* /*objname*/) {}
void call_itt_notify(int /*t*/, void* /*ptr*/) {}
void itt_set_sync_name(void* /*obj*/, const tchar* /*name*/) {}
void itt_make_task_group(d1::itt_domain_enum /*domain*/, void* /*group*/, unsigned long long /*group_extra*/,
                         void* /*parent*/, unsigned long long /*parent_extra*/, string_resource_index /*name_index*/) {}
void itt_metadata_str_add(d1::itt_domain_enum /*domain*/, void* /*addr*/, unsigned long long /*addr_extra*/,
                          string_resource_index /*key*/, const char* /*value*/ ) { }
void itt_metadata_ptr_add(d1::itt_domain_enum /*domain*/, void * /*addr*/, unsigned long long /*addr_extra*/,
                          string_resource_index /*key*/, void * /*value*/ ) {}
void itt_relation_add(d1::itt_domain_enum /*domain*/, void* /*addr0*/, unsigned long long /*addr0_extra*/,
                      itt_relation /*relation*/, void* /*addr1*/, unsigned long long /*addr1_extra*/ ) { }
void itt_task_begin(d1::itt_domain_enum /*domain*/, void* /*task*/, unsigned long long /*task_extra*/,
                        void* /*parent*/, unsigned long long /*parent_extra*/, string_resource_index /*name_index*/ ) { }
void itt_task_end(d1::itt_domain_enum /*domain*/ ) { }
void itt_region_begin(d1::itt_domain_enum /*domain*/, void* /*region*/, unsigned long long /*region_extra*/,
                          void* /*parent*/, unsigned long long /*parent_extra*/, string_resource_index /*name_index*/ ) { }
void itt_region_end(d1::itt_domain_enum /*domain*/, void* /*region*/, unsigned long long /*region_extra*/ ) { }
#endif /* __TBB_USE_ITT_NOTIFY */

const tchar
    *SyncType_Scheduler = _T("%Constant")
    ;
const tchar
    *SyncObj_ContextsList = _T("TBB Scheduler")
    ;
} // namespace r1
} // namespace detail
} // namespace tbb
