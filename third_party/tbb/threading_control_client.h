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

#ifndef _TBB_threading_control_client_H
#define _TBB_threading_control_client_H

#include "third_party/tbb/detail/_assert.h"

namespace tbb {
namespace detail {
namespace r1 {

class pm_client;
class thread_dispatcher_client;

class threading_control_client {
public:
    threading_control_client() = default;
    threading_control_client(const threading_control_client&) = default;
    threading_control_client& operator=(const threading_control_client&) = default;

    threading_control_client(pm_client* p, thread_dispatcher_client* t) : my_pm_client(p), my_thread_dispatcher_client(t) {
        __TBB_ASSERT(my_pm_client, nullptr);
        __TBB_ASSERT(my_thread_dispatcher_client, nullptr);
    }

    pm_client* get_pm_client() {
        return my_pm_client;
    }

    thread_dispatcher_client* get_thread_dispatcher_client() {
        return my_thread_dispatcher_client;
    }

private:
    pm_client* my_pm_client{nullptr};
    thread_dispatcher_client* my_thread_dispatcher_client{nullptr};
};


}
}
}

#endif // _TBB_threading_control_client_H
