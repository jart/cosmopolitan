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

#include "third_party/tbb/detail/_config.hh"
#include "third_party/tbb/detail/_template_helpers.hh"

#include "third_party/tbb/global_control.hh"
#include "third_party/tbb/tbb_allocator.hh"
#include "third_party/tbb/spin_mutex.hh"

#include "third_party/tbb/governor.hh"
#include "third_party/tbb/threading_control.hh"
#include "third_party/tbb/market.hh"
#include "third_party/tbb/misc.hh"

#include "third_party/libcxx/atomic"
#include "third_party/libcxx/set"

namespace tbb {
namespace detail {
namespace r1 {

//! Comparator for a set of global_control objects
struct control_storage_comparator {
    bool operator()(const d1::global_control* lhs, const d1::global_control* rhs) const;
};

class control_storage {
    friend struct global_control_impl;
    friend std::size_t global_control_active_value(int);
    friend void global_control_lock();
    friend void global_control_unlock();
    friend std::size_t global_control_active_value_unsafe(d1::global_control::parameter);
protected:
    std::size_t my_active_value{0};
    std::set<d1::global_control*, control_storage_comparator, tbb_allocator<d1::global_control*>> my_list{};
    spin_mutex my_list_mutex{};
public:
    virtual std::size_t default_value() const = 0;
    virtual void apply_active(std::size_t new_active) {
        my_active_value = new_active;
    }
    virtual bool is_first_arg_preferred(std::size_t a, std::size_t b) const {
        return a>b; // prefer max by default
    }
    virtual std::size_t active_value() {
        spin_mutex::scoped_lock lock(my_list_mutex); // protect my_list.empty() call
        return !my_list.empty() ? my_active_value : default_value();
    }

    std::size_t active_value_unsafe() {
        return !my_list.empty() ? my_active_value : default_value();
    }
};

class alignas(max_nfs_size) allowed_parallelism_control : public control_storage {
    std::size_t default_value() const override {
        return max(1U, governor::default_num_threads());
    }
    bool is_first_arg_preferred(std::size_t a, std::size_t b) const override {
        return a<b; // prefer min allowed parallelism
    }
    void apply_active(std::size_t new_active) override {
        control_storage::apply_active(new_active);
        __TBB_ASSERT(my_active_value >= 1, nullptr);
        // -1 to take external thread into account
        threading_control::set_active_num_workers(my_active_value - 1);
    }
    std::size_t active_value() override {
        spin_mutex::scoped_lock lock(my_list_mutex); // protect my_list.empty() call
        if (my_list.empty()) {
            return default_value();
        }

        // non-zero, if market is active
        const std::size_t workers = threading_control::max_num_workers();
        // We can't exceed market's maximal number of workers.
        // +1 to take external thread into account
        return workers ? min(workers + 1, my_active_value) : my_active_value;
    }
public:
    std::size_t active_value_if_present() const {
        return !my_list.empty() ? my_active_value : 0;
    }
};

class alignas(max_nfs_size) stack_size_control : public control_storage {
    std::size_t default_value() const override {
#if _WIN32_WINNT >= 0x0602 /* _WIN32_WINNT_WIN8 */
        static auto ThreadStackSizeDefault = [] {
            ULONG_PTR hi, lo;
            GetCurrentThreadStackLimits(&lo, &hi);
            return hi - lo;
        }();
        return ThreadStackSizeDefault;
#else
        return ThreadStackSize;
#endif
    }
    void apply_active(std::size_t new_active) override {
        control_storage::apply_active(new_active);
#if __TBB_WIN8UI_SUPPORT && (_WIN32_WINNT < 0x0A00)
        __TBB_ASSERT( false, "For Windows 8 Store* apps we must not set stack size" );
#endif
    }
};

class alignas(max_nfs_size) terminate_on_exception_control : public control_storage {
    std::size_t default_value() const override {
        return 0;
    }
};

class alignas(max_nfs_size) lifetime_control : public control_storage {
    bool is_first_arg_preferred(std::size_t, std::size_t) const override {
        return false; // not interested
    }
    std::size_t default_value() const override {
        return 0;
    }
    void apply_active(std::size_t new_active) override {
        if (new_active == 1) {
            // reserve the market reference
            threading_control::register_lifetime_control();
        } else if (new_active == 0) { // new_active == 0
            threading_control::unregister_lifetime_control(/*blocking_terminate*/ false);
        }
        control_storage::apply_active(new_active);
    }

public:
    bool is_empty() {
        spin_mutex::scoped_lock lock(my_list_mutex);
        return my_list.empty();
    }
};

static allowed_parallelism_control allowed_parallelism_ctl;
static stack_size_control stack_size_ctl;
static terminate_on_exception_control terminate_on_exception_ctl;
static lifetime_control lifetime_ctl;
static control_storage *controls[] = {&allowed_parallelism_ctl, &stack_size_ctl, &terminate_on_exception_ctl, &lifetime_ctl};

void global_control_lock() {
    for (auto& ctl : controls) {
        ctl->my_list_mutex.lock();
    }
}

void global_control_unlock() {
    int N = std::distance(std::begin(controls), std::end(controls));
    for (int i = N - 1; i >= 0; --i) {
        controls[i]->my_list_mutex.unlock();
    }
}

std::size_t global_control_active_value_unsafe(d1::global_control::parameter param) {
    __TBB_ASSERT_RELEASE(param < d1::global_control::parameter_max, nullptr);
    return controls[param]->active_value_unsafe();
}

//! Comparator for a set of global_control objects
inline bool control_storage_comparator::operator()(const d1::global_control* lhs, const d1::global_control* rhs) const {
    __TBB_ASSERT_RELEASE(lhs->my_param < d1::global_control::parameter_max , nullptr);
    return lhs->my_value < rhs->my_value || (lhs->my_value == rhs->my_value && lhs < rhs);
}

bool terminate_on_exception() {
    return d1::global_control::active_value(d1::global_control::terminate_on_exception) == 1;
}

struct global_control_impl {
private:
    static bool erase_if_present(control_storage* const c, d1::global_control& gc) {
        auto it = c->my_list.find(&gc);
        if (it != c->my_list.end()) {
            c->my_list.erase(it);
            return true;
        }
        return false;
    }

public:

    static void create(d1::global_control& gc) {
        __TBB_ASSERT_RELEASE(gc.my_param < d1::global_control::parameter_max, nullptr);
        control_storage* const c = controls[gc.my_param];

        spin_mutex::scoped_lock lock(c->my_list_mutex);
        if (c->my_list.empty() || c->is_first_arg_preferred(gc.my_value, c->my_active_value)) {
            // to guarantee that apply_active() is called with current active value,
            // calls it here and in internal_destroy() under my_list_mutex
            c->apply_active(gc.my_value);
        }
        c->my_list.insert(&gc);
    }

    static void destroy(d1::global_control& gc) {
        __TBB_ASSERT_RELEASE(gc.my_param < d1::global_control::parameter_max, nullptr);
        control_storage* const c = controls[gc.my_param];
        // Concurrent reading and changing global parameter is possible.
        spin_mutex::scoped_lock lock(c->my_list_mutex);
        __TBB_ASSERT(gc.my_param == d1::global_control::scheduler_handle || !c->my_list.empty(), nullptr);
        std::size_t new_active = (std::size_t)(-1), old_active = c->my_active_value;

        if (!erase_if_present(c, gc)) {
            __TBB_ASSERT(gc.my_param == d1::global_control::scheduler_handle , nullptr);
            return;
        }
        if (c->my_list.empty()) {
            __TBB_ASSERT(new_active == (std::size_t) - 1, nullptr);
            new_active = c->default_value();
        } else {
            new_active = (*c->my_list.begin())->my_value;
        }
        if (new_active != old_active) {
            c->apply_active(new_active);
        }
    }

    static bool remove_and_check_if_empty(d1::global_control& gc) {
        __TBB_ASSERT_RELEASE(gc.my_param < d1::global_control::parameter_max, nullptr);
        control_storage* const c = controls[gc.my_param];

        spin_mutex::scoped_lock lock(c->my_list_mutex);
        __TBB_ASSERT(!c->my_list.empty(), nullptr);
        erase_if_present(c, gc);
        return c->my_list.empty();
    }
#if TBB_USE_ASSERT
    static bool is_present(d1::global_control& gc) {
        __TBB_ASSERT_RELEASE(gc.my_param < d1::global_control::parameter_max, nullptr);
        control_storage* const c = controls[gc.my_param];

        spin_mutex::scoped_lock lock(c->my_list_mutex);
        auto it = c->my_list.find(&gc);
        if (it != c->my_list.end()) {
            return true;
        }
        return false;
    }
#endif // TBB_USE_ASSERT
};

void __TBB_EXPORTED_FUNC create(d1::global_control& gc) {
    global_control_impl::create(gc);
}
void __TBB_EXPORTED_FUNC destroy(d1::global_control& gc) {
    global_control_impl::destroy(gc);
}

bool remove_and_check_if_empty(d1::global_control& gc) {
    return global_control_impl::remove_and_check_if_empty(gc);
}
#if TBB_USE_ASSERT
bool is_present(d1::global_control& gc) {
    return global_control_impl::is_present(gc);
}
#endif // TBB_USE_ASSERT
std::size_t __TBB_EXPORTED_FUNC global_control_active_value(int param) {
    __TBB_ASSERT_RELEASE(param < d1::global_control::parameter_max, nullptr);
    return controls[param]->active_value();
}

} // namespace r1
} // namespace detail
} // namespace tbb
