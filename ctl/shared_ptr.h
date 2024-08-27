// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_SHARED_PTR_H_
#define COSMOPOLITAN_CTL_SHARED_PTR_H_
#include "enable_if.h"
#include "is_void.h"
#include "new.h"
#include "unique_ptr.h"

// TODO(mrdomino): move
#ifndef __cplusplus
#define _CTL_ATOMIC(x) _Atomic(x)
#else
#define _CTL_ATOMIC(x) x
#endif

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
    _CTL_ATOMIC(size_t) shared;
    _CTL_ATOMIC(size_t) weak;

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
        return unique_ptr(new shared_emplace());
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

template<typename T>
class weak_ptr;

template<typename T>
class shared_ptr
{
  public:
    using element_type = T; // TODO(mrdomino): remove extent
    using weak_type = weak_ptr<T>;

    constexpr shared_ptr() noexcept : p(nullptr), rc(nullptr)
    {
    }

    constexpr shared_ptr(nullptr_t) noexcept : p(nullptr), rc(nullptr)
    {
    }

    template<typename U>
        requires is_convertible_v<U, T>
    explicit shared_ptr(U* const p)
      : p(p)
      , rc(__::shared_pointer<T, default_delete<T>>::make(p,
                                                          default_delete<T>()))
    {
    }

    template<typename U, typename D>
        requires is_convertible_v<U, T>
    shared_ptr(U* const p, D d)
      : p(p), rc(__::shared_pointer<T, D>::make(p, ctl::move(d)))
    {
    }

    template<typename D>
    shared_ptr(nullptr_t, D d)
      : p(nullptr), rc(__::shared_pointer<T, D>::make(nullptr, ctl::move(d)))
    {
    }

    // TODO(mrdomino): allocators

    template<typename U>
        requires is_convertible_v<U, T>
    shared_ptr(const shared_ptr<U>& r, element_type* const p) noexcept
      : p(p), rc(r.rc)
    {
        if (rc)
            rc->keep_shared();
    }

    template<typename U>
        requires is_convertible_v<U, T>
    shared_ptr(shared_ptr<U>&& r, element_type* const p) noexcept
      : p(p), rc(r.rc)
    {
        r.p = nullptr;
        r.rc = nullptr;
    }

    shared_ptr(const shared_ptr& r) noexcept : p(r.p), rc(r.rc)
    {
        if (rc)
            rc->keep_shared();
    }

    template<typename U>
        requires is_convertible_v<U, T>
    shared_ptr(const shared_ptr<U>& r) noexcept : p(r.p), rc(r.rc)
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
        requires is_convertible_v<U, T>
    shared_ptr(shared_ptr<U>&& r) noexcept : p(r.p), rc(r.rc)
    {
        r.p = nullptr;
        r.rc = nullptr;
    }

    template<typename U>
        requires is_convertible_v<U, T>
    explicit shared_ptr(const weak_ptr<U>& r) : p(r.p), rc(r.rc)
    {
        // XXX keep_shared?
        if (r.expired())
            // XXX throw bad_weak_ptr?
            __builtin_trap();
    }

    template<typename U, typename D>
        requires is_convertible_v<U, T>
    shared_ptr(unique_ptr<U, D> r)
      : p(r.get()), rc(__::shared_pointer<U, D>::make(r.get(), r.get_deleter()))
    {
        r.release();
    }

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

    template<typename U>
        requires is_convertible_v<U, T>
    shared_ptr& operator=(shared_ptr<U> r) noexcept
    {
        swap(p, r.p);
        swap(rc, r.rc);
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

    template<typename U = T>
    typename enable_if<!is_void_v<U>, U&>::type operator*() const noexcept
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

    template<typename U = T>
    typename enable_if<!is_void_v<U>, U&>::type operator[](ptrdiff_t i) const
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
    static shared_ptr<T> make_from_rc(T* const p,
                                      __::shared_control* rc) noexcept
    {
        shared_ptr<T> r;
        r.p = p;
        r.rc = rc;
        return r;
    }

    template<typename U>
    friend class shared_ptr;

    template<typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&... args);

    template<typename U>
    friend class weak_ptr;

    T* p;
    __::shared_control* rc;
};

template<typename T>
class weak_ptr
{
  public:
    using element_type = T; // TODO(mrdomino): remove extent

    constexpr weak_ptr() noexcept : p(nullptr), rc(nullptr)
    {
    }

    weak_ptr(const weak_ptr& r) noexcept : p(r.p), rc(r.rc)
    {
        if (rc)
            rc->keep_weak();
    }

    template<typename U>
        requires is_convertible_v<U, T>
    weak_ptr(const weak_ptr<U>& r) noexcept : p(r.p), rc(r.rc)
    {
        if (rc)
            rc->keep_weak();
    }

    template<typename U>
        requires is_convertible_v<U, T>
    weak_ptr(const shared_ptr<U>& r) noexcept : p(r.p), rc(r.rc)
    {
        if (rc)
            rc->keep_weak();
    }

    weak_ptr(weak_ptr&& r) noexcept : p(r.p), rc(r.rc)
    {
        r.p = nullptr;
        r.rc = nullptr;
    }

    template<typename U>
        requires is_convertible_v<U, T>
    weak_ptr(weak_ptr<U>&& r) noexcept : p(r.p), rc(r.rc)
    {
        r.p = nullptr;
        r.rc = nullptr;
    }

    ~weak_ptr()
    {
        if (rc)
            rc->drop_weak();
    }

    weak_ptr& operator=(weak_ptr r) noexcept
    {
        swap(r);
        return *this;
    }

    template<typename U>
        requires is_convertible_v<U, T>
    weak_ptr& operator=(weak_ptr<U> r) noexcept
    {
        swap(p, r.p);
        swap(rc, r.rc);
        return *this;
    }

    void reset() noexcept
    {
        if (rc)
            rc->drop_shared();
        p = nullptr;
        rc = nullptr;
    }

    void swap(weak_ptr& r) noexcept
    {
        using ctl::swap;
        swap(p, r.p);
        swap(rc, r.rc);
    }

    size_t use_count() const noexcept
    {
        return rc ? rc->use_count() : 0;
    }

    bool expired() const noexcept
    {
        return use_count() == 0;
    }

  private:
    T* p;
    __::shared_control* rc;
};

template<typename T, typename... Args>
shared_ptr<T>
make_shared(Args&&... args)
{
    auto rc = __::shared_emplace<T>::make_unique();
    rc->construct(ctl::forward<Args>(args)...);
    auto r = shared_ptr<T>::make_from_rc(&rc->t, rc.get());
    rc.release();
    return r;
}

// TODO(mrdomino): non-member functions (make_shared et al)
// TODO(mrdomino): weak_ptr

// TODO(someday): std::atomic<std::shared_ptr>

} // namespace ctl

#endif // COSMOPOLITAN_CTL_SHARED_PTR_H_
