// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_UNIQUE_PTR_H_
#define COSMOPOLITAN_CTL_UNIQUE_PTR_H_
#include <__utility/forward.h>
#include <__utility/move.h>
#include <__utility/swap.h>

namespace ctl {

template <typename T>
using default_delete = decltype([](T* p) {
    delete p;
});

template<typename T, typename D = default_delete<T>>
struct unique_ptr
{
    T* p = 0;

    using pointer = T*;
    using element_type = T;
    using deleter_type = D;

    constexpr unique_ptr(nullptr_t = nullptr) { }
    explicit unique_ptr(pointer p) : p(p) { }

};

} // namespace ctl
#endif // COSMOPOLITAN_CTL_UNIQUE_PTR_H_
