// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=c++ ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_OPTIONAL_H_
#define COSMOPOLITAN_CTL_OPTIONAL_H_
#include <__utility/forward.h>
#include <__utility/move.h>
#include <__utility/swap.h>

template<typename T>
class Optional
{
  public:
    using value_type = T;

    ~Optional() = default;

    Optional() noexcept : present_(false)
    {
    }

    Optional(const T& value) : present_(true), value_(value)
    {
    }

    Optional(T&& value) : present_(true), value_(std::move(value))
    {
    }

    Optional(const Optional& other) : present_(other.present_)
    {
        if (present_)
            new (&value_) T(other.value_);
    }

    Optional(Optional&& other) noexcept : present_(other.present_)
    {
        if (present_)
            value_ = std::move(other.value_);
    }

    Optional& operator=(const Optional& other)
    {
        if (this != &other) {
            present_ = other.present_;
            if (present_)
                value_ = other.value_;
        }
        return *this;
    }

    Optional& operator=(Optional&& other) noexcept
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

    void swap(Optional& other) noexcept
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

#endif // COSMOPOLITAN_CTL_OPTIONAL_H_
