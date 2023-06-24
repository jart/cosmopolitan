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

#include "third_party/tbb/threading_control.hh"
#include "third_party/tbb/permit_manager.hh"
#include "third_party/tbb/market.hh"
#include "third_party/tbb/thread_dispatcher.hh"
#include "third_party/tbb/governor.hh"
#include "third_party/tbb/thread_dispatcher_client.hh"

namespace tbb {
namespace detail {
namespace r1 {

// ---------------------------------------- threading_control_impl --------------------------------------------------------------

std::size_t global_control_active_value_unsafe(d1::global_control::parameter);

std::pair<unsigned, unsigned> threading_control_impl::calculate_workers_limits() {
    // Expecting that 4P is suitable for most applications.
    // Limit to 2P for large thread number.
    // TODO: ask RML for max concurrency and possibly correct hard_limit
    unsigned factor = governor::default_num_threads() <= 128 ? 4 : 2;

    // The requested number of threads is intentionally not considered in
    // computation of the hard limit, in order to separate responsibilities
    // and avoid complicated interactions between global_control and task_scheduler_init.
    // The threading control guarantees that at least 256 threads might be created.
    unsigned workers_app_limit = global_control_active_value_unsafe(global_control::max_allowed_parallelism);
    unsigned workers_hard_limit = max(max(factor * governor::default_num_threads(), 256u), workers_app_limit);
    unsigned workers_soft_limit = calc_workers_soft_limit(workers_hard_limit);
    
    return std::make_pair(workers_soft_limit, workers_hard_limit);
}

unsigned threading_control_impl::calc_workers_soft_limit(unsigned workers_hard_limit) {
    unsigned workers_soft_limit{};
    unsigned soft_limit = global_control_active_value_unsafe(global_control::max_allowed_parallelism);

    // if user set no limits (yet), use default value
    workers_soft_limit = soft_limit != 0 ? soft_limit - 1 : governor::default_num_threads() - 1;

    if (workers_soft_limit >= workers_hard_limit) {
        workers_soft_limit = workers_hard_limit - 1;
    }

    return workers_soft_limit;
}

cache_aligned_unique_ptr<permit_manager> threading_control_impl::make_permit_manager(unsigned workers_soft_limit) {
    return make_cache_aligned_unique<market>(workers_soft_limit);
}

cache_aligned_unique_ptr<thread_dispatcher> threading_control_impl::make_thread_dispatcher(threading_control& tc,
                                                                                           unsigned workers_soft_limit,
                                                                                           unsigned workers_hard_limit)
{
    stack_size_type stack_size = global_control_active_value_unsafe(global_control::thread_stack_size);

    cache_aligned_unique_ptr<thread_dispatcher> td =
        make_cache_aligned_unique<thread_dispatcher>(tc, workers_hard_limit, stack_size);
    // This check relies on the fact that for shared RML default_concurrency == max_concurrency
    if (!governor::UsePrivateRML && td->my_server->default_concurrency() < workers_soft_limit) {
        runtime_warning("RML might limit the number of workers to %u while %u is requested.\n",
            td->my_server->default_concurrency(), workers_soft_limit);
    }

    return td;
}

threading_control_impl::threading_control_impl(threading_control* tc) {
    unsigned workers_soft_limit{}, workers_hard_limit{};
    std::tie(workers_soft_limit, workers_hard_limit) = calculate_workers_limits();

    my_permit_manager = make_permit_manager(workers_soft_limit);
    my_thread_dispatcher = make_thread_dispatcher(*tc, workers_soft_limit, workers_hard_limit);
    my_thread_request_serializer =
        make_cache_aligned_unique<thread_request_serializer_proxy>(*my_thread_dispatcher, workers_soft_limit);
    my_permit_manager->set_thread_request_observer(*my_thread_request_serializer);

    my_cancellation_disseminator = make_cache_aligned_unique<cancellation_disseminator>();
    my_waiting_threads_monitor = make_cache_aligned_unique<thread_control_monitor>();
}

void threading_control_impl::release(bool blocking_terminate) {
    my_thread_dispatcher->release(blocking_terminate);
}

void threading_control_impl::set_active_num_workers(unsigned soft_limit) {
    __TBB_ASSERT(soft_limit <= my_thread_dispatcher->my_num_workers_hard_limit, nullptr);
    my_thread_request_serializer->set_active_num_workers(soft_limit);
    my_permit_manager->set_active_num_workers(soft_limit);
}

threading_control_client threading_control_impl::create_client(arena& a) {
    pm_client* pm_client = my_permit_manager->create_client(a);
    thread_dispatcher_client* td_client = my_thread_dispatcher->create_client(a);

    return threading_control_client{pm_client, td_client};
}

threading_control_impl::client_snapshot threading_control_impl::prepare_client_destruction(threading_control_client client) {
    auto td_client = client.get_thread_dispatcher_client();
    return {td_client->get_aba_epoch(), td_client->priority_level(), td_client, client.get_pm_client()};
}

bool threading_control_impl::try_destroy_client(threading_control_impl::client_snapshot snapshot) {
    if (my_thread_dispatcher->try_unregister_client(snapshot.my_td_client, snapshot.aba_epoch, snapshot.priority_level)) {
        my_permit_manager->unregister_and_destroy_client(*snapshot.my_pm_client);
        return true;
    }
    return false;
}

void threading_control_impl::publish_client(threading_control_client tc_client) {
    my_permit_manager->register_client(tc_client.get_pm_client());
    my_thread_dispatcher->register_client(tc_client.get_thread_dispatcher_client());
}

void threading_control_impl::register_thread(thread_data& td) {
    my_cancellation_disseminator->register_thread(td);
}
void threading_control_impl::unregister_thread(thread_data& td) {
    my_cancellation_disseminator->unregister_thread(td);
}

void threading_control_impl::propagate_task_group_state(std::atomic<uint32_t> d1::task_group_context::*mptr_state,
                                                        d1::task_group_context& src, uint32_t new_state)
{
    my_cancellation_disseminator->propagate_task_group_state(mptr_state, src, new_state);
}

std::size_t threading_control_impl::worker_stack_size() {
    return my_thread_dispatcher->worker_stack_size();
}

unsigned threading_control_impl::max_num_workers() {
    return my_thread_dispatcher->my_num_workers_hard_limit;
}

void threading_control_impl::adjust_demand(threading_control_client tc_client, int mandatory_delta, int workers_delta) {
    auto& c = *tc_client.get_pm_client();
    my_thread_request_serializer->register_mandatory_request(mandatory_delta);
    my_permit_manager->adjust_demand(c, mandatory_delta, workers_delta);
}

thread_control_monitor& threading_control_impl::get_waiting_threads_monitor() {
    return *my_waiting_threads_monitor;
}

// ---------------------------------------- threading_control -------------------------------------------------------------------

// Defined in global_control.cpp
void global_control_lock();
void global_control_unlock();

void threading_control::add_ref(bool is_public) {
    ++my_ref_count;
    if (is_public) {
        my_public_ref_count++;
    }
}

bool threading_control::remove_ref(bool is_public) {
    if (is_public) {
        __TBB_ASSERT(g_threading_control == this, "Global threading control instance was destroyed prematurely?");
        __TBB_ASSERT(my_public_ref_count.load(std::memory_order_relaxed), nullptr);
        --my_public_ref_count;
    }

    bool is_last_ref = --my_ref_count == 0;
    if (is_last_ref) {
        __TBB_ASSERT(!my_public_ref_count.load(std::memory_order_relaxed), nullptr);
        g_threading_control = nullptr;
    }

    return is_last_ref;
}

threading_control* threading_control::get_threading_control(bool is_public) {
    threading_control* control = g_threading_control;
    if (control) {
        control->add_ref(is_public);
    }

    return control;
}

threading_control* threading_control::create_threading_control() {
    // Global control should be locked before threading_control_impl
    global_control_lock();

    threading_control* thr_control{ nullptr };
    try_call([&] {
        global_mutex_type::scoped_lock lock(g_threading_control_mutex);

        thr_control = get_threading_control(/*public = */ true);
        if (thr_control == nullptr) {
            thr_control =  new (cache_aligned_allocate(sizeof(threading_control))) threading_control(/*public_ref = */ 1, /*private_ref = */ 1);
            thr_control->my_pimpl = make_cache_aligned_unique<threading_control_impl>(thr_control);

            __TBB_InitOnce::add_ref();

            if (global_control_active_value_unsafe(global_control::scheduler_handle)) {
                ++thr_control->my_public_ref_count;
                ++thr_control->my_ref_count;
            }

            g_threading_control = thr_control;
        }
    }).on_exception([&] {
        global_control_unlock();

        cache_aligned_deleter deleter{};
        deleter(thr_control);
    });

    global_control_unlock();
    return thr_control;
}

void threading_control::destroy () {
    cache_aligned_deleter deleter;
    deleter(this);
    __TBB_InitOnce::remove_ref();
}

void threading_control::wait_last_reference(global_mutex_type::scoped_lock& lock) {
    while (my_public_ref_count.load(std::memory_order_relaxed) == 1 && my_ref_count.load(std::memory_order_relaxed) > 1) {
        lock.release();
        // To guarantee that request_close_connection() is called by the last external thread, we need to wait till all
        // references are released. Re-read my_public_ref_count to limit waiting if new external threads are created.
        // Theoretically, new private references to the threading control can be added during waiting making it potentially
        // endless.
        // TODO: revise why the weak scheduler needs threading control's pointer and try to remove this wait.
        // Note that the threading control should know about its schedulers for cancellation/exception/priority propagation,
        // see e.g. task_group_context::cancel_group_execution()
        while (my_public_ref_count.load(std::memory_order_acquire) == 1 && my_ref_count.load(std::memory_order_acquire) > 1) {
            yield();
        }
        lock.acquire(g_threading_control_mutex);
    }
}

bool threading_control::release(bool is_public, bool blocking_terminate) {
    bool do_release = false;
    {
        global_mutex_type::scoped_lock lock(g_threading_control_mutex);
        if (blocking_terminate) {
            __TBB_ASSERT(is_public, "Only an object with a public reference can request the blocking terminate");
            wait_last_reference(lock);
        }
        do_release = remove_ref(is_public);
    }

    if (do_release) {
        __TBB_ASSERT(!my_public_ref_count.load(std::memory_order_relaxed), "No public references must remain if we remove the threading control.");
        // inform RML that blocking termination is required
        my_pimpl->release(blocking_terminate);
        return blocking_terminate;
    }
    return false;
}

threading_control::threading_control(unsigned public_ref, unsigned ref) : my_public_ref_count(public_ref), my_ref_count(ref)
{}

threading_control* threading_control::register_public_reference() {
    threading_control* control{nullptr};
    global_mutex_type::scoped_lock lock(g_threading_control_mutex);
    control = get_threading_control(/*public = */ true);
    if (!control) {
        // We are going to create threading_control_impl, we should acquire mutexes in right order
        lock.release();
        control = create_threading_control();
    }

    return control;
}

bool threading_control::unregister_public_reference(bool blocking_terminate) {
    __TBB_ASSERT(g_threading_control, "Threading control should exist until last public reference");
    __TBB_ASSERT(g_threading_control->my_public_ref_count.load(std::memory_order_relaxed), nullptr);
    return g_threading_control->release(/*public = */ true, /*blocking_terminate = */ blocking_terminate);
}

threading_control_client threading_control::create_client(arena& a) {
    {
        global_mutex_type::scoped_lock lock(g_threading_control_mutex);
        add_ref(/*public = */ false);
    }

    return my_pimpl->create_client(a);
}

void threading_control::publish_client(threading_control_client client) {
    return my_pimpl->publish_client(client);
}

threading_control::client_snapshot threading_control::prepare_client_destruction(threading_control_client client) {
    return my_pimpl->prepare_client_destruction(client);
}

bool threading_control::try_destroy_client(threading_control::client_snapshot deleter) {
    bool res = my_pimpl->try_destroy_client(deleter);
    if (res) {
        release(/*public = */ false, /*blocking_terminate = */ false);
    }
    return res;
}

void threading_control::set_active_num_workers(unsigned soft_limit) {
    threading_control* thr_control = get_threading_control(/*public = */ false);
    if (thr_control != nullptr) {
        thr_control->my_pimpl->set_active_num_workers(soft_limit);
        thr_control->release(/*is_public=*/false, /*blocking_terminate=*/false);
    }
}

bool threading_control::is_present() {
    global_mutex_type::scoped_lock lock(g_threading_control_mutex);
    return g_threading_control != nullptr;
}

bool threading_control::register_lifetime_control() {
    global_mutex_type::scoped_lock lock(g_threading_control_mutex);
    return get_threading_control(/*public = */ true) != nullptr;
}

bool threading_control::unregister_lifetime_control(bool blocking_terminate) {
    threading_control* thr_control{nullptr};
    {
        global_mutex_type::scoped_lock lock(g_threading_control_mutex);
        thr_control = g_threading_control;
    }

    bool released{true};
    if (thr_control) {
        released = thr_control->release(/*public = */ true, /*blocking_terminate = */ blocking_terminate);
    }

    return released;
}

void threading_control::register_thread(thread_data& td) {
    my_pimpl->register_thread(td);
}

void threading_control::unregister_thread(thread_data& td) {
    my_pimpl->unregister_thread(td);
}

void threading_control::propagate_task_group_state(std::atomic<uint32_t> d1::task_group_context::*mptr_state,
                                                   d1::task_group_context& src, uint32_t new_state)
{
    my_pimpl->propagate_task_group_state(mptr_state, src, new_state);
}

std::size_t threading_control::worker_stack_size() {
    return my_pimpl->worker_stack_size();
}

unsigned threading_control::max_num_workers() {
    global_mutex_type::scoped_lock lock(g_threading_control_mutex);
    return g_threading_control ? g_threading_control->my_pimpl->max_num_workers() : 0;
}

void threading_control::adjust_demand(threading_control_client client, int mandatory_delta, int workers_delta) {
    my_pimpl->adjust_demand(client, mandatory_delta, workers_delta);
}

thread_control_monitor& threading_control::get_waiting_threads_monitor() {
    return my_pimpl->get_waiting_threads_monitor();
}

} // r1
} // detail
} // tbb
