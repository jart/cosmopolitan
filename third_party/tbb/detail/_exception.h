// clang-format off
/*
    Copyright (c) 2005-2021 Intel Corporation

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

#ifndef __TBB__exception_H
#define __TBB__exception_H

#include "third_party/tbb/detail/_config.h"

#include "third_party/libcxx/new"          // std::bad_alloc
#include "third_party/libcxx/exception"    // std::exception
#include "third_party/libcxx/stdexcept"    // std::runtime_error

namespace tbb {
namespace detail {
inline namespace d0 {
enum class exception_id {
    bad_alloc = 1,
    bad_last_alloc,
    user_abort,
    nonpositive_step,
    out_of_range,
    reservation_length_error,
    missing_wait,
    invalid_load_factor,
    invalid_key,
    bad_tagged_msg_cast,
    unsafe_wait,
    last_entry
};
} // namespace d0

#if _MSC_VER
    #pragma warning(disable: 4275)
#endif

namespace r1 {
//! Exception for concurrent containers
class TBB_EXPORT bad_last_alloc : public std::bad_alloc {
public:
    const char* __TBB_EXPORTED_METHOD what() const noexcept(true) override;
};

//! Exception for user-initiated abort
class TBB_EXPORT user_abort : public std::exception {
public:
    const char* __TBB_EXPORTED_METHOD what() const noexcept(true) override;
};

//! Exception for missing wait on structured_task_group
class TBB_EXPORT missing_wait : public std::exception {
public:
    const char* __TBB_EXPORTED_METHOD what() const noexcept(true) override;
};

//! Exception for impossible finalization of task_sheduler_handle
class TBB_EXPORT unsafe_wait : public std::runtime_error {
public:
    unsafe_wait(const char* msg) : std::runtime_error(msg) {}
};

//! Gathers all throw operators in one place.
/** Its purpose is to minimize code bloat that can be caused by throw operators
    scattered in multiple places, especially in templates. **/
TBB_EXPORT void __TBB_EXPORTED_FUNC throw_exception ( exception_id );
} // namespace r1

inline namespace d0 {
using r1::throw_exception;
} // namespace d0

} // namespace detail
} // namespace tbb

#endif // __TBB__exception_H

