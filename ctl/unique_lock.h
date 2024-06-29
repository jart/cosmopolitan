// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_UNIQUE_LOCK_H_
#define CTL_UNIQUE_LOCK_H_
#include "mutex.h"
#include "utility.h"

namespace ctl {

struct defer_lock_t
{
    explicit defer_lock_t() = default;
};

struct try_to_lock_t
{
    explicit try_to_lock_t() = default;
};

struct adopt_lock_t
{
    explicit adopt_lock_t() = default;
};

inline constexpr defer_lock_t defer_lock{};
inline constexpr try_to_lock_t try_to_lock{};
inline constexpr adopt_lock_t adopt_lock{};

class unique_lock
{
  public:
    unique_lock() noexcept : mutex_(nullptr), owns_lock_(false)
    {
    }

    explicit unique_lock(ctl::mutex& m) : mutex_(&m), owns_lock_(false)
    {
        lock();
    }

    unique_lock(ctl::mutex& m, defer_lock_t) noexcept
      : mutex_(&m), owns_lock_(false)
    {
    }

    unique_lock(ctl::mutex& m, try_to_lock_t)
      : mutex_(&m), owns_lock_(mutex_->try_lock())
    {
    }

    unique_lock(ctl::mutex& m, adopt_lock_t) : mutex_(&m), owns_lock_(true)
    {
    }

    ~unique_lock()
    {
        if (owns_lock_)
            mutex_->unlock();
    }

    unique_lock(const unique_lock&) = delete;
    unique_lock& operator=(const unique_lock&) = delete;

    unique_lock(unique_lock&& other) noexcept
      : mutex_(other.mutex_), owns_lock_(other.owns_lock_)
    {
        other.mutex_ = nullptr;
        other.owns_lock_ = false;
    }

    unique_lock& operator=(unique_lock&& other) noexcept
    {
        if (owns_lock_)
            mutex_->unlock();
        mutex_ = other.mutex_;
        owns_lock_ = other.owns_lock_;
        other.mutex_ = nullptr;
        other.owns_lock_ = false;
        return *this;
    }

    void lock()
    {
        if (!mutex_)
            __builtin_trap();
        if (owns_lock_)
            __builtin_trap();
        mutex_->lock();
        owns_lock_ = true;
    }

    bool try_lock()
    {
        if (!mutex_)
            __builtin_trap();
        if (owns_lock_)
            __builtin_trap();
        owns_lock_ = mutex_->try_lock();
        return owns_lock_;
    }

    void unlock()
    {
        if (!owns_lock_)
            __builtin_trap();
        mutex_->unlock();
        owns_lock_ = false;
    }

    void swap(unique_lock& other) noexcept
    {
        using ctl::swap;
        swap(mutex_, other.mutex_);
        swap(owns_lock_, other.owns_lock_);
    }

    ctl::mutex* release() noexcept
    {
        ctl::mutex* result = mutex_;
        mutex_ = nullptr;
        owns_lock_ = false;
        return result;
    }

    bool owns_lock() const noexcept
    {
        return owns_lock_;
    }

    explicit operator bool() const noexcept
    {
        return owns_lock_;
    }

    ctl::mutex* mutex() const noexcept
    {
        return mutex_;
    }

  private:
    ctl::mutex* mutex_;
    bool owns_lock_;
};

} // namespace ctl

#endif // CTL_UNIQUE_LOCK_H_
