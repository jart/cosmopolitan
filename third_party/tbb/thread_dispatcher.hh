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

#ifndef _TBB_thread_dispatcher_H
#define _TBB_thread_dispatcher_H

#include "third_party/tbb/detail/_config.hh"
#include "third_party/tbb/detail/_utils.hh"
#include "third_party/tbb/rw_mutex.hh"
#include "third_party/tbb/task_arena.hh"

#include "third_party/tbb/arena.hh"
#include "third_party/tbb/governor.hh"
#include "third_party/tbb/thread_data.hh"
#include "third_party/tbb/rml_tbb.hh"
#include "third_party/tbb/thread_dispatcher_client.hh"

namespace tbb {
namespace detail {
namespace r1 {

class threading_control_impl;

class thread_dispatcher : no_copy, rml::tbb_client {
    using client_list_type = intrusive_list<thread_dispatcher_client>;
    using client_list_mutex_type = d1::rw_mutex;
public:
    thread_dispatcher(threading_control& tc, unsigned hard_limit, std::size_t stack_size);
    ~thread_dispatcher();

    thread_dispatcher_client* create_client(arena& a);
    void register_client(thread_dispatcher_client* client);
    bool try_unregister_client(thread_dispatcher_client* client, std::uint64_t aba_epoch, unsigned priority);

    void adjust_job_count_estimate(int delta);
    void release(bool blocking_terminate);
    void process(job& j) override;
    //! Used when RML asks for join mode during workers termination.
    bool must_join_workers() const;
    //! Returns the requested stack size of worker threads.
    std::size_t worker_stack_size() const;

private:
    version_type version () const override { return 0; }
    unsigned max_job_count () const override { return my_num_workers_hard_limit; }
    std::size_t min_stack_size () const override { return worker_stack_size(); }
    void cleanup(job& j) override;
    void acknowledge_close_connection() override;
    ::rml::job* create_one_job() override;

    thread_dispatcher_client* select_next_client(thread_dispatcher_client* hint);
    void destroy_client(thread_dispatcher_client* client);
    void insert_client(thread_dispatcher_client& client);
    void remove_client(thread_dispatcher_client& client);
    bool is_client_alive(thread_dispatcher_client* client);
    thread_dispatcher_client* client_in_need(client_list_type* clients, thread_dispatcher_client* hint);
    thread_dispatcher_client* client_in_need(thread_dispatcher_client* prev);

    friend class threading_control_impl;
    static constexpr unsigned num_priority_levels = d1::num_priority_levels;
    client_list_mutex_type my_list_mutex;
    client_list_type my_client_list[num_priority_levels];

    thread_dispatcher_client* my_next_client{nullptr};

    //! Shutdown mode
    bool my_join_workers{false};

    threading_control& my_threading_control;

    //! ABA prevention marker to assign to newly created clients
    std::atomic<std::uint64_t> my_clients_aba_epoch{0};

    //! Maximal number of workers allowed for use by the underlying resource manager
    /** It can't be changed after thread_dispatcher creation. **/
    unsigned my_num_workers_hard_limit{0};

    //! Stack size of worker threads
    std::size_t my_stack_size{0};

    //! First unused index of worker
    /** Used to assign indices to the new workers coming from RML **/
    std::atomic<unsigned> my_first_unused_worker_idx{0};

    //! Pointer to the RML server object that services this TBB instance.
    rml::tbb_server* my_server{nullptr};
};

} // namespace r1
} // namespace detail
} // namespace tbb

#endif // _TBB_thread_dispatcher_H
