// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_NEW_H_
#define COSMOPOLITAN_CTL_NEW_H_

// clang-format off

namespace ctl {

enum class align_val_t : size_t {};

} // namespace ctl

void* operator new(size_t);
void* operator new[](size_t);
void* operator new(size_t, ctl::align_val_t);
void* operator new[](size_t, ctl::align_val_t);
void* operator new(size_t, void*);
void* operator new[](size_t, void*);

void operator delete(void*) noexcept;
void operator delete[](void*) noexcept;
void operator delete(void*, ctl::align_val_t) noexcept;
void operator delete[](void*, ctl::align_val_t) noexcept;
void operator delete(void*, size_t) noexcept;
void operator delete[](void*, size_t) noexcept;
void operator delete(void*, size_t, ctl::align_val_t) noexcept;
void operator delete[](void*, size_t, ctl::align_val_t) noexcept;

#endif // COSMOPOLITAN_CTL_NEW_H_
