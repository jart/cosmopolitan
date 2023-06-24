// clang-format off
/*
    Copyright (c) 2020-2021 Intel Corporation

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

#include "third_party/tbb/detail/_utils.h"
#include "third_party/tbb/concurrent_queue.h"
#include "third_party/tbb/cache_aligned_allocator.h"
#include "third_party/tbb/concurrent_monitor.h"

namespace tbb {
namespace detail {
namespace r1 {

static constexpr std::size_t monitors_number = 2;

std::uint8_t* __TBB_EXPORTED_FUNC allocate_bounded_queue_rep( std::size_t queue_rep_size )
{
    std::size_t monitors_mem_size = sizeof(concurrent_monitor) * monitors_number;
    std::uint8_t* mem = static_cast<std::uint8_t*>(cache_aligned_allocate(queue_rep_size + monitors_mem_size));

    concurrent_monitor* monitors = reinterpret_cast<concurrent_monitor*>(mem + queue_rep_size);
    for (std::size_t i = 0; i < monitors_number; ++i) {
        new (monitors + i) concurrent_monitor();
    }

    return mem;
}

void __TBB_EXPORTED_FUNC deallocate_bounded_queue_rep( std::uint8_t* mem, std::size_t queue_rep_size )
{
    concurrent_monitor* monitors = reinterpret_cast<concurrent_monitor*>(mem + queue_rep_size);
    for (std::size_t i = 0; i < monitors_number; ++i) {
        monitors[i].~concurrent_monitor();
    }

    cache_aligned_deallocate(mem);
}

void __TBB_EXPORTED_FUNC wait_bounded_queue_monitor( concurrent_monitor* monitors, std::size_t monitor_tag,
                                                        std::ptrdiff_t target, d1::delegate_base& predicate )
{
    __TBB_ASSERT(monitor_tag < monitors_number, nullptr);
    concurrent_monitor& monitor = monitors[monitor_tag];

    monitor.wait<concurrent_monitor::thread_context>([&] { return !predicate(); }, std::uintptr_t(target));
}

void __TBB_EXPORTED_FUNC abort_bounded_queue_monitors( concurrent_monitor* monitors ) {
    concurrent_monitor& items_avail = monitors[d2::cbq_items_avail_tag];
    concurrent_monitor& slots_avail = monitors[d2::cbq_slots_avail_tag];

    items_avail.abort_all();
    slots_avail.abort_all();
}

struct predicate_leq {
    std::size_t my_ticket;
    predicate_leq( std::size_t ticket ) : my_ticket(ticket) {}
    bool operator() ( std::uintptr_t ticket ) const { return static_cast<std::size_t>(ticket) <= my_ticket; }
};

void __TBB_EXPORTED_FUNC notify_bounded_queue_monitor( concurrent_monitor* monitors,
                                                               std::size_t monitor_tag, std::size_t ticket)
{
    __TBB_ASSERT(monitor_tag < monitors_number, nullptr);
    concurrent_monitor& monitor = monitors[monitor_tag];
    monitor.notify(predicate_leq(ticket));
}

} // namespace r1
} // namespace detail
} // namespace tbb
