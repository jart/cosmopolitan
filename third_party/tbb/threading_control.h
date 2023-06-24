// clang-format off
/*
    Copyright (c) 2022-2023 Intel Corporation

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

#ifndef _TBB_threading_control_H
#define _TBB_threading_control_H

#include "third_party/tbb/mutex.h"
#include "third_party/tbb/global_control.h"

#include "third_party/tbb/threading_control_client.h"
#include "third_party/tbb/intrusive_list.h"
#include "third_party/tbb/main.h"
#include "third_party/tbb/permit_manager.h"
#include "third_party/tbb/pm_client.h"
#include "third_party/tbb/thread_dispatcher.h"
#include "third_party/tbb/cancellation_disseminator.h"
#include "third_party/tbb/thread_request_serializer.h"
#include "third_party/tbb/scheduler_common.h"

namespace tbb {
namespace detail {
namespace r1 {

class arena;
class thread_data;

class threading_control;

class threading_control_impl {
public:
    threading_control_impl(threading_control*);

public:
    void release(bool blocking_terminate);

    threading_control_client create_client(arena& a);
    void publish_client(threading_control_client client);

    struct client_snapshot {
        std::uint64_t aba_epoch;
        unsigned priority_level;
        thread_dispatcher_client* my_td_client;
        pm_client* my_pm_client;
    };

    client_snapshot prepare_client_destruction(threading_control_client client);
    bool try_destroy_client(client_snapshot deleter);

    void register_thread(thread_data& td);
    void unregister_thread(thread_data& td);
    void propagate_task_group_state(std::atomic<uint32_t> d1::task_group_context::*mptr_state,
                                    d1::task_group_context& src, uint32_t new_state);

    void set_active_num_workers(unsigned soft_limit);
    std::size_t worker_stack_size();
    unsigned max_num_workers();

    void adjust_demand(threading_control_client, int mandatory_delta, int workers_delta);

    thread_control_monitor& get_waiting_threads_monitor();

private:
    static unsigned calc_workers_soft_limit(unsigned workers_hard_limit);
    static std::pair<unsigned, unsigned> calculate_workers_limits();
    static cache_aligned_unique_ptr<permit_manager> make_permit_manager(unsigned workers_soft_limit);
    static cache_aligned_unique_ptr<thread_dispatcher> make_thread_dispatcher(threading_control& control,
                                                                              unsigned workers_soft_limit,
                                                                              unsigned workers_hard_limit);

    // TODO: Consider allocation one chunk of memory and construct objects on it
    cache_aligned_unique_ptr<permit_manager> my_permit_manager{nullptr};
    cache_aligned_unique_ptr<thread_dispatcher> my_thread_dispatcher{nullptr};
    cache_aligned_unique_ptr<thread_request_serializer_proxy> my_thread_request_serializer{nullptr};
    cache_aligned_unique_ptr<cancellation_disseminator> my_cancellation_disseminator{nullptr};
    cache_aligned_unique_ptr<thread_control_monitor> my_waiting_threads_monitor{nullptr};
};


class threading_control {
    using global_mutex_type = d1::mutex;
public:
    using client_snapshot = threading_control_impl::client_snapshot;

    static threading_control* register_public_reference();
    static bool unregister_public_reference(bool blocking_terminate);

    static bool is_present();
    static void set_active_num_workers(unsigned soft_limit);
    static bool register_lifetime_control();
    static bool unregister_lifetime_control(bool blocking_terminate);

    threading_control_client create_client(arena& a);
    void publish_client(threading_control_client client);
    client_snapshot prepare_client_destruction(threading_control_client client);
    bool try_destroy_client(client_snapshot deleter);

    void register_thread(thread_data& td);
    void unregister_thread(thread_data& td);
    void propagate_task_group_state(std::atomic<uint32_t> d1::task_group_context::*mptr_state,
                                    d1::task_group_context& src, uint32_t new_state);

    std::size_t worker_stack_size();
    static unsigned max_num_workers();

    void adjust_demand(threading_control_client client, int mandatory_delta, int workers_delta);

    thread_control_monitor& get_waiting_threads_monitor();

private:
    threading_control(unsigned public_ref, unsigned ref);
    void add_ref(bool is_public);
    bool remove_ref(bool is_public);

    static threading_control* get_threading_control(bool is_public);
    static threading_control* create_threading_control();

    bool release(bool is_public, bool blocking_terminate);
    void wait_last_reference(global_mutex_type::scoped_lock& lock);
    void destroy();

    friend class thread_dispatcher;

    static threading_control* g_threading_control;
    //! Mutex guarding creation/destruction of g_threading_control, insertions/deletions in my_arenas, and cancellation propagation
    static global_mutex_type g_threading_control_mutex;

    cache_aligned_unique_ptr<threading_control_impl> my_pimpl{nullptr};
    //! Count of external threads attached
    std::atomic<unsigned> my_public_ref_count{0};
    //! Reference count controlling threading_control object lifetime
    std::atomic<unsigned> my_ref_count{0};
};

} // r1
} // detail
} // tbb


#endif // _TBB_threading_control_H
