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

    static shared_pointer* make(T* p)
    {
        return new shared_pointer(p);
    }

  private:
    shared_pointer(T* p) noexcept : p(p)
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

template<typename T>
class shared_ptr
{
  public:
    constexpr shared_ptr(nullptr_t = nullptr) noexcept
      : p(nullptr), ctl(nullptr)
    {
    }
    shared_ptr(T* const p2)
    {
        auto hold = ctl::unique_ptr(p2);
        ctl = __::shared_pointer<T>::make(p2);
        p = hold.release();
    }

    ~shared_ptr()
    {
        if (ctl)
            ctl->release_shared();
    }

  private:
    T* p;
    __::shared_control* ctl;
};

} // namespace ctl

#endif // COSMOPOLITAN_CTL_SHARED_PTR_H_
