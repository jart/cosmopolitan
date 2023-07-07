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

#ifndef _TBB_thread_serializer_handlers_H
#define _TBB_thread_serializer_handlers_H

#include "third_party/tbb/mutex.hh"
#include "third_party/tbb/rw_mutex.hh"

#include "third_party/tbb/thread_dispatcher.hh"

namespace tbb {
namespace detail {
namespace r1 {

class thread_request_observer {
protected:
    virtual ~thread_request_observer() {}
public:
    virtual void update(int delta) = 0;
};


class thread_request_serializer : public thread_request_observer {
    using mutex_type = d1::mutex;
public:
    thread_request_serializer(thread_dispatcher& td, int soft_limit);
    void set_active_num_workers(int soft_limit);
    bool is_no_workers_avaliable() { return my_soft_limit == 0; }

private:
    friend class thread_request_serializer_proxy;
    void update(int delta) override;
    static int limit_delta(int delta, int limit, int new_value);

    thread_dispatcher& my_thread_dispatcher;
    int my_soft_limit{ 0 };
    int my_total_request{ 0 };
    // my_pending_delta is set to pending_delta_base to have ability to hold negative values
    // consider increase base since thead number will be bigger than 1 << 15
    static constexpr std::uint64_t pending_delta_base = 1 << 15;
    std::atomic<std::uint64_t> my_pending_delta{ pending_delta_base };
    mutex_type my_mutex;
};

// Handles mandatory concurrency i.e. enables worker threads for enqueue tasks
class thread_request_serializer_proxy : public thread_request_observer {
    using mutex_type = d1::rw_mutex;
public:
    thread_request_serializer_proxy(thread_dispatcher& td, int soft_limit);
    void register_mandatory_request(int mandatory_delta);
    void set_active_num_workers(int soft_limit);

private:
    void update(int delta) override;
    void enable_mandatory_concurrency(mutex_type::scoped_lock& lock);
    void disable_mandatory_concurrency(mutex_type::scoped_lock& lock);

    std::atomic<int> my_num_mandatory_requests{0};
    bool my_is_mandatory_concurrency_enabled{false};
    thread_request_serializer my_serializer;
    mutex_type my_mutex;
};

} // namespace r1
} // namespace detail
} // namespace tbb

#endif // _TBB_thread_serializer_handlers_H
