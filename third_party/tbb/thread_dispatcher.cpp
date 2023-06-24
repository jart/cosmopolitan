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

#include "third_party/tbb/thread_dispatcher.h"
#include "third_party/tbb/threading_control.h"

namespace tbb {
namespace detail {
namespace r1 {

thread_dispatcher::thread_dispatcher(threading_control& tc, unsigned hard_limit, std::size_t stack_size)
    : my_threading_control(tc)
    , my_num_workers_hard_limit(hard_limit)
    , my_stack_size(stack_size)
{
    my_server = governor::create_rml_server( *this );
    __TBB_ASSERT( my_server, "Failed to create RML server" );
}

thread_dispatcher::~thread_dispatcher() {
    poison_pointer(my_server);
}

thread_dispatcher_client* thread_dispatcher::select_next_client(thread_dispatcher_client* hint) {
    unsigned next_client_priority_level = num_priority_levels;
    if (hint) {
        next_client_priority_level = hint->priority_level();
    }

    for (unsigned idx = 0; idx < next_client_priority_level; ++idx) {
        if (!my_client_list[idx].empty()) {
            return &*my_client_list[idx].begin();
        }
    }

    return hint;
}

thread_dispatcher_client* thread_dispatcher::create_client(arena& a) {
    return new (cache_aligned_allocate(sizeof(thread_dispatcher_client))) thread_dispatcher_client(a, my_clients_aba_epoch);
}


void thread_dispatcher::register_client(thread_dispatcher_client* client) {
    client_list_mutex_type::scoped_lock lock(my_list_mutex);
    insert_client(*client);
}

bool thread_dispatcher::try_unregister_client(thread_dispatcher_client* client, std::uint64_t aba_epoch, unsigned priority) {
    __TBB_ASSERT(client, nullptr);
    // we hold reference to the server, so market cannot be destroyed at any moment here
    __TBB_ASSERT(!is_poisoned(my_server), nullptr);
    my_list_mutex.lock();
    for (auto& it : my_client_list[priority]) {
        if (client == &it) {
            if (it.get_aba_epoch() == aba_epoch) {
                // Client is alive
                // Acquire my_references to sync with threads that just left the arena
                // Pay attention that references should be read before workers_requested because
                // if references is no zero some other thread might call adjust_demand and lead to
                // a race over workers_requested
                if (!client->references() && !client->has_request()) {
                    // Client is abandoned. Destroy it.
                    remove_client(*client);
                    ++my_clients_aba_epoch;

                    my_list_mutex.unlock();
                    destroy_client(client);

                    return true;
                }
            }
            break;
        }
    }
    my_list_mutex.unlock();
    return false;
}

void thread_dispatcher::destroy_client(thread_dispatcher_client* client) {
    client->~thread_dispatcher_client();
    cache_aligned_deallocate(client);
}

// Should be called under lock
void thread_dispatcher::insert_client(thread_dispatcher_client& client) {
    __TBB_ASSERT(client.priority_level() < num_priority_levels, nullptr);
    my_client_list[client.priority_level()].push_front(client);

    __TBB_ASSERT(!my_next_client || my_next_client->priority_level() < num_priority_levels, nullptr);
    my_next_client = select_next_client(my_next_client);
}

// Should be called under lock
void thread_dispatcher::remove_client(thread_dispatcher_client& client) {
    __TBB_ASSERT(client.priority_level() < num_priority_levels, nullptr);
    my_client_list[client.priority_level()].remove(client);

    if (my_next_client == &client) {
        my_next_client = nullptr;
    }
    my_next_client = select_next_client(my_next_client);
}

bool thread_dispatcher::is_client_alive(thread_dispatcher_client* client) {
    if (!client) {
        return false;
    }

    // Still cannot access internals of the client since the object itself might be destroyed.
    for (auto& priority_list : my_client_list) {
        for (auto& c : priority_list) {
            if (client == &c) {
                return true;
            }
        }
    }
    return false;
}

thread_dispatcher_client* thread_dispatcher::client_in_need(client_list_type* clients, thread_dispatcher_client* hint) {
    // TODO: make sure client with higher priority returned only if there are available slots in it.
    hint = select_next_client(hint);
    if (!hint) {
        return nullptr;
    }

    client_list_type::iterator it = hint;
    unsigned curr_priority_level = hint->priority_level();
    __TBB_ASSERT(it != clients[curr_priority_level].end(), nullptr);
    do {
        thread_dispatcher_client& t = *it;
        if (++it == clients[curr_priority_level].end()) {
            do {
                ++curr_priority_level %= num_priority_levels;
            } while (clients[curr_priority_level].empty());
            it = clients[curr_priority_level].begin();
        }
        if (t.try_join()) {
            return &t;
        }
    } while (it != hint);
    return nullptr;
}

thread_dispatcher_client* thread_dispatcher::client_in_need(thread_dispatcher_client* prev) {
    client_list_mutex_type::scoped_lock lock(my_list_mutex, /*is_writer=*/false);
    if (is_client_alive(prev)) {
        return client_in_need(my_client_list, prev);
    }
    return client_in_need(my_client_list, my_next_client);
}

void thread_dispatcher::adjust_job_count_estimate(int delta) {
    my_server->adjust_job_count_estimate(delta);
}

void thread_dispatcher::release(bool blocking_terminate) {
    my_join_workers = blocking_terminate;
    my_server->request_close_connection();
}

void thread_dispatcher::process(job& j) {
    thread_data& td = static_cast<thread_data&>(j);
    // td.my_last_client can be dead. Don't access it until client_in_need is called
    thread_dispatcher_client* client = td.my_last_client;
    for (int i = 0; i < 2; ++i) {
        while ((client = client_in_need(client)) ) {
            td.my_last_client = client;
            client->process(td);
        }
        // Workers leave thread_dispatcher because there is no client in need. It can happen earlier than
        // adjust_job_count_estimate() decreases my_slack and RML can put this thread to sleep.
        // It might result in a busy-loop checking for my_slack<0 and calling this method instantly.
        // the yield refines this spinning.
        if ( !i ) {
            yield();
        }
    }
}


//! Used when RML asks for join mode during workers termination.
bool thread_dispatcher::must_join_workers() const { return my_join_workers; }

//! Returns the requested stack size of worker threads.
std::size_t thread_dispatcher::worker_stack_size() const { return my_stack_size; }

void thread_dispatcher::acknowledge_close_connection() {
    my_threading_control.destroy();
}

::rml::job* thread_dispatcher::create_one_job() {
    unsigned short index = ++my_first_unused_worker_idx;
    __TBB_ASSERT(index > 0, nullptr);
    ITT_THREAD_SET_NAME(_T("TBB Worker Thread"));
    // index serves as a hint decreasing conflicts between workers when they migrate between arenas
    thread_data* td = new (cache_aligned_allocate(sizeof(thread_data))) thread_data{ index, true };
    __TBB_ASSERT(index <= my_num_workers_hard_limit, nullptr);
    my_threading_control.register_thread(*td);
    return td;
}

void thread_dispatcher::cleanup(job& j) {
    my_threading_control.unregister_thread(static_cast<thread_data&>(j));
    governor::auto_terminate(&j);
}

} // namespace r1
} // namespace detail
} // namespace tbb
