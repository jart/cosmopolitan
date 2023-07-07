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

#ifndef __TBB_collaborative_call_once_H
#define __TBB_collaborative_call_once_H

#include "third_party/tbb/task_arena.hh"
#include "third_party/tbb/task_group.hh"

#include "third_party/libcxx/atomic"

namespace tbb {
namespace detail {
namespace d1 {

#if _MSC_VER && !defined(__INTEL_COMPILER)
    // Suppress warning: structure was padded due to alignment specifier
    #pragma warning (push)
    #pragma warning (disable: 4324)
#endif

constexpr std::uintptr_t collaborative_once_max_references = max_nfs_size;
constexpr std::uintptr_t collaborative_once_references_mask = collaborative_once_max_references-1;

class alignas(max_nfs_size) collaborative_once_runner : no_copy {

    struct storage_t {
        task_arena m_arena{ task_arena::attach{} };
        wait_context m_wait_context{1};
    };

    std::atomic<std::int64_t> m_ref_count{0};
    std::atomic<bool> m_is_ready{false};

    // Storage with task_arena and wait_context must be initialized only by winner thread
    union {
        storage_t m_storage;
    };

    template<typename Fn>
    void isolated_execute(Fn f) {
        auto func = [f] {
            f();
           // delegate_base requires bool returning functor while isolate_within_arena ignores the result
            return true;
        };

        delegated_function<decltype(func)> delegate(func);

        r1::isolate_within_arena(delegate, reinterpret_cast<std::intptr_t>(this));
    }

public:
    class lifetime_guard : no_copy {
        collaborative_once_runner& m_runner;
    public:
        lifetime_guard(collaborative_once_runner& r) : m_runner(r) {
            m_runner.m_ref_count++;
        }
        ~lifetime_guard() {
            m_runner.m_ref_count--;
        }
    };

    collaborative_once_runner() {}

    ~collaborative_once_runner() {
        spin_wait_until_eq(m_ref_count, 0, std::memory_order_acquire);
        if (m_is_ready.load(std::memory_order_relaxed)) {
            m_storage.~storage_t();
        }
    }

    std::uintptr_t to_bits() {
        return reinterpret_cast<std::uintptr_t>(this);
    }

    static collaborative_once_runner* from_bits(std::uintptr_t bits) {
        __TBB_ASSERT( (bits & collaborative_once_references_mask) == 0, "invalid pointer, last log2(max_nfs_size) bits must be zero" );
        return reinterpret_cast<collaborative_once_runner*>(bits);
    }

    template <typename F>
    void run_once(F&& f) {
        __TBB_ASSERT(!m_is_ready.load(std::memory_order_relaxed), "storage with task_arena and wait_context is already initialized");
        // Initialize internal state
        new(&m_storage) storage_t();
        m_storage.m_arena.execute([&] {
            isolated_execute([&] {
                task_group_context context{ task_group_context::bound,
                    task_group_context::default_traits | task_group_context::concurrent_wait };

                function_stack_task<F> t{ std::forward<F>(f), m_storage.m_wait_context };

                // Set the ready flag after entering the execute body to prevent
                // moonlighting threads from occupying all slots inside the arena.
                m_is_ready.store(true, std::memory_order_release);
                execute_and_wait(t, context, m_storage.m_wait_context, context);
            });
        });
    }

    void assist() noexcept {
        // Do not join the arena until the winner thread takes the slot
        spin_wait_while_eq(m_is_ready, false);
        m_storage.m_arena.execute([&] {
            isolated_execute([&] {
                // We do not want to get an exception from user functor on moonlighting threads.
                // The exception is handled with the winner thread
                task_group_context stub_context;
                wait(m_storage.m_wait_context, stub_context);
            });
        });
    }

};

class collaborative_once_flag : no_copy {
    enum state : std::uintptr_t {
        uninitialized,
        done,
#if TBB_USE_ASSERT
        dead
#endif
    };
    std::atomic<std::uintptr_t> m_state{ state::uninitialized };

    template <typename Fn, typename... Args>
    friend void collaborative_call_once(collaborative_once_flag& flag, Fn&& f, Args&&... args);

    void set_completion_state(std::uintptr_t runner_bits, std::uintptr_t desired) {
        std::uintptr_t expected = runner_bits;
        do {
            expected = runner_bits;
            // Possible inefficiency: when we start waiting,
            // some moonlighting threads might continue coming that will prolong our waiting.
            // Fortunately, there are limited number of threads on the system so wait time is limited.
            spin_wait_until_eq(m_state, expected);
        } while (!m_state.compare_exchange_strong(expected, desired));
    }
    
    template <typename Fn>
    void do_collaborative_call_once(Fn&& f) {
        std::uintptr_t expected = m_state.load(std::memory_order_acquire);
        collaborative_once_runner runner;

        do {
            if (expected == state::uninitialized && m_state.compare_exchange_strong(expected, runner.to_bits())) {
                // Winner thread
                runner.run_once([&] {
                    try_call([&] {
                        std::forward<Fn>(f)();
                    }).on_exception([&] {
                        // Reset the state to uninitialized to allow other threads to try initialization again
                        set_completion_state(runner.to_bits(), state::uninitialized);
                    });
                    // We successfully executed functor
                    set_completion_state(runner.to_bits(), state::done);
                });
                break;
            } else {
                // Moonlighting thread: we need to add a reference to the state to prolong runner lifetime.
                // However, the maximum number of references are limited with runner alignment.
                // So, we use CAS loop and spin_wait to guarantee that references never exceed "max_value".
                do {
                    auto max_value = expected | collaborative_once_references_mask;
                    expected = spin_wait_while_eq(m_state, max_value);
                // "expected > state::done" prevents storing values, when state is uninitialized or done
                } while (expected > state::done && !m_state.compare_exchange_strong(expected, expected + 1));

                if (auto shared_runner = collaborative_once_runner::from_bits(expected & ~collaborative_once_references_mask)) {
                    collaborative_once_runner::lifetime_guard guard{*shared_runner};
                    m_state.fetch_sub(1);

                    // The moonlighting threads are not expected to handle exceptions from user functor.
                    // Therefore, no exception is expected from assist().
                    shared_runner->assist();
                }
            }
            __TBB_ASSERT(m_state.load(std::memory_order_relaxed) != state::dead,
                         "collaborative_once_flag has been prematurely destroyed");
        } while (expected != state::done);
    }

#if TBB_USE_ASSERT
public:
    ~collaborative_once_flag() {
        m_state.store(state::dead, std::memory_order_relaxed);
    }
#endif
};


template <typename Fn, typename... Args>
void collaborative_call_once(collaborative_once_flag& flag, Fn&& fn, Args&&... args) {
    __TBB_ASSERT(flag.m_state.load(std::memory_order_relaxed) != collaborative_once_flag::dead,
                 "collaborative_once_flag has been prematurely destroyed");
    if (flag.m_state.load(std::memory_order_acquire) != collaborative_once_flag::done) {
    #if __TBB_GCC_PARAMETER_PACK_IN_LAMBDAS_BROKEN
        // Using stored_pack to suppress bug in GCC 4.8
        // with parameter pack expansion in lambda
        auto stored_pack = save_pack(std::forward<Args>(args)...);
        auto func = [&] { call(std::forward<Fn>(fn), std::move(stored_pack)); };
    #else
        auto func = [&] { fn(std::forward<Args>(args)...); };
    #endif
        flag.do_collaborative_call_once(func);
    }
}

#if _MSC_VER && !defined(__INTEL_COMPILER)
    #pragma warning (pop) // 4324 warning
#endif

} // namespace d1
} // namespace detail

using detail::d1::collaborative_call_once;
using detail::d1::collaborative_once_flag;
} // namespace tbb

#endif // __TBB_collaborative_call_once_H
