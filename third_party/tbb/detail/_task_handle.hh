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


#ifndef __TBB_task_handle_H
#define __TBB_task_handle_H

#include "third_party/tbb/detail/_config.hh"
#include "third_party/tbb/detail/_task.hh"
#include "third_party/tbb/detail/_small_object_pool.hh"
#include "third_party/tbb/detail/_utils.hh"
#include "third_party/libcxx/memory"

namespace tbb {
namespace detail {

namespace d1 { class task_group_context; class wait_context; struct execution_data; }
namespace d2 {

class task_handle;

class task_handle_task : public d1::task {
    std::uint64_t m_version_and_traits{};
    d1::wait_context& m_wait_ctx;
    d1::task_group_context& m_ctx;
    d1::small_object_allocator m_allocator;
public:
    void finalize(const d1::execution_data* ed = nullptr) {
        if (ed) {
            m_allocator.delete_object(this, *ed);
        } else {
            m_allocator.delete_object(this);
        }
    }

    task_handle_task(d1::wait_context& wo, d1::task_group_context& ctx, d1::small_object_allocator& alloc)
        : m_wait_ctx(wo)
        , m_ctx(ctx)
        , m_allocator(alloc) {
        suppress_unused_warning(m_version_and_traits);
    }

    ~task_handle_task() override {
        m_wait_ctx.release();
    }

    d1::task_group_context& ctx() const { return m_ctx; }
};


class task_handle {
    struct task_handle_task_finalizer_t{
        void operator()(task_handle_task* p){ p->finalize(); }
    };
    using handle_impl_t = std::unique_ptr<task_handle_task, task_handle_task_finalizer_t>;

    handle_impl_t m_handle = {nullptr};
public:
    task_handle() = default;
    task_handle(task_handle&&) = default;
    task_handle& operator=(task_handle&&) = default;

    explicit operator bool() const noexcept { return static_cast<bool>(m_handle); }

    friend bool operator==(task_handle const& th, std::nullptr_t) noexcept;
    friend bool operator==(std::nullptr_t, task_handle const& th) noexcept;

    friend bool operator!=(task_handle const& th, std::nullptr_t) noexcept;
    friend bool operator!=(std::nullptr_t, task_handle const& th) noexcept;

private:
    friend struct task_handle_accessor;

    task_handle(task_handle_task* t) : m_handle {t}{};

    d1::task* release() {
       return m_handle.release();
    }
};

struct task_handle_accessor {
static task_handle              construct(task_handle_task* t)  { return {t}; }
static d1::task*                release(task_handle& th)        { return th.release(); }
static d1::task_group_context&  ctx_of(task_handle& th)         {
    __TBB_ASSERT(th.m_handle, "ctx_of does not expect empty task_handle.");
    return th.m_handle->ctx();
}
};

inline bool operator==(task_handle const& th, std::nullptr_t) noexcept {
    return th.m_handle == nullptr;
}
inline bool operator==(std::nullptr_t, task_handle const& th) noexcept {
    return th.m_handle == nullptr;
}

inline bool operator!=(task_handle const& th, std::nullptr_t) noexcept {
    return th.m_handle != nullptr;
}

inline bool operator!=(std::nullptr_t, task_handle const& th) noexcept {
    return th.m_handle != nullptr;
}

} // namespace d2
} // namespace detail
} // namespace tbb

#endif /* __TBB_task_handle_H */
