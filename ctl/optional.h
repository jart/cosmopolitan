// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=c++ ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_OPTIONAL_H_
#define COSMOPOLITAN_CTL_OPTIONAL_H_
#include <__utility/forward.h>
#include <__utility/move.h>
#include <__utility/swap.h>

namespace ctl {

template<typename T>
class optional
{
  public:
    using value_type = T;

    ~optional() = default;

    optional() noexcept : present_(false)
    {
    }

    optional(const T& value) : present_(true), value_(value)
    {
    }

    optional(T&& value) : present_(true), value_(std::move(value))
    {
    }

    optional(const optional& other) : present_(other.present_)
    {
        if (present_)
            new (&value_) T(other.value_);
    }

    optional(optional&& other) noexcept : present_(other.present_)
    {
        if (present_)
            value_ = std::move(other.value_);
    }

    optional& operator=(const optional& other)
    {
        if (this != &other) {
            present_ = other.present_;
            if (present_)
                value_ = other.value_;
        }
        return *this;
    }

    optional& operator=(optional&& other) noexcept
    {
        if (this != &other) {
            present_ = other.present_;
            if (present_)
                value_ = std::move(other.value_);
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
        return std::move(value_);
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
        present_ = true;
        value_ = T(std::forward<Args>(args)...);
    }

    void swap(optional& other) noexcept
    {
        if (present_ && other.present_) {
            std::swap(value_, other.value_);
        } else if (present_) {
            other.emplace(std::move(value_));
            reset();
        } else if (other.present_) {
            emplace(std::move(other.value_));
            other.reset();
        }
    }

  private:
    bool present_;
    T value_;
};

} // namespace ctl

#endif // COSMOPOLITAN_CTL_OPTIONAL_H_
