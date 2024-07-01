// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_BAD_ALLOC_H_
#define CTL_BAD_ALLOC_H_
#include "exception.h"

namespace ctl {

class bad_alloc : public ctl::exception
{
  public:
    bad_alloc() noexcept = default;
    ~bad_alloc() override = default;

    const char* what() const noexcept override
    {
        return "ctl::bad_alloc";
    }
};

} // namespace ctl

#endif // CTL_BAD_ALLOC_H_
