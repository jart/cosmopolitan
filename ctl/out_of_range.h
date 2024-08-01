// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_OUT_OF_RANGE_H_
#define CTL_OUT_OF_RANGE_H_
#include "exception.h"

namespace ctl {

class out_of_range : public ctl::exception
{
  public:
    out_of_range() noexcept = default;
    ~out_of_range() override = default;

    const char* what() const noexcept override
    {
        return "ctl::out_of_range";
    }
};

} // namespace ctl

#endif // CTL_OUT_OF_RANGE_H_
