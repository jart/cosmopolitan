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

#include "third_party/tbb/misc.hh"
#include "third_party/tbb/thread_request_serializer.hh"

namespace tbb {
namespace detail {
namespace r1 {

thread_request_serializer::thread_request_serializer(thread_dispatcher& td, int soft_limit)
    : my_thread_dispatcher(td)
    , my_soft_limit(soft_limit)
{}

void thread_request_serializer::update(int delta) {
    constexpr std::uint64_t delta_mask = (pending_delta_base << 1) - 1;
    constexpr std::uint64_t counter_value = delta_mask + 1;

    int prev_pending_delta = my_pending_delta.fetch_add(counter_value + delta);

    // There is a pseudo request aggregator, so only thread that see pending_delta_base in my_pending_delta
    // Will enter to critical section and call adjust_job_count_estimate
    if (prev_pending_delta == pending_delta_base) {
        delta = int(my_pending_delta.exchange(pending_delta_base) & delta_mask) - int(pending_delta_base);
        mutex_type::scoped_lock lock(my_mutex);
        my_total_request += delta;
        delta = limit_delta(delta, my_soft_limit, my_total_request);
        my_thread_dispatcher.adjust_job_count_estimate(delta);
    }
}

void thread_request_serializer::set_active_num_workers(int soft_limit) {
    mutex_type::scoped_lock lock(my_mutex);
    int delta = soft_limit - my_soft_limit;
    delta = limit_delta(delta, my_total_request, soft_limit);
    my_thread_dispatcher.adjust_job_count_estimate(delta);
    my_soft_limit = soft_limit;
}

int thread_request_serializer::limit_delta(int delta, int limit, int new_value) {
    // This method can be described with such pseudocode:
    // bool above_limit = prev_value >= limit && new_value >= limit;
    // bool below_limit = prev_value <= limit && new_value <= limit;
    // enum request_type { ABOVE_LIMIT, CROSS_LIMIT, BELOW_LIMIT };
    // request = above_limit ? ABOVE_LIMIT : below_limit ? BELOW_LIMIT : CROSS_LIMIT;

    // switch (request) {
    // case ABOVE_LIMIT:
    //     delta = 0;
    // case CROSS_LIMIT:
    //     delta = delta > 0 ? limit - prev_value : new_value - limit;
    // case BELOW_LIMIT:
    //     // No changes to delta
    // }

   int prev_value = new_value - delta;

    // actual new_value and prev_value cannot exceed the limit
    new_value = min(limit, new_value);
    prev_value = min(limit, prev_value);
    return new_value - prev_value;
}


thread_request_serializer_proxy::thread_request_serializer_proxy(thread_dispatcher& td, int soft_limit) : my_serializer(td, soft_limit)
{}

void thread_request_serializer_proxy::register_mandatory_request(int mandatory_delta) {
    if (mandatory_delta != 0) {
        mutex_type::scoped_lock lock(my_mutex, /* is_write = */ false);
        int prev_value = my_num_mandatory_requests.fetch_add(mandatory_delta);

        const bool should_try_enable = mandatory_delta > 0 && prev_value == 0;
        const bool should_try_disable = mandatory_delta < 0 && prev_value == 1;

        if (should_try_enable) {
            enable_mandatory_concurrency(lock);
        } else if (should_try_disable) {
            disable_mandatory_concurrency(lock);
        }
    }
}

void thread_request_serializer_proxy::set_active_num_workers(int soft_limit) {
    mutex_type::scoped_lock lock(my_mutex, /* is_write = */ true);

    if (soft_limit != 0) {
        my_is_mandatory_concurrency_enabled = false;
        my_serializer.set_active_num_workers(soft_limit);
    } else {
        if (my_num_mandatory_requests > 0 && !my_is_mandatory_concurrency_enabled) {
            my_is_mandatory_concurrency_enabled = true;
            my_serializer.set_active_num_workers(1);
        }
    }
}

void thread_request_serializer_proxy::update(int delta) { my_serializer.update(delta); }

void thread_request_serializer_proxy::enable_mandatory_concurrency(mutex_type::scoped_lock& lock) {
    lock.upgrade_to_writer();
    bool still_should_enable = my_num_mandatory_requests.load(std::memory_order_relaxed) > 0 &&
            !my_is_mandatory_concurrency_enabled && my_serializer.is_no_workers_avaliable();

    if (still_should_enable) {
        my_is_mandatory_concurrency_enabled = true;
        my_serializer.set_active_num_workers(1);
    }
}

void thread_request_serializer_proxy::disable_mandatory_concurrency(mutex_type::scoped_lock& lock) {
    lock.upgrade_to_writer();
    bool still_should_disable = my_num_mandatory_requests.load(std::memory_order_relaxed) <= 0 &&
            my_is_mandatory_concurrency_enabled && !my_serializer.is_no_workers_avaliable();

    if (still_should_disable) {
        my_is_mandatory_concurrency_enabled = false;
        my_serializer.set_active_num_workers(0);
    }
}

} // r1
} // detail
} // tbb
