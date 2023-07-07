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

#ifndef _TBB_thread_dispatcher_client_H
#define _TBB_thread_dispatcher_client_H

#include "third_party/tbb/detail/_intrusive_list_node.hh"
#include "third_party/tbb/arena.hh"

namespace tbb {
namespace detail {
namespace r1 {

class thread_dispatcher_client : public d1::intrusive_list_node /* Need for list in thread pool */ {
public:
    thread_dispatcher_client(arena& a, std::uint64_t aba_epoch) : my_arena(a), my_aba_epoch(aba_epoch) {}

    // Interface of communication with thread pool
    bool try_join() {
        return my_arena.try_join();
    }
    void process(thread_data& td) {
        my_arena.process(td);
    }

    unsigned priority_level() {
        return my_arena.priority_level();
    }

    std::uint64_t get_aba_epoch() {
        return my_aba_epoch;
    }

    unsigned references() {
        return my_arena.references();
    }

    bool has_request() {
        return my_arena.has_request();
    }

private:
    arena& my_arena;
    std::uint64_t my_aba_epoch;
};

} // namespace r1
} // namespace detail
} // namespace tbb

#endif // _TBB_thread_dispatcher_client_H
