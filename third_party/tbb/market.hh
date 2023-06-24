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

#ifndef _TBB_market_H
#define _TBB_market_H

#include "third_party/tbb/rw_mutex.hh"
#include "third_party/tbb/tbb_allocator.hh"
#include "third_party/tbb/task_arena.hh"

#include "third_party/tbb/permit_manager.hh"
#include "third_party/tbb/pm_client.hh"

#include "third_party/libcxx/atomic"
#include "third_party/libcxx/vector"

namespace tbb {
namespace detail {
namespace r1 {

class market : public permit_manager {
public:
    market(unsigned soft_limit);

    pm_client* create_client(arena& a) override;
    void register_client(pm_client* client) override;
    void unregister_and_destroy_client(pm_client& c) override;

    //! Request that arena's need in workers should be adjusted.
    void adjust_demand(pm_client&, int mandatory_delta, int workers_delta) override;

    //! Set number of active workers
    void set_active_num_workers(int soft_limit) override;
private:
    //! Recalculates the number of workers assigned to each arena in the list.
    void update_allotment();

    //! Keys for the arena map array. The lower the value the higher priority of the arena list.
    static constexpr unsigned num_priority_levels = d1::num_priority_levels;

    using mutex_type = d1::rw_mutex;
    mutex_type my_mutex;

    //! Current application-imposed limit on the number of workers
    int my_num_workers_soft_limit;

    //! Number of workers that were requested by all arenas on all priority levels
    int my_total_demand{0};

    //! Number of workers that were requested by arenas per single priority list item
    int my_priority_level_demand[num_priority_levels] = {0};

    //! How many times mandatory concurrency was requested from the market
    int my_mandatory_num_requested{0};

    //! Per priority list of registered arenas
    using clients_container_type = std::vector<pm_client*, tbb::tbb_allocator<pm_client*>>;
    clients_container_type my_clients[num_priority_levels];
}; // class market

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* _TBB_market_H */
