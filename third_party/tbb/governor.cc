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

#include "third_party/tbb/governor.hh"
#include "third_party/tbb/threading_control.hh"
#include "third_party/tbb/main.hh"
#include "third_party/tbb/thread_data.hh"
#include "third_party/tbb/market.hh"
#include "third_party/tbb/arena.hh"
#include "third_party/tbb/dynamic_link.hh"
#include "third_party/tbb/concurrent_monitor.hh"
#include "third_party/tbb/thread_dispatcher.hh"

#include "third_party/tbb/task_group.hh"
#include "third_party/tbb/global_control.hh"
#include "third_party/tbb/tbb_allocator.hh"
#include "third_party/tbb/info.hh"

#include "third_party/tbb/task_dispatcher.hh"

#include "third_party/libcxx/cstdio"
#include "third_party/libcxx/cstdlib"
#include "third_party/libcxx/cstring"
#include "third_party/libcxx/atomic"
#include "third_party/libcxx/algorithm"

namespace tbb {
namespace detail {
namespace r1 {

void clear_address_waiter_table();

//! global_control.cpp contains definition
bool remove_and_check_if_empty(d1::global_control& gc);
bool is_present(d1::global_control& gc);

namespace rml {
tbb_server* make_private_server( tbb_client& client );
} // namespace rml

namespace system_topology {
    void destroy();
}

//------------------------------------------------------------------------
// governor
//------------------------------------------------------------------------

void governor::acquire_resources () {
#if __TBB_USE_POSIX
    int status = theTLS.create(auto_terminate);
#else
    int status = theTLS.create();
#endif
    if( status )
        handle_perror(status, "TBB failed to initialize task scheduler TLS\n");
    detect_cpu_features(cpu_features);

    is_rethrow_broken = gcc_rethrow_exception_broken();
}

void governor::release_resources () {
    theRMLServerFactory.close();
    destroy_process_mask();

    __TBB_ASSERT(!(__TBB_InitOnce::initialization_done() && theTLS.get()), "TBB is unloaded while thread data still alive?");

    int status = theTLS.destroy();
    if( status )
        runtime_warning("failed to destroy task scheduler TLS: %s", std::strerror(status));
    clear_address_waiter_table();

    system_topology::destroy();
    dynamic_unlink_all();
}

rml::tbb_server* governor::create_rml_server ( rml::tbb_client& client ) {
    rml::tbb_server* server = nullptr;
    if( !UsePrivateRML ) {
        ::rml::factory::status_type status = theRMLServerFactory.make_server( server, client );
        if( status != ::rml::factory::st_success ) {
            UsePrivateRML = true;
            runtime_warning( "rml::tbb_factory::make_server failed with status %x, falling back on private rml", status );
        }
    }
    if ( !server ) {
        __TBB_ASSERT( UsePrivateRML, nullptr);
        server = rml::make_private_server( client );
    }
    __TBB_ASSERT( server, "Failed to create RML server" );
    return server;
}

void governor::one_time_init() {
    if ( !__TBB_InitOnce::initialization_done() ) {
        DoOneTimeInitialization();
    }
}

bool governor::does_client_join_workers(const rml::tbb_client &client) {
    return ((const thread_dispatcher&)client).must_join_workers();
}

/*
    There is no portable way to get stack base address in Posix, however the modern
    Linux versions provide pthread_attr_np API that can be used  to obtain thread's
    stack size and base address. Unfortunately even this function does not provide
    enough information for the main thread on IA-64 architecture (RSE spill area
    and memory stack are allocated as two separate discontinuous chunks of memory),
    and there is no portable way to discern the main and the secondary threads.
    Thus for macOS* and IA-64 architecture for Linux* OS we use the TBB worker stack size for
    all threads and use the current stack top as the stack base. This simplified
    approach is based on the following assumptions:
    1) If the default stack size is insufficient for the user app needs, the
    required amount will be explicitly specified by the user at the point of the
    TBB scheduler initialization (as an argument to tbb::task_scheduler_init
    constructor).
    2) When an external thread initializes the scheduler, it has enough space on its
    stack. Here "enough" means "at least as much as worker threads have".
    3) If the user app strives to conserve the memory by cutting stack size, it
    should do this for TBB workers too (as in the #1).
*/
static std::uintptr_t get_stack_base(std::size_t stack_size) {
    // Stacks are growing top-down. Highest address is called "stack base",
    // and the lowest is "stack limit".
#if __TBB_USE_WINAPI
    suppress_unused_warning(stack_size);
    NT_TIB* pteb = (NT_TIB*)NtCurrentTeb();
    __TBB_ASSERT(&pteb < pteb->StackBase && &pteb > pteb->StackLimit, "invalid stack info in TEB");
    return reinterpret_cast<std::uintptr_t>(pteb->StackBase);
#else
    // There is no portable way to get stack base address in Posix, so we use
    // non-portable method (on all modern Linux) or the simplified approach
    // based on the common sense assumptions. The most important assumption
    // is that the main thread's stack size is not less than that of other threads.

    // Points to the lowest addressable byte of a stack.
    void* stack_limit = nullptr;
#if __linux__ && !__bg__
    size_t np_stack_size = 0;
    pthread_attr_t np_attr_stack;
    if (0 == pthread_getattr_np(pthread_self(), &np_attr_stack)) {
        if (0 == pthread_attr_getstack(&np_attr_stack, &stack_limit, &np_stack_size)) {
            __TBB_ASSERT( &stack_limit > stack_limit, "stack size must be positive" );
        }
        pthread_attr_destroy(&np_attr_stack);
    }
#endif /* __linux__ */
    std::uintptr_t stack_base{};
    if (stack_limit) {
        stack_base = reinterpret_cast<std::uintptr_t>(stack_limit) + stack_size;
    } else {
        // Use an anchor as a base stack address.
        int anchor{};
        stack_base = reinterpret_cast<std::uintptr_t>(&anchor);
    }
    return stack_base;
#endif /* __TBB_USE_WINAPI */
}

#if (_WIN32||_WIN64) && !__TBB_DYNAMIC_LOAD_ENABLED
static void register_external_thread_destructor() {
    struct thread_destructor {
        ~thread_destructor() {
            governor::terminate_external_thread();
        }
    };
    // ~thread_destructor() will be call during the calling thread termination
    static thread_local thread_destructor thr_destructor;
}
#endif // (_WIN32||_WIN64) && !__TBB_DYNAMIC_LOAD_ENABLED

void governor::init_external_thread() {
    one_time_init();
    // Create new scheduler instance with arena
    int num_slots = default_num_threads();
    // TODO_REVAMP: support an external thread without an implicit arena
    int num_reserved_slots = 1;
    unsigned arena_priority_level = 1; // corresponds to tbb::task_arena::priority::normal
    std::size_t stack_size = 0;
    threading_control* thr_control = threading_control::register_public_reference();
    arena& a = arena::create(thr_control, num_slots, num_reserved_slots, arena_priority_level);
    // External thread always occupies the first slot
    thread_data& td = *new(cache_aligned_allocate(sizeof(thread_data))) thread_data(0, false);
    td.attach_arena(a, /*slot index*/ 0);
    __TBB_ASSERT(td.my_inbox.is_idle_state(false), nullptr);

    stack_size = a.my_threading_control->worker_stack_size();
    std::uintptr_t stack_base = get_stack_base(stack_size);
    task_dispatcher& task_disp = td.my_arena_slot->default_task_dispatcher();
    td.enter_task_dispatcher(task_disp, calculate_stealing_threshold(stack_base, stack_size));

    td.my_arena_slot->occupy();
    thr_control->register_thread(td);
    set_thread_data(td);
#if (_WIN32||_WIN64) && !__TBB_DYNAMIC_LOAD_ENABLED
    // The external thread destructor is called from dllMain but it is not available with a static build.
    // Therefore, we need to register the current thread to call the destructor during thread termination.
    register_external_thread_destructor();
#endif
}

void governor::auto_terminate(void* tls) {
    __TBB_ASSERT(get_thread_data_if_initialized() == nullptr ||
        get_thread_data_if_initialized() == tls, nullptr);
    if (tls) {
        thread_data* td = static_cast<thread_data*>(tls);

        auto clear_tls = [td] {
            td->~thread_data();
            cache_aligned_deallocate(td);
            clear_thread_data();
        };

        // Only external thread can be inside an arena during termination.
        if (td->my_arena_slot) {
            arena* a = td->my_arena;
            threading_control* thr_control = a->my_threading_control;

            // If the TLS slot is already cleared by OS or underlying concurrency
            // runtime, restore its value to properly clean up arena
            if (!is_thread_data_set(td)) {
                set_thread_data(*td);
            }

            a->my_observers.notify_exit_observers(td->my_last_observer, td->my_is_worker);

            td->leave_task_dispatcher();
            td->my_arena_slot->release();
            // Release an arena
            a->on_thread_leaving(arena::ref_external);

            thr_control->unregister_thread(*td);

            // The tls should be cleared before market::release because
            // market can destroy the tls key if we keep the last reference
            clear_tls();

            // If there was an associated arena, it added a public market reference
            thr_control->unregister_public_reference(/* blocking terminate =*/ false);
        } else {
            clear_tls();
        }
    }
    __TBB_ASSERT(get_thread_data_if_initialized() == nullptr, nullptr);
}

void governor::initialize_rml_factory () {
    ::rml::factory::status_type res = theRMLServerFactory.open();
    UsePrivateRML = res != ::rml::factory::st_success;
}

void __TBB_EXPORTED_FUNC get(d1::task_scheduler_handle& handle) {
    handle.m_ctl = new(allocate_memory(sizeof(global_control))) global_control(global_control::scheduler_handle, 1);
}

void release_impl(d1::task_scheduler_handle& handle) {
    if (handle.m_ctl != nullptr) {
        handle.m_ctl->~global_control();
        deallocate_memory(handle.m_ctl);
        handle.m_ctl = nullptr;
    }
}

bool finalize_impl(d1::task_scheduler_handle& handle) {
    __TBB_ASSERT_RELEASE(handle, "trying to finalize with null handle");
    __TBB_ASSERT(is_present(*handle.m_ctl), "finalize or release was already called on this object");

    bool ok = true; // ok if threading_control does not exist yet
    if (threading_control::is_present()) {
        thread_data* td = governor::get_thread_data_if_initialized();
        if (td) {
            task_dispatcher* task_disp = td->my_task_dispatcher;
            __TBB_ASSERT(task_disp, nullptr);
            if (task_disp->m_properties.outermost && !td->my_is_worker) { // is not inside a parallel region
                governor::auto_terminate(td);
            }
        }

        if (remove_and_check_if_empty(*handle.m_ctl)) {
            ok = threading_control::unregister_lifetime_control(/*blocking_terminate*/ true);
        } else {
            ok = false;
        }
    }

    return ok;
}

bool __TBB_EXPORTED_FUNC finalize(d1::task_scheduler_handle& handle, std::intptr_t mode) {
    if (mode == d1::release_nothrowing) {
        release_impl(handle);
        return true;
    } else {
        bool ok = finalize_impl(handle);
        // TODO: it is unsafe when finalize is called concurrently and further library unload
        release_impl(handle);
        if (mode == d1::finalize_throwing && !ok) {
            throw_exception(exception_id::unsafe_wait);
        }
        return ok;
    }
}

#if __TBB_ARENA_BINDING

#if __TBB_WEAK_SYMBOLS_PRESENT
#pragma weak __TBB_internal_initialize_system_topology
#pragma weak __TBB_internal_destroy_system_topology
#pragma weak __TBB_internal_allocate_binding_handler
#pragma weak __TBB_internal_deallocate_binding_handler
#pragma weak __TBB_internal_apply_affinity
#pragma weak __TBB_internal_restore_affinity
#pragma weak __TBB_internal_get_default_concurrency

extern "C" {
void __TBB_internal_initialize_system_topology(
    size_t groups_num,
    int& numa_nodes_count, int*& numa_indexes_list,
    int& core_types_count, int*& core_types_indexes_list
);
void __TBB_internal_destroy_system_topology( );

//TODO: consider renaming to `create_binding_handler` and `destroy_binding_handler`
binding_handler* __TBB_internal_allocate_binding_handler( int slot_num, int numa_id, int core_type_id, int max_threads_per_core );
void __TBB_internal_deallocate_binding_handler( binding_handler* handler_ptr );

void __TBB_internal_apply_affinity( binding_handler* handler_ptr, int slot_num );
void __TBB_internal_restore_affinity( binding_handler* handler_ptr, int slot_num );

int __TBB_internal_get_default_concurrency( int numa_id, int core_type_id, int max_threads_per_core );
}
#endif /* __TBB_WEAK_SYMBOLS_PRESENT */

// Stubs that will be used if TBBbind library is unavailable.
static void dummy_destroy_system_topology ( ) { }
static binding_handler* dummy_allocate_binding_handler ( int, int, int, int ) { return nullptr; }
static void dummy_deallocate_binding_handler ( binding_handler* ) { }
static void dummy_apply_affinity ( binding_handler*, int ) { }
static void dummy_restore_affinity ( binding_handler*, int ) { }
static int dummy_get_default_concurrency( int, int, int ) { return governor::default_num_threads(); }

// Handlers for communication with TBBbind
static void (*initialize_system_topology_ptr)(
    size_t groups_num,
    int& numa_nodes_count, int*& numa_indexes_list,
    int& core_types_count, int*& core_types_indexes_list
) = nullptr;
static void (*destroy_system_topology_ptr)( ) = dummy_destroy_system_topology;

static binding_handler* (*allocate_binding_handler_ptr)( int slot_num, int numa_id, int core_type_id, int max_threads_per_core )
    = dummy_allocate_binding_handler;
static void (*deallocate_binding_handler_ptr)( binding_handler* handler_ptr )
    = dummy_deallocate_binding_handler;
static void (*apply_affinity_ptr)( binding_handler* handler_ptr, int slot_num )
    = dummy_apply_affinity;
static void (*restore_affinity_ptr)( binding_handler* handler_ptr, int slot_num )
    = dummy_restore_affinity;
int (*get_default_concurrency_ptr)( int numa_id, int core_type_id, int max_threads_per_core )
    = dummy_get_default_concurrency;

#if _WIN32 || _WIN64 || __unix__
// Table describing how to link the handlers.
static const dynamic_link_descriptor TbbBindLinkTable[] = {
    DLD(__TBB_internal_initialize_system_topology, initialize_system_topology_ptr),
    DLD(__TBB_internal_destroy_system_topology, destroy_system_topology_ptr),
    DLD(__TBB_internal_allocate_binding_handler, allocate_binding_handler_ptr),
    DLD(__TBB_internal_deallocate_binding_handler, deallocate_binding_handler_ptr),
    DLD(__TBB_internal_apply_affinity, apply_affinity_ptr),
    DLD(__TBB_internal_restore_affinity, restore_affinity_ptr),
    DLD(__TBB_internal_get_default_concurrency, get_default_concurrency_ptr)
};

static const unsigned LinkTableSize = sizeof(TbbBindLinkTable) / sizeof(dynamic_link_descriptor);

#if TBB_USE_DEBUG
#define DEBUG_SUFFIX "_debug"
#else
#define DEBUG_SUFFIX
#endif /* TBB_USE_DEBUG */

#if _WIN32 || _WIN64
#define LIBRARY_EXTENSION ".dll"
#define LIBRARY_PREFIX
#elif __unix__
#define LIBRARY_EXTENSION __TBB_STRING(.so.3)
#define LIBRARY_PREFIX "lib"
#endif /* __unix__ */

#define TBBBIND_NAME LIBRARY_PREFIX "tbbbind" DEBUG_SUFFIX LIBRARY_EXTENSION
#define TBBBIND_2_0_NAME LIBRARY_PREFIX "tbbbind_2_0" DEBUG_SUFFIX LIBRARY_EXTENSION

#define TBBBIND_2_5_NAME LIBRARY_PREFIX "tbbbind_2_5" DEBUG_SUFFIX LIBRARY_EXTENSION
#endif /* _WIN32 || _WIN64 || __unix__ */

// Representation of system hardware topology information on the TBB side.
// System topology may be initialized by third-party component (e.g. hwloc)
// or just filled in with default stubs.
namespace system_topology {

constexpr int automatic = -1;

static std::atomic<do_once_state> initialization_state;

namespace {
int  numa_nodes_count = 0;
int* numa_nodes_indexes = nullptr;

int  core_types_count = 0;
int* core_types_indexes = nullptr;

const char* load_tbbbind_shared_object() {
#if _WIN32 || _WIN64 || __unix__
#if _WIN32 && !_WIN64
    // For 32-bit Windows applications, process affinity masks can only support up to 32 logical CPUs.
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    if (si.dwNumberOfProcessors > 32) return nullptr;
#endif /* _WIN32 && !_WIN64 */
    for (const auto& tbbbind_version : {TBBBIND_2_5_NAME, TBBBIND_2_0_NAME, TBBBIND_NAME}) {
        if (dynamic_link(tbbbind_version, TbbBindLinkTable, LinkTableSize, nullptr, DYNAMIC_LINK_LOCAL_BINDING)) {
            return tbbbind_version;
        }
    }
#endif /* _WIN32 || _WIN64 || __unix__ */
    return nullptr;
}

int processor_groups_num() {
#if _WIN32
    return NumberOfProcessorGroups();
#else
    // Stub to improve code readability by reducing number of the compile-time conditions
    return 1;
#endif
}
} // internal namespace

// Tries to load TBBbind library API, if success, gets NUMA topology information from it,
// in another case, fills NUMA topology by stubs.
void initialization_impl() {
    governor::one_time_init();

    if (const char* tbbbind_name = load_tbbbind_shared_object()) {
        initialize_system_topology_ptr(
            processor_groups_num(),
            numa_nodes_count, numa_nodes_indexes,
            core_types_count, core_types_indexes
        );

        PrintExtraVersionInfo("TBBBIND", tbbbind_name);
        return;
    }

    static int dummy_index = automatic;

    numa_nodes_count = 1;
    numa_nodes_indexes = &dummy_index;

    core_types_count = 1;
    core_types_indexes = &dummy_index;

    PrintExtraVersionInfo("TBBBIND", "UNAVAILABLE");
}

void initialize() {
    atomic_do_once(initialization_impl, initialization_state);
}

void destroy() {
    destroy_system_topology_ptr();
}
} // namespace system_topology

binding_handler* construct_binding_handler(int slot_num, int numa_id, int core_type_id, int max_threads_per_core) {
    system_topology::initialize();
    return allocate_binding_handler_ptr(slot_num, numa_id, core_type_id, max_threads_per_core);
}

void destroy_binding_handler(binding_handler* handler_ptr) {
    __TBB_ASSERT(deallocate_binding_handler_ptr, "tbbbind loading was not performed");
    deallocate_binding_handler_ptr(handler_ptr);
}

void apply_affinity_mask(binding_handler* handler_ptr, int slot_index) {
    __TBB_ASSERT(slot_index >= 0, "Negative thread index");
    __TBB_ASSERT(apply_affinity_ptr, "tbbbind loading was not performed");
    apply_affinity_ptr(handler_ptr, slot_index);
}

void restore_affinity_mask(binding_handler* handler_ptr, int slot_index) {
    __TBB_ASSERT(slot_index >= 0, "Negative thread index");
    __TBB_ASSERT(restore_affinity_ptr, "tbbbind loading was not performed");
    restore_affinity_ptr(handler_ptr, slot_index);
}

unsigned __TBB_EXPORTED_FUNC numa_node_count() {
    system_topology::initialize();
    return system_topology::numa_nodes_count;
}

void __TBB_EXPORTED_FUNC fill_numa_indices(int* index_array) {
    system_topology::initialize();
    std::memcpy(index_array, system_topology::numa_nodes_indexes, system_topology::numa_nodes_count * sizeof(int));
}

int __TBB_EXPORTED_FUNC numa_default_concurrency(int node_id) {
    if (node_id >= 0) {
        system_topology::initialize();
        int result = get_default_concurrency_ptr(
            node_id,
            /*core_type*/system_topology::automatic,
            /*threads_per_core*/system_topology::automatic
        );
        if (result > 0) return result;
    }
    return governor::default_num_threads();
}

unsigned __TBB_EXPORTED_FUNC core_type_count(intptr_t /*reserved*/) {
    system_topology::initialize();
    return system_topology::core_types_count;
}

void __TBB_EXPORTED_FUNC fill_core_type_indices(int* index_array, intptr_t /*reserved*/) {
    system_topology::initialize();
    std::memcpy(index_array, system_topology::core_types_indexes, system_topology::core_types_count * sizeof(int));
}

void constraints_assertion(d1::constraints c) {
    bool is_topology_initialized = system_topology::initialization_state == do_once_state::initialized;
    __TBB_ASSERT_RELEASE(c.max_threads_per_core == system_topology::automatic || c.max_threads_per_core > 0,
        "Wrong max_threads_per_core constraints field value.");

    auto numa_nodes_begin = system_topology::numa_nodes_indexes;
    auto numa_nodes_end = system_topology::numa_nodes_indexes + system_topology::numa_nodes_count;
    __TBB_ASSERT_RELEASE(
        c.numa_id == system_topology::automatic ||
        (is_topology_initialized && std::find(numa_nodes_begin, numa_nodes_end, c.numa_id) != numa_nodes_end),
        "The constraints::numa_id value is not known to the library. Use tbb::info::numa_nodes() to get the list of possible values.");

    int* core_types_begin = system_topology::core_types_indexes;
    int* core_types_end = system_topology::core_types_indexes + system_topology::core_types_count;
    __TBB_ASSERT_RELEASE(c.core_type == system_topology::automatic ||
        (is_topology_initialized && std::find(core_types_begin, core_types_end, c.core_type) != core_types_end),
        "The constraints::core_type value is not known to the library. Use tbb::info::core_types() to get the list of possible values.");
}

int __TBB_EXPORTED_FUNC constraints_default_concurrency(const d1::constraints& c, intptr_t /*reserved*/) {
    constraints_assertion(c);

    if (c.numa_id >= 0 || c.core_type >= 0 || c.max_threads_per_core > 0) {
        system_topology::initialize();
        return get_default_concurrency_ptr(c.numa_id, c.core_type, c.max_threads_per_core);
    }
    return governor::default_num_threads();
}

int __TBB_EXPORTED_FUNC constraints_threads_per_core(const d1::constraints&, intptr_t /*reserved*/) {
    return system_topology::automatic;
}
#endif /* __TBB_ARENA_BINDING */

} // namespace r1
} // namespace detail
} // namespace tbb
