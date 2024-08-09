// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_SHARED_PTR_H_
#define COSMOPOLITAN_CTL_SHARED_PTR_H_
#include "new.h"
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

    void keep_shared() noexcept;
    void drop_shared() noexcept;
    void keep_weak() noexcept;
    void drop_weak() noexcept;
    size_t use_count() const noexcept;
    size_t weak_count() const noexcept;

  private:
    virtual void on_zero_shared() noexcept = 0;
    virtual void on_zero_weak() noexcept = 0;
};

template<typename T, typename D>
struct shared_pointer : shared_control
{
    T* p;
    [[no_unique_address]] D d;

    static shared_pointer* make(T* const p, auto&& d)
    {
        auto p2 = unique_ptr(p);
        auto r = new shared_pointer(p2.release(), ctl::forward<decltype(d)>(d));
        return r;
    }

  private:
    explicit constexpr shared_pointer(T* const p, auto&& d) noexcept
      : p(p), d(ctl::forward<decltype(d)>(d))
    {
    }

    void on_zero_shared() noexcept override
    {
        ctl::move(d)(p);
    }

    void on_zero_weak() noexcept override
    {
        delete this;
    }
};

template<typename T>
struct shared_emplace : shared_control
{
    union
    {
        T t;
    };

    template<typename... Args>
    void construct(Args&&... args)
    {
        ::new (&t) T(ctl::forward<Args>(args)...);
    }

    static unique_ptr<shared_emplace> make_unique()
    {
        return new shared_emplace();
    }

  private:
    explicit constexpr shared_emplace() noexcept
    {
    }

    void on_zero_shared() noexcept override
    {
        t.~T();
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

template<typename T, typename D = default_delete<T>>
class shared_ptr
{
  public:
    using element_type = T; // TODO(mrdomino): remove extent?
    using deleter_type = D;

    constexpr shared_ptr(nullptr_t = nullptr) noexcept : p(nullptr), rc(nullptr)
    {
    }

    explicit shared_ptr(auto* const p)
      : p(p), rc(__::shared_pointer<T, D>::make(p, D()))
    {
    }

    shared_ptr(const shared_ptr& r) noexcept : p(r.p), rc(r.rc)
    {
        if (rc)
            rc->keep_shared();
    }

    shared_ptr(shared_ptr&& r) noexcept : p(r.p), rc(r.rc)
    {
        r.p = nullptr;
        r.rc = nullptr;
    }

    template<typename U>
    shared_ptr(const shared_ptr<U>& r, T* const p) noexcept : p(p), rc(r.rc)
    {
        if (rc)
            rc->keep_shared();
    }

    template<typename U>
    shared_ptr(shared_ptr<U>&& r, T* const p) noexcept : p(p), rc(r.rc)
    {
        r.p = nullptr;
        r.rc = nullptr;
    }

    // TODO(mrdomino): moar ctors

    ~shared_ptr()
    {
        if (rc)
            rc->drop_shared();
    }

    shared_ptr& operator=(shared_ptr r) noexcept
    {
        swap(r);
        return *this;
    }

    void reset() noexcept
    {
        if (rc)
            rc->drop_shared();
        p = nullptr;
        rc = nullptr;
    }

    void reset(auto* const p2)
    {
        shared_ptr<T>(p2).swap(*this);
    }

    void swap(shared_ptr& r) noexcept
    {
        using ctl::swap;
        swap(p, r.p);
        swap(rc, r.rc);
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
        return rc ? rc->use_count() : 0;
    }

    explicit operator bool() const noexcept
    {
        return p;
    }

    template<typename U>
    bool owner_before(const shared_ptr<U>& r) const noexcept
    {
        return p < r.p;
    }

    // TODO(mrdomino): owner_before(weak_ptr const&)

  private:
    constexpr shared_ptr(T* const p, __::shared_control* rc) noexcept
      : p(p), rc(rc)
    {
    }

    template<typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&... args);

    T* p;
    __::shared_control* rc;
};

template<typename T, typename... Args>
shared_ptr<T>
make_shared(Args&&... args)
{
    auto rc = __::shared_emplace<T>::make_unique();
    rc->construct(ctl::forward<Args>(args)...);
    auto r = shared_ptr<T>(&rc->t, rc.get());
    rc.release();
    return r;
}

// TODO(mrdomino): non-member functions (make_shared et al)
// TODO(mrdomino): weak_ptr

// TODO(someday): std::atomic<std::shared_ptr>

} // namespace ctl

#endif // COSMOPOLITAN_CTL_SHARED_PTR_H_
