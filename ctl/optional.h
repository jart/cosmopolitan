// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_OPTIONAL_H_
#define COSMOPOLITAN_CTL_OPTIONAL_H_
#include "new.h"
#include "utility.h"

namespace ctl {

template<typename T>
class optional
{
  public:
    using value_type = T;

    ~optional()
    {
        if (present_)
            value_.~T();
    }

    optional() noexcept : present_(false)
    {
    }

    optional(const T& value) : present_(true)
    {
        new (&value_) T(value);
    }

    optional(T&& value) : present_(true)
    {
        new (&value_) T(ctl::move(value));
    }

    optional(const optional& other) : present_(other.present_)
    {
        if (other.present_)
            new (&value_) T(other.value_);
    }

    optional(optional&& other) noexcept : present_(other.present_)
    {
        if (other.present_)
            new (&value_) T(ctl::move(other.value_));
    }

    optional& operator=(const optional& other)
    {
        if (this != &other) {
            reset();
            if (other.present_)
                new (&value_) T(other.value_);
            present_ = other.present_;
        }
        return *this;
    }

    optional& operator=(optional&& other) noexcept
    {
        if (this != &other) {
            reset();
            if (other.present_)
                new (&value_) T(ctl::move(other.value_));
            present_ = other.present_;
        }
        return *this;
    }

    T& value() &
    {
        if (!present_)
            __builtin_trap();
        return value_;
    }

    const T& value() const&
    {
        if (!present_)
            __builtin_trap();
        return value_;
    }

    T&& value() &&
    {
        if (!present_)
            __builtin_trap();
        return ctl::move(value_);
    }

    explicit operator bool() const noexcept
    {
        return present_;
    }

    bool has_value() const noexcept
    {
        return present_;
    }

    void reset() noexcept
    {
        if (present_) {
            value_.~T();
            present_ = false;
        }
    }

    template<typename... Args>
    void emplace(Args&&... args)
    {
        reset();
        present_ = true;
        new (&value_) T(ctl::forward<Args>(args)...);
    }

    void swap(optional& other) noexcept
    {
        using ctl::swap;
        if (present_ && other.present_) {
            swap(value_, other.value_);
        } else if (present_) {
            other.emplace(ctl::move(value_));
            reset();
        } else if (other.present_) {
            emplace(ctl::move(other.value_));
            other.reset();
        }
    }

  private:
    union
    {
        T value_;
    };
    bool present_;
};

} // namespace ctl

#endif // COSMOPOLITAN_CTL_OPTIONAL_H_
