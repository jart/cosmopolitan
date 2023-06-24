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

#ifndef __TBB_small_object_pool_impl_H
#define __TBB_small_object_pool_impl_H

#include "third_party/tbb/detail/_small_object_pool.h"
#include "third_party/tbb/detail/_utils.h"

#include "third_party/libcxx/cstddef"
#include "third_party/libcxx/cstdint"
#include "third_party/libcxx/atomic"


namespace tbb {
namespace detail {
namespace r1 {

class thread_data;

class small_object_pool_impl : public d1::small_object_pool
{
    static constexpr std::size_t small_object_size = 256;
    struct small_object {
        small_object* next;
    };
    static small_object* const dead_public_list;
public:
    void* allocate_impl(small_object_pool*& allocator, std::size_t number_of_bytes);
    void deallocate_impl(void* ptr, std::size_t number_of_bytes, thread_data& td);
    void destroy();
private:
    static std::int64_t cleanup_list(small_object* list);
    ~small_object_pool_impl() = default;
private:
    alignas(max_nfs_size) small_object* m_private_list;
    std::int64_t m_private_counter{};
    alignas(max_nfs_size) std::atomic<small_object*> m_public_list;
    std::atomic<std::int64_t> m_public_counter{};
};

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* __TBB_small_object_pool_impl_H */
