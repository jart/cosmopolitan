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

#ifndef _TBB_permit_manager_H
#define _TBB_permit_manager_H

#include "third_party/tbb/detail/_utils.h"
#include "third_party/tbb/thread_request_serializer.h"

namespace tbb {
namespace detail {
namespace r1 {

class arena;
class pm_client;

class permit_manager : no_copy {
public:
    virtual ~permit_manager() {}
    virtual pm_client* create_client(arena& a) = 0;
    virtual void register_client(pm_client* client) = 0;
    virtual void unregister_and_destroy_client(pm_client& c) = 0;

    virtual void set_active_num_workers(int soft_limit) = 0;
    virtual void adjust_demand(pm_client&, int mandatory_delta, int workers_delta) = 0;

    void set_thread_request_observer(thread_request_observer& tr_observer) {
        __TBB_ASSERT(!my_thread_request_observer, "set_thread_request_observer was called already?");
        my_thread_request_observer = &tr_observer;
    }
protected:
    void notify_thread_request(int delta) {
        __TBB_ASSERT(my_thread_request_observer, "set_thread_request_observer was not called?");
        if (delta) {
            my_thread_request_observer->update(delta);
        }
    }
private:
    thread_request_observer* my_thread_request_observer{nullptr};
};


} // namespace r1
} // namespace detail
} // namespace tbb

#endif // _TBB_permit_manager_H
