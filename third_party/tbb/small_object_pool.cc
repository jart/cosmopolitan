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

#include "third_party/tbb/cache_aligned_allocator.hh"
#include "third_party/tbb/detail/_small_object_pool.hh"
#include "third_party/tbb/detail/_task.hh"
#include "third_party/tbb/governor.hh"
#include "third_party/tbb/thread_data.hh"
#include "third_party/tbb/task_dispatcher.hh"

#include "third_party/libcxx/cstddef"

namespace tbb {
namespace detail {
namespace r1 {

small_object_pool_impl::small_object* const small_object_pool_impl::dead_public_list =
                reinterpret_cast<small_object_pool_impl::small_object*>(1);

void* __TBB_EXPORTED_FUNC allocate(d1::small_object_pool*& allocator, std::size_t number_of_bytes, const d1::execution_data& ed) {
    auto& tls = static_cast<const execution_data_ext&>(ed).task_disp->get_thread_data();
    auto pool = tls.my_small_object_pool;
    return pool->allocate_impl(allocator, number_of_bytes);
}

void* __TBB_EXPORTED_FUNC allocate(d1::small_object_pool*& allocator, std::size_t number_of_bytes) {
    // TODO: optimize if the allocator contains a valid pool.
    auto tls = governor::get_thread_data();
    auto pool = tls->my_small_object_pool;
    return pool->allocate_impl(allocator, number_of_bytes);
}

void* small_object_pool_impl::allocate_impl(d1::small_object_pool*& allocator, std::size_t number_of_bytes)
{
    small_object* obj{nullptr};

    if (number_of_bytes <= small_object_size) {
        if (m_private_list) {
            obj = m_private_list;
            m_private_list = m_private_list->next;
        } else if (m_public_list.load(std::memory_order_relaxed)) {
            // No fence required for read of my_public_list above, because std::atomic::exchange() has a fence.
            obj = m_public_list.exchange(nullptr);
            __TBB_ASSERT( obj, "another thread emptied the my_public_list" );
            m_private_list = obj->next;
        } else {
            obj = new (cache_aligned_allocate(small_object_size)) small_object{nullptr};
            ++m_private_counter;
        }
    } else {
        obj = new (cache_aligned_allocate(number_of_bytes)) small_object{nullptr};
    }
    allocator = this;

    // Return uninitialized memory for further construction on user side.
    obj->~small_object();
    return obj;
}

void __TBB_EXPORTED_FUNC deallocate(d1::small_object_pool& allocator, void* ptr, std::size_t number_of_bytes) {
    auto pool = static_cast<small_object_pool_impl*>(&allocator);
    auto tls = governor::get_thread_data();
    pool->deallocate_impl(ptr, number_of_bytes, *tls);
}

void __TBB_EXPORTED_FUNC deallocate(d1::small_object_pool& allocator, void* ptr, std::size_t number_of_bytes, const d1::execution_data& ed) {
    auto& tls = static_cast<const execution_data_ext&>(ed).task_disp->get_thread_data();
    auto pool = static_cast<small_object_pool_impl*>(&allocator);
    pool->deallocate_impl(ptr, number_of_bytes, tls);
}

void small_object_pool_impl::deallocate_impl(void* ptr, std::size_t number_of_bytes, thread_data& td) {
    __TBB_ASSERT(ptr != nullptr, "pointer to deallocate should not be null");
    __TBB_ASSERT(number_of_bytes >= sizeof(small_object), "number of bytes should be at least sizeof(small_object)");

    if (number_of_bytes <= small_object_size) {
        auto obj = new (ptr) small_object{nullptr};
        if (td.my_small_object_pool == this) {
            obj->next = m_private_list;
            m_private_list = obj;
        } else {
            auto old_public_list = m_public_list.load(std::memory_order_relaxed);

            for (;;) {
                if (old_public_list == dead_public_list) {
                    obj->~small_object();
                    cache_aligned_deallocate(obj);
                    if (++m_public_counter == 0)
                    {
                        this->~small_object_pool_impl();
                        cache_aligned_deallocate(this);
                    }
                    break;
                }
                obj->next = old_public_list;
                if (m_public_list.compare_exchange_strong(old_public_list, obj)) {
                    break;
                }
            }
        }
    } else {
        cache_aligned_deallocate(ptr);
    }
}

std::int64_t small_object_pool_impl::cleanup_list(small_object* list)
{
    std::int64_t removed_count{};

    while (list) {
        small_object* current = list;
        list = list->next;
        current->~small_object();
        cache_aligned_deallocate(current);
        ++removed_count;
    }
    return removed_count;
}

void small_object_pool_impl::destroy()
{
    // clean up private list and subtract the removed count from private counter
    m_private_counter -= cleanup_list(m_private_list);
    // Grab public list and place dead mark
    small_object* public_list = m_public_list.exchange(dead_public_list);
    // clean up public list and subtract from private (intentionally) counter
    m_private_counter -= cleanup_list(public_list);
    __TBB_ASSERT(m_private_counter >= 0, "Private counter may not be less than 0");
    // Equivalent to fetch_sub(m_private_counter) - m_private_counter. But we need to do it
    // atomically with operator-= not to access m_private_counter after the subtraction.
    auto new_value = m_public_counter -= m_private_counter;
    // check if this method is responsible to clean up the resources
    if (new_value == 0) {
        this->~small_object_pool_impl();
        cache_aligned_deallocate(this);
    }
}

} // namespace r1
} // namespace detail
} // namespace tbb
