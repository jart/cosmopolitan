// clang-format off
/*
    Copyright (c) 2021 Intel Corporation

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

#ifndef __TBB_detail__mutex_common_H
#define __TBB_detail__mutex_common_H

#include "third_party/tbb/detail/_config.hh"
#include "third_party/tbb/detail/_utils.hh"

#if __TBB_CPP20_CONCEPTS_PRESENT
// MISSING #include <concepts>

namespace tbb {
namespace detail {
inline namespace d0 {

template <typename Lock, typename Mutex>
concept mutex_scoped_lock = std::default_initializable<Lock> &&
                            std::constructible_from<Lock, Mutex&> &&
                            requires( Lock& lock, Mutex& mutex ) {
                                lock.acquire(mutex);
                                { lock.try_acquire(mutex) } -> adaptive_same_as<bool>;
                                lock.release();
                            };

template <typename Lock, typename Mutex>
concept rw_mutex_scoped_lock = mutex_scoped_lock<Lock, Mutex> &&
                               std::constructible_from<Lock, Mutex&, bool> &&
                               requires( Lock& lock, Mutex& mutex ) {
                                   lock.acquire(mutex, false);
                                   { lock.try_acquire(mutex, false) } -> adaptive_same_as<bool>;
                                   { lock.upgrade_to_writer() } -> adaptive_same_as<bool>;
                                   { lock.downgrade_to_reader() } -> adaptive_same_as<bool>;
                               };

template <typename Mutex>
concept scoped_lockable = mutex_scoped_lock<typename Mutex::scoped_lock, Mutex>;

template <typename Mutex>
concept rw_scoped_lockable = scoped_lockable<Mutex> &&
                             rw_mutex_scoped_lock<typename Mutex::scoped_lock, Mutex>;

} // namespace d0
} // namespace detail
} // namespace tbb

#endif // __TBB_CPP20_CONCEPTS_PRESENT
#endif // __TBB_detail__mutex_common_H
