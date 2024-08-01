// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_RUNTIME_ERROR_H_
#define CTL_RUNTIME_ERROR_H_
#include "ctl/string.h"

namespace ctl {

class runtime_error
{
  public:
    explicit runtime_error(const string& what_arg) : msg_(what_arg)
    {
    }

    explicit runtime_error(const char* what_arg) : msg_(what_arg)
    {
    }

    virtual ~runtime_error() noexcept = default;

    virtual const char* what() const noexcept
    {
        return msg_.c_str();
    }

  private:
    string msg_;
};

} // namespace ctl

#endif // CTL_RUNTIME_ERROR_H_
