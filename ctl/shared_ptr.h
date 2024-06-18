// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_SHARED_PTR_H_
#define COSMOPOLITAN_CTL_SHARED_PTR_H_
#include "unique_ptr.h"

namespace ctl {

// TODO(mrdomino): move
struct exception
{
    virtual const char* what() const noexcept
    {
        return "exception";
    }
};

namespace __ {

struct shared_control
{
    _Atomic(size_t) shared;
    _Atomic(size_t) weak;

    constexpr shared_control() noexcept : shared(0), weak(0)
    {
    }

    shared_control(const shared_control&) = delete;

    virtual ~shared_control()
    {
    }

    void add_shared() noexcept;
    void release_shared() noexcept;
    void add_weak() noexcept;
    void release_weak() noexcept;
    size_t use_count() const noexcept;
    size_t weak_count() const noexcept;

  private:
    virtual void on_zero_shared() noexcept = 0;
    virtual void on_zero_weak() noexcept = 0;
};

template<typename T>
struct shared_pointer : shared_control
{
    T* p;

    static shared_pointer* make(T* const p)
    {
        auto p2 = unique_ptr(p);
        auto r = new shared_pointer(p2.release());
        return r;
    }

  private:
    explicit constexpr shared_pointer(T* const p) noexcept : p(p)
    {
    }

    void on_zero_shared() noexcept override
    {
        delete p;
    }

    void on_zero_weak() noexcept override
    {
        delete this;
    }
};

} // namespace __

struct bad_weak_ptr : ctl::exception
{
    const char* what() const noexcept override
    {
        return "bad_weak_ptr";
    }
};

// TODO(mrdomino): typename D = default_delete<T>
template<typename T>
class shared_ptr
{
  public:
    using element_type = T; // TODO(mrdomino): remove extent?

    constexpr shared_ptr(nullptr_t = nullptr) noexcept
      : p(nullptr), ctl(nullptr)
    {
    }

    explicit shared_ptr(auto* const p) : p(p), ctl(__::shared_pointer<T>::make(p))
    {
    }

    shared_ptr(const shared_ptr& r) noexcept : p(r.p), ctl(r.ctl)
    {
        if (ctl)
            ctl->add_shared();
    }

    shared_ptr(shared_ptr&& r) noexcept : p(r.p), ctl(r.ctl)
    {
        r.p = nullptr;
        r.ctl = nullptr;
    }

    template <typename U>
    shared_ptr(const shared_ptr<U>& r, T* const p) noexcept : p(p), ctl(r.ctl)
    {
        if (ctl)
            ctl->add_shared();
    }

    template <typename U>
    shared_ptr(shared_ptr<U>&& r, T* const p) noexcept : p(p), ctl(r.ctl)
    {
        r.p = nullptr;
        r.ctl = nullptr;
    }

    // TODO(mrdomino): moar ctors

    ~shared_ptr()
    {
        if (ctl)
            ctl->release_shared();
    }

    shared_ptr& operator=(shared_ptr r) noexcept
    {
        swap(r);
        return *this;
    }

    void reset() noexcept
    {
        if (ctl)
            ctl->release_shared();
        p = nullptr;
        ctl = nullptr;
    }

    void reset(auto* const p2)
    {
        shared_ptr<T>(p2).swap(*this);
    }

    void swap(shared_ptr& r) noexcept
    {
        using std::swap;
        swap(p, r.p);
        swap(ctl, r.ctl);
    }

    element_type* get() const noexcept
    {
        return p;
    }

    // TODO(mrdomino): fix for shared_ptr<void>
    T& operator*() const noexcept
    {
        if (!p)
            __builtin_trap();
        return *p;
    }

    // TODO(mrdomino): fix for shared_ptr<T[]>
    T* operator->() const noexcept
    {
        if (!p)
            __builtin_trap();
        return *p;
    }

    element_type& operator[](ptrdiff_t i) const
    {
        return *(p + i);
    }

    size_t use_count() const noexcept
    {
        return ctl ? ctl->use_count() : 0;
    }

    explicit operator bool() const noexcept
    {
        return p;
    }

    template <typename U>
    bool owner_before(const shared_ptr<U>& r) const noexcept
    {
        return p < r.p;
    }

    // TODO(mrdomino): owner_before(weak_ptr const&)

  private:
    T* p;
    __::shared_control* ctl;
};

// TODO(mrdomino): non-member functions (make_shared et al)
// TODO(mrdomino): weak_ptr

// TODO(someday): std::atomic<std::shared_ptr>

} // namespace ctl

#endif // COSMOPOLITAN_CTL_SHARED_PTR_H_
