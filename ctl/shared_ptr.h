// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_SHARED_PTR_H_
#define CTL_SHARED_PTR_H_

#include "exception.h"
#include "is_base_of.h"
#include "is_constructible.h"
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

    ~shared_emplace() override
    {
    }

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
    explicit constexpr shared_emplace() noexcept
    {
    }

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
    shared_ptr(U*, D);

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

    template<typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&...);

    element_type* p = nullptr;
    __::shared_ref* rc = nullptr;
};

template<typename T>
class enable_shared_from_this
{
  public:
    shared_ptr<T> shared_from_this()
    {
        return shared_ptr<T>(weak_this);
    }
    shared_ptr<T const> shared_from_this() const
    {
        return shared_ptr<T>(weak_this);
    }

    weak_ptr<T> weak_from_this()
    {
        return weak_this;
    }
    weak_ptr<T const> weak_from_this() const
    {
        return weak_this;
    }

  protected:
    constexpr enable_shared_from_this() noexcept = default;
    enable_shared_from_this(const enable_shared_from_this& r) noexcept
    {
    }
    ~enable_shared_from_this() = default;

    enable_shared_from_this& operator=(
      const enable_shared_from_this& r) noexcept
    {
        return *this;
    }

  private:
    template<typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&...);

    template<typename U>
    friend class shared_ptr;

    weak_ptr<T> weak_this;
};

template<typename T>
template<typename U, typename D>
    requires __::shared_ptr_compatible<T, U>
shared_ptr<T>::shared_ptr(U* const p, D d)
  : p(p), rc(__::shared_pointer<U, D>::make(p, move(d)))
{
    if constexpr (is_base_of_v<enable_shared_from_this<U>, U>) {
        p->weak_this = *this;
    }
}

// Our make_shared supports passing a weak self reference as the first parameter
// to your constructor, e.g.:
//
//     struct Tree : ctl::weak_self_base
//     {
//         ctl::shared_ptr<Tree> l, r;
//         ctl::weak_ptr<Tree> parent;
//         Tree(weak_ptr<Tree> const& self, auto&& l2, auto&& r2)
//           : l(ctl::forward<decltype(l2)>(l2)),
//             r(ctl::forward<decltype(r2)>(r2))
//         {
//             if (l) l->parent = self;
//             if (r) r->parent = self;
//         }
//     };
//
//     int main() {
//         auto t = ctl::make_shared<Tree>(
//             ctl::make_shared<Tree>(nullptr, nullptr), nullptr);
//         return t->l->parent.lock().get() == t.get() ? 0 : 1;
//     }
//
// As shown, passing the parameter at object construction time lets you complete
// object construction without needing a separate Init method. But because we go
// off spec as far as the STL is concerned, there is a potential ambiguity where
// you might have a constructor with a weak_ptr first parameter that is intended
// to be something other than a self-reference. So this feature is opt-in by way
// of inheriting from the following struct.
struct weak_self_base
{};

template<typename T, typename... Args>
shared_ptr<T>
make_shared(Args&&... args)
{
    unique_ptr rc = __::shared_emplace<T>::make();
    if constexpr (is_base_of_v<weak_self_base, T> &&
                  is_constructible_v<T, const weak_ptr<T>&, Args...>) {
        // A __::shared_ref has a virtual weak reference that is owned by all of
        // the shared references. We can avoid some unnecessary refcount changes
        // by "borrowing" that reference and passing it to the constructor, then
        // promoting it to a shared reference by swapping it with the shared_ptr
        // that we return.
        weak_ptr<T> w;
        w.p = &rc->t;
        w.rc = rc.get();
        try {
            rc->construct(const_cast<const weak_ptr<T>&>(w),
                          forward<Args>(args)...);
        } catch (...) {
            w.p = nullptr;
            w.rc = nullptr;
            throw;
        }
        rc.release();
        shared_ptr<T> r;
        swap(r.p, w.p);
        swap(r.rc, w.rc);
        return r;
    } else {
        rc->construct(forward<Args>(args)...);
        shared_ptr<T> r;
        r.p = &rc->t;
        r.rc = rc.release();
        if constexpr (is_base_of_v<enable_shared_from_this<T>, T>) {
            r->weak_this = r;
        }
        return r;
    }
}

} // namespace ctl

#endif // CTL_SHARED_PTR_H_
