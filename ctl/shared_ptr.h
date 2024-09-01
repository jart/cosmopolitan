// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_SHARED_PTR_H_
#define CTL_SHARED_PTR_H_

#include "exception.h"
#include "is_convertible.h"
#include "remove_extent.h"
#include "unique_ptr.h"

// XXX currently needed to use placement-new syntax (move to cxx.inc?)
void*
operator new(size_t, void*) noexcept;

namespace ctl {

class bad_weak_ptr : public exception
{
  public:
    const char* what() const noexcept override
    {
        return "ctl::bad_weak_ptr";
    }
};

namespace __ {

template<typename T>
struct ptr_ref
{
    using type = T&;
};

template<>
struct ptr_ref<void>
{
    using type = void;
};

static inline __attribute__((always_inline)) void
incref(size_t* r) noexcept
{
#ifdef NDEBUG
    __atomic_fetch_add(r, 1, __ATOMIC_RELAXED);
#else
    ssize_t refs = __atomic_fetch_add(r, 1, __ATOMIC_RELAXED);
    if (refs < 0)
        __builtin_trap();
#endif
}

static inline __attribute__((always_inline)) bool
decref(size_t* r) noexcept
{
    if (!__atomic_fetch_sub(r, 1, __ATOMIC_RELEASE)) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        return true;
    }
    return false;
}

class shared_ref
{
  public:
    constexpr shared_ref() noexcept = default;
    shared_ref(const shared_ref&) = delete;
    shared_ref& operator=(const shared_ref&) = delete;

    virtual ~shared_ref() = default;

    void keep_shared() noexcept
    {
        incref(&shared);
    }

    void drop_shared() noexcept
    {
        if (decref(&shared)) {
            dispose();
            drop_weak();
        }
    }

    void keep_weak() noexcept
    {
        incref(&weak);
    }

    void drop_weak() noexcept
    {
        if (decref(&weak)) {
            delete this;
        }
    }

    size_t use_count() const noexcept
    {
        return shared + 1;
    }

    size_t weak_count() const noexcept
    {
        return weak;
    }

  private:
    virtual void dispose() noexcept = 0;

    size_t shared = 0;
    size_t weak = 0;
};

template<typename T, typename D>
class shared_pointer : public shared_ref
{
  public:
    static shared_pointer* make(T* const p, D d)
    {
        return make(unique_ptr<T, D>(p, move(d)));
    }

    static shared_pointer* make(unique_ptr<T, D> p)
    {
        return new shared_pointer(p.release(), move(p.get_deleter()));
    }

  private:
    shared_pointer(T* const p, D d) noexcept : p(p), d(move(d))
    {
    }

    void dispose() noexcept override
    {
        move(d)(p);
    }

    T* const p;
    [[no_unique_address]] D d;
};

template<typename T>
class shared_emplace : public shared_ref
{
  public:
    union
    {
        T t;
    };

    template<typename... Args>
    void construct(Args&&... args)
    {
        ::new (&t) T(forward<Args>(args)...);
    }

    static unique_ptr<shared_emplace> make()
    {
        return unique_ptr(new shared_emplace());
    }

  private:
    explicit constexpr shared_emplace() noexcept = default;

    void dispose() noexcept override
    {
        t.~T();
    }
};

template<typename T, typename U>
concept shared_ptr_compatible = is_convertible_v<U*, T*>;

} // namespace __

template<typename T>
class weak_ptr;

template<typename T>
class shared_ptr
{
  public:
    using element_type = remove_extent_t<T>;
    using weak_type = weak_ptr<T>;

    constexpr shared_ptr() noexcept = default;
    constexpr shared_ptr(nullptr_t) noexcept
    {
    }

    template<typename U>
        requires __::shared_ptr_compatible<T, U>
    explicit shared_ptr(U* const p) : shared_ptr(p, default_delete<U>())
    {
    }

    template<typename U, typename D>
        requires __::shared_ptr_compatible<T, U>
    shared_ptr(U* const p, D d)
      : p(p), rc(__::shared_pointer<U, D>::make(p, move(d)))
    {
    }

    template<typename U>
    shared_ptr(const shared_ptr<U>& r, element_type* p) noexcept
      : p(p), rc(r.rc)
    {
        if (rc)
            rc->keep_shared();
    }

    template<typename U>
    shared_ptr(shared_ptr<U>&& r, element_type* p) noexcept : p(p), rc(r.rc)
    {
        r.p = nullptr;
        r.rc = nullptr;
    }

    template<typename U>
        requires __::shared_ptr_compatible<T, U>
    shared_ptr(const shared_ptr<U>& r) noexcept : p(r.p), rc(r.rc)
    {
        if (rc)
            rc->keep_shared();
    }

    template<typename U>
        requires __::shared_ptr_compatible<T, U>
    shared_ptr(shared_ptr<U>&& r) noexcept : p(r.p), rc(r.rc)
    {
        r.p = nullptr;
        r.rc = nullptr;
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
        requires __::shared_ptr_compatible<T, U>
    explicit shared_ptr(const weak_ptr<U>& r) : p(r.p), rc(r.rc)
    {
        if (r.expired()) {
            throw bad_weak_ptr();
        }
        rc->keep_shared();
    }

    template<typename U, typename D>
        requires __::shared_ptr_compatible<T, U>
    shared_ptr(unique_ptr<U, D>&& r)
      : p(r.p), rc(__::shared_pointer<U, D>::make(move(r)))
    {
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
        requires __::shared_ptr_compatible<T, U>
    shared_ptr& operator=(shared_ptr<U> r) noexcept
    {
        shared_ptr<T>(move(r)).swap(*this);
        return *this;
    }

    void reset() noexcept
    {
        shared_ptr().swap(*this);
    }

    template<typename U>
        requires __::shared_ptr_compatible<T, U>
    void reset(U* const p2)
    {
        shared_ptr<T>(p2).swap(*this);
    }

    template<typename U, typename D>
        requires __::shared_ptr_compatible<T, U>
    void reset(U* const p2, D d)
    {
        shared_ptr<T>(p2, d).swap(*this);
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

    typename __::ptr_ref<T>::type operator*() const noexcept
    {
        if (!p)
            __builtin_trap();
        return *p;
    }

    T* operator->() const noexcept
    {
        if (!p)
            __builtin_trap();
        return p;
    }

    long use_count() const noexcept
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
        return rc < r.rc;
    }

    template<typename U>
    bool owner_before(const weak_ptr<U>& r) const noexcept
    {
        return !r.owner_before(*this);
    }

  private:
    template<typename U>
    friend class weak_ptr;

    template<typename U>
    friend class shared_ptr;

    template<typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&... args);

    element_type* p = nullptr;
    __::shared_ref* rc = nullptr;
};

template<typename T>
class weak_ptr
{
  public:
    using element_type = remove_extent_t<T>;

    constexpr weak_ptr() noexcept = default;

    template<typename U>
        requires __::shared_ptr_compatible<T, U>
    weak_ptr(const shared_ptr<U>& r) noexcept : p(r.p), rc(r.rc)
    {
        if (rc)
            rc->keep_weak();
    }

    ~weak_ptr()
    {
        if (rc)
            rc->drop_weak();
    }

    long use_count() const noexcept
    {
        return rc ? rc->use_count() : 0;
    }

    bool expired() const noexcept
    {
        return !use_count();
    }

    void reset() noexcept
    {
        weak_ptr().swap(*this);
    }

    void swap(weak_ptr& r) noexcept
    {
        using ctl::swap;
        swap(p, r.p);
        swap(rc, r.rc);
    }

    shared_ptr<T> lock() const noexcept
    {
        if (expired())
            return nullptr;
        shared_ptr<T> r;
        r.p = p;
        r.rc = rc;
        if (rc)
            rc->keep_shared();
        return r;
    }

    template<typename U>
    bool owner_before(const weak_ptr<U>& r) const noexcept
    {
        return rc < r.rc;
    }

    template<typename U>
    bool owner_before(const shared_ptr<U>& r) const noexcept
    {
        return rc < r.rc;
    }

  private:
    template<typename U>
    friend class shared_ptr;

    element_type* p = nullptr;
    __::shared_ref* rc = nullptr;
};

template<typename T, typename... Args>
shared_ptr<T>
make_shared(Args&&... args)
{
    auto rc = __::shared_emplace<T>::make();
    rc->construct(forward<Args>(args)...);
    shared_ptr<T> r;
    r.p = &rc->t;
    r.rc = rc.release();
    return r;
}

} // namespace ctl

#endif // CTL_SHARED_PTR_H_
